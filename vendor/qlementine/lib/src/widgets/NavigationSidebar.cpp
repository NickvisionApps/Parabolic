// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/NavigationSidebar.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/utils/FontUtils.hpp>
#include <oclero/qlementine/utils/ImageUtils.hpp>
#include <oclero/qlementine/utils/PrimitiveUtils.hpp>

#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QHelpEvent>
#include <QToolTip>
#include <QScrollBar>

#include <algorithm>
#include <cmath>

namespace oclero::qlementine {
constexpr auto chevronPenWidth = 1.01;
constexpr auto indicatorFillOpacity = 0.16f;
constexpr auto wheelNotchDelta = 120;
constexpr auto wheelNotchRows = 3;

NavigationSidebar::NavigationSidebar(QWidget* parent)
  : NavigationBar(parent) {
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  // Compact animation.
  _compactAnimation.setStartValue(0.);
  _compactAnimation.setEndValue(0.);
  _compactAnimation.setEasingCurve(QEasingCurve::Type::OutCubic);
  QObject::connect(&_compactAnimation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
    _compactProgress = value.toDouble();
    updateGeometry();
    relayout();
  });

  // Expansion animation: every item moves toward its target by the same eased steps.
  _expansionAnimation.setStartValue(0.);
  _expansionAnimation.setEndValue(1.);
  _expansionAnimation.setEasingCurve(QEasingCurve::Type::OutCubic);
  QObject::connect(&_expansionAnimation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
    const auto progress = value.toDouble();
    const auto step = progress - _expansionProgress;
    _expansionProgress = progress;
    for (auto& item : items()) {
      const auto target = item.expanded ? 1. : 0.;
      item.expansion = progress >= 1. ? target : std::clamp(item.expansion + (item.expanded ? step : -step), 0., 1.);
    }
    updateGeometry();
    relayout();
  });

  // Scroll bar, only visible when the top items don't fit.
  _scrollBar = new QScrollBar(Qt::Vertical, this);
  _scrollBar->setFocusPolicy(Qt::NoFocus);
  _scrollBar->hide();
  QObject::connect(_scrollBar, &QScrollBar::valueChanged, this, [this](int value) {
    _scrollOffset = value;
    relayout();
  });

  QObject::connect(this, &AbstractItemListWidget::currentIndexChanged, this, [this]() {
    ensureItemVisible(currentIndex());
  });
}

QSize NavigationSidebar::sizeHint() const {
  const_cast<NavigationSidebar*>(this)->updateItemsSizeHints();

  const auto padding = getPadding();
  const auto gap = style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing);

  auto neededH = padding.top() + padding.bottom() + slotHeight(_headerWidget) + slotHeight(_footerWidget);
  const auto bottomRows = visibleItems(true);
  if (!bottomRows.empty()) {
    neededH += rowsHeight(bottomRows) + gap;
  }
  neededH += rowsHeight(visibleItems(false));

  return { naturalWidth(), neededH };
}

int NavigationSidebar::naturalWidth() const {
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  const auto padding = getPadding();

  // Width interpolated between the expanded and compact states.
  const auto defaultExpandedWidth = qlementineStyle ? qlementineStyle->theme().controlDefaultWidth * 5 / 2 : 240;
  const auto expandedW = _expandedWidth > 0 ? _expandedWidth : defaultExpandedWidth;
  const auto compactW = iconSize().width() + 2 * (padding.left() + getItemPadding().left());
  return static_cast<int>(std::round(expandedW + (compactW - expandedW) * _compactProgress));
}

int NavigationSidebar::addBottomItem(
  const QString& text, const QIcon& icon, const QString& badge, const QVariant& itemData) {
  Item item;
  item.text = text;
  item.icon = icon;
  item.badge = badge;
  item.data = itemData;
  item.bottom = true;
  return insertItem(std::move(item));
}

int NavigationSidebar::addChildItem(
  int parentIndex, const QString& text, const QIcon& icon, const QString& badge, const QVariant& itemData) {
  // Only entries can have children.
  if (parentIndex < 0 || parentIndex >= itemCount() || items()[parentIndex].kind != ItemKind::Entry) {
    return -1;
  }

  Item item;
  item.text = text;
  item.icon = icon;
  item.badge = badge;
  item.data = itemData;
  item.parent = parentIndex;
  item.bottom = items()[parentIndex].bottom;
  return insertItem(std::move(item));
}

