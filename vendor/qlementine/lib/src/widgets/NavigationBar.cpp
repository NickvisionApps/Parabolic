// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/NavigationBar.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/utils/ImageUtils.hpp>

#include <QPainter>

namespace oclero::qlementine {
const QColor& NavigationBar::getBgColor(const Theme& theme) const {
  return theme.backgroundColorMain2;
}

const QColor& NavigationBar::getItemBgColor(MouseState mouse, const Theme& theme) const {
  switch (mouse) {
    case MouseState::Hovered:
      return theme.backgroundColorMain3;
    case MouseState::Pressed:
      return theme.backgroundColorMain4;
    default:
      return theme.backgroundColorMainTransparent;
  }
}

const QColor& NavigationBar::getItemFgColor(MouseState mouse, bool /*selected*/, const Theme& theme) const {
  if (mouse == MouseState::Disabled)
    return theme.secondaryColorDisabled;
  else
    return theme.secondaryColor;
}

const QColor& NavigationBar::getItemBadgeBgColor(MouseState mouse, bool /*selected*/, const Theme& theme) const {
  if (mouse == MouseState::Disabled)
    return theme.secondaryAlternativeColorDisabled;
  else
    return theme.secondaryAlternativeColor;
}

const QColor& NavigationBar::getItemBadgeFgColor(MouseState mouse, bool /*selected*/, const Theme& theme) const {
  if (mouse == MouseState::Disabled)
    return theme.secondaryColorForegroundDisabled;
  else
    return theme.secondaryColorForeground;
}

QMargins NavigationBar::getItemPadding() const {
  const auto* style = this->style();
  const auto left = style->pixelMetric(QStyle::PM_LayoutLeftMargin);
  const auto top = style->pixelMetric(QStyle::PM_LayoutTopMargin) / 2;
  const auto right = style->pixelMetric(QStyle::PM_LayoutRightMargin);
  const auto bottom = style->pixelMetric(QStyle::PM_LayoutBottomMargin) / 2;
  return QMargins{ left, top, right, bottom };
}

double NavigationBar::getItemRadius() const {
  return 0.0;
}

QMargins NavigationBar::getPadding() const {
  return QMargins{ 0, 0, 0, 0 };
}

int NavigationBar::getSpacing() const {
  return 0.0;
}

int NavigationBar::getItemMinimumHeight() const {
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  return qlementineStyle ? qlementineStyle->theme().controlHeightMedium * 2 : 48;
}

void NavigationBar::drawCurrentItemIndicator(QPainter& p) const {
  const auto currentItemRect = getAnimatedCurrentItemRect();
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  const auto thickness = qlementineStyle ? qlementineStyle->theme().borderWidth * 3 : 3;
  const auto rect = QRect{ currentItemRect.x(), currentItemRect.y() + currentItemRect.height() - thickness,
    currentItemRect.width(), thickness };
  const auto& color = getCurrentItemIndicatorColor();
  p.setPen(Qt::NoPen);
  p.fillRect(rect, color);
}
QFont NavigationBar::labelFont() const {
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
  const auto& biggerFont = qlementineStyle ? qlementineStyle->fontForTextRole(qlementine::TextRole::H5) : this->font();
  return biggerFont;
}
} // namespace oclero::qlementine
