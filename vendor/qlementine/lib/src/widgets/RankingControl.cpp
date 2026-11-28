// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/RankingControl.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/Theme.hpp>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

#include <algorithm>
#include <cmath>

namespace oclero::qlementine {
namespace {
constexpr auto starSize = 20;
constexpr auto starSpacing = 4;
constexpr auto starSlotWidth = starSize + starSpacing;
constexpr auto starInnerRatio = 0.5;
constexpr auto starOutlineWidth = 1.5;
constexpr auto placeholderAlpha = 110;
} // namespace

RankingControl::RankingControl(QWidget* parent)
  : QWidget(parent) {
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
  setCursor(Qt::PointingHandCursor);
}

int RankingControl::maximum() const {
  return _maximum;
}

void RankingControl::setMaximum(int maximum) {
  maximum = std::max(1, maximum);
  if (maximum != _maximum) {
    _maximum = maximum;
    updateGeometry();
    setValue(_value);
    update();
    Q_EMIT maximumChanged();
  }
}

double RankingControl::value() const {
  return _value;
}

void RankingControl::setValue(double value) {
  value = std::clamp(_halfStepsEnabled ? std::round(value * 2.) / 2. : std::round(value), 0., static_cast<double>(_maximum));
  if (value != _value) {
    _value = value;
    update();
    Q_EMIT valueChanged(_value);
  }
}

double RankingControl::placeholderValue() const {
  return _placeholderValue;
}

void RankingControl::setPlaceholderValue(double placeholderValue) {
  placeholderValue = std::clamp(placeholderValue, 0., static_cast<double>(_maximum));
  if (placeholderValue != _placeholderValue) {
    _placeholderValue = placeholderValue;
    update();
    Q_EMIT placeholderValueChanged();
  }
}

bool RankingControl::halfStepsEnabled() const {
  return _halfStepsEnabled;
}

void RankingControl::setHalfStepsEnabled(bool enabled) {
  if (enabled != _halfStepsEnabled) {
    _halfStepsEnabled = enabled;
    setValue(_value);
    update();
    Q_EMIT halfStepsEnabledChanged();
  }
}

bool RankingControl::readOnly() const {
  return _readOnly;
}

void RankingControl::setReadOnly(bool readOnly) {
  if (readOnly != _readOnly) {
    _readOnly = readOnly;
    _hoverValue = -1.;
    setCursor(_readOnly ? Qt::ArrowCursor : Qt::PointingHandCursor);
    update();
    Q_EMIT readOnlyChanged();
  }
}

bool RankingControl::clearEnabled() const {
  return _clearEnabled;
}

void RankingControl::setClearEnabled(bool enabled) {
  if (enabled != _clearEnabled) {
    _clearEnabled = enabled;
    Q_EMIT clearEnabledChanged();
  }
}

QSize RankingControl::minimumSizeHint() const {
  return QSize(_maximum * starSlotWidth - starSpacing, starSize);
}

QSize RankingControl::sizeHint() const {
  return minimumSizeHint();
}

void RankingControl::paintEvent(QPaintEvent*) {
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  const auto enabled = isEnabled();
  auto fillColor = qlementineStyle ? (enabled ? qlementineStyle->theme().primaryColor : qlementineStyle->theme().primaryColorDisabled)
                                   : palette().highlight().color();
  const auto outlineColor =
    qlementineStyle ? (enabled ? qlementineStyle->theme().secondaryAlternativeColor : qlementineStyle->theme().secondaryColorDisabled)
                    : palette().placeholderText().color();

  const auto hasValue = _value > 0.;
  const auto isHovering = _hoverValue >= 0. && enabled && !_readOnly;
  const auto displayedValue = isHovering ? _hoverValue : hasValue ? _value : _placeholderValue;
  if (!isHovering && !hasValue) {
    fillColor.setAlpha(placeholderAlpha);
  }

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);
  const auto outerRadius = starSize / 2. - starOutlineWidth / 2.;
  const auto innerRadius = outerRadius * starInnerRatio;
  const auto yCenter = height() / 2.;
  for (auto i = 0; i < _maximum; ++i) {
    const auto xCenter = i * starSlotWidth + starSize / 2.;
    QPainterPath starPath;
    for (auto point = 0; point < 10; ++point) {
      const auto angle = -M_PI / 2. + point * M_PI / 5.;
      const auto radius = point % 2 == 0 ? outerRadius : innerRadius;
      const auto pointPos = QPointF(xCenter + radius * std::cos(angle), yCenter + radius * std::sin(angle));
      if (point == 0) {
        starPath.moveTo(pointPos);
      } else {
        starPath.lineTo(pointPos);
      }
    }
    starPath.closeSubpath();

    painter.setPen(QPen(outlineColor, starOutlineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(starPath);

    const auto fillRatio = std::clamp(displayedValue - i, 0., 1.);
    if (fillRatio > 0.) {
      painter.save();
      painter.setClipRect(QRectF(i * starSlotWidth, 0., starSize * fillRatio, height()));
      painter.setPen(QPen(fillColor, starOutlineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
      painter.setBrush(fillColor);
      painter.drawPath(starPath);
      painter.restore();
    }
  }
}

void RankingControl::mouseMoveEvent(QMouseEvent* evt) {
  if (!_readOnly) {
    const auto hoverValue = valueAtPos(evt->pos());
    if (hoverValue != _hoverValue) {
      _hoverValue = hoverValue;
      update();
    }
  }
  QWidget::mouseMoveEvent(evt);
}

void RankingControl::mousePressEvent(QMouseEvent* evt) {
  if (evt->button() == Qt::LeftButton && !_readOnly) {
    const auto newValue = valueAtPos(evt->pos());
    if (newValue >= 0.) {
      setValue(_clearEnabled && newValue == _value ? 0. : newValue);
    }
  }
  QWidget::mousePressEvent(evt);
}

void RankingControl::leaveEvent(QEvent* evt) {
  if (_hoverValue >= 0.) {
    _hoverValue = -1.;
    update();
  }
  QWidget::leaveEvent(evt);
}

void RankingControl::keyPressEvent(QKeyEvent* evt) {
  if (_readOnly) {
    QWidget::keyPressEvent(evt);
    return;
  }

  const auto step = _halfStepsEnabled ? .5 : 1.;
  switch (evt->key()) {
    case Qt::Key_Left:
    case Qt::Key_Down:
      setValue(_value - step);
      break;
    case Qt::Key_Right:
    case Qt::Key_Up:
      setValue(_value + step);
      break;
    case Qt::Key_Home:
      setValue(0.);
      break;
    case Qt::Key_End:
      setValue(_maximum);
      break;
    default:
      QWidget::keyPressEvent(evt);
      return;
  }
  evt->accept();
}

double RankingControl::valueAtPos(const QPoint& pos) const {
  const auto index = pos.x() / starSlotWidth;
  if (pos.x() < 0 || index >= _maximum) {
    return -1.;
  }
  const auto ratio = static_cast<double>(pos.x() - index * starSlotWidth) / starSize;
  return index + (_halfStepsEnabled && ratio < .5 ? .5 : 1.);
}
} // namespace oclero::qlementine
