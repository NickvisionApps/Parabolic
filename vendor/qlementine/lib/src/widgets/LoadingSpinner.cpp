// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT
#include <oclero/qlementine/widgets/LoadingSpinner.hpp>

#include <QStyle>
#include <QPainter>
#include <QPaintEvent>
#include <QTimerEvent>

#include <math.h>

namespace {
qreal easeOutQuad(const qreal x) {
  return x * x * x * x;
}
} // namespace

namespace oclero::qlementine {
LoadingSpinner::LoadingSpinner(QWidget* parent)
  : QWidget(parent) {
  setFocusPolicy(Qt::NoFocus);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setAttribute(Qt::WA_TransparentForMouseEvents);
}

bool LoadingSpinner::spinning() const {
  return _spinning;
}

void LoadingSpinner::setSpinning(bool spinning) {
  if (spinning != _spinning) {
    _spinning = spinning;

    if (_spinning && isVisible()) {
      _timerId = startTimer(128);
    } else {
      if (_timerId != -1) {
        killTimer(_timerId);
      }
      _timerId = -1;
      _i = 0;
    }

    Q_EMIT spinningChanged();
    if (isVisible()) {
      update();
    }
  }
}

QSize LoadingSpinner::minimumSizeHint() const {
  const auto* style = this->style();
  const auto defaultIconExtent = style ? style->pixelMetric(QStyle::PM_ButtonIconSize) : 16;
  return QSize(defaultIconExtent, defaultIconExtent);
}

QSize LoadingSpinner::sizeHint() const {
  return minimumSizeHint();
}

void LoadingSpinner::paintEvent(QPaintEvent*) {
  if (!_spinning)
    return;

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setPen(Qt::NoPen);

  const auto rect = this->rect();
  const auto diameter = qMin(rect.width(), rect.height());
  const auto ellipseTopLeft = QPointF((rect.width() - diameter) / 2., (rect.height() - diameter) / 2.);
  const auto ellipseCenter = ellipseTopLeft + QPointF(diameter / 2., diameter / 2.);
  painter.translate(ellipseCenter);

  constexpr auto itemCount = 12;
  constexpr auto totalAngle = 360;
  constexpr auto itemAngle = totalAngle / itemCount;
  const auto startAngle = ((_i + 1) * itemAngle) % totalAngle;
  painter.rotate(startAngle);

  const auto baseColor = palette().text().color();
  for (auto i = 0; i < itemCount; ++i) {
    const auto itemWidth = diameter / 8.;
    const auto itemHeight = diameter / 4.;
    const auto itemX = -itemWidth / 2.;
    const auto itemY = -diameter / 2.;
    const auto itemRect = QRectF(QPointF(itemX, itemY), QSizeF(itemWidth, itemHeight));
    const auto radius = itemWidth / 2.25;
    const auto alpha = 255. * easeOutQuad(i / static_cast<qreal>(itemCount - 1));
    const auto color = QColor(baseColor.red(), baseColor.green(), baseColor.blue(), alpha);
    painter.setBrush(color);
    painter.drawRoundedRect(itemRect, radius, radius);

    painter.rotate(itemAngle);
  }
}

void LoadingSpinner::timerEvent(QTimerEvent* evt) {
  if (evt->timerId() == _timerId) {
    _i = (_i + 1) % 12;
    if (isVisible()) {
      update();
    }
  }
}

void LoadingSpinner::showEvent(QShowEvent* evt) {
  QWidget::showEvent(evt);
  if (_spinning) {
    _timerId = startTimer(128);
  }
}

void LoadingSpinner::hideEvent(QHideEvent* evt) {
  QWidget::hideEvent(evt);
  if (_timerId != -1) {
    killTimer(_timerId);
  }
  _timerId = -1;
  _i = 0;
}
} // namespace oclero::qlementine
