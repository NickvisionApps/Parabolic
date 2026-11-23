// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/AbstractItemListWidget.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/widgets/RoundedFocusFrame.hpp>
#include <oclero/qlementine/utils/ImageUtils.hpp>
#include <oclero/qlementine/utils/FontUtils.hpp>

#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>

#include <cmath>

namespace oclero::qlementine {
constexpr auto animationFactor = 1;

AbstractItemListWidget::AbstractItemListWidget(QWidget* parent)
  : QWidget(parent) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  const auto* style = this->style();
  const auto iconExtent = style->pixelMetric(QStyle::PM_SmallIconSize);
  setIconSize({ iconExtent, iconExtent });
  setMouseTracking(true);
  setFocusPolicy(Qt::FocusPolicy::TabFocus);

  // Focus frame.
  _focusFrame = new RoundedFocusFrame(this);
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style);
  _focusFrame->setRadiuses(RadiusesF{ qlementineStyle ? qlementineStyle->theme().borderRadius : 0. });
  _focusFrame->setWidget(this);

  // Badge.
  _badgeFont = qlementineStyle ? qlementineStyle->fontForTextRole(qlementine::TextRole::Caption) : this->font();
  _badgeFont.setBold(true);
}

AbstractItemListWidget::~AbstractItemListWidget() = default;

int AbstractItemListWidget::itemCount() const {
  return static_cast<int>(_items.size());
}

int AbstractItemListWidget::currentIndex() const {
  return _currentIndex;
}

void AbstractItemListWidget::setCurrentIndex(int index) {
  index = index < 0 || index > itemCount() - 1 ? -1 : index;
  if (index != -1 && !canSelectItem(index)) {
    return;
  }
  if (index != _currentIndex) {
    const auto oldData = currentData();
    _currentIndex = index;
    _focusedIndex = index;
    update();
    updateCurrentIndexAnimation();
    updateItemsAnimations();
    Q_EMIT currentIndexChanged();
    if (currentData() != oldData) {
      Q_EMIT currentDataChanged();
    }
  }
}

QVariant AbstractItemListWidget::currentData() const {
  if (_currentIndex > -1 && _currentIndex < itemCount()) {
    return _items.at(_currentIndex).data;
  }
  return {};
}

void AbstractItemListWidget::setCurrentData(const QVariant& currentData) {
  if (!currentData.isValid())
    return;

  const auto index = findItemIndex(currentData);
  if (index > -1 && index < itemCount()) {
    setCurrentIndex(index);
  }
}

int AbstractItemListWidget::addItem(
  const QString& text, const QIcon& icon, const QString& badge, const QVariant& itemData) {
  Item item;
  item.text = text;
  item.icon = icon;
  item.badge = badge;
  item.data = itemData;
  return insertItem(std::move(item));
}

int AbstractItemListWidget::insertItem(Item newItem) {
  const auto* style = this->style();
  const auto animDuration = style->styleHint(QStyle::SH_Widget_Animation_Duration) * animationFactor;

  auto* bgColorAnimation = new QVariantAnimation(this);
  QObject::connect(bgColorAnimation, &QVariantAnimation::valueChanged, this, [this]() {
    update();
  });

  auto* fgColorAnimation = new QVariantAnimation(this);
  QObject::connect(fgColorAnimation, &QVariantAnimation::valueChanged, this, [this]() {
    update();
  });

  auto* badgeBgColorAnimation = new QVariantAnimation(this);
  QObject::connect(badgeBgColorAnimation, &QVariantAnimation::valueChanged, this, [this]() {
    update();
  });

  auto* badgeFgColorAnimation = new QVariantAnimation(this);
  QObject::connect(badgeFgColorAnimation, &QVariantAnimation::valueChanged, this, [this]() {
    update();
  });

  newItem.bgColorAnimation = bgColorAnimation;
  newItem.fgColorAnimation = fgColorAnimation;
  newItem.badgeBgAnimation = badgeBgColorAnimation;
  newItem.badgeFgAnimation = badgeFgColorAnimation;
  _items.emplace_back(std::move(newItem));

  if (_currentIndex == -1) {
    const auto newIndex = itemCount() - 1;
    _currentIndex = canSelectItem(0) ? 0 : (canSelectItem(newIndex) ? newIndex : -1);
    _focusedIndex = _currentIndex;
  }

  update();
  updateGeometry();
  updateItemRects();
  updateItemsAnimations();
  updateCurrentIndexAnimation();
  Q_EMIT itemCountChanged();
  Q_EMIT currentIndexChanged();

  const auto [bgColor, fgColor, badgeBgColor, badgeFgColor] = getItemBgAndFgColor(itemCount(), MouseState::Normal);
  bgColorAnimation->setDuration(animDuration);
  bgColorAnimation->setStartValue(QVariant::fromValue<QColor>(bgColor));
  bgColorAnimation->setEndValue(QVariant::fromValue<QColor>(bgColor));
  bgColorAnimation->setEasingCurve(QEasingCurve::Type::InOutCubic);

  fgColorAnimation->setStartValue(QVariant::fromValue<QColor>(fgColor));
  fgColorAnimation->setEndValue(QVariant::fromValue<QColor>(fgColor));
  fgColorAnimation->setDuration(animDuration);
  fgColorAnimation->setEasingCurve(QEasingCurve::Type::InOutCubic);

  badgeBgColorAnimation->setStartValue(QVariant::fromValue<QColor>(badgeBgColor));
  badgeBgColorAnimation->setEndValue(QVariant::fromValue<QColor>(badgeBgColor));
  badgeBgColorAnimation->setDuration(animDuration);
  badgeBgColorAnimation->setEasingCurve(QEasingCurve::Type::InOutCubic);

  badgeFgColorAnimation->setStartValue(QVariant::fromValue<QColor>(badgeFgColor));
  badgeFgColorAnimation->setEndValue(QVariant::fromValue<QColor>(badgeFgColor));
  badgeFgColorAnimation->setDuration(animDuration);
  badgeFgColorAnimation->setEasingCurve(QEasingCurve::Type::InOutCubic);

  return itemCount() - 1;
}

