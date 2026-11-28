// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/PillButton.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/utils/ImageUtils.hpp>

#include <QFontMetrics>
#include <QPainter>
#include <QPaintEvent>

#include <algorithm>

namespace oclero::qlementine {
namespace {
constexpr auto horizontalPadding = 12;
constexpr auto verticalPadding = 5;
constexpr auto contentSpacing = 6;
constexpr auto badgeDiameter = 7;
} // namespace

PillButton::PillButton(QWidget* parent)
  : QPushButton(parent) {
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

PillButton::PillButton(const QString& text, QWidget* parent)
  : PillButton(parent) {
  setText(text);
}

PillButton::PillButton(const QIcon& icon, const QString& text, QWidget* parent)
  : PillButton(text, parent) {
  setIcon(icon);
}

bool PillButton::isBadgeVisible() const {
  return _badgeVisible;
}

void PillButton::setBadgeVisible(bool visible) {
  if (visible != _badgeVisible) {
    _badgeVisible = visible;
    updateGeometry();
    update();
    Q_EMIT badgeVisibleChanged();
  }
}

QFont PillButton::pillFont() const {
  auto result = font();
  const auto* qlementine = qobject_cast<const QlementineStyle*>(style());
  if (qlementine) {
    result = qlementine->fontForTextRole(TextRole::Caption);
  }
  result.setBold(true);
  return result;
}

QSize PillButton::sizeHint() const {
  const QFontMetrics metrics(pillFont());
  const auto hasIcon = !icon().isNull();
  const auto hasText = !text().isEmpty();

  auto width = horizontalPadding * 2;
  auto height = metrics.height();

  if (hasIcon) {
    width += iconSize().width();
    height = std::max(height, iconSize().height());
  }
  if (hasText) {
    width += (hasIcon ? contentSpacing : 0) + metrics.horizontalAdvance(text());
  }
  if (_badgeVisible) {
    width += ((hasIcon || hasText) ? contentSpacing : 0) + badgeDiameter;
  }

  return QSize(width, height + verticalPadding * 2);
}

QSize PillButton::minimumSizeHint() const {
  return sizeHint();
}

void PillButton::paintEvent(QPaintEvent* /*e*/) {
  const auto* qlementine = qobject_cast<const QlementineStyle*>(style());
  const auto mouse = !isEnabled()   ? MouseState::Disabled
                     : isDown()     ? MouseState::Pressed
                     : underMouse() ? MouseState::Hovered
                                    : MouseState::Normal;

  auto foreground = QColor(palette().color(QPalette::ButtonText));
  auto background = foreground;
  auto badgeColor = QColor(Qt::red);
  if (qlementine) {
    const auto& theme = qlementine->theme();
    switch (mouse) {
      case MouseState::Hovered:
        foreground = theme.primaryColorHovered;
        break;
      case MouseState::Pressed:
        foreground = theme.primaryColorPressed;
        break;
      case MouseState::Disabled:
        foreground = theme.primaryColorDisabled;
        break;
      default:
        foreground = theme.primaryColor;
        break;
    }
    background = foreground;
    badgeColor =
      qlementine->statusColor(Status::Error, mouse == MouseState::Disabled ? MouseState::Disabled : MouseState::Normal);
  }
  background.setAlphaF(mouse == MouseState::Pressed ? 0.30 : mouse == MouseState::Hovered ? 0.22 : 0.14);

  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);

  const auto bounds = QRectF(rect());
  p.setPen(Qt::NoPen);
  p.setBrush(background);
  p.drawRoundedRect(bounds, bounds.height() / 2., bounds.height() / 2.);

  if (hasFocus() && !isDown() && qlementine) {
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(qlementine->theme().focusColor, 2.));
    p.drawRoundedRect(bounds.adjusted(1., 1., -1., -1.), bounds.height() / 2., bounds.height() / 2.);
  }

  const auto font = pillFont();
  const QFontMetrics metrics(font);
  const auto hasIcon = !icon().isNull();
  const auto hasText = !text().isEmpty();

  const auto sizeHintWidth = sizeHint().width();
  auto x = (width() - sizeHintWidth) / 2 + horizontalPadding;
  const auto centerY = height() / 2;

  if (hasIcon) {
    const auto size = iconSize();
    const auto pixmap = icon().pixmap(size, devicePixelRatioF(), isEnabled() ? QIcon::Normal : QIcon::Disabled);
    const auto tinted = colorizePixmap(pixmap, foreground);
    p.drawPixmap(QPointF(x, centerY - size.height() / 2.), tinted);
    x += size.width() + (hasText ? contentSpacing : 0);
  }

  if (hasText) {
    p.setFont(font);
    p.setPen(foreground);
    const auto textWidth = metrics.horizontalAdvance(text());
    const auto textRect = QRect(x, 0, textWidth, height());
    p.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text());
    x += textWidth;
  }

  if (_badgeVisible) {
    x += (hasIcon || hasText) ? contentSpacing : 0;
    p.setPen(Qt::NoPen);
    p.setBrush(badgeColor);
    p.drawEllipse(QRectF(x, centerY - badgeDiameter / 2., badgeDiameter, badgeDiameter));
  }
}
} // namespace oclero::qlementine
