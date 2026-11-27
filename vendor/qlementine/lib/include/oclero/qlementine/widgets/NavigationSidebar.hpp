// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>
#include <oclero/qlementine/widgets/NavigationBar.hpp>

#include <QPointer>
#include <QVariantAnimation>

#include <vector>

class QScrollBar;

namespace oclero::qlementine {
/// A vertical navigation sidebar, with top items, bottom items, sections, separators,
/// nested items, a header and a footer.
/// It can be collapsed to a compact (icon-only) rail, in which the header and footer widgets are hidden.
class QLEMENTINE_EXPORT NavigationSidebar : public NavigationBar {
  Q_OBJECT

  Q_PROPERTY(bool compact READ compact WRITE setCompact NOTIFY compactChanged)
  Q_PROPERTY(int expandedWidth READ expandedWidth WRITE setExpandedWidth NOTIFY expandedWidthChanged)

public:
  explicit NavigationSidebar(QWidget* parent = nullptr);
  ~NavigationSidebar() override = default;

  QSize sizeHint() const override;

  int addBottomItem(
    const QString& text, const QIcon& icon = {}, const QString& badge = {}, const QVariant& itemData = {});
  int addChildItem(int parentIndex, const QString& text, const QIcon& icon = {}, const QString& badge = {},
    const QVariant& itemData = {});
  int addSection(const QString& title);
  int addSeparator();
  int addBottomSeparator();
  void removeItem(int index) override;

  bool isItemExpanded(int index) const;
  Q_SLOT void setItemExpanded(int index, bool expanded);

  bool compact() const;
  Q_SLOT void setCompact(bool compact);
  Q_SLOT void toggleCompact();

  /// Width when not compact. A value of 0 uses the default width from the theme.
  int expandedWidth() const;
  Q_SLOT void setExpandedWidth(int width);

  QWidget* headerWidget() const;
  void setHeaderWidget(QWidget* widget);

  QWidget* footerWidget() const;
  void setFooterWidget(QWidget* widget);

Q_SIGNALS:
  void compactChanged();
  void expandedWidthChanged();
  void headerWidgetChanged();
  void footerWidgetChanged();
  void itemExpandedChanged(int index);
  void itemAboutToBeRemoved(int index);

protected: // QWidget override.
  void keyPressEvent(QKeyEvent* e) override;
  void mouseReleaseEvent(QMouseEvent* e) override;
  void wheelEvent(QWheelEvent* e) override;
  void paintEvent(QPaintEvent* e) override;
  bool event(QEvent* e) override;

protected: // Inherited via AbstractItemListWidget
  int itemAtPos(const QPoint& pos) const override;
  void updateItemsSizeHints() override;
  void updateItemRects() override;
  QRect getCurrentItemRect() const override;
  bool canSelectItem(int index) const override;

protected: // Inherited via NavigationBar
  const QColor& getItemBadgeBgColor(MouseState mouse, bool selected, const Theme& theme) const override;
  const QColor& getItemBadgeFgColor(MouseState mouse, bool selected, const Theme& theme) const override;
  QMargins getItemPadding() const override;
  double getItemRadius() const override;
  QMargins getPadding() const override;
  int getSpacing() const override;
  int getItemMinimumHeight() const override;
  void drawBackground(QPainter& p) const override;
  void drawItemBackground(QPainter& p, const Item& item) const override;
  void drawItemForeground(QPainter& p, const Item& item) const override;
  void drawCurrentItemIndicator(QPainter& p) const override;
  QFont labelFont() const override;

private:
  QFont sectionFont() const;
  bool hasChildItems(int index) const;
  std::vector<int> visibleItems(bool bottom, int parent = -1) const;
  double rowScale(int index) const;
  int rowHeight(int index) const;
  int rowSpacing(int index) const;
  int rowsHeight(const std::vector<int>& rows) const;
  int slotHeight(const QWidget* widget) const;
  void ensureItemVisible(int index);
  void relayout();
  int naturalWidth() const;

private:
  bool _compact{ false };
  double _compactProgress{ 0. };
  QVariantAnimation _compactAnimation;
  QVariantAnimation _expansionAnimation;
  double _expansionProgress{ 0. };
  QScrollBar* _scrollBar{ nullptr };
  int _expandedWidth{ 0 };
  int _scrollOffset{ 0 };
  QRect _viewportRect;
  QPointer<QWidget> _headerWidget;
  QPointer<QWidget> _footerWidget;
};
} // namespace oclero::qlementine
