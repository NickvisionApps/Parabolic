// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <QPoint>

#include <memory>

class QEvent;
class QPainter;

namespace oclero::qlementine {
class UnifiedWindow;

/// Platform-specific part of UnifiedWindow. One implementation is compiled per platform, and each one defines
/// createWindowBackend(). The backend configures the native window, feeds the title bar (insets, caption button
/// layout, style, hover state) and handles the system interactions.
class WindowBackend {
public:
  explicit WindowBackend(UnifiedWindow& window)
    : _window(window) {}
  virtual ~WindowBackend() = default;

  /// Called once from the UnifiedWindow constructor, before the native window exists.
  /// Set the window flags and attributes here.
  virtual void prepareWindow() = 0;

  /// Called each time the native window handle is created or changes (QEvent::WinIdChange) and on the first show.
  /// Apply native styling here. Must be safe to call several times.
  virtual void attachNativeWindow() = 0;

  /// Receives the events of the UnifiedWindow after they have been handled by QWidget, such as Show, Resize,
  /// WindowStateChange, ActivationChange, ThemeChange, ScreenChangeInternal and ApplicationPaletteChange.
  virtual void handleEvent(QEvent* evt) = 0;

  /// Lets the backend draw the window frame (shadow, rounded corners, border) before the content.
  /// Returns true if it painted the whole window background, false to let the window fill it.
  virtual bool paintFrame(QPainter& painter) {
    Q_UNUSED(painter);
    return false;
  }

  /// Radius, in device-independent pixels, of the window corners drawn by paintFrame(). 0 means square corners.
  /// The window uses it to clip its content (title bar and central widget) to the rounded shape.
  virtual int cornerRadius() const {
    return 0;
  }

  /// Starts a system window move. The default implementation uses QWindow::startSystemMove().
  virtual void startMove();

  /// Shows the system window menu at the global position. The default implementation does nothing.
  virtual void showSystemMenu(const QPoint& globalPos) {
    Q_UNUSED(globalPos);
  }

  /// Handles a double-click in the drag area. The default implementation toggles maximize.
  virtual void handleTitleBarDoubleClick();

protected:
  UnifiedWindow& _window;
};

std::unique_ptr<WindowBackend> createWindowBackend(UnifiedWindow& window);
} // namespace oclero::qlementine