int NavigationSidebar::addSection(const QString& title) {
  Item item;
  item.text = title;
  item.kind = ItemKind::Section;
  return insertItem(std::move(item));
}

int NavigationSidebar::addSeparator() {
  Item item;
  item.kind = ItemKind::Separator;
  return insertItem(std::move(item));
}

int NavigationSidebar::addBottomSeparator() {
  Item item;
  item.kind = ItemKind::Separator;
  item.bottom = true;
  return insertItem(std::move(item));
}

void NavigationSidebar::removeItem(int index) {
  if (index < 0 || index >= itemCount()) {
    return;
  }

  // Remove the children first, from the last one, so the indexes stay valid.
  for (auto i = itemCount() - 1; i > index; --i) {
    if (items()[i].parent == index) {
      removeItem(i);
    }
  }

  Q_EMIT itemAboutToBeRemoved(index);

  // If the current item is removed, select the closest selectable one.
  const auto current = currentIndex();
  auto target = current;
  if (current == index) {
    target = index - 1;
    while (target > -1 && !canSelectItem(target)) {
      --target;
    }
    if (target == -1) {
      target = index + 1;
      while (target < itemCount() && !canSelectItem(target)) {
        ++target;
      }
      target = target < itemCount() ? target : -1;
    }
    setCurrentIndex(target);
  }

  AbstractItemListWidget::removeItem(index);

  // The indexes after the removed item have shifted.
  for (auto& item : items()) {
    if (item.parent > index) {
      --item.parent;
    }
  }

  if (target > index) {
    setCurrentIndex(target - 1);
  }
  setFocusedIndex(currentIndex());
  relayout();
}

bool NavigationSidebar::isItemExpanded(int index) const {
  return index >= 0 && index < itemCount() && items()[index].expanded;
}

void NavigationSidebar::setItemExpanded(int index, bool expanded) {
  if (index < 0 || index >= itemCount() || items()[index].expanded == expanded) {
    return;
  }

  items()[index].expanded = expanded;

  // The focus must not stay on a row that is going to disappear.
  if (!expanded) {
    for (auto focused = focusedIndex(); focused > -1 && focused < itemCount(); focused = items()[focused].parent) {
      if (items()[focused].parent == index) {
        setFocusedIndex(index);
        break;
      }
    }
  }

  _expansionAnimation.stop();
  if (isVisible()) {
    _expansionProgress = 0.;
    _expansionAnimation.setDuration(style()->styleHint(QStyle::SH_Widget_Animation_Duration));
    _expansionAnimation.start();
  } else {
    for (auto& item : items()) {
      item.expansion = item.expanded ? 1. : 0.;
    }
  }

  relayout();
  Q_EMIT itemExpandedChanged(index);
}

bool NavigationSidebar::compact() const {
  return _compact;
}

void NavigationSidebar::setCompact(bool compact) {
  if (compact != _compact) {
    _compact = compact;
    _compactAnimation.stop();
    if (isVisible()) {
      _compactAnimation.setDuration(style()->styleHint(QStyle::SH_Widget_Animation_Duration));
      _compactAnimation.setStartValue(_compactProgress);
      _compactAnimation.setEndValue(_compact ? 1. : 0.);
      _compactAnimation.start();
    } else {
      _compactProgress = _compact ? 1. : 0.;
    }
    updateGeometry();
    relayout();
    Q_EMIT compactChanged();
  }
}

void NavigationSidebar::toggleCompact() {
  setCompact(!_compact);
}

int NavigationSidebar::expandedWidth() const {
  return _expandedWidth;
}

void NavigationSidebar::setExpandedWidth(int newWidth) {
  newWidth = std::max(0, newWidth);
  if (newWidth != _expandedWidth) {
    _expandedWidth = newWidth;
    updateGeometry();
    Q_EMIT expandedWidthChanged();
  }
}

bool NavigationSidebar::borderVisible() const {
  return _borderVisible;
}

void NavigationSidebar::setBorderVisible(bool visible) {
  if (visible != _borderVisible) {
    _borderVisible = visible;
    update();
    Q_EMIT borderVisibleChanged();
  }
}

