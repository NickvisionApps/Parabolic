// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QList>
#include <QRect>
#include <QString>
#include <QWidget>

#include <memory>
#include <optional>

namespace oclero::qlementine {
/// Look of the title bar and its caption buttons.
enum class TitleBarStyle {
  Qlementine,
  Adwaita,
};

/// System buttons that can be displayed in the title bar.
enum class CaptionButton {
  Close,
  Minimize,
  Maximize,
  Menu,
};

/// Buttons on each side of the title, in visual order from left to right, following the user's desktop preferences.
struct CaptionButtonLayout {
  QList<CaptionButton> left;
  QList<CaptionButton> right;
};

/// A title bar that is part of the window content (client-side decoration or extended content view).
/// It hosts an optional content widget (menu bar, search field...), draws the caption buttons when the
/// platform doesn't provide them, and tells the window which parts of it can be used to drag the window.
class UnifiedWindow;

class QLEMENTINE_EXPORT TitleBar : public QWidget {
  Q_OBJECT

  Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
  Q_PROPERTY(bool captionButtonsVisible READ captionButtonsVisible WRITE setCaptionButtonsVisible NOTIFY
      captionButtonsVisibleChanged)

public:
  explicit TitleBar(QWidget* parent = nullptr);
  ~TitleBar() override;

  QString title() const;
  Q_SLOT void setTitle(const QString& title);

  /// Widget displayed between the leading and trailing system areas, next to the title.
  /// The title bar takes ownership of the new widget and releases the previous one without deleting it.
  QWidget* contentWidget() const;
  void setContentWidget(QWidget* widget);

  TitleBarStyle titleBarStyle() const;
  Q_SLOT void setTitleBarStyle(TitleBarStyle style);

  /// False when the platform draws the buttons itself (traffic lights on macOS).
  bool captionButtonsVisible() const;
  Q_SLOT void setCaptionButtonsVisible(bool visible);

  CaptionButtonLayout captionButtonLayout() const;
  Q_SLOT void setCaptionButtonLayout(const CaptionButtonLayout& layout);

  /// Space reserved on the left and right edges for system-drawn controls (traffic lights, native caption buttons).
  /// Expressed in device-independent pixels, relative to the title bar.
  int leftInset() const;
  int rightInset() const;
  void setSystemInsets(int left, int right);

  /// Width of the area on the left that is covered by a widget placed under the title bar, such as a sidebar that
  /// extends up to the top of the window (see UnifiedWindow::setLeadingWidget()). The title bar doesn't paint its
  /// background there and its content starts after it, unless contentAfterLeading() is false: the background then
  /// covers the leading area too, so the whole row has the same background,
  /// except with a QToolBar, which paints its own background. Caption buttons and the drag area
  /// still apply over it.
  int leadingWidth() const;
  void setLeadingWidth(int width);

  /// Whether the content widget starts after the leading area (true, the default) or right after the window controls
  /// on the left (traffic lights, or caption buttons displayed on the left), over the leading area (false). Use false
  /// to place a toolbar next to the window controls. The content always stays clear of the window controls on both
  /// sides, on every platform.
  bool contentAfterLeading() const;
  void setContentAfterLeading(bool after);

  /// Marks a widget placed in the title bar as interactive (true) or as a part of the draggable area (false).
  /// By default, the content widget container, plain QWidget/QFrame containers and display-only widgets (QLabel,
  /// QProgressBar, QLCDNumber, IconWidget, StatusBadgeWidget, NotificationBadge) are draggable and every other
  /// widget is interactive. QTabBar, QMenuBar, QToolBar and AbstractItemListWidget (NavigationBar, SegmentedControl)
  /// are draggable only where there is no tab or item, and not over their child widgets. Overrides always win.
  void setHitTestVisible(QWidget* widget, bool visible);

  /// Whether the point (in title bar coordinates) can be used to move the window.
  bool isDragArea(const QPoint& pos) const;

  /// Caption button drawn by the title bar under the point (in title bar coordinates), if any.
  std::optional<CaptionButton> captionButtonAt(const QPoint& pos) const;

  /// Rectangle of a caption button in title bar coordinates. Empty if the button is not displayed.
  QRect captionButtonRect(CaptionButton button) const;

  /// Hover and press feedback for platforms where the mouse events over the buttons are handled by the OS
  /// (non-client messages on Windows). Pass std::nullopt to clear.
  void setHoveredCaptionButton(std::optional<CaptionButton> button);
  void setPressedCaptionButton(std::optional<CaptionButton> button);

  QSize minimumSizeHint() const override;
  QSize sizeHint() const override;

Q_SIGNALS:
  void titleChanged();
  void captionButtonsVisibleChanged();
  void closeRequested();
  void minimizeRequested();
  /// Toggles between maximized and normal.
  void maximizeRequested();
  void menuRequested(const QPoint& globalPos);
  /// The user pressed the mouse in the drag area and the window should start moving.
  void moveRequested();
  void doubleClicked();

protected:
  bool event(QEvent* evt) override;
  void paintEvent(QPaintEvent* evt) override;
  void resizeEvent(QResizeEvent* evt) override;
  void mousePressEvent(QMouseEvent* evt) override;
  void mouseMoveEvent(QMouseEvent* evt) override;
  void mouseReleaseEvent(QMouseEvent* evt) override;
  void mouseDoubleClickEvent(QMouseEvent* evt) override;
  void leaveEvent(QEvent* evt) override;

private:
  friend class UnifiedWindow;

  /// The border at the bottom of the title bar. A UnifiedWindow hides it when its central widget is inset, as it has
  /// its own border.
  bool borderVisible() const;
  void setBorderVisible(bool visible);

  struct Impl;
  std::unique_ptr<Impl> _impl;
};
} // namespace oclero::qlementine