void AbstractItemListWidget::removeItem(int index) {
  if (index >= 0 && index < itemCount()) {
    auto& item = _items[index];
    delete item.bgColorAnimation;
    delete item.fgColorAnimation;
    delete item.badgeBgAnimation;
    delete item.badgeFgAnimation;

    _items.erase(_items.begin() + index);

    updateGeometry();
    update();

    if (_currentIndex == index) {
      setCurrentIndex(std::max(-1, index - 1));
    }

    Q_EMIT itemCountChanged();
  }
}

int AbstractItemListWidget::findItemIndex(const QVariant& itemData) const {
  if (!itemData.isValid())
    return -1;

  const auto it = std::find_if(_items.cbegin(), _items.cend(), [&itemData](const auto& item) {
    return item.data == itemData;
  });
  return it != _items.cend() ? static_cast<int>(std::distance(_items.cbegin(), it)) : -1;
}

void AbstractItemListWidget::setItemData(int index, const QVariant& itemData) {
  if (index >= 0 && index < itemCount()) {
    _items[index].data = itemData;
  }
}

QVariant AbstractItemListWidget::getItemData(int index) const {
  if (index >= 0 && index < itemCount()) {
    return _items.at(index).data;
  }
  return {};
}

void AbstractItemListWidget::setItemText(int index, const QString& text) {
  if (index >= 0 && index < itemCount()) {
    _items[index].text = text;
    updateGeometry();
    update();
  }
}

QString AbstractItemListWidget::getItemText(int index) const {
  if (index >= 0 && index < itemCount()) {
    return _items.at(index).text;
  }
  return {};
}

void AbstractItemListWidget::setItemIcon(int index, const QIcon& icon) {
  if (index >= 0 && index < itemCount()) {
    _items[index].icon = icon;
    updateGeometry();
    update();
  }
}

QIcon AbstractItemListWidget::getItemIcon(int index) const {
  if (index >= 0 && index < itemCount()) {
    return _items.at(index).icon;
  }
  return {};
}

void AbstractItemListWidget::setItemBadge(int index, const QString& badge) {
  if (index >= 0 && index < itemCount()) {
    _items[index].badge = badge;
    updateGeometry();
    update();
  }
}

QString AbstractItemListWidget::getItemBadge(int index) const {
  if (index >= 0 && index < itemCount()) {
    return _items.at(index).badge;
  }
  return {};
}

void AbstractItemListWidget::setItemEnabled(int index, bool enabled) {
  if (index >= 0 && index < itemCount()) {
    _items[index].enabled = enabled;
    updateItemsAnimations();
    update();
  }
}

bool AbstractItemListWidget::isItemEnabled(int index) const {
  if (index >= 0 && index < itemCount()) {
    return _items.at(index).enabled;
  }
  return false;
}

void AbstractItemListWidget::moveToNextItem() {
  setCurrentIndex(std::min(_currentIndex + 1, itemCount() - 1));
}

void AbstractItemListWidget::moveToPreviousItem() {
  setCurrentIndex(std::max(_currentIndex - 1, 0));
}

const QSize& AbstractItemListWidget::iconSize() const {
  return _iconSize;
}

void AbstractItemListWidget::setIconSize(const QSize& size) {
  if (size != _iconSize) {
    _iconSize = size;
    updateGeometry();
    update();
    Q_EMIT iconSizeChanged();
  }
}


bool AbstractItemListWidget::itemsShouldExpand() const {
  return _itemsShouldExpand;
}

void AbstractItemListWidget::setItemsShouldExpand(bool expand) {
  if (expand != _itemsShouldExpand) {
    _itemsShouldExpand = expand;
    updateItemRects();
    updateCurrentIndexAnimation(true);
    _focusFrame->update();
    _focusFrame->updateGeometry();
    Q_EMIT itemsShouldExpandChanged();
  }
}