QWidget* NavigationSidebar::headerWidget() const {
  return _headerWidget;
}

void NavigationSidebar::setHeaderWidget(QWidget* widget) {
  if (widget != _headerWidget) {
    delete _headerWidget;
    _headerWidget = widget;
    if (_headerWidget) {
      _headerWidget->setParent(this);
    }
    updateGeometry();
    relayout();
    Q_EMIT headerWidgetChanged();
  }
}

QWidget* NavigationSidebar::footerWidget() const {
  return _footerWidget;
}

void NavigationSidebar::setFooterWidget(QWidget* widget) {
  if (widget != _footerWidget) {
    delete _footerWidget;
    _footerWidget = widget;
    if (_footerWidget) {
      _footerWidget->setParent(this);
    }
    updateGeometry();
    relayout();
    Q_EMIT footerWidgetChanged();
  }
}

void NavigationSidebar::keyPressEvent(QKeyEvent* e) {
  const auto key = e->key();

  // Move the focus to the previous or next entry, in the order they are displayed.
  if (key == Qt::Key_Up || key == Qt::Key_Down) {
    const auto step = key == Qt::Key_Down ? 1 : -1;
    auto rows = visibleItems(false);
    const auto bottomRows = visibleItems(true);
    rows.insert(rows.end(), bottomRows.begin(), bottomRows.end());
    const auto rowCount = static_cast<int>(rows.size());
    const auto focusedRow = std::find(rows.begin(), rows.end(), focusedIndex());
    auto row = focusedRow != rows.end() ? static_cast<int>(std::distance(rows.begin(), focusedRow)) + step
                                        : (step > 0 ? 0 : rowCount - 1);
    for (; row >= 0 && row < rowCount; row += step) {
      if (items()[rows[row]].kind == ItemKind::Entry) {
        setFocusedIndex(rows[row]);
        ensureItemVisible(rows[row]);
        break;
      }
    }
    e->accept();
    return;
  }

  // Expand or collapse the focused entry, or go back to its parent.
  if (key == Qt::Key_Left || key == Qt::Key_Right) {
    const auto focused = focusedIndex();
    if (focused > -1 && focused < itemCount()) {
      const auto& item = items()[focused];
      if (key == Qt::Key_Right && hasChildItems(focused)) {
        setItemExpanded(focused, true);
      } else if (key == Qt::Key_Left && item.expanded) {
        setItemExpanded(focused, false);
      } else if (key == Qt::Key_Left && item.parent > -1) {
        setFocusedIndex(item.parent);
        ensureItemVisible(item.parent);
      }
    }
    e->accept();
    return;
  }

  NavigationBar::keyPressEvent(e);
}

void NavigationSidebar::mouseReleaseEvent(QMouseEvent* e) {
  // Clicking an entry that has children also expands or collapses it.
  const auto index = itemAtPos(e->pos());
  const auto shouldToggle =
    e->button() == Qt::LeftButton && index > -1 && index == pressedIndex() && hasChildItems(index);
  NavigationBar::mouseReleaseEvent(e);
  if (shouldToggle) {
    setItemExpanded(index, !items()[index].expanded);
  }
}

void NavigationSidebar::wheelEvent(QWheelEvent* e) {
  const auto pixelDelta = e->pixelDelta().y();
  const auto delta =
    pixelDelta != 0 ? pixelDelta : e->angleDelta().y() * wheelNotchRows * getItemMinimumHeight() / wheelNotchDelta;
  const auto previousOffset = _scrollOffset;
  _scrollOffset -= delta;
  relayout();
  setHoveredIndex(itemAtPos(e->position().toPoint()));

  // Let the parent scroll if we are already at the limit.
  e->setAccepted(_scrollOffset != previousOffset);
}

