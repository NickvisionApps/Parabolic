// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include "WindowBackend.hpp"
#include "LinuxButtonLayout.hpp"

#include <oclero/qlementine/widgets/UnifiedWindow.hpp>

#include <QCoreApplication>
#include <QGuiApplication>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QWindow>

// X11 frame extents need xcb. CMake must define QLEMENTINE_WITH_XCB and link xcb to enable them.
#if defined(QLEMENTINE_WITH_XCB) && __has_include(<xcb/xcb.h>)
#  include <QtGui/qguiapplication_platform.h>
#  include <xcb/xcb.h>
#  include <cstdint>
#  include <cstdlib>
#  include <cstring>
#  define QLEMENTINE_LINUX_XCB
#endif

namespace oclero::qlementine {
namespace {
constexpr auto gnomeShadowMargin = 24;
constexpr auto defaultShadowMargin = 16;
constexpr auto gnomeCornerRadius = 12;
constexpr auto defaultCornerRadius = 8;
constexpr auto resizeCornerSize = 16;
constexpr auto translationContext = "oclero::qlementine::UnifiedWindow";

class LinuxWindowBackend final
  : public QObject
  , public WindowBackend {
public:
  explicit LinuxWindowBackend(UnifiedWindow& window)
    : QObject(nullptr)
    , WindowBackend(window)
    , _buttonLayout(LinuxButtonLayout::instance()) {
    _layoutConnection = connect(&_buttonLayout, &LinuxButtonLayout::changed, this, [this]() {
      _window.titleBar()->setCaptionButtonLayout(_buttonLayout.layout());
    });
  }

  ~LinuxWindowBackend() override {
    disconnect(_layoutConnection);
  }

  void prepareWindow() override {
    // Qt::FramelessWindowHint also prevents Qt from adding its own decoration on Wayland.
    _window.setWindowFlag(Qt::FramelessWindowHint, true);
    // The transparent margin around the window holds the shadow.
    _window.setAttribute(Qt::WA_TranslucentBackground, true);
    _window.setMouseTracking(true);
    _window.installEventFilter(this);
    updateFrame();
  }

  void attachNativeWindow() override {
    auto* titleBar = _window.titleBar();
    titleBar->setTitleBarStyle(_buttonLayout.isGnome() ? TitleBarStyle::Adwaita : TitleBarStyle::Qlementine);
    titleBar->setCaptionButtonLayout(_buttonLayout.layout());
    titleBar->setCaptionButtonsVisible(true);
    updateFrame();
  }

  void handleEvent(QEvent* evt) override {
    if (evt->type() == QEvent::WindowStateChange || evt->type() == QEvent::ScreenChangeInternal) {
      updateFrame();
    }
  }

  int cornerRadius() const override {
    if (_window.isMaximized() || _window.isFullScreen()) {
      return 0;
    }
    return _buttonLayout.isGnome() ? gnomeCornerRadius : defaultCornerRadius;
  }

  bool paintFrame(QPainter& painter) override {
    const auto margins = _window.contentsMargins();
    const auto background = _window.palette().color(QPalette::Window);

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Maximized or fullscreen: no shadow, no corners.
    if (margins.isNull()) {
      painter.fillRect(_window.rect(), background);
      painter.restore();
      return true;
    }

    const auto radius = static_cast<double>(cornerRadius());
    const auto frame = QRectF(_window.rect().marginsRemoved(margins));

    // Soft shadow, slightly offset downwards.
    painter.setBrush(Qt::NoBrush);
    for (auto i = 0; i <= margins.left(); ++i) {
      const auto falloff = 1. - static_cast<double>(i) / (margins.left() + 1);
      painter.setPen(QPen(QColor(0, 0, 0, static_cast<int>(50 * falloff * falloff)), 1));
      painter.drawRoundedRect(frame.adjusted(-i, -i, i, i).translated(0, 2), radius + i, radius + i);
    }

    // Window background clipped to the rounded shape.
    auto shape = QPainterPath{};
    shape.addRoundedRect(frame, radius, radius);
    painter.fillPath(shape, background);

    // 1px border.
    auto border = _window.palette().color(QPalette::WindowText);
    border.setAlpha(40);
    painter.setPen(QPen(border, 1));
    painter.drawRoundedRect(frame.adjusted(.5, .5, -.5, -.5), radius, radius);

    painter.restore();
    return true;
  }

  void showSystemMenu(const QPoint& globalPos) override {
    QMenu menu(&_window);
    menu.addAction(QCoreApplication::translate(translationContext, "Restore"), [this]() {
      _window.showNormal();
    })->setEnabled(_window.isMaximized() || _window.isFullScreen());
    menu.addAction(QCoreApplication::translate(translationContext, "Minimize"), [this]() {
      _window.showMinimized();
    });
    menu.addAction(QCoreApplication::translate(translationContext, "Maximize"), [this]() {
      _window.showMaximized();
    })->setEnabled(!_window.isMaximized() && _window.minimumSize() != _window.maximumSize());
    menu.addSeparator();
    menu.addAction(QCoreApplication::translate(translationContext, "Close"), [this]() { _window.close(); });
    menu.exec(globalPos);
  }

protected:
  bool eventFilter(QObject* watched, QEvent* evt) override {
    if (watched != &_window) {
      return QObject::eventFilter(watched, evt);
    }

    switch (evt->type()) {
      case QEvent::MouseMove: {
        const auto* mouseEvent = static_cast<QMouseEvent*>(evt);
        if (mouseEvent->buttons() != Qt::NoButton) {
          break;
        }
        const auto edges = edgesAt(mouseEvent->position().toPoint());
        const auto horizontal = edges.testFlag(Qt::LeftEdge) || edges.testFlag(Qt::RightEdge);
        const auto vertical = edges.testFlag(Qt::TopEdge) || edges.testFlag(Qt::BottomEdge);
        if (horizontal && vertical) {
          const auto isMainDiagonal = edges.testFlag(Qt::LeftEdge) == edges.testFlag(Qt::TopEdge);
          _window.setCursor(isMainDiagonal ? Qt::SizeFDiagCursor : Qt::SizeBDiagCursor);
        } else if (horizontal) {
          _window.setCursor(Qt::SizeHorCursor);
        } else if (vertical) {
          _window.setCursor(Qt::SizeVerCursor);
        } else {
          _window.unsetCursor();
        }
        break;
      }
      case QEvent::MouseButtonPress: {
        const auto* mouseEvent = static_cast<QMouseEvent*>(evt);
        const auto edges = edgesAt(mouseEvent->position().toPoint());
        if (mouseEvent->button() == Qt::LeftButton && edges.toInt() != 0 && _window.windowHandle()) {
          return _window.windowHandle()->startSystemResize(edges);
        }
        break;
      }
      case QEvent::Leave:
        _window.unsetCursor();
        break;
      default:
        break;
    }
    return QObject::eventFilter(watched, evt);
  }

private:
  Qt::Edges edgesAt(const QPoint& pos) const {
    if (_window.isMaximized() || _window.isFullScreen() || _window.minimumSize() == _window.maximumSize()) {
      return {};
    }
    const auto frame = _window.rect().marginsRemoved(_window.contentsMargins());
    if (frame.contains(pos)) {
      return {};
    }

    auto left = pos.x() < frame.left();
    auto right = pos.x() > frame.right();
    auto top = pos.y() < frame.top();
    auto bottom = pos.y() > frame.bottom();

    // Near a corner, the side bands also resize diagonally.
    if ((left || right) && !top && !bottom) {
      top = pos.y() < frame.top() + resizeCornerSize;
      bottom = pos.y() > frame.bottom() - resizeCornerSize;
    } else if ((top || bottom) && !left && !right) {
      left = pos.x() < frame.left() + resizeCornerSize;
      right = pos.x() > frame.right() - resizeCornerSize;
    }

    auto edges = Qt::Edges{};
    edges.setFlag(Qt::LeftEdge, left);
    edges.setFlag(Qt::RightEdge, right);
    edges.setFlag(Qt::TopEdge, top);
    edges.setFlag(Qt::BottomEdge, bottom);
    return edges;
  }

  void updateFrame() {
    const auto fullBleed = _window.isMaximized() || _window.isFullScreen();
    const auto margin = fullBleed ? 0 : (_buttonLayout.isGnome() ? gnomeShadowMargin : defaultShadowMargin);
    _window.setContentsMargins(margin, margin, margin, margin);
    _window.update();

    // X11: tells the window manager that the margin is a shadow, so that maximizing and snapping use the frame.
    // Does nothing until the native window exists, and is refreshed when it is created and when the screen changes.
#ifdef QLEMENTINE_LINUX_XCB
    auto* x11 = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
    auto* handle = _window.windowHandle();
    if (x11 && handle) {
      const auto pixels = static_cast<uint32_t>(margin * handle->devicePixelRatio());
      const uint32_t extents[4] = { pixels, pixels, pixels, pixels };
      auto* connection = x11->connection();
      const char atomName[] = "_GTK_FRAME_EXTENTS";
      const auto atomCookie = xcb_intern_atom(connection, 0, static_cast<uint16_t>(std::strlen(atomName)), atomName);
      auto* atomReply = xcb_intern_atom_reply(connection, atomCookie, nullptr);
      if (atomReply) {
        xcb_change_property(connection, XCB_PROP_MODE_REPLACE, static_cast<xcb_window_t>(handle->winId()),
          atomReply->atom, XCB_ATOM_CARDINAL, 32, 4, extents);
        xcb_flush(connection);
        std::free(atomReply);
      }
    }
#endif
  }

  LinuxButtonLayout& _buttonLayout;
  QMetaObject::Connection _layoutConnection;
};
} // namespace

std::unique_ptr<WindowBackend> createWindowBackend(UnifiedWindow& window) {
  return std::make_unique<LinuxWindowBackend>(window);
}
} // namespace oclero::qlementine
