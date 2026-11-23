// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/Switch.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/QlementineStyleOption.hpp>
#include <oclero/qlementine/style/Theme.hpp>

#include <oclero/qlementine/utils/StateUtils.hpp>
#include <oclero/qlementine/utils/ImageUtils.hpp>
#include <oclero/qlementine/utils/FontUtils.hpp>
#include <oclero/qlementine/utils/PrimitiveUtils.hpp>

#include <QPainter>
#include <QEvent>

namespace oclero::qlementine {
Switch::Switch(QWidget* parent)
  : QAbstractButton(parent) {
  setCheckable(true);
  setChecked(false);
  setAutoRepeat(false);
  setupAnimation();
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // Like QCheckBox
  // Focus frame.
  _focusFrame = new RoundedFocusFrame(this);
  _focusFrame->setWidget(this);

  refreshThemeMetrics();
}

QSize Switch::sizeHint() const {
  const auto* style = this->style();
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style);

  const auto fm = fontMetrics();
  const auto textW = qlementine::textWidth(fm, text());
  const auto textH = fm.height();
  const auto spacing = style->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
  const auto switchRect = getSwitchRect();
  auto w = (textW > 0 ? textW + spacing : 0) + switchRect.width();
  const auto iconSize = this->iconSize();
  const auto hasIcon = !icon().isNull() || iconSize.isEmpty();
  const auto iconH = hasIcon ? iconSize.height() : 0;
  if (hasIcon) {
    w += iconSize.width() + spacing;
  }
  const auto defaultH = qlementineStyle ? qlementineStyle->theme().controlHeightMedium : 0;
  const auto h = std::max({ textH, switchRect.height(), iconH, defaultH });
  return { w, h };
}

void Switch::paintEvent(QPaintEvent*) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setRenderHint(QPainter::SmoothPixmapTransform, true);

  const auto* style = this->style();
  const auto spacing = style->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
  const auto contentRect = rect();
  const auto iconSize = this->iconSize();
  const auto hasIcon = !icon().isNull() || iconSize.isEmpty();
  const auto hasText = !text().isEmpty();

  // Draw switch button.
  const auto switchRect = getSwitchRect();
  const auto switchRadius = switchRect.height() / 2.;
  const auto& bgColor = _bgAnimation.currentValue().value<QColor>();
  const auto& fgColor = _fgAnimation.currentValue().value<QColor>();
  const auto& borderColor = _borderAnimation.currentValue().value<QColor>();
  const auto& textColor = getTextColor();
  const auto borderW = 1.0;
  const auto halfBorderW = borderW / 2.;
  const auto bgRect = switchRect.toRectF().marginsRemoved({ halfBorderW, halfBorderW, halfBorderW, halfBorderW });
  p.setPen(Qt::NoPen);
  p.setBrush(bgColor);
  p.drawRoundedRect(bgRect, switchRadius, switchRadius);
  drawRoundedRectBorder(&p, switchRect, borderColor, borderW, switchRadius);

  // Draw handle.
  const auto handleXRatio = _handleAnimation.currentValue().toDouble();
  const auto handlePadding = _handlePaddingAnimation.currentValue().toInt();
  const auto handleDiameter = static_cast<double>(switchRect.height() - handlePadding * 2);
  const auto handleGrooveWidth = switchRect.width() - handlePadding * 2 - handleDiameter;
  const auto handleX = switchRect.x() + handlePadding + handleGrooveWidth * handleXRatio;
  const auto handleY = static_cast<double>(switchRect.y() + handlePadding);
  const auto handleRect = QRectF{ handleX, handleY, handleDiameter, handleDiameter };
  p.setPen(Qt::NoPen);
  p.setBrush(fgColor);
  p.drawEllipse(handleRect);
  auto availableX = switchRect.x() + switchRect.width() + spacing;
  auto availableW = contentRect.width() - switchRect.width() - spacing;

  // Draw accessibility symbols.
  if (showAccessibilitySymbols() && checkState() != Qt::PartiallyChecked) {
    constexpr auto checkThickness = 1.01; // A pen width of 1 causes visual bugs.
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen{ bgColor, checkThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin });
    if (checkState() == Qt::Checked) {
      drawCheckBoxIndicator(handleRect.toRect(), &p, _symbolAnimation.currentValue().toReal());
    } else {
      drawCloseIndicator(handleRect.toRect().marginsRemoved({ 1, 1, 1, 1 }), &p);
    }
  }

  // Draw icon.
  const auto extent = iconSize.height();
  if (hasIcon && availableW >= extent) {
    const auto pixmap =
      qlementine::getPixmap(icon(), { extent, extent }, MouseState::Normal, CheckState::Checked, this);
    const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style);
    const auto coloredPixmap = qlementineStyle ? qlementineStyle->getColorizedPixmap(
                                                   pixmap, qlementineStyle->autoIconColor(this), textColor, textColor)
                                               : pixmap;
    const auto iconX = availableX;
    const auto iconY = contentRect.y() + (contentRect.height() - extent) / 2;
    const auto iconRect = QRect{ iconX, iconY, extent, extent };
    p.drawPixmap(iconRect, coloredPixmap);

    availableX += extent + spacing;
    availableW -= extent + spacing;
  }

  // Draw text.
  if (hasText && availableW > 0) {
    const auto textX = availableX;
    const auto textY = contentRect.y();
    const auto textH = contentRect.height();
    const auto textW = availableW;
    const auto textRect = QRect{ textX, textY, textW, textH };
    const auto fm = fontMetrics();
    const auto elidedText = fm.elidedText(text(), Qt::TextElideMode::ElideRight, textRect.width(), Qt::TextSingleLine);
    p.setBrush(Qt::NoBrush);
    p.setPen(textColor);
    p.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, elidedText);
  }
}