void NavigationSidebar::paintEvent(QPaintEvent*) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setRenderHint(QPainter::SmoothPixmapTransform, true);

  // Background.
  drawBackground(p);

  // Top items are clipped to the scrollable viewport, bottom items to the area below it.
  const auto viewportBottom = _viewportRect.y() + _viewportRect.height();
  const auto bottomRect = QRect{ 0, viewportBottom, width(), std::max(0, height() - viewportBottom) };
  for (const auto isBottom : { false, true }) {
    p.save();
    p.setClipRect(isBottom ? bottomRect : _viewportRect);

    // Items backgrounds.
    for (const auto& item : items()) {
      if (item.bottom == isBottom) {
        drawItemBackground(p, item);
      }
    }

    // Current item indicator (drawn above items backgrounds).
    drawCurrentItemIndicator(p);

    // Items foregrounds.
    for (const auto& item : items()) {
      if (item.bottom == isBottom) {
        drawItemForeground(p, item);
      }
    }
    p.restore();
  }
}

bool NavigationSidebar::event(QEvent* e) {
  const auto type = e->type();
  if (type == QEvent::Type::LayoutRequest || type == QEvent::Type::ContentsRectChange) {
    // The size hint of the header or the footer may have changed, or the top margin reserved for a title bar.
    updateGeometry();
    relayout();
  } else if (type == QEvent::Type::ToolTip && _compact) {
    // The texts are not visible in compact mode: show them in a tooltip.
    const auto* helpEvent = static_cast<QHelpEvent*>(e);
    const auto index = itemAtPos(helpEvent->pos());
    if (index > -1 && !items()[index].text.isEmpty()) {
      QToolTip::showText(helpEvent->globalPos(), items()[index].text, this, items()[index].rect);
    } else {
      QToolTip::hideText();
    }
    return true;
  }
  return NavigationBar::event(e);
}

int NavigationSidebar::itemAtPos(const QPoint& pos) const {
  for (auto i = 0; i < itemCount(); ++i) {
    const auto& item = items()[i];
    if (item.kind != ItemKind::Entry || item.rect.isEmpty()) {
      continue;
    }

    // Top items scrolled out of the viewport can't be hit.
    const auto& area = item.bottom ? rect() : _viewportRect;
    if (area.contains(pos) && pos.y() >= item.rect.top() && pos.y() <= item.rect.bottom()) {
      return i;
    }
  }
  return -1;
}

void NavigationSidebar::updateItemsSizeHints() {
  // Get values identical for every item.
  const auto itemPadding = getItemPadding();
  const auto verticalSpacing = style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
  const auto textH = QFontMetrics(labelFont(), this).height();
  const auto sectionH = QFontMetrics(sectionFont(), this).height() + verticalSpacing * 3 / 2;
  const auto badgePadding = getBadgePadding();
  const auto badgeH = QFontMetrics(badgeFont(), this).height() + badgePadding.top() + badgePadding.bottom();
  const auto entryH = std::max({ textH, iconSize().height(), badgeH }) + itemPadding.top() + itemPadding.bottom();

  // Only the heights matter: the width is the sidebar width.
  for (auto& item : items()) {
    switch (item.kind) {
      case ItemKind::Entry:
        item.sizeHint = QSize{ 0, std::max(entryH, getItemMinimumHeight()) };
        break;
      case ItemKind::Section:
        // A section shrinks until it disappears in compact mode.
        item.sizeHint = QSize{ 0, qRound(sectionH * (1. - _compactProgress)) };
        break;
      case ItemKind::Separator:
        item.sizeHint = QSize{ 0, verticalSpacing };
        break;
    }
  }
}