int AbstractItemListWidget::itemAtPos(const QPoint& pos) const {
  const auto padding = style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing) / 2;
  const auto itemSpacing = style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing) / 2;
  const auto itemCount = this->itemCount();
  for (auto itemIndex = 0; itemIndex < itemCount; ++itemIndex) {
    const auto& itemRect = _items[itemIndex].rect;
    const auto leftPadding = itemIndex == 0 ? padding : itemSpacing / 2;
    const auto rightPadding = itemIndex == 0 ? padding : itemSpacing / 2;
    if (hitTestItemRect(pos, itemRect, leftPadding, rightPadding)) {
      return itemIndex;
    }
  }
  return -1;
}

bool AbstractItemListWidget::hitTestItemRect(
  const QPoint& pos, const QRect& itemRect, int leftPadding, int rightPadding) const {
  // To make it easier for the user to click, we take into account the padding around the item.
  return pos.x() >= itemRect.x() - leftPadding // Padding on the left.
         && pos.x() <= itemRect.x() + itemRect.width() + rightPadding // Padding on the right.
         && pos.y() >= 0 && pos.y() <= height(); // Widget full height.
}

void AbstractItemListWidget::keyPressEvent(QKeyEvent* e) {
  QWidget::keyPressEvent(e);
  const auto key = e->key();
  if (key == Qt::Key_Right) {
    const auto next = std::min(itemCount() - 1, _focusedIndex + 1);
    setFocusedIndex(next);
    e->accept();
  } else if (key == Qt::Key_Left) {
    const auto next = std::max(0, _focusedIndex - 1);
    setFocusedIndex(next);
    e->accept();
  } else if (!e->isAutoRepeat() && (key == Qt::Key_Space || key == Qt::Key_Enter || key == Qt::Key_Return)) {
    setPressedIndex(_focusedIndex);
  }
}

void AbstractItemListWidget::keyReleaseEvent(QKeyEvent* e) {
  QWidget::keyReleaseEvent(e);
  if (e->isAutoRepeat())
    return;

  setPressedIndex(-1);
  const auto key = e->key();
  if (!e->isAutoRepeat() && (key == Qt::Key_Space || key == Qt::Key_Enter || key == Qt::Key_Return)) {
    setCurrentIndex(_focusedIndex);
  }
}

void AbstractItemListWidget::enterEvent(QEnterEvent* e) {
  QWidget::enterEvent(e);
  update();
}

void AbstractItemListWidget::leaveEvent(QEvent* e) {
  QWidget::leaveEvent(e);
  setHoveredIndex(-1, false);
  setPressedIndex(-1, false);
  updateItemsAnimations();
}

void AbstractItemListWidget::mousePressEvent(QMouseEvent* e) {
  QWidget::mousePressEvent(e);
  const auto index = itemAtPos(e->pos());
  if (isItemEnabled(index) && (e->button() == Qt::LeftButton)) {
    setPressedIndex(index);
  }
}

void AbstractItemListWidget::mouseReleaseEvent(QMouseEvent* e) {
  QWidget::mouseReleaseEvent(e);
  if (e->button() == Qt::LeftButton) {
    const auto index = itemAtPos(e->pos());
    const auto pressedBackup = _pressedIndex;
    // Update item states.
    setPressedIndex(-1, false);
    if (index == -1) {
      setHoveredIndex(index, false);
    }
    updateItemsAnimations();

    // Current index.
    if (index == pressedBackup && index != -1) {
      setCurrentIndex(index);
    }
  }
}

void AbstractItemListWidget::mouseMoveEvent(QMouseEvent* e) {
  QWidget::mouseMoveEvent(e);
  const auto index = itemAtPos(e->pos());
  setHoveredIndex(index);
  if (_pressedIndex != index && _pressedIndex != -1) {
    setPressedIndex(index);
  }
}

QSize AbstractItemListWidget::sizeHint() const {
  // Make a first pass to ask every item how much space they need if space was unlimited.
  const_cast<AbstractItemListWidget*>(this)->updateItemsSizeHints();

  // Get constant values from style.
  const auto padding = getPadding();
  const auto spacing = getSpacing();

  // Compute necessary size.
  auto neededW = 0;
  auto neededH = getItemMinimumHeight();
  for (const auto& item : _items) {
    neededW += item.sizeHint.width();
    neededH = std::max(neededH, item.sizeHint.height());
  }
  const auto itemCount = this->itemCount();
  const auto spacings = itemCount > 1 ? (itemCount - 1) * spacing : 0;
  neededW += spacings + padding.left() + padding.right();
  neededH += padding.top() + padding.bottom();

  return { neededW, neededH };
}