void Switch::enterEvent(QEnterEvent* e) {
  QAbstractButton::enterEvent(e);
  _isMouseOver = true;
  startAnimation();
}

void Switch::changeEvent(QEvent* e) {
  QAbstractButton::changeEvent(e);
  const auto type = e->type();
  if (type == QEvent::Type::EnabledChange || type == QEvent::Type::PaletteChange
      || type == QEvent::Type::ApplicationPaletteChange || type == QEvent::Type::StyleChange) {
    refreshThemeMetrics();
    startAnimation();
  }
}

void Switch::focusInEvent(QFocusEvent* e) {
  QAbstractButton::focusInEvent(e);
  startAnimation();
}

void Switch::focusOutEvent(QFocusEvent* e) {
  QAbstractButton::focusOutEvent(e);
  startAnimation();
}

void Switch::checkStateSet() {
  if (_blockRefresh) {
    return;
  }
  _intermediate = false;
  const auto state = checkState();
  if (state != _publishedState) {
    _publishedState = state;
    Q_EMIT checkStateChanged(state);
  }
  startAnimation();
}

void Switch::nextCheckState() {
  if (_tristate) {
    setCheckState(static_cast<Qt::CheckState>((checkState() + 1) % 3));
  } else {
    QAbstractButton::nextCheckState();
    checkStateSet();
  }
}

void Switch::startAnimation() {
  const auto animationDuration = isVisible() ? style()->styleHint(QStyle::SH_Widget_Animation_Duration) : 0;

  const auto currentBg = _bgAnimation.currentValue();
  _bgAnimation.stop();
  _bgAnimation.setDuration(animationDuration);
  _bgAnimation.setStartValue(currentBg);
  _bgAnimation.setEndValue(getBgColor());
  _bgAnimation.start();

  const auto currentBorder = _borderAnimation.currentValue();
  _borderAnimation.stop();
  _borderAnimation.setDuration(animationDuration);
  _borderAnimation.setStartValue(currentBorder);
  _borderAnimation.setEndValue(getBorderColor());
  _borderAnimation.start();

  const auto currentFg = _fgAnimation.currentValue();
  _fgAnimation.stop();
  _fgAnimation.setDuration(animationDuration);
  _fgAnimation.setStartValue(currentFg);
  _fgAnimation.setEndValue(getFgColor());
  _fgAnimation.start();

  const auto currentXRatio = _handleAnimation.currentValue();
  const auto state = checkState();
  _handleAnimation.stop();
  _handleAnimation.setDuration(animationDuration);
  _handleAnimation.setStartValue(currentXRatio);
  _handleAnimation.setEndValue(state == Qt::Checked ? 1. : (state == Qt::Unchecked ? 0. : 0.5));
  _handleAnimation.start();

  const auto currentPadding = _handlePaddingAnimation.currentValue();
  _handlePaddingAnimation.stop();
  _handlePaddingAnimation.setDuration(animationDuration);
  _handlePaddingAnimation.setStartValue(currentPadding);
  _handlePaddingAnimation.setEndValue(_fullHandlePadding * (state == Qt::PartiallyChecked ? 3. : 1.));
  _handlePaddingAnimation.start();

  const auto currentSymbol = _symbolAnimation.currentValue();
  _symbolAnimation.stop();
  _symbolAnimation.setDuration(animationDuration);
  _symbolAnimation.setStartValue(currentSymbol);
  _symbolAnimation.setEndValue(1.);
  _symbolAnimation.start();
}

