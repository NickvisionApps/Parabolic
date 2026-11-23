// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>
#include <oclero/qlementine/widgets/TitleBar.hpp>

#include <QStatusBar>
#include <QWidget>

#include <memory>

namespace oclero::qlementine {
/// A top-level window whose content extends into the title bar area.
/// macOS keeps the traffic lights at the top left, Windows keeps the caption buttons at the top right and
/// supports snap layouts, and Linux draws client-side decorations following the user's button layout.
class QLEMENTINE_EXPORT UnifiedWindow : public QWidget {
  Q_OBJECT

public:
  explicit UnifiedWindow(QWidget* parent = nullptr);
  ~UnifiedWindow() override;

  TitleBar* titleBar() const;

  /// The window takes ownership of the new widget and releases the previous one without deleting it.
  QWidget* centralWidget() const;
  void setCentralWidget(QWidget* widget);

  /// Radius of the rounded corners of the central widget, in pixels. The corners are drawn with the background of the
  /// window, so they are visible next to the title bar, the leading widget and the status bar. The default is 0:
  /// the corners are square and the central widget has no border. Use a radius to inset the central widget.
  /// An inset central widget has its own border, so the border of the title bar is hidden then.
  int contentCornerRadius() const;
  void setContentCornerRadius(int radius);

  /// Sides of the central widget that are inset in the window: they have a border, and a corner is rounded when both of
  /// its sides are inset. A side that is not inset blends into the window, without border. The default is the top
  /// side, under the title bar, and the left side, next to the leading widget. The bottom side is also inset when a
  /// status bar is displayed below the central widget.
  Qt::Edges contentInsetEdges() const;
  void setContentInsetEdges(Qt::Edges edges);

  /// A status bar displayed at the bottom of the window, across its whole width, below the central widget and the
  /// leading widget. The leading widget stops above it.
  /// The window takes ownership of the new status bar and releases the previous one without deleting it.
  QStatusBar* statusBar() const;
  void setStatusBar(QStatusBar* statusBar);

  /// A widget, such as a sidebar, displayed at the left of the window over the full height, including the title bar
  /// area. Its width is its size hint, limited by its minimum and maximum widths, and at
  /// least wide enough to contain the window controls displayed on the left. The title bar is displayed over it
  /// without background, and the caption buttons on the left (traffic lights on macOS) are drawn over it. The window
  /// keeps the top contents margin of the widget equal to the title bar height, so that its content starts below it.
  /// The window takes ownership of the new widget and releases the previous one without deleting it.
  QWidget* leadingWidget() const;
  void setLeadingWidget(QWidget* widget);

protected:
  bool event(QEvent* evt) override;
  void paintEvent(QPaintEvent* evt) override;

private:
  struct Impl;
  std::unique_ptr<Impl> _impl;
};
} // namespace oclero::qlementine