void NavigationSidebar::updateItemRects() {
  // Make a first pass to ask every item how much space they need.
  updateItemsSizeHints();

  // Get constant values.
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  const auto padding = getPadding();
  const auto gap = style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
  const auto borderWidth = qlementineStyle ? qlementineStyle->theme().borderWidth : 1;
  const auto contentX = padding.left();
  const auto contentW = std::max(0, width() - padding.left() - padding.right());

  // Rows that are not displayed have an empty rect.
  for (auto& item : items()) {
    item.rect = QRect{};
  }

  const auto placeRows = [&](const std::vector<int>& rows, int y) {
    for (auto i = 0; i < static_cast<int>(rows.size()); ++i) {
      const auto index = rows[i];
      y += i > 0 ? rowSpacing(index) : 0;
      items()[index].rect = QRect{ contentX, y, contentW, rowHeight(index) };
      y += rowHeight(index);
    }
  };

  // Header: only displayed when the sidebar is not compact.
  auto top = padding.top();
  const auto headerSlotH = slotHeight(_headerWidget);
  if (_headerWidget) {
    _headerWidget->setGeometry(contentX, top, contentW, _headerWidget->sizeHint().height());
    _headerWidget->setVisible(_compactProgress == 0.);
  }
  top += headerSlotH;

  // Footer: same.
  auto bottom = height() - padding.bottom();
  const auto footerSlotH = slotHeight(_footerWidget);
  if (_footerWidget) {
    const auto footerH = _footerWidget->sizeHint().height();
    _footerWidget->setGeometry(contentX, bottom - footerH, contentW, footerH);
    _footerWidget->setVisible(_compactProgress == 0.);
  }
  bottom -= footerSlotH;

  // Bottom items are pinned above the footer.
  const auto bottomRows = visibleItems(true);
  if (!bottomRows.empty()) {
    const auto bottomRowsH = rowsHeight(bottomRows);
    placeRows(bottomRows, bottom - bottomRowsH);
    bottom -= bottomRowsH + gap;
  }

  // Top items take the remaining space, and scroll if they don't fit.
  const auto topRows = visibleItems(false);
  const auto maxScrollOffset = std::max(0, rowsHeight(topRows) - std::max(0, bottom - top));
  _viewportRect = QRect{ 0, top, width(), std::max(0, bottom - top) };
  _scrollOffset = std::clamp(_scrollOffset, 0, maxScrollOffset);
  placeRows(topRows, top - _scrollOffset);

  // Scroll bar.
  const auto scrollBarW = _scrollBar->sizeHint().width();
  _scrollBar->setGeometry(width() - borderWidth - scrollBarW, _viewportRect.y(), scrollBarW, _viewportRect.height());
  {
    const QSignalBlocker blocker(_scrollBar);
    _scrollBar->setRange(0, maxScrollOffset);
    _scrollBar->setPageStep(_viewportRect.height());
    _scrollBar->setSingleStep(getItemMinimumHeight());
    _scrollBar->setValue(_scrollOffset);
  }
  _scrollBar->setVisible(maxScrollOffset > 0);
}

QRect NavigationSidebar::getCurrentItemRect() const {
  const auto current = currentIndex();
  if (current < 0 || current >= itemCount()) {
    const auto padding = getPadding();
    return QRect{ padding.left(), padding.top(), 0, 0 };
  }

  // While a parent reveals or hides the current item, the indicator moves between the item
  // and the closest ancestor that is fully visible.
  auto anchor = current;
  while (rowScale(anchor) < 1.) {
    anchor = items()[anchor].parent;
  }

  const auto scale = rowScale(current);
  const auto& anchorRect = items()[anchor].rect;
  if (scale <= 0.) {
    return anchorRect;
  }

  const auto& currentRect = items()[current].rect;
  const auto interpolate = [scale](int from, int to) {
    return from + qRound((to - from) * scale);
  };
  return QRect{ interpolate(anchorRect.x(), currentRect.x()), interpolate(anchorRect.y(), currentRect.y()),
    interpolate(anchorRect.width(), currentRect.width()), interpolate(anchorRect.height(), currentRect.height()) };
}

bool NavigationSidebar::canSelectItem(int index) const {
  return index >= 0 && index < itemCount() && items()[index].kind == ItemKind::Entry;
}

const QColor& NavigationSidebar::getItemFgColor(MouseState mouse, bool selected, const Theme& theme) const {
  if (!selected) {
    return NavigationBar::getItemFgColor(mouse, selected, theme);
  }
  return mouse == MouseState::Disabled ? theme.primaryColorDisabled : theme.primaryColor;
}

const QColor& NavigationSidebar::getItemBadgeBgColor(MouseState mouse, bool selected, const Theme& theme) const {
  if (!selected) {
    return NavigationBar::getItemBadgeBgColor(mouse, selected, theme);
  }
  return mouse == MouseState::Disabled ? theme.primaryAlternativeColorDisabled : theme.primaryAlternativeColor;
}

const QColor& NavigationSidebar::getItemBadgeFgColor(MouseState mouse, bool selected, const Theme& theme) const {
  if (!selected) {
    return NavigationBar::getItemBadgeFgColor(mouse, selected, theme);
  }
  return mouse == MouseState::Disabled ? theme.primaryColorForegroundDisabled : theme.primaryColorForeground;
}