void Switch::refreshThemeMetrics() {
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  _fullHandlePadding = qlementineStyle ? qlementineStyle->theme().borderWidth * 2 : 2.0;

  if (_focusFrame) {
    _focusFrame->setRadiuses(RadiusesF{ qlementineStyle ? qlementineStyle->theme().borderRadius : 0.0 });
  }

  updateGeometry();
}

void Switch::setupAnimation() {
  constexpr auto animationDuration = 0; // Don't animate before it is shown.

  const auto& startBgColor = getBgColor();
  _bgAnimation.setDuration(animationDuration);
  _bgAnimation.setEasingCurve(QEasingCurve::Type::OutCubic);
  _bgAnimation.setStartValue(startBgColor);
  _bgAnimation.setEndValue(startBgColor);
  QObject::connect(&_bgAnimation, &QVariantAnimation::valueChanged, this, [this]() {
    update();
  });

  const auto& startBorderColor = getBorderColor();
  _borderAnimation.setDuration(animationDuration);
  _borderAnimation.setEasingCurve(QEasingCurve::Type::OutCubic);
  _borderAnimation.setStartValue(startBorderColor);
  _borderAnimation.setEndValue(startBorderColor);
  QObject::connect(&_borderAnimation, &QVariantAnimation::valueChanged, this, [this]() {
    update();
  });

  const auto& startFgColor = getFgColor();
  _fgAnimation.setDuration(animationDuration);
  _fgAnimation.setEasingCurve(QEasingCurve::Type::OutCubic);
  _fgAnimation.setStartValue(startFgColor);
  _fgAnimation.setEndValue(startFgColor);
  QObject::connect(&_fgAnimation, &QVariantAnimation::valueChanged, this, [this]() {
    update();
  });

  QObject::connect(this, &QAbstractButton::pressed, this, [this]() {
    startAnimation();
  });
  QObject::connect(this, &QAbstractButton::released, this, [this]() {
    startAnimation();
  });
  QObject::connect(this, &QAbstractButton::toggled, this, [this]() {
    startAnimation();
  });
  QObject::connect(this, &Switch::checkStateChanged, this, [this]() {
    startAnimation();
  });

  _handleAnimation.setDuration(animationDuration);
  _handleAnimation.setEasingCurve(QEasingCurve::Type::OutCubic);
  _handleAnimation.setStartValue(0.);
  _handleAnimation.setEndValue(0.);
  QObject::connect(&_handleAnimation, &QVariantAnimation::valueChanged, this, [this]() {
    update();
  });

  _handlePaddingAnimation.setDuration(animationDuration);
  _handlePaddingAnimation.setEasingCurve(QEasingCurve::Type::OutCubic);
  _handlePaddingAnimation.setStartValue(_fullHandlePadding);
  _handlePaddingAnimation.setEndValue(_fullHandlePadding);
  QObject::connect(&_handlePaddingAnimation, &QVariantAnimation::valueChanged, this, [this]() {
    update();
  });

  _symbolAnimation.setDuration(animationDuration);
  _symbolAnimation.setEasingCurve(QEasingCurve::Type::OutCubic);
  _symbolAnimation.setStartValue(0.);
  _symbolAnimation.setEndValue(0.);
  QObject::connect(&_symbolAnimation, &QVariantAnimation::valueChanged, this, [this]() {
    update();
  });
}

const QColor& Switch::getBgColor() const {
  const auto* style = this->style();
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style);
  const auto palette = style->standardPalette();
  const auto& bgColor = qlementineStyle
                          ? qlementineStyle->switchGrooveColor(
                              getMouseState(isDown(), _isMouseOver, isEnabled()), getCheckState(checkState()))
                          : palette.color(isEnabled() ? QPalette::ColorGroup::Normal : QPalette::ColorGroup::Disabled,
                              QPalette::ColorRole::Button);
  return bgColor;
}

