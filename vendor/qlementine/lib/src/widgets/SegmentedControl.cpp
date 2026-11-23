// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/SegmentedControl.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/widgets/RoundedFocusFrame.hpp>
#include <oclero/qlementine/utils/ImageUtils.hpp>

namespace oclero::qlementine {
const QColor& SegmentedControl::getBgColor(const Theme& theme) const {
  return isEnabled() ? theme.backgroundColorMain4 : theme.backgroundColorMain2;
}

const QColor& SegmentedControl::getItemBgColor(MouseState mouse, const Theme& theme) const {
  switch (mouse) {
    case MouseState::Hovered:
      return theme.neutralColor;
    case MouseState::Pressed:
      return theme.neutralColorHovered;
    default:
      return theme.neutralColorTransparent;
  }
}

const QColor& SegmentedControl::getItemFgColor(MouseState mouse, bool selected, const Theme& theme) const {
  switch (mouse) {
    case MouseState::Hovered:
      return selected ? theme.primaryColorForegroundHovered : theme.secondaryColor;
    case MouseState::Pressed:
      return selected ? theme.primaryColorForegroundPressed : theme.secondaryColor;
    case MouseState::Disabled:
      return selected ? theme.primaryColorForegroundDisabled : theme.secondaryColorDisabled;
    default:
      return selected ? theme.primaryColorForeground : theme.secondaryColor;
  }
}

const QColor& SegmentedControl::getItemBadgeBgColor(MouseState mouse, bool selected, const Theme& theme) const {
  if (mouse == MouseState::Disabled)
    return selected ? theme.primaryAlternativeColorDisabled : theme.secondaryAlternativeColorDisabled;
  else
    return selected ? theme.primaryAlternativeColor : theme.secondaryAlternativeColor;
}

const QColor& SegmentedControl::getItemBadgeFgColor(MouseState mouse, bool selected, const Theme& theme) const {
  if (mouse == MouseState::Disabled)
    return selected ? theme.primaryColorForegroundDisabled : theme.secondaryColorForegroundDisabled;
  else
    return selected ? theme.primaryColorForeground : theme.secondaryColorForeground;
}

void SegmentedControl::initStyleOptionFocus(QStyleOptionFocusRoundedRect& opt) const {
  const auto* style = this->style();
  const auto deltaX = style->pixelMetric(QStyle::PM_FocusFrameHMargin, &opt, this);
  const auto deltaY = style->pixelMetric(QStyle::PM_FocusFrameVMargin, &opt, this);
  opt.rect =
    getFocusedItemRect().translated(deltaX, deltaY).marginsAdded({ deltaX / 2, deltaY / 2, deltaX / 2, deltaY / 2 });
  opt.radiuses = getItemRadius();
}
} // namespace oclero::qlementine