void AbstractItemListWidget::updateItemsSizeHints() {
  // Get values identical for every item.
  const auto itemPadding = getItemPadding();
  const auto itemSpacing = getItemSpacing();
  const auto fm = QFontMetrics(labelFont(), this);
  const auto iconH = _iconSize.height();
  const auto iconW = _iconSize.width();
  const auto textH = fm.height();
  const auto badgeFm = QFontMetrics(_badgeFont, this);
  const auto badgePadding = getBadgePadding();
  const auto badgeH = badgeFm.height() + badgePadding.top() + badgePadding.bottom();
  const auto itemH = std::max({ textH, iconH, badgeH }) + itemPadding.top() + itemPadding.bottom();

  // Compute necessary size for every item.
  for (auto& item : _items) {
    auto itemW = itemPadding.left() + itemPadding.right();
    auto elementCount = 0;

    if (!item.icon.isNull()) {
      itemW += iconW;
      ++elementCount;
    }

    if (!item.text.isEmpty()) {
      itemW += qlementine::textWidth(fm, item.text);
      ++elementCount;
    }

    if (!item.badge.isEmpty()) {
      const auto badgeW =
        std::max(badgeH, badgePadding.left() + badgePadding.right() + qlementine::textWidth(badgeFm, item.badge));
      itemW += badgeW;
      ++elementCount;
    }

    // Spacings.
    itemW += elementCount > 1 ? (elementCount - 1) * itemSpacing : 0;

    item.sizeHint = QSize{ itemW, itemH };
  }
}

void AbstractItemListWidget::updateItemRects() {
  // Make a first pass to ask every item how much space they need if space was unlimited.
  updateItemsSizeHints();

  // Get constant values.
  const auto padding = getPadding();
  const auto spacing = getSpacing();

  // Check if items can take the space they want.
  const auto totalRect = rect();
  const auto availableItemsRect = totalRect.marginsRemoved(padding);

  // Compute theoritical necessary width to display items in their necessary size.
  auto itemsNecessaryW = 0;
  for (const auto& item : _items) {
    itemsNecessaryW += item.sizeHint.width();
  }
  const auto itemCount = this->itemCount();
  const auto spacings = itemCount > 1 ? (itemCount - 1) * spacing : 0;
  itemsNecessaryW += spacings;

  const auto itemY = availableItemsRect.y();
  const auto itemH = availableItemsRect.height();

  if (itemsNecessaryW > availableItemsRect.width()) {
    // Not enough space: we have to reduce the items widths.
    const auto itemW = itemCount > 0 ? (availableItemsRect.width() - (itemCount - 1) * spacing) / itemCount : 0;
    auto availableX = availableItemsRect.x();
    _actualRect = totalRect;

    for (auto& item : _items) {
      const auto itemX = availableX;
      item.rect = QRect{ QPoint{ itemX, itemY }, QSize{ itemW, itemH } };
      availableX += itemW + spacing;
    }
  } else if (_itemsShouldExpand) {
    // Enough space: we can increase each items widths with the superfluous with (very basic algorithm...).
    const auto superfluousW = availableItemsRect.width() - itemsNecessaryW;
    const auto increaseW = itemCount > 0 ? superfluousW / itemCount : 0;
    auto availableX = availableItemsRect.x();
    _actualRect = totalRect;

    for (auto& item : _items) {
      const auto itemX = availableX;
      const auto itemW = item.sizeHint.width() + increaseW;
      item.rect = QRect{ QPoint{ itemX, itemY }, QSize{ itemW, itemH } };
      availableX += itemW + spacing;
    }
  } else {
    // Enough space but we keep the items centered with their necessary size.
    const auto centeredItemsRectX = availableItemsRect.x() + (availableItemsRect.width() - itemsNecessaryW) / 2;
    const auto centeredItemsRect = QRect{ centeredItemsRectX, itemY, itemsNecessaryW, itemH };
    auto availableX = centeredItemsRect.x();
    _actualRect = centeredItemsRect.marginsAdded(padding);

    for (auto& item : _items) {
      const auto itemX = availableX;
      const auto itemW = item.sizeHint.width();
      item.rect = QRect{ QPoint{ itemX, itemY }, QSize{ itemW, itemH } };
      availableX += itemW + spacing;
    }
  }
}

MouseState AbstractItemListWidget::getItemMouseState(int index, const Item& item) const {
  if (!isEnabled() || !item.enabled)
    return MouseState::Disabled;
  if (index == _pressedIndex)
    return MouseState::Pressed;
  if (index == _hoveredIndex)
    return MouseState::Hovered;
  else
    return MouseState::Transparent;
}

std::tuple<QColor, QColor, QColor, QColor> AbstractItemListWidget::getItemBgAndFgColor(
  int index, MouseState mouse) const {
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  const auto& palette = this->palette();
  const auto& itemBgColor =
    qlementineStyle ? getItemBgColor(mouse, qlementineStyle->theme()) : getItemBgColor(mouse, palette);
  const auto& itemFgColor = qlementineStyle ? getItemFgColor(mouse, index == _currentIndex, qlementineStyle->theme())
                                            : getItemFgColor(mouse, index == _currentIndex, palette);
  const auto& badgeBgColor = qlementineStyle
                               ? getItemBadgeBgColor(mouse, index == _currentIndex, qlementineStyle->theme())
                               : getItemBadgeBgColor(mouse, index == _currentIndex, palette);
  const auto& badgeFgColor = qlementineStyle
                               ? getItemBadgeFgColor(mouse, index == _currentIndex, qlementineStyle->theme())
                               : getItemBadgeFgColor(mouse, index == _currentIndex, palette);
  return { itemBgColor, itemFgColor, badgeBgColor, badgeFgColor };
}

