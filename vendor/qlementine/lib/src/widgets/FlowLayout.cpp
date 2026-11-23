// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/FlowLayout.hpp>

#include <QStyle>
#include <QWidget>

#include <algorithm>

namespace oclero::qlementine {
FlowLayout::FlowLayout(QWidget* parent)
  : QLayout(parent) {}

FlowLayout::~FlowLayout() {
  while (auto* item = takeAt(0)) {
    delete item;
  }
}

int FlowLayout::horizontalSpacing() const {
  return _horizontalSpacing >= 0 ? _horizontalSpacing : smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

void FlowLayout::setHorizontalSpacing(int spacing) {
  if (spacing != _horizontalSpacing) {
    _horizontalSpacing = spacing;
    invalidate();
  }
}

int FlowLayout::verticalSpacing() const {
  return _verticalSpacing >= 0 ? _verticalSpacing : smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}

void FlowLayout::setVerticalSpacing(int spacing) {
  if (spacing != _verticalSpacing) {
    _verticalSpacing = spacing;
    invalidate();
  }
}

void FlowLayout::addItem(QLayoutItem* item) {
  _items.append(item);
}

int FlowLayout::count() const {
  return static_cast<int>(_items.size());
}

QLayoutItem* FlowLayout::itemAt(int index) const {
  return _items.value(index, nullptr);
}

QLayoutItem* FlowLayout::takeAt(int index) {
  return index >= 0 && index < _items.size() ? _items.takeAt(index) : nullptr;
}

Qt::Orientations FlowLayout::expandingDirections() const {
  return {};
}

bool FlowLayout::hasHeightForWidth() const {
  return true;
}

int FlowLayout::heightForWidth(int width) const {
  return doLayout(QRect(0, 0, width, 0), true);
}

void FlowLayout::setGeometry(const QRect& rect) {
  QLayout::setGeometry(rect);
  doLayout(rect, false);
}

QSize FlowLayout::minimumSize() const {
  auto size = QSize();
  for (const auto* item : _items) {
    size = size.expandedTo(item->minimumSize());
  }
  const auto margins = contentsMargins();
  return size + QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
}

QSize FlowLayout::sizeHint() const {
  return minimumSize();
}

int FlowLayout::doLayout(const QRect& rect, bool testOnly) const {
  const auto margins = contentsMargins();
  const auto area = rect.adjusted(margins.left(), margins.top(), -margins.right(), -margins.bottom());
  const auto horizontal = horizontalSpacing();
  const auto vertical = verticalSpacing();
  auto x = area.x();
  auto y = area.y();
  auto rowHeight = 0;

  for (auto* item : _items) {
    const auto itemSize = item->sizeHint();
    if (x > area.x() && x + itemSize.width() > area.right() + 1) {
      x = area.x();
      y += rowHeight + vertical;
      rowHeight = 0;
    }
    if (!testOnly) {
      item->setGeometry(QRect(QPoint(x, y), itemSize));
    }
    x += itemSize.width() + horizontal;
    rowHeight = std::max(rowHeight, itemSize.height());
  }

  return y + rowHeight - rect.y() + margins.bottom();
}

int FlowLayout::smartSpacing(QStyle::PixelMetric pixelMetric) const {
  auto* parentObject = parent();
  if (!parentObject) {
    return -1;
  }
  if (parentObject->isWidgetType()) {
    auto* parentWidget = static_cast<QWidget*>(parentObject);
    return parentWidget->style()->pixelMetric(pixelMetric, nullptr, parentWidget);
  }
  return static_cast<QLayout*>(parentObject)->spacing();
}
} // namespace oclero::qlementine
