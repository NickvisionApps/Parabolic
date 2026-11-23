// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include "WindowBackend.hpp"

#include <oclero/qlementine/widgets/TitleBar.hpp>
#include <oclero/qlementine/widgets/UnifiedWindow.hpp>

#include <QAbstractNativeEventFilter>
#include <QByteArray>
#include <QColor>
#include <QCoreApplication>
#include <QEvent>
#include <QPalette>
#include <QPoint>
#include <QRect>

#include <array>
#include <optional>

#ifndef NOMINMAX
#  define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <shellapi.h>

namespace oclero::qlementine {
namespace {
// Attribute values that may be missing from older SDKs.
constexpr DWORD dwmaUseImmersiveDarkModeBefore20H1 = 19;
constexpr DWORD dwmaUseImmersiveDarkMode = 20;
constexpr DWORD dwmaWindowCornerPreference = 33;
constexpr DWORD dwmCornerRound = 2;

// Windows build numbers where the attributes become available.
constexpr DWORD buildDarkModeBefore20H1 = 17763;
constexpr DWORD buildDarkMode = 19041;
constexpr DWORD buildWindows11 = 22000;

DWORD windowsBuildNumber() {
  static const DWORD build = []() -> DWORD {
    using RtlGetVersionFunc = LONG(WINAPI*)(OSVERSIONINFOW*);
    const auto ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll) {
      return 0;
    }
    const auto func = reinterpret_cast<RtlGetVersionFunc>(reinterpret_cast<void*>(GetProcAddress(ntdll, "RtlGetVersion")));
    if (!func) {
      return 0;
    }
    OSVERSIONINFOW info{};
    info.dwOSVersionInfoSize = sizeof(info);
    if (func(&info) != 0) {
      return 0;
    }
    return info.dwBuildNumber;
  }();
  return build;
}

// Thickness of the invisible resize frame, in physical pixels.
SIZE resizeFrameSize(HWND hwnd) {
  const auto dpi = GetDpiForWindow(hwnd);
  const auto padding = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
  return SIZE{GetSystemMetricsForDpi(SM_CXSIZEFRAME, dpi) + padding, GetSystemMetricsForDpi(SM_CYSIZEFRAME, dpi) + padding};
}

std::optional<CaptionButton> captionButtonFromHitTest(WPARAM hitTest) {
  switch (hitTest) {
    case HTMINBUTTON:
      return CaptionButton::Minimize;
    case HTMAXBUTTON:
      return CaptionButton::Maximize;
    case HTCLOSE:
      return CaptionButton::Close;
    case HTSYSMENU:
      return CaptionButton::Menu;
    default:
      return std::nullopt;
  }
}

class WinWindowBackend final
  : public WindowBackend
  , public QAbstractNativeEventFilter {
public:
  explicit WinWindowBackend(UnifiedWindow& window)
    : WindowBackend(window) {
    QCoreApplication::instance()->installNativeEventFilter(this);
  }

  ~WinWindowBackend() override {
    if (auto* app = QCoreApplication::instance()) {
      app->removeNativeEventFilter(this);
    }
  }

  void prepareWindow() override {
    // Keep the standard top-level flags: the frameless hint would remove snapping, shadow and animations.
  }

  void attachNativeWindow() override {
    const auto previousHwnd = _hwnd;
    _hwnd = reinterpret_cast<HWND>(_window.winId());
    if (!_hwnd) {
      return;
    }

    // Window styles required by the system for snapping, animations and the system menu.
    const auto style = GetWindowLongPtrW(_hwnd, GWL_STYLE);
    const auto requiredStyle = static_cast<LONG_PTR>(WS_THICKFRAME | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
    const auto needsStyle = !_window.isFullScreen() && (style & requiredStyle) != requiredStyle;
    if (needsStyle) {
      SetWindowLongPtrW(_hwnd, GWL_STYLE, style | requiredStyle);
    }
    if (needsStyle || previousHwnd != _hwnd) {
      SetWindowPos(_hwnd,
        nullptr,
        0,
        0,
        0,
        0,
        SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }

    // A 1px extended frame brings back the window shadow.
    const MARGINS margins{0, 0, 1, 0};
    DwmExtendFrameIntoClientArea(_hwnd, &margins);

    applyDwmAttributes();

    // Title bar configuration.
    auto* titleBar = _window.titleBar();
    CaptionButtonLayout layout;
    layout.right = { CaptionButton::Minimize, CaptionButton::Maximize, CaptionButton::Close };
    titleBar->setCaptionButtonLayout(layout);
    titleBar->setCaptionButtonsVisible(true);
    titleBar->setSystemInsets(0, 0);
  }

  void handleEvent(QEvent* evt) override {
    switch (evt->type()) {
      case QEvent::Show:
      case QEvent::WindowStateChange:
      case QEvent::ActivationChange:
      case QEvent::ThemeChange:
      case QEvent::ApplicationPaletteChange:
      case QEvent::ScreenChangeInternal:
        applyDwmAttributes();
        break;
      default:
        break;
    }
  }

  void showSystemMenu(const QPoint& globalPos) override {
    if (!_hwnd) {
      return;
    }
    POINT origin{0, 0};
    ClientToScreen(_hwnd, &origin);
    const auto localPos = _window.mapFromGlobal(globalPos);
    const auto dpr = _window.devicePixelRatioF();
    showMenu(POINT{origin.x + qRound(localPos.x() * dpr), origin.y + qRound(localPos.y() * dpr)});
  }

  bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override {
    if (!_hwnd || eventType != "windows_generic_MSG") {
      return false;
    }
    const auto* msg = static_cast<const MSG*>(message);
    if (msg->hwnd != _hwnd) {
      return false;
    }

    switch (msg->message) {
      // Client area covers the whole window, except the overshoot when maximized.
      case WM_NCCALCSIZE: {
        if (!msg->wParam) {
          return false;
        }
        auto& rect = reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam)->rgrc[0];
        if (IsZoomed(_hwnd) && !_window.isFullScreen()) {
          const auto frame = resizeFrameSize(_hwnd);
          rect.left += frame.cx;
          rect.right -= frame.cx;
          rect.top += frame.cy;
          rect.bottom -= frame.cy;

          // Keep a thin edge so that an auto-hide taskbar can still be revealed.
          MONITORINFO monitorInfo{};
          monitorInfo.cbSize = sizeof(monitorInfo);
          if (GetMonitorInfoW(MonitorFromWindow(_hwnd, MONITOR_DEFAULTTONEAREST), &monitorInfo)) {
            APPBARDATA appBarData{};
            appBarData.cbSize = sizeof(appBarData);
            appBarData.rc = monitorInfo.rcMonitor;
            for (const auto edge : std::array<UINT, 4>{ABE_LEFT, ABE_TOP, ABE_RIGHT, ABE_BOTTOM}) {
              appBarData.uEdge = edge;
              if (SHAppBarMessage(ABM_GETAUTOHIDEBAREX, &appBarData)) {
                switch (edge) {
                  case ABE_LEFT:
                    rect.left += 1;
                    break;
                  case ABE_TOP:
                    rect.top += 1;
                    break;
                  case ABE_RIGHT:
                    rect.right -= 1;
                    break;
                  default:
                    rect.bottom -= 1;
                    break;
                }
              }
            }
          }
        }
        *result = 0;
        return true;
      }

      // Resize borders, caption buttons and drag area.
      case WM_NCHITTEST: {
        POINT screenPos{GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};

        if (!IsZoomed(_hwnd) && !_window.isFullScreen()) {
          RECT windowRect{};
          GetWindowRect(_hwnd, &windowRect);
          const auto frame = resizeFrameSize(_hwnd);
          const auto isLeft = screenPos.x < windowRect.left + frame.cx;
          const auto isRight = screenPos.x >= windowRect.right - frame.cx;
          const auto isTop = screenPos.y < windowRect.top + frame.cy;
          const auto isBottom = screenPos.y >= windowRect.bottom - frame.cy;
          if (isTop || isBottom || isLeft || isRight) {
            if (isTop && isLeft) {
              *result = HTTOPLEFT;
            } else if (isTop && isRight) {
              *result = HTTOPRIGHT;
            } else if (isBottom && isLeft) {
              *result = HTBOTTOMLEFT;
            } else if (isBottom && isRight) {
              *result = HTBOTTOMRIGHT;
            } else if (isTop) {
              *result = HTTOP;
            } else if (isBottom) {
              *result = HTBOTTOM;
            } else if (isLeft) {
              *result = HTLEFT;
            } else {
              *result = HTRIGHT;
            }
            return true;
          }
        }

        // Physical pixels to the title bar's device-independent pixels.
        ScreenToClient(_hwnd, &screenPos);
        const auto dpr = _window.devicePixelRatioF();
        auto* titleBar = _window.titleBar();
        const auto localPos = titleBar->mapFrom(&_window, QPoint(qRound(screenPos.x / dpr), qRound(screenPos.y / dpr)));
        *result = HTCLIENT;
        if (titleBar->isVisible() && titleBar->rect().contains(localPos)) {
          if (const auto button = titleBar->captionButtonAt(localPos)) {
            switch (*button) {
              case CaptionButton::Minimize:
                *result = HTMINBUTTON;
                break;
              case CaptionButton::Maximize:
                *result = HTMAXBUTTON;
                break;
              case CaptionButton::Close:
                *result = HTCLOSE;
                break;
              case CaptionButton::Menu:
                *result = HTSYSMENU;
                break;
            }
          } else if (titleBar->isDragArea(localPos)) {
            *result = HTCAPTION;
          }
        }
        return true;
      }

      // Hover feedback. The message is not consumed so that the system can show the snap layouts flyout.
      case WM_NCMOUSEMOVE: {
        const auto button = captionButtonFromHitTest(msg->wParam);
        if (button && !_trackingLeave) {
          TRACKMOUSEEVENT trackEvent{};
          trackEvent.cbSize = sizeof(trackEvent);
          trackEvent.dwFlags = TME_LEAVE | TME_NONCLIENT;
          trackEvent.hwndTrack = _hwnd;
          _trackingLeave = TrackMouseEvent(&trackEvent) != FALSE;
        }
        if (_pressed && _pressed != button) {
          _pressed.reset();
          _window.titleBar()->setPressedCaptionButton(std::nullopt);
        }
        _window.titleBar()->setHoveredCaptionButton(button);
        return false;
      }

      case WM_NCMOUSELEAVE:
      case WM_CAPTURECHANGED:
      case WM_NCACTIVATE: {
        if (msg->message == WM_NCMOUSELEAVE) {
          _trackingLeave = false;
        } else if (msg->message == WM_NCACTIVATE && msg->wParam) {
          return false;
        }
        _pressed.reset();
        _window.titleBar()->setPressedCaptionButton(std::nullopt);
        _window.titleBar()->setHoveredCaptionButton(std::nullopt);
        return false;
      }

      // Press feedback. The default tracking loop is skipped to keep control over the visual state.
      case WM_NCLBUTTONDOWN:
      case WM_NCLBUTTONDBLCLK: {
        const auto button = captionButtonFromHitTest(msg->wParam);
        if (!button) {
          return false;
        }
        // Double-clicking the system menu icon closes the window.
        if (msg->message == WM_NCLBUTTONDBLCLK && *button == CaptionButton::Menu) {
          _window.close();
          *result = 0;
          return true;
        }
        _pressed = button;
        _window.titleBar()->setHoveredCaptionButton(button);
        _window.titleBar()->setPressedCaptionButton(button);
        *result = 0;
        return true;
      }

      // Action of the caption buttons.
      case WM_NCLBUTTONUP: {
        const auto button = captionButtonFromHitTest(msg->wParam);
        const auto pressed = _pressed;
        _pressed.reset();
        _window.titleBar()->setPressedCaptionButton(std::nullopt);
        if (!button || pressed != button) {
          return false;
        }
        switch (*button) {
          case CaptionButton::Close:
            _window.close();
            break;
          case CaptionButton::Minimize:
            _window.showMinimized();
            break;
          case CaptionButton::Maximize:
            if (_window.isMaximized()) {
              _window.showNormal();
            } else {
              _window.showMaximized();
            }
            break;
          case CaptionButton::Menu:
            showMenu(POINT{GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)});
            break;
        }
        *result = 0;
        return true;
      }

      // System menu.
      case WM_NCRBUTTONUP: {
        if (msg->wParam != HTCAPTION && msg->wParam != HTSYSMENU) {
          return false;
        }
        showMenu(POINT{GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)});
        *result = 0;
        return true;
      }

      default:
        return false;
    }
  }

private:
  void applyDwmAttributes() {
    if (!_hwnd) {
      return;
    }
    const auto build = windowsBuildNumber();

    // Dark title bar flag, following the window palette.
    if (build >= buildDarkModeBefore20H1) {
      const BOOL isDark = _window.palette().color(QPalette::Window).lightness() < 128 ? TRUE : FALSE;
      DwmSetWindowAttribute(
        _hwnd, build >= buildDarkMode ? dwmaUseImmersiveDarkMode : dwmaUseImmersiveDarkModeBefore20H1, &isDark, sizeof(isDark));
    }

    // Rounded corners (Windows 11).
    if (build >= buildWindows11) {
      const DWORD cornerPreference = dwmCornerRound;
      DwmSetWindowAttribute(_hwnd, dwmaWindowCornerPreference, &cornerPreference, sizeof(cornerPreference));
    }
  }