const QColor& AbstractItemListWidget::getCurrentItemIndicatorColor() const {
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  if (qlementineStyle != nullptr) {
    return qlementineStyle->buttonBackgroundColor(
      isEnabled() ? MouseState::Normal : MouseState::Disabled, ColorRole::Primary);
  }
  return palette().color(
    isEnabled() ? QPalette::ColorGroup::Normal : QPalette::ColorGroup::Disabled, QPalette::ColorRole::Highlight);
}

QRect AbstractItemListWidget::getCurrentItemRect() const {
  if (_currentIndex >= 0 && _currentIndex < itemCount()) {
    return _items[_currentIndex].rect;
  } else {
    const auto padding = getPadding();
    return QRect{ padding.left(), padding.top(), 0, height() - padding.top() - padding.bottom() };
  }
}

QRect qlementine::AbstractItemListWidget::getFocusedItemRect() const {
  if (_focusedIndex >= 0 && _focusedIndex < itemCount()) {
    return _items[_focusedIndex].rect;
  } else {
    return rect();
  }
}

QRect qlementine::AbstractItemListWidget::getAnimatedCurrentItemRect() const {
  auto currentItemRect = _currentIndexAnimation.currentValue().toRect();
  if (currentItemRect.width() == 0 || !_firstShow) {
    currentItemRect = getCurrentItemRect();
  }
  return currentItemRect;
}

void AbstractItemListWidget::updateCurrentIndexAnimation(bool immediate) {
  const auto currentItemRect = getCurrentItemRect();
  _currentIndexAnimation.stop();
  if (immediate || !_currentIndexAnimation.currentValue().isValid() || !_currentIndexAnimation.startValue().isValid()
      || !_firstShow) {
    _currentIndexAnimation.setStartValue(currentItemRect);
  } else {
    _currentIndexAnimation.setStartValue(_currentIndexAnimation.currentValue());
  }
  _currentIndexAnimation.setEndValue(currentItemRect);
  _currentIndexAnimation.start();
}

void AbstractItemListWidget::updateItemsAnimations() {
  for (auto i = 0; i < itemCount(); ++i) {
    auto& item = _items[i];

    const auto itemMouse = getItemMouseState(i, item);
    const auto [itemBgColor, itemFgColor, badgeBgColor, badgeFgColor] = getItemBgAndFgColor(i, itemMouse);
    item.bgColorAnimation->stop();
    item.fgColorAnimation->stop();
    item.badgeBgAnimation->stop();
    item.badgeFgAnimation->stop();

    item.bgColorAnimation->setStartValue(item.bgColorAnimation->currentValue().value<QColor>());
    item.fgColorAnimation->setStartValue(item.fgColorAnimation->currentValue().value<QColor>());
    item.badgeBgAnimation->setStartValue(item.badgeBgAnimation->currentValue().value<QColor>());
    item.badgeFgAnimation->setStartValue(item.badgeFgAnimation->currentValue().value<QColor>());

    item.bgColorAnimation->setEndValue(QVariant::fromValue<QColor>(itemBgColor));
    item.fgColorAnimation->setEndValue(QVariant::fromValue<QColor>(itemFgColor));
    item.badgeBgAnimation->setEndValue(QVariant::fromValue<QColor>(badgeBgColor));
    item.badgeFgAnimation->setEndValue(QVariant::fromValue<QColor>(badgeFgColor));

    item.bgColorAnimation->start();
    item.fgColorAnimation->start();
    item.badgeBgAnimation->start();
    item.badgeFgAnimation->start();
  }
}

std::vector<AbstractItemListWidget::Item>& AbstractItemListWidget::items() {
  return _items;
}

const std::vector<AbstractItemListWidget::Item>& AbstractItemListWidget::items() const {
  return _items;
}

int AbstractItemListWidget::focusedIndex() const {
  return _focusedIndex;
}

int AbstractItemListWidget::pressedIndex() const {
  return _pressedIndex;
}

void AbstractItemListWidget::updateFocusFrame() {
  _focusFrame->update();
  _focusFrame->updateGeometry();
}

bool AbstractItemListWidget::canSelectItem(int /*index*/) const {
  return true;
}

void AbstractItemListWidget::setFocusedIndex(int index) {
  index = index < 0 || index > itemCount() - 1 ? -1 : index;
  if (index != _focusedIndex) {
    _focusedIndex = index;
    _focusFrame->updateGeometry();
    _focusFrame->update();
  }
}