QMargins NavigationSidebar::getItemPadding() const {
  const auto* style = this->style();
  const auto horizontal = style->pixelMetric(QStyle::PM_LayoutLeftMargin) / 2;
  const auto vertical = style->pixelMetric(QStyle::PM_LayoutTopMargin) / 4;
  return QMargins{ horizontal, vertical, horizontal, vertical };
}

double NavigationSidebar::getItemRadius() const {
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  return qlementineStyle ? qlementineStyle->theme().borderRadius : 0.;
}

QMargins NavigationSidebar::getPadding() const {
  const auto* style = this->style();
  const auto left = style->pixelMetric(QStyle::PM_LayoutLeftMargin) / 2;
  const auto top = style->pixelMetric(QStyle::PM_LayoutTopMargin) / 2 + contentsMargins().top();
  const auto right = style->pixelMetric(QStyle::PM_LayoutRightMargin) / 2;
  const auto bottom = style->pixelMetric(QStyle::PM_LayoutBottomMargin) / 2;
  return QMargins{ left, top, right, bottom };
}

int NavigationSidebar::getSpacing() const {
  return style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing) / 4;
}

int NavigationSidebar::getItemMinimumHeight() const {
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  return qlementineStyle ? qlementineStyle->theme().controlHeightLarge + qlementineStyle->theme().spacing / 2 : 32;
}

void NavigationSidebar::drawBackground(QPainter& p) const {
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  const auto borderWidth = qlementineStyle ? qlementineStyle->theme().borderWidth : 1;
  const auto& borderColor = qlementineStyle ? qlementineStyle->theme().borderColor : palette().color(QPalette::Mid);

  // The border is on the side of the content.
  p.fillRect(rect(), AbstractItemListWidget::getBgColor());
  if (_borderVisible) {
    const auto top = contentsMargins().top();
    p.fillRect(QRect{ width() - borderWidth, top, borderWidth, height() - top }, borderColor);
  }
}

void NavigationSidebar::drawItemBackground(QPainter& p, const Item& item) const {
  if (item.kind != ItemKind::Entry || item.rect.isEmpty()) {
    return;
  }

  const auto radius = getItemRadius();
  p.setPen(Qt::NoPen);
  p.setBrush(item.bgColorAnimation->currentValue().value<QColor>());
  p.drawRoundedRect(item.rect, radius, radius);
}