const QColor& Switch::getBorderColor() const {
  const auto* style = this->style();
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style);
  const auto palette = style->standardPalette();
  const auto& borderColor =
    qlementineStyle ? qlementineStyle->switchGrooveBorderColor(getMouseState(isDown(), _isMouseOver, isEnabled()),
                        getFocusState(hasFocus()), getCheckState(checkState()))
                    : palette.color(isEnabled() ? QPalette::ColorGroup::Normal : QPalette::ColorGroup::Disabled,
                        QPalette::ColorRole::ButtonText);
  return borderColor;
}

const QColor& Switch::getFgColor() const {
  const auto* style = this->style();
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style);
  const auto palette = style->standardPalette();
  const auto& fgColor = qlementineStyle
                          ? qlementineStyle->switchHandleColor(
                              getMouseState(isDown(), _isMouseOver, isEnabled()), getCheckState(checkState()))
                          : palette.color(isEnabled() ? QPalette::ColorGroup::Normal : QPalette::ColorGroup::Disabled,
                              QPalette::ColorRole::ButtonText);
  return fgColor;
}

const QColor& Switch::getTextColor() const {
  const auto* style = this->style();
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style);
  const auto palette = style->standardPalette();
  const auto& textColor =
    qlementineStyle ? qlementineStyle->labelForegroundColor(getMouseState(isDown(), _isMouseOver, isEnabled()), this)
                    : palette.color(isEnabled() ? QPalette::ColorGroup::Normal : QPalette::ColorGroup::Disabled,
                        QPalette::ColorRole::Text);
  return textColor;
}

QRect Switch::getSwitchRect() const {
  const auto* style = this->style();
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style);
  const auto contentRect = rect();
  const auto switchW = qlementineStyle ? qlementineStyle->theme().controlHeightLarge : 28;
  const auto switchH = qlementineStyle ? qlementineStyle->theme().controlHeightSmall : 16;
  const auto switchX = contentRect.x();
  const auto switchY = contentRect.y() + (contentRect.height() - switchH) / 2;
  const auto switchRect = QRect{ switchX, switchY, switchW, switchH };
  return switchRect;
}

void Switch::leaveEvent(QEvent* e) {
  QAbstractButton::leaveEvent(e);
  _isMouseOver = false;
  startAnimation();
}

void Switch::initStyleOptionFocus(QStyleOptionFocusRoundedRect& opt) const {
  const auto* style = this->style();
  const auto deltaX = style->pixelMetric(QStyle::PM_FocusFrameHMargin, &opt, this);
  const auto deltaY = style->pixelMetric(QStyle::PM_FocusFrameVMargin, &opt, this);
  const auto switchRect = getSwitchRect();
  opt.rect = switchRect.marginsAdded({ deltaX / 2, deltaY / 2, deltaX / 2, deltaY / 2 }).translated(deltaX, deltaY);
  opt.radiuses = switchRect.height() / 2.;
}

void Switch::setTristate(bool tristate) {
  _tristate = tristate;
  update();
  Q_EMIT tristateChanged(tristate);
}

bool Switch::isTristate() const {
  return _tristate;
}

void Switch::setShowAccessibilitySymbols(bool showAccessibilitySymbols) {
  _showAccessibilitySymbols = showAccessibilitySymbols;
  update();
  Q_EMIT showAccessibilitySymbolsChanged(showAccessibilitySymbols);
}

bool Switch::showAccessibilitySymbols() const {
  return _showAccessibilitySymbols;
}

Qt::CheckState Switch::checkState() const {
  if (_tristate && _intermediate) {
    return Qt::PartiallyChecked;
  }
  return isChecked() ? Qt::Checked : Qt::Unchecked;
}

void Switch::setCheckState(Qt::CheckState state) {
  // Heavily inspired from tristate QCheckBox code.
  if (state == Qt::PartiallyChecked) {
    _tristate = true;
    _intermediate = true;
  } else {
    _intermediate = false;
  }
  _blockRefresh = true;
  setChecked(state != Qt::Unchecked);
  _blockRefresh = false;
  update();

  if (state != _publishedState) {
    _publishedState = state;
    Q_EMIT checkStateChanged(state);
  }
}

} // namespace oclero::qlementine