void AbstractItemListWidget::setHoveredIndex(int index, bool updateAnims) {
  index = index < 0 || index > itemCount() - 1 ? -1 : index;
  if (index != _hoveredIndex) {
    _hoveredIndex = index;
    if (updateAnims)
      updateItemsAnimations();
  }
}

void AbstractItemListWidget::setPressedIndex(int index, bool updateAnims) {
  index = index < 0 || index > itemCount() - 1 ? -1 : index;
  if (index != _pressedIndex) {
    _pressedIndex = index;
    if (updateAnims)
      updateItemsAnimations();
  }
}

const QFont& AbstractItemListWidget::badgeFont() const {
  return _badgeFont;
}

const QRect& AbstractItemListWidget::actualRect() const {
  return _actualRect;
}

void AbstractItemListWidget::initStyleOptionFocus(QStyleOptionFocusRoundedRect& opt) const {
  const auto* style = this->style();
  const auto deltaX = style->pixelMetric(QStyle::PM_FocusFrameHMargin, &opt, this);
  const auto deltaY = style->pixelMetric(QStyle::PM_FocusFrameVMargin, &opt, this);
  opt.rect = getFocusedItemRect().translated(deltaX, deltaY);
  opt.radiuses = getItemRadius();
}

void AbstractItemListWidget::paintEvent(QPaintEvent*) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setRenderHint(QPainter::SmoothPixmapTransform, true);

  // Background.
  drawBackground(p);

  // Items backgrounds.
  for (const auto& item : _items) {
    drawItemBackground(p, item);
  }

  // Current item background (drawn above items backgrounds).
  drawCurrentItemIndicator(p);

  // Items foregrounds.
  for (const auto& item : _items) {
    drawItemForeground(p, item);
  }
}

void AbstractItemListWidget::resizeEvent(QResizeEvent* e) {
  QWidget::resizeEvent(e);
  updateItemRects();
  updateCurrentIndexAnimation(true);
  _focusFrame->update();
  _focusFrame->updateGeometry();
}

void AbstractItemListWidget::focusInEvent(QFocusEvent* e) {
  QWidget::focusInEvent(e);
  if (_focusedIndex == -1) {
    setFocusedIndex(0);
  }
}

void AbstractItemListWidget::showEvent(QShowEvent* e) {
  QWidget::showEvent(e);

  if (!_firstShow) {
    // Animation for current item.
    const auto currentItemRect = getCurrentItemRect();
    const auto animDuration = style()->styleHint(QStyle::SH_Widget_Animation_Duration) * animationFactor;
    _currentIndexAnimation.setStartValue(currentItemRect);
    _currentIndexAnimation.setEndValue(currentItemRect);
    _currentIndexAnimation.setDuration(animDuration);
    _currentIndexAnimation.setEasingCurve(QEasingCurve::Type::InOutCubic);
    QObject::connect(&_currentIndexAnimation, &QVariantAnimation::valueChanged, this, [this]() {
      update();
    });
  }

  _firstShow = true;
}

bool AbstractItemListWidget::event(QEvent* e) {
  if (e->type() == QEvent::Type::PaletteChange) {
    updateItemsAnimations();
  }
  return QWidget::event(e);
}

void AbstractItemListWidget::changeEvent(QEvent* e) {
  if (QEvent::EnabledChange == e->type()) {
    updateItemsAnimations();
  }
  QWidget::changeEvent(e);
}

QMargins AbstractItemListWidget::getPadding() const {
  const auto* style = this->style();
  const auto left = style->pixelMetric(QStyle::PM_LayoutLeftMargin) / 4;
  const auto top = style->pixelMetric(QStyle::PM_LayoutTopMargin) / 4;
  const auto right = style->pixelMetric(QStyle::PM_LayoutRightMargin) / 4;
  const auto bottom = style->pixelMetric(QStyle::PM_LayoutBottomMargin) / 4;
  return QMargins{ left, top, right, bottom };
}

int AbstractItemListWidget::getSpacing() const {
  return style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing) / 2;
}

double AbstractItemListWidget::getRadius() const {
  const auto padding = getPadding();
  const auto itemRadius = getItemRadius();
  return padding.left() + itemRadius;
}

const QColor& AbstractItemListWidget::getBgColor(const QPalette& palette) const {
  return palette.color(QPalette::Window);
}

const QColor& AbstractItemListWidget::getBgColor() const {
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  return qlementineStyle ? getBgColor(qlementineStyle->theme()) : getBgColor(palette());
}

QMargins AbstractItemListWidget::getBadgePadding() const {
  const auto* style = this->style();
  const auto left = style->pixelMetric(QStyle::PM_LayoutLeftMargin) / 4;
  const auto top = style->pixelMetric(QStyle::PM_LayoutTopMargin) / 8;
  const auto right = style->pixelMetric(QStyle::PM_LayoutRightMargin) / 4;
  const auto bottom = style->pixelMetric(QStyle::PM_LayoutBottomMargin) / 8;
  return QMargins{ left, top, right, bottom };
}

