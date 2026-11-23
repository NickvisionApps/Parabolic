// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include "WindowBackend.hpp"

#include <oclero/qlementine/widgets/TitleBar.hpp>
#include <oclero/qlementine/widgets/UnifiedWindow.hpp>

#include <QEvent>
#include <QObject>
#include <QWidget>

#include <AppKit/AppKit.h>

#include <cmath>

namespace oclero::qlementine {
namespace {
class MacWindowBackend final
  : public QObject
  , public WindowBackend {
public:
  explicit MacWindowBackend(UnifiedWindow& window)
    : QObject(nullptr)
    , WindowBackend(window)
    , _observers([[NSMutableArray alloc] init]) {}

  ~MacWindowBackend() override {
    clearObservers();
  }

  void prepareWindow() override {
    // Qt::FramelessWindowHint would remove the traffic lights, so the native NSWindow configuration is used instead.
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
    // Qt Widgets add the safe area margins (the native title bar height) to the contents margins of top-level widgets,
    // which would keep the content below the title bar.
    _window.setAttribute(Qt::WA_ContentsMarginsRespectsSafeArea, false);
    _window.setWindowFlag(Qt::ExpandedClientAreaHint, true);
    _window.setWindowFlag(Qt::NoTitleBarBackgroundHint, true);
#endif
  }

  void attachNativeWindow() override {
    NSView* view = (__bridge NSView*)reinterpret_cast<void*>(_window.winId());
    NSWindow* window = view ? view.window : nil;
    if (!window) {
      return;
    }

    // Native window configuration, done once per NSWindow and again if Qt rewrote the style mask.
    if (_nsWindow != window || !(window.styleMask & NSWindowStyleMaskFullSizeContentView)) {
      window.styleMask |= NSWindowStyleMaskFullSizeContentView;
      window.titlebarAppearsTransparent = YES;
      window.titleVisibility = NSWindowTitleHidden;
      window.tabbingMode = NSWindowTabbingModeDisallowed;
      if (@available(macOS 11.0, *)) {
        window.titlebarSeparatorStyle = NSTitlebarSeparatorStyleNone;
      }
      _window.titleBar()->setCaptionButtonsVisible(false);
    }

    // Notifications and event filter, once per NSWindow.
    if (_nsWindow != window) {
      clearObservers();
      _nsWindow = window;
      NSArray<NSNotificationName>* names = @[
        NSWindowWillEnterFullScreenNotification,
        NSWindowDidEnterFullScreenNotification,
        NSWindowWillExitFullScreenNotification,
        NSWindowDidExitFullScreenNotification,
        NSWindowDidResizeNotification,
        NSWindowDidChangeScreenNotification,
        NSWindowDidDeminiaturizeNotification,
      ];
      for (NSNotificationName name in names) {
        [_observers addObject:[[NSNotificationCenter defaultCenter] addObserverForName:name
                                                                                object:window
                                                                                 queue:[NSOperationQueue mainQueue]
                                                                            usingBlock:^(NSNotification*) {
                                                                              updateLayout();
                                                                            }]];
      }
      _window.titleBar()->installEventFilter(this);
    }

    updateLayout();
  }

  void handleEvent(QEvent* evt) override {
    switch (evt->type()) {
      case QEvent::Show:
      case QEvent::Resize:
      case QEvent::WindowStateChange:
      case QEvent::ThemeChange:
      case QEvent::ScreenChangeInternal:
      case QEvent::ApplicationPaletteChange:
        attachNativeWindow();
        break;
      default:
        break;
    }
  }

  void handleTitleBarDoubleClick() override {
    NSWindow* window = _nsWindow;
    if (!window || (window.styleMask & NSWindowStyleMaskFullScreen)) {
      return;
    }
    NSString* action = [[NSUserDefaults standardUserDefaults] stringForKey:@"AppleActionOnDoubleClick"];
    if ([action isEqualToString:@"Minimize"]) {
      [window performMiniaturize:nil];
    } else if (![action isEqualToString:@"None"]) {
      [window performZoom:nil];
    }
  }

protected:
  bool eventFilter(QObject* watched, QEvent* evt) override {
    if (watched == _window.titleBar() && evt->type() == QEvent::Resize) {
      updateLayout();
    }
    return false;
  }

private:
  void updateLayout() {
    NSWindow* window = _nsWindow;
    if (!window) {
      return;
    }

    // Native fullscreen hides the traffic lights.
    int leftInset = 0;
    if (!(window.styleMask & NSWindowStyleMaskFullScreen) &&
        window.contentLayoutRect.size.height < NSHeight(window.contentView.frame) - 0.5) {
      // Vertically center the traffic lights in the title bar. They live in a private titlebar container view.
      NSButton* closeButton = [window standardWindowButton:NSWindowCloseButton];
      NSButton* zoomButton = [window standardWindowButton:NSWindowZoomButton];
      const auto titleBarHeight = static_cast<CGFloat>(_window.titleBar()->height());
      NSView* titlebarView = closeButton.superview;
      NSView* container = titlebarView.superview;
      if (titleBarHeight > 0 && container && container.superview) {
        const NSRect parentBounds = container.superview.bounds;
        const CGFloat containerY = container.superview.isFlipped ? 0 : parentBounds.size.height - titleBarHeight;
        const NSRect containerFrame = NSMakeRect(0, containerY, parentBounds.size.width, titleBarHeight);
        if (!NSEqualRects(container.frame, containerFrame)) {
          container.frame = containerFrame;
        }
        if (!NSEqualRects(titlebarView.frame, container.bounds)) {
          titlebarView.frame = container.bounds;
        }
        for (NSButton* button in @[
               closeButton,
               [window standardWindowButton:NSWindowMiniaturizeButton],
               zoomButton,
             ]) {
          const CGFloat buttonY = (titleBarHeight - button.frame.size.height) / 2;
          if (button && button.superview == titlebarView && std::abs(button.frame.origin.y - buttonY) > 0.01) {
            [button setFrameOrigin:NSMakePoint(button.frame.origin.x, buttonY)];
          }
        }
      }

      // Space reserved for the traffic lights, read from the actual button position.
      if (zoomButton) {
        const NSRect zoomFrame = [zoomButton convertRect:zoomButton.bounds toView:nil];
        leftInset = static_cast<int>(std::ceil(NSMaxX(zoomFrame))) + 8;
      }
    }

    if (leftInset != _leftInset) {
      _leftInset = leftInset;
      _window.titleBar()->setSystemInsets(leftInset, 0);
    }
  }

  void clearObservers() {
    for (id observer in _observers) {
      [[NSNotificationCenter defaultCenter] removeObserver:observer];
    }
    [_observers removeAllObjects];
  }

  NSMutableArray* _observers;
  NSWindow* __weak _nsWindow = nil;
  int _leftInset = -1;
};
} // namespace

std::unique_ptr<WindowBackend> createWindowBackend(UnifiedWindow& window) {
  return std::make_unique<MacWindowBackend>(window);
}
} // namespace oclero::qlementine
