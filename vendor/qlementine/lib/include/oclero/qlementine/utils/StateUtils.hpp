// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>
#include <oclero/qlementine/style/Theme.hpp>

#include <QStyle>
#include <QStyleOption>

namespace oclero::qlementine {
QLEMENTINE_EXPORT MouseState getMouseState(QStyle::State const& state);
QLEMENTINE_EXPORT MouseState getMouseState(bool const pressed, bool const hovered, bool const enabled);
QLEMENTINE_EXPORT MouseState getToolButtonMouseState(QStyle::State const& state);
QLEMENTINE_EXPORT MouseState getMenuItemMouseState(QStyle::State const& state);
QLEMENTINE_EXPORT MouseState getComboBoxItemMouseState(QStyle::State const& state);
QLEMENTINE_EXPORT MouseState getTabItemMouseState(QStyle::State const& state, const bool tabIsHovered);
QLEMENTINE_EXPORT ColorRole getColorRole(QStyle::State const& state, bool const isDefault);
QLEMENTINE_EXPORT ColorRole getColorRole(bool checked, bool const isDefault);
QLEMENTINE_EXPORT ColorRole getColorRole(CheckState const checked);
QLEMENTINE_EXPORT MouseState getSliderHandleState(
  QStyle::State const& state, QStyle::SubControls const activeSubControls);
QLEMENTINE_EXPORT MouseState getScrollBarHandleState(
  QStyle::State const& state, QStyle::SubControls const activeSubControls);
QLEMENTINE_EXPORT FocusState getFocusState(QStyle::State const& state);
QLEMENTINE_EXPORT FocusState getFocusState(bool focused);
QLEMENTINE_EXPORT CheckState getCheckState(QStyle::State const& state);
QLEMENTINE_EXPORT CheckState getCheckState(Qt::CheckState const& state);
QLEMENTINE_EXPORT CheckState getCheckState(bool checked);
QLEMENTINE_EXPORT ActiveState getActiveState(QStyle::State const& state);
QLEMENTINE_EXPORT SelectionState getSelectionState(QStyle::State const& state);
QLEMENTINE_EXPORT AlternateState getAlternateState(QStyleOptionViewItem::ViewItemFeatures const& state);
QLEMENTINE_EXPORT QStyle::State getState(const bool enabled, const bool hover, const bool pressed);
QLEMENTINE_EXPORT QIcon::Mode getIconMode(MouseState const mouse);
QLEMENTINE_EXPORT QIcon::State getIconState(CheckState const checked);
QLEMENTINE_EXPORT QPalette::ColorGroup getPaletteColorGroup(QStyle::State const& state);
QLEMENTINE_EXPORT QPalette::ColorGroup getPaletteColorGroup(MouseState const mouse);

QLEMENTINE_EXPORT QString mouseStateToString(MouseState const state);
QLEMENTINE_EXPORT QString focusStateToString(FocusState const state);
QLEMENTINE_EXPORT QString activeStateToString(ActiveState const state);
QLEMENTINE_EXPORT QString selectionStateToString(SelectionState const state);
QLEMENTINE_EXPORT QString checkStateToString(CheckState const state);
QLEMENTINE_EXPORT QString printState(QStyle::State const& state);
} // namespace oclero::qlementine