const QColor& AbstractItemListWidget::getItemBadgeBgColor(
  MouseState mouse, bool /*selected*/, const QPalette& palette) const {
  return palette.color(mouse != MouseState::Disabled ? QPalette::Normal : QPalette::Disabled, QPalette::Dark);
}

const QColor& AbstractItemListWidget::getItemBadgeFgColor(
  MouseState mouse, bool /*selected*/, const QPalette& palette) const {
  return palette.color(mouse != MouseState::Disabled ? QPalette::Normal : QPalette::Disabled, QPalette::BrightText);
}

void AbstractItemListWidget::drawBackground(QPainter& p) const {
  const auto& bgColor = getBgColor();
  const auto radius = getRadius();
  p.setPen(Qt::NoPen);
  p.setBrush(bgColor);
  p.drawRoundedRect(actualRect(), radius, radius);
}

void AbstractItemListWidget::drawItemBackground(QPainter& p, const Item& item) const {
  const auto& itemRect = item.rect;
  const auto currentItemRect = getAnimatedCurrentItemRect();
  const auto radius = getItemRadius();
  if (shouldDrawItemBgForCurrent() || itemRect.x() != currentItemRect.x()) {
    const auto itemBgColor = item.bgColorAnimation->currentValue().value<QColor>();
    p.setPen(Qt::NoPen);
    p.setBrush(itemBgColor);
    p.drawRoundedRect(itemRect, radius, radius);
  }
}

void AbstractItemListWidget::drawItemForeground(QPainter& p, const Item& item) const {
  const auto itemPadding = getItemPadding();
  const auto itemSpacing = getItemSpacing();
  const auto badgeFm = QFontMetrics{ badgeFont(), this };
  const auto badgePadding = getBadgePadding();
  const auto badgeH = badgeFm.height() + badgePadding.top() + badgePadding.bottom();
  const auto badgeRadius = badgeH / 2;
  const auto badgeW =
    std::max(badgeH, badgePadding.left() + badgePadding.right() + qlementine::textWidth(badgeFm, item.badge));
  const auto& itemRect = item.rect;
  const auto itemContentW = std::min(item.sizeHint.width(), itemRect.width());
  const auto itemContentH = itemRect.height();
  const auto itemContentX = itemRect.x() + (itemRect.width() - itemContentW) / 2;
  const auto itemContentY = itemRect.y();
  const auto itemContentRect = QRect{ QPoint{ itemContentX, itemContentY }, QSize{ itemContentW, itemContentH } };
  const auto itemFgColor = item.fgColorAnimation->currentValue().value<QColor>();
  const auto hasIcon = !item.icon.isNull();
  const auto hasText = !item.text.isEmpty();
  const auto hasBadge = !item.badge.isEmpty();
  const auto& iconSize = this->iconSize();
  auto availableX = itemContentRect.x() + itemPadding.left();
  auto availableW = itemContentRect.width() - itemPadding.left() - itemPadding.right();

  // Compute rects.
  const auto iconX = availableX;
  const auto iconY = itemContentRect.y() + (itemContentRect.height() - iconSize.height()) / 2;
  auto iconRect = QRect{ QPoint{ iconX, iconY }, iconSize };
  const auto showIcon = hasIcon && itemContentRect.width() > iconRect.width();
  const auto iconTakenSpace = showIcon ? iconRect.width() + itemSpacing : 0;
  availableX += iconTakenSpace;
  availableW -= iconTakenSpace;

  const auto badgeX = itemContentRect.x() + itemContentRect.width() - itemPadding.left() - badgeW;
  const auto badgeY = itemContentRect.y() + (itemContentRect.height() - badgeH) / 2;
  const auto badgeSize = QSize{ badgeW, badgeH };
  auto badgeRect = QRect{ QPoint{ badgeX, badgeY }, badgeSize };
  auto showBadge = hasBadge && availableW > badgeRect.width();
  const auto badgeTakenSpace = showBadge ? badgeRect.width() + itemSpacing : 0;
  availableW -= badgeTakenSpace;

  const auto textX = availableX;
  const auto textY = itemContentRect.y();
  const auto textW = availableW;
  const auto textH = itemContentRect.height();
  auto textRect = QRect{ QPoint{ textX, textY }, QSize{ textW, textH } };
  const auto showText =
    ((!hasIcon && !hasBadge) || hasText) && (showIcon ? availableW > itemSpacing * 3 : itemRect.width() > 0);

  if (showIcon && !showText) {
    if (showBadge) {
      // Center icon and badge.
      const auto newItemContentW = iconSize.width() + itemSpacing + badgeSize.width();
      const auto newItemContentX = itemContentRect.x() + (itemContentRect.width() - newItemContentW) / 2;
      iconRect = QRect{ QPoint{ newItemContentX, iconY }, iconSize };
      badgeRect = QRect{ QPoint{ newItemContentX + iconSize.width() + itemSpacing, badgeY }, badgeSize };
    } else {
      // Center icon.
      const auto newIconX = itemContentRect.x() + (itemContentRect.width() - iconSize.width()) / 2;
      iconRect = QRect{ QPoint{ newIconX, iconY }, iconSize };
    }
  } else if (!showIcon && showText) {
    if (showBadge) {
      if (badgeRect.width() > std::max(static_cast<int>(itemContentRect.width() / 2.5), itemSpacing * 4)) {
        // Don't show badge, give priority to text.
        showBadge = false;
        textRect = itemRect.marginsRemoved({ itemSpacing / 2, 0, itemSpacing / 2, 0 });
        if (textRect.width() > itemSpacing * 4) {
          textRect = textRect.marginsRemoved({ itemSpacing, 0, itemSpacing, 0 });
        }
      }
    } else {
      // Text can take full width (it is elided anyway).
      textRect = itemRect.marginsRemoved({ itemSpacing / 2, 0, itemSpacing / 2, 0 });
      if (textRect.width() > itemSpacing * 4) {
        textRect = textRect.marginsRemoved({ itemSpacing, 0, itemSpacing, 0 });
      }
    }
  }

  // Icon.
  if (showIcon) {
    const auto pixmap = item.icon.pixmap(iconSize.height(), QIcon::Normal, QIcon::Off);
    const auto coloredPixmap = colorizePixmap(pixmap, itemFgColor);
    // Draw image.
    p.drawPixmap(iconRect, coloredPixmap);
  }

  // Badge.
  if (showBadge) {
    const auto badgeTextRect = badgeRect.marginsRemoved(badgePadding);
    const auto textFlags = Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextSingleLine;
    const auto badgeBgColor = item.badgeBgAnimation->currentValue().value<QColor>();
    const auto badgeFgColor = item.badgeFgAnimation->currentValue().value<QColor>();

    // Draw background.
    p.setPen(Qt::NoPen);
    p.setBrush(badgeBgColor);
    p.drawRoundedRect(badgeRect, badgeRadius, badgeRadius);

    // Draw foreground.
    p.setFont(badgeFont());
    p.setPen(badgeFgColor);
    p.drawText(badgeTextRect, textFlags, item.badge);
  }

  // Text.
  if (showText) {
    const auto fm = QFontMetrics(labelFont(), this);
    const auto elidedText =
      fm.elidedText(item.text, Qt::TextElideMode::ElideRight, textRect.width(), Qt::TextSingleLine);
    const auto textFlags = Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextSingleLine;

    // Draw text.
    p.setFont(labelFont());
    p.setPen(itemFgColor);
    p.drawText(textRect, textFlags, elidedText);
  }
}

