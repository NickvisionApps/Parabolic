// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT
#include <oclero/qlementine/widgets/LoadingRing.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/utils/PrimitiveUtils.hpp>

#include <QStyle>
#include <QPainter>
#include <QPaintEvent>

#include <algorithm>
#include <cmath>

namespace oclero::qlementine {
LoadingRing::LoadingRing(QWidget* parent)
  : QWidget(parent) {
  setFocusPolicy(Qt::NoFocus);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setAttribute(Qt::WA_TransparentForMouseEvents);

  _animation.setStartValue(0.);
  _animation.setEndValue(1.);
  _animation.setDuration(1800);
  _animation.setEasingCurve(QEasingCurve::Type::Linear);
  _animation.setLoopCount(-1);
  QObject::connect(&_animation, &QVariantAnimation::valueChanged, this, [this]() {
    update();
  });
}

bool LoadingRing::indeterminate() const {
  return _indeterminate;
}

void LoadingRing::setIndeterminate(bool indeterminate) {
  if (indeterminate != _indeterminate) {
    _indeterminate = indeterminate;
    updateAnimation();
    Q_EMIT indeterminateChanged();
    update();
  }
}

int LoadingRing::minimum() const {
  return _minimum;
}

void LoadingRing::setMinimum(int minimum) {
  if (minimum != _minimum) {
    _minimum = minimum;
    _maximum = std::max(_maximum, _minimum);
    _value = std::clamp(_value, _minimum, _maximum);
    Q_EMIT minimumChanged();
    update();
  }
}

int LoadingRing::maximum() const {
  return _maximum;
}

void LoadingRing::setMaximum(int maximum) {
  if (maximum != _maximum) {
    _maximum = maximum;
    _minimum = std::min(_minimum, _maximum);
    _value = std::clamp(_value, _minimum, _maximum);
    Q_EMIT maximumChanged();
    update();
  }
}

int LoadingRing::value() const {
  return _value;
}

void LoadingRing::setValue(int value) {
  const auto clampedValue = std::clamp(value, _minimum, _maximum);
  if (clampedValue != _value) {
    _value = clampedValue;
    Q_EMIT valueChanged();
    update();
  }
}

QSize LoadingRing::minimumSizeHint() const {
  const auto* style = this->style();
  const auto defaultIconExtent = style ? style->pixelMetric(QStyle::PM_ButtonIconSize) : 16;
  return QSize(defaultIconExtent, defaultIconExtent);
}

QSize LoadingRing::sizeHint() const {
  return minimumSizeHint();
}

void LoadingRing::paintEvent(QPaintEvent*) {
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  const auto grooveColor = qlementineStyle
                             ? qlementineStyle->progressBarGrooveColor(MouseState::Normal)
                             : palette().color(QPalette::Mid);
  const auto valueColor = qlementineStyle
                            ? qlementineStyle->progressBarValueColor(MouseState::Normal)
                            : palette().color(QPalette::Highlight);

  const auto diameter = std::min(width(), height());
  const auto thickness = std::max(2., diameter / 8.);
  const auto ringRect = QRectF((width() - diameter) / 2., (height() - diameter) / 2., diameter, diameter)
                          .marginsRemoved({ thickness / 2., thickness / 2., thickness / 2., thickness / 2. });

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setBrush(Qt::NoBrush);

  painter.setPen(QPen(grooveColor, thickness, Qt::SolidLine, Qt::RoundCap));
  painter.drawEllipse(ringRect);

  painter.setPen(QPen(valueColor, thickness, Qt::SolidLine, Qt::RoundCap));
  if (_indeterminate) {
    const auto progress = _animation.currentValue().toReal();
    const auto sweepAngle = 24. + 216. * std::pow(std::sin(QLEMENTINE_PI * progress), 2);
    const auto startAngle = 90. - progress * 720.;
    painter.drawArc(ringRect, static_cast<int>(startAngle * 16.), static_cast<int>(-sweepAngle * 16.));
  } else if (_maximum > _minimum && _value > _minimum) {
    const auto ratio = static_cast<qreal>(_value - _minimum) / static_cast<qreal>(_maximum - _minimum);
    painter.drawArc(ringRect, 90 * 16, static_cast<int>(-360. * 16. * ratio));
  }
}

void LoadingRing::showEvent(QShowEvent* evt) {
  QWidget::showEvent(evt);
  updateAnimation();
}

void LoadingRing::hideEvent(QHideEvent* evt) {
  QWidget::hideEvent(evt);
  updateAnimation();
}

void LoadingRing::updateAnimation() {
  const auto shouldRun = _indeterminate && isVisible();
  if (shouldRun && _animation.state() != QAbstractAnimation::Running) {
    _animation.start();
  } else if (!shouldRun && _animation.state() != QAbstractAnimation::Stopped) {
    _animation.stop();
  }
}
} // namespace oclero::qlementine