  void showMenu(POINT screenPos) {
    const auto menu = GetSystemMenu(_hwnd, FALSE);
    if (!menu) {
      return;
    }
    const auto isMaximized = IsZoomed(_hwnd) != FALSE;
    const auto isMinimized = IsIconic(_hwnd) != FALSE;
    const auto isNormal = !isMaximized && !isMinimized;
    EnableMenuItem(menu, SC_RESTORE, MF_BYCOMMAND | (isNormal ? MF_GRAYED : MF_ENABLED));
    EnableMenuItem(menu, SC_MOVE, MF_BYCOMMAND | (isNormal ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(menu, SC_SIZE, MF_BYCOMMAND | (isNormal ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(menu, SC_MAXIMIZE, MF_BYCOMMAND | (isMaximized ? MF_GRAYED : MF_ENABLED));
    EnableMenuItem(menu, SC_MINIMIZE, MF_BYCOMMAND | (isMinimized ? MF_GRAYED : MF_ENABLED));
    SetForegroundWindow(_hwnd);
    const auto command = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, screenPos.x, screenPos.y, 0, _hwnd, nullptr);
    if (command) {
      PostMessageW(_hwnd, WM_SYSCOMMAND, static_cast<WPARAM>(command), 0);
    }
  }

  HWND _hwnd = nullptr;
  bool _trackingLeave = false;
  std::optional<CaptionButton> _pressed;
};
} // namespace

std::unique_ptr<WindowBackend> createWindowBackend(UnifiedWindow& window) {
  return std::make_unique<WinWindowBackend>(window);
}
} // namespace oclero::qlementine