void NavigationSidebar::drawItemForeground(QPainter& p, const Item& item) const {
  if (item.rect.isEmpty()) {
    return;
  }

  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  const auto& rect = item.rect;
  const auto itemPadding = getItemPadding();
  const auto itemSpacing = getItemSpacing();
  const auto expandedOpacity = 1. - _compactProgress;

  // Separator.
  if (item.kind == ItemKind::Separator) {
    const auto lineThickness = qlementineStyle ? qlementineStyle->theme().borderWidth : 1;
    const auto& lineColor = qlementineStyle ? qlementineStyle->theme().borderColor : palette().color(QPalette::Mid);
    p.fillRect(
      QRect{ rect.x(), rect.y() + (rect.height() - lineThickness) / 2, rect.width(), lineThickness }, lineColor);
    return;
  }

  // Section.
  if (item.kind == ItemKind::Section) {
    const auto& textColor = qlementineStyle
                              ? (isEnabled() ? qlementineStyle->theme().secondaryAlternativeColor
                                             : qlementineStyle->theme().secondaryAlternativeColorDisabled)
                              : palette().color(isEnabled() ? QPalette::Normal : QPalette::Disabled, QPalette::Text);
    const auto verticalSpacing = style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
    const auto font = sectionFont();
    const auto textRect =
      rect.marginsRemoved(QMargins{ itemPadding.left(), verticalSpacing, itemPadding.right(), verticalSpacing / 2 });
    p.setOpacity(expandedOpacity);
    p.setFont(font);
    p.setPen(textColor);
    p.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextSingleLine,
      QFontMetrics(font, this).elidedText(item.text, Qt::ElideRight, textRect.width(), Qt::TextSingleLine));
    p.setOpacity(1.);
    return;
  }

  // Entry.
  const auto index = static_cast<int>(&item - items().data());
  const auto itemFgColor = item.fgColorAnimation->currentValue().value<QColor>();
  const auto& iconSize = this->iconSize();
  const auto hasIcon = !item.icon.isNull();
  const auto hasBadge = !item.badge.isEmpty();

  // Children fade and are clipped while their parent expands or collapses.
  const auto rowOpacity = rowScale(index);
  p.save();
  p.setClipRect(rect, Qt::IntersectClip);
  p.setOpacity(rowOpacity);

  // Children are indented, except in compact mode.
  auto depth = 0;
  for (auto parent = item.parent; parent > -1; parent = items()[parent].parent) {
    ++depth;
  }
  const auto indent = static_cast<int>(depth * style()->pixelMetric(QStyle::PM_LayoutLeftMargin) * expandedOpacity);

  // When the sidebar is forced wider than it needs (e.g. to contain the window controls), center the content.
  const auto extraWidth = std::max(0, width() - naturalWidth()) / 2;
  const auto contentX = rect.x() + itemPadding.left() + indent + extraWidth;
  const auto contentRight = rect.x() + rect.width() - itemPadding.right() - extraWidth;
  auto textLeft = contentX;
  auto textRight = contentRight;

  // Icon.
  auto iconRect = QRect{};
  if (hasIcon) {
    iconRect = QRect{ QPoint{ contentX, rect.y() + (rect.height() - iconSize.height()) / 2 }, iconSize };
    const auto pixmap = item.icon.pixmap(iconSize.height(), QIcon::Normal, QIcon::Off);
    p.drawPixmap(iconRect, colorizePixmap(pixmap, itemFgColor));
    textLeft += iconSize.width() + itemSpacing;
  }

  // Chevron, rotated according to the expansion.
  if (hasChildItems(index)) {
    const auto chevronRect =
      QRect{ QPoint{ contentRight - iconSize.width(), rect.y() + (rect.height() - iconSize.height()) / 2 }, iconSize };
    const auto chevronCenter = QRectF{ chevronRect }.center();
    p.save();
    p.setOpacity(rowOpacity * expandedOpacity);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen{ itemFgColor, chevronPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin });
    p.translate(chevronCenter);
    p.rotate(90. * item.expansion);
    p.translate(-chevronCenter);
    drawArrowRight(chevronRect, &p);
    p.restore();
    textRight -= iconSize.width() + itemSpacing;
  }

  // Badge: a pill when expanded, a dot on the icon when compact.
  if (hasBadge) {
    const auto badgeFm = QFontMetrics{ badgeFont(), this };
    const auto badgePadding = getBadgePadding();
    const auto badgeH = badgeFm.height() + badgePadding.top() + badgePadding.bottom();
    const auto badgeW =
      std::max(badgeH, badgePadding.left() + badgePadding.right() + qlementine::textWidth(badgeFm, item.badge));
    const auto badgeRect = QRect{ textRight - badgeW, rect.y() + (rect.height() - badgeH) / 2, badgeW, badgeH };
    const auto badgeBgColor = item.badgeBgAnimation->currentValue().value<QColor>();
    const auto badgeFgColor = item.badgeFgAnimation->currentValue().value<QColor>();

    p.setOpacity(rowOpacity * expandedOpacity);
    p.setPen(Qt::NoPen);
    p.setBrush(badgeBgColor);
    p.drawRoundedRect(badgeRect, badgeH / 2., badgeH / 2.);
    p.setFont(badgeFont());
    p.setPen(badgeFgColor);
    p.drawText(
      badgeRect.marginsRemoved(badgePadding), Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextSingleLine, item.badge);

    if (hasIcon) {
      p.setOpacity(rowOpacity * _compactProgress);
      p.setPen(Qt::NoPen);
      p.setBrush(badgeBgColor);
      p.drawEllipse(QPointF{ iconRect.right() + 1., static_cast<qreal>(iconRect.top()) }, badgeH / 4., badgeH / 4.);
    }

    textRight -= badgeW + itemSpacing;
  }

  // Text, elided and faded out in compact mode.
  if (!item.text.isEmpty() && textRight > textLeft) {
    const auto textRect = QRect{ textLeft, rect.y(), textRight - textLeft, rect.height() };
    p.setOpacity(rowOpacity * expandedOpacity);
    p.setFont(labelFont());
    p.setPen(itemFgColor);
    p.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextSingleLine,
      QFontMetrics(labelFont(), this).elidedText(item.text, Qt::ElideRight, textRect.width(), Qt::TextSingleLine));
  }

  p.restore();
}