void AbstractItemListWidget::drawCurrentItemIndicator(QPainter& p) const {
  const auto currentItemRect = getAnimatedCurrentItemRect();
  const auto& color = getCurrentItemIndicatorColor();
  const auto radius = getItemRadius();
  p.setPen(Qt::NoPen);
  p.setBrush(color);
  p.drawRoundedRect(currentItemRect, radius, radius);
}

bool AbstractItemListWidget::shouldDrawItemBgForCurrent() const {
  return true;
}

QFont AbstractItemListWidget::labelFont() const {
  return this->font();
}

QMargins AbstractItemListWidget::getItemPadding() const {
  const auto* style = this->style();
  const auto left = style->pixelMetric(QStyle::PM_LayoutLeftMargin);
  const auto top = static_cast<int>(std::ceil(style->pixelMetric(QStyle::PM_LayoutTopMargin) * 0.375));
  const auto right = style->pixelMetric(QStyle::PM_LayoutRightMargin);
  const auto bottom = static_cast<int>(std::ceil(style->pixelMetric(QStyle::PM_LayoutBottomMargin) * 0.375));
  return QMargins{ left, top, right, bottom };
}

int AbstractItemListWidget::getItemMinimumHeight() const {
  const auto* style = this->style();
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style);
  const auto height = qlementineStyle ? qlementineStyle->theme().controlHeightLarge : 0;
  const auto padding = getPadding();
  return std::max(0, height - padding.top() - padding.bottom());
}

int AbstractItemListWidget::getItemSpacing() const {
  return style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
}

double AbstractItemListWidget::getItemRadius() const {
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  return qlementineStyle ? qlementineStyle->theme().borderRadius : 0;
}

const QColor& AbstractItemListWidget::getItemFgColor(
  MouseState mouse, bool /*selected*/, const QPalette& palette) const {
  return palette.color(mouse != MouseState::Disabled ? QPalette::Normal : QPalette::Disabled, QPalette::ButtonText);
}

const QColor& AbstractItemListWidget::getItemBgColor(MouseState mouse, const QPalette& palette) const {
  return palette.color(mouse != MouseState::Disabled ? QPalette::Normal : QPalette::Disabled, QPalette::Button);
}

} // namespace oclero::qlementine
