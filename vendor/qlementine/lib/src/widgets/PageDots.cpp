// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/PageDots.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/Theme.hpp>

#include <QMouseEvent>
#include <QPainter>

#include <algorithm>

namespace oclero::qlementine {
namespace {
constexpr auto dotDiameter = 6;
constexpr auto dotSlotWidth = 14;
constexpr auto dotSlotHeight = 14;
constexpr auto inactiveAlpha = 110;
} // namespace

PageDots::PageDots(QWidget* parent)
  : QWidget(parent) {
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setCursor(Qt::PointingHandCursor);
}

int PageDots::count() const {
  return _count;
}

void PageDots::setCount(int count) {
  count = std::max(0, count);
  if (count != _count) {
    _count = count;
    if (_currentIndex >= _count) {
      setCurrentIndex(_count - 1);
    }
    updateGeometry();
    update();
    Q_EMIT countChanged();
  }
}

int PageDots::currentIndex() const {
  return _currentIndex;
}

void PageDots::setCurrentIndex(int index) {
  index = _count == 0 ? 0 : std::clamp(index, 0, _count - 1);
  if (index != _currentIndex) {
    _currentIndex = index;
    update();
    Q_EMIT currentIndexChanged();
  }
}

QSize PageDots::minimumSizeHint() const {
  return QSize(_count * dotSlotWidth, dotSlotHeight);
}

QSize PageDots::sizeHint() const {
  return minimumSizeHint();
}

void PageDots::paintEvent(QPaintEvent*) {
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  const auto& activeColor = qlementineStyle ? qlementineStyle->theme().secondaryColor : palette().text().color();
  auto inactiveColor =
    qlementineStyle ? qlementineStyle->theme().secondaryAlternativeColor : palette().placeholderText().color();
  inactiveColor.setAlpha(inactiveAlpha);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setPen(Qt::NoPen);
  const auto radius = dotDiameter / 2.;
  for (auto i = 0; i < _count; ++i) {
    painter.setBrush(i == _currentIndex ? activeColor : inactiveColor);
    painter.drawEllipse(QPointF(dotSlotWidth * (i + .5), height() / 2.), radius, radius);
  }
}

void PageDots::mousePressEvent(QMouseEvent* evt) {
  if (evt->button() == Qt::LeftButton) {
    const auto index = indexAtPos(evt->pos());
    if (index != -1) {
      setCurrentIndex(index);
    }
  }
  QWidget::mousePressEvent(evt);
}

int PageDots::indexAtPos(const QPoint& pos) const {
  const auto index = pos.x() / dotSlotWidth;
  return pos.x() >= 0 && index < _count ? index : -1;
}
} // namespace oclero::qlementine