void NavigationSidebar::drawCurrentItemIndicator(QPainter& p) const {
  const auto currentItemRect = getAnimatedCurrentItemRect();
  if (currentItemRect.isEmpty()) {
    return;
  }

  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  const auto radius = getItemRadius();
  const auto barW = (qlementineStyle ? qlementineStyle->theme().borderWidth : 1) * 3;
  const auto barH = currentItemRect.height() / 2;
  const auto barColor = getCurrentItemIndicatorColor();
  auto fillColor = barColor;
  fillColor.setAlphaF(indicatorFillOpacity);

  // Tinted background.
  p.setPen(Qt::NoPen);
  p.setBrush(fillColor);
  p.drawRoundedRect(currentItemRect, radius, radius);

  // Accent bar on the left.
  p.setBrush(barColor);
  p.drawRoundedRect(QRectF{ static_cast<qreal>(currentItemRect.x() + barW),
                      currentItemRect.y() + (currentItemRect.height() - barH) / 2., static_cast<qreal>(barW),
                      static_cast<qreal>(barH) },
    barW / 2., barW / 2.);
}

QFont NavigationSidebar::labelFont() const {
  return font();
}

QFont NavigationSidebar::sectionFont() const {
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  return qlementineStyle ? qlementineStyle->fontForTextRole(TextRole::Caption) : font();
}

bool NavigationSidebar::hasChildItems(int index) const {
  return std::any_of(items().cbegin(), items().cend(), [index](const auto& item) {
    return item.parent == index;
  });
}

std::vector<int> NavigationSidebar::visibleItems(bool bottom, int parent) const {
  std::vector<int> rows;
  for (auto i = 0; i < itemCount(); ++i) {
    const auto& item = items()[i];
    if (item.parent != parent || (parent == -1 && item.bottom != bottom)) {
      continue;
    }

    // Children stay in the list as long as they are not totally collapsed.
    rows.push_back(i);
    if (item.expansion > 0.) {
      const auto children = visibleItems(bottom, i);
      rows.insert(rows.end(), children.begin(), children.end());
    }
  }
  return rows;
}

double NavigationSidebar::rowScale(int index) const {
  auto scale = 1.;
  for (auto parent = items()[index].parent; parent > -1; parent = items()[parent].parent) {
    scale *= items()[parent].expansion;
  }
  return scale;
}

int NavigationSidebar::rowHeight(int index) const {
  return qRound(items()[index].sizeHint.height() * rowScale(index));
}

int NavigationSidebar::rowSpacing(int index) const {
  return qRound(getSpacing() * rowScale(index));
}

int NavigationSidebar::rowsHeight(const std::vector<int>& rows) const {
  auto total = 0;
  for (auto i = 0; i < static_cast<int>(rows.size()); ++i) {
    total += rowHeight(rows[i]) + (i > 0 ? rowSpacing(rows[i]) : 0);
  }
  return total;
}

int NavigationSidebar::slotHeight(const QWidget* widget) const {
  if (!widget) {
    return 0;
  }

  // The space taken by the header and the footer shrinks with the compact animation.
  widget->ensurePolished();
  const auto gap = style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
  return qRound((widget->sizeHint().height() + gap) * (1. - _compactProgress));
}

void NavigationSidebar::ensureItemVisible(int index) {
  if (index < 0 || index >= itemCount()) {
    return;
  }

  const auto& itemRect = items()[index].rect;
  if (items()[index].bottom || itemRect.isEmpty()) {
    return;
  }

  if (itemRect.top() < _viewportRect.top()) {
    _scrollOffset -= _viewportRect.top() - itemRect.top();
  } else if (itemRect.bottom() > _viewportRect.bottom()) {
    _scrollOffset += itemRect.bottom() - _viewportRect.bottom();
  } else {
    return;
  }
  relayout();
}

void NavigationSidebar::relayout() {
  updateItemRects();
  updateCurrentIndexAnimation(true);
  updateFocusFrame();
  update();
}
} // namespace oclero::qlementine
