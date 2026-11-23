// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/utils/StateUtils.hpp>

namespace oclero::qlementine {
MouseState getMouseState(QStyle::State const& state) {
  if (!state.testFlag(QStyle::State_Enabled)) {
    return MouseState::Disabled;
  } else if (state.testFlag(QStyle::State_Sunken)) {
    return MouseState::Pressed;
  } else if (state.testFlag(QStyle::State_MouseOver)) {
    return MouseState::Hovered;
  } else {
    return MouseState::Normal;
  }
}

MouseState getMouseState(bool const pressed, bool const hovered, bool const enabled) {
  if (!enabled)
    return MouseState::Disabled;
  if (pressed)
    return MouseState::Pressed;
  if (hovered)
    return MouseState::Hovered;
  else
    return MouseState::Normal;
}

MouseState getToolButtonMouseState(QStyle::State const& state) {
  if (!state.testFlag(QStyle::State_Enabled)) {
    return MouseState::Disabled;
  } else if (state.testFlag(QStyle::State_Sunken)) {
    return MouseState::Pressed;
  } else if (state.testFlag(QStyle::State_MouseOver) || state.testFlag(QStyle::State_Selected)) {
    return MouseState::Hovered;
  } else {
    return MouseState::Transparent;
  }
}

MouseState getMenuItemMouseState(QStyle::State const& state) {
  if (!state.testFlag(QStyle::State_Enabled)) {
    return MouseState::Disabled;
  } else if (state.testFlag(QStyle::State_Sunken)) {
    return MouseState::Pressed;
  } else if (state.testFlag(QStyle::State_MouseOver) || state.testFlag(QStyle::State_Selected)) {
    return MouseState::Hovered;
  } else {
    return MouseState::Transparent;
  }
}

MouseState getComboBoxItemMouseState(QStyle::State const& state) {
  if (!state.testFlag(QStyle::State_Enabled)) {
    return MouseState::Disabled;
  } else if (state.testFlag(QStyle::State_Sunken)) {
    return MouseState::Pressed;
  } else if (state.testFlag(QStyle::State_Selected)) {
    return MouseState::Hovered;
  } else {
    return MouseState::Transparent;
  }
}

MouseState getTabItemMouseState(QStyle::State const& state, const bool tabIsHovered) {
  const auto selected = state.testFlag(QStyle::State_Selected);
  if (selected || tabIsHovered) {
    if (!state.testFlag(QStyle::State_Enabled)) {
      return MouseState::Disabled;
    } else if (state.testFlag(QStyle::State_Sunken)) {
      return MouseState::Pressed;
    } else if (state.testFlag(QStyle::State_MouseOver)) {
      return MouseState::Hovered;
    } else {
      return MouseState::Normal;
    }
  } else {
    return state.testFlag(QStyle::State_Enabled) ? MouseState::Transparent : MouseState::Disabled;
  }
}

ColorRole getColorRole(QStyle::State const& state, bool const isDefault) {
  return getColorRole(state.testFlag(QStyle::State_On), isDefault);
}

ColorRole getColorRole(bool checked, bool const isDefault) {
  return checked || isDefault ? ColorRole::Primary : ColorRole::Secondary;
}

ColorRole getColorRole(CheckState const checked) {
  return getColorRole(checked == CheckState::Checked, false);
}

MouseState getSliderHandleState(QStyle::State const& state, QStyle::SubControls const activeSubControls) {
  const auto handleActive = activeSubControls == QStyle::SC_SliderHandle && state;
  return handleActive ? getMouseState(state) : MouseState::Normal;
}

MouseState getScrollBarHandleState(QStyle::State const& state, QStyle::SubControls const activeSubControls) {
  const auto handleActive = activeSubControls == QStyle::SC_ScrollBarSlider && state;
  if (handleActive) {
    return getMouseState(state);
  } else {
    if (!state.testFlag(QStyle::State_Enabled)) {
      return MouseState::Disabled;
    } else if (state.testFlag(QStyle::State_MouseOver)) {
      return MouseState::Normal;
    } else {
      return MouseState::Transparent;
    }
  }
}

FocusState getFocusState(QStyle::State const& state) {
  return state.testFlag(QStyle::State_HasFocus) ? FocusState::Focused : FocusState::NotFocused;
}

FocusState getFocusState(bool focused) {
  return focused ? FocusState::Focused : FocusState::NotFocused;
}

CheckState getCheckState(QStyle::State const& state) {
  if (state.testFlag(QStyle::State_On)) {
    return CheckState::Checked;
  } else if (state.testFlag(QStyle::State_NoChange)) {
    return CheckState::Indeterminate;
  } else {
    return CheckState::NotChecked;
  }
}

CheckState getCheckState(Qt::CheckState const& state) {
  if (state == Qt::Checked) {
    return CheckState::Checked;
  } else if (state == Qt::PartiallyChecked) {
    return CheckState::Indeterminate;
  } else {
    return CheckState::NotChecked;
  }
}

CheckState getCheckState(bool checked) {
  return checked ? CheckState::Checked : CheckState::NotChecked;
}

ActiveState getActiveState(QStyle::State const& state) {
  return state.testFlag(QStyle::State_Active) ? ActiveState::Active : ActiveState::NotActive;
}

SelectionState getSelectionState(QStyle::State const& state) {
  return state.testFlag(QStyle::State_Selected) ? SelectionState::Selected : SelectionState::NotSelected;
}

AlternateState getAlternateState(QStyleOptionViewItem::ViewItemFeatures const& state) {
  return state.testFlag(QStyleOptionViewItem::Alternate) ? AlternateState::Alternate : AlternateState::NotAlternate;
}

QStyle::State getState(const bool enabled, const bool hover, const bool pressed) {
  QStyle::State result;
  result.setFlag(QStyle::State_Enabled, enabled);
  result.setFlag(QStyle::State_Sunken, pressed);
  result.setFlag(QStyle::State_MouseOver, hover);
  return result;
}

QIcon::Mode getIconMode(MouseState const mouse) {
  switch (mouse) {
    case MouseState::Disabled:
      return QIcon::Mode::Disabled;
    case MouseState::Hovered:
    case MouseState::Pressed:
      return QIcon::Mode::Active;
    case MouseState::Normal:
    case MouseState::Transparent:
    default:
      return QIcon::Mode::Normal;
  }
}

QIcon::State getIconState(CheckState const checked) {
  switch (checked) {
    case CheckState::Checked:
      return QIcon::State::On;
    case CheckState::NotChecked:
    case CheckState::Indeterminate:
    default:
      return QIcon::State::Off;
  }
}

QPalette::ColorGroup getPaletteColorGroup(QStyle::State const& state) {
  if (!state.testFlag(QStyle::State_Enabled)) {
    return QPalette::ColorGroup::Disabled;
  } else {
    return QPalette::ColorGroup::Normal;
  }
}

QPalette::ColorGroup getPaletteColorGroup(MouseState const mouse) {
  switch (mouse) {
    case MouseState::Disabled:
      return QPalette::ColorGroup::Disabled;
    case MouseState::Hovered:
      return QPalette::ColorGroup::Current;
    case MouseState::Pressed:
      return QPalette::ColorGroup::Active;
    default:
      return QPalette::ColorGroup::Normal;
  }
}

QString mouseStateToString(MouseState const state) {
  switch (state) {
    case MouseState::Disabled:
      return "disabled";
    case MouseState::Hovered:
      return "hovered";
    case MouseState::Normal:
      return "normal";
    case MouseState::Pressed:
      return "pressed";
    case MouseState::Transparent:
      return "transparent";
    default:
      return {};
  }
}

QString focusStateToString(FocusState const state) {
  switch (state) {
    case FocusState::Focused:
      return "focused";
    case FocusState::NotFocused:
      return "not focused";
    default:
      return {};
  }
}

QString activeStateToString(ActiveState const state) {
  switch (state) {
    case ActiveState::Active:
      return "active";
    case ActiveState::NotActive:
      return "not active";
    default:
      return {};
  }
}

QString selectionStateToString(SelectionState const state) {
  switch (state) {
    case SelectionState::Selected:
      return "selected";
    case SelectionState::NotSelected:
      return "not selected";
    default:
      return {};
  }
}

QString checkStateToString(CheckState const state) {
  switch (state) {
    case CheckState::Checked:
      return "checked";
    case CheckState::NotChecked:
      return "not checked";
    case CheckState::Indeterminate:
      return "indeterminate";
    default:
      return {};
  }
}

QString printState(QStyle::State const& state) {
  QString result;

  const auto mouse = getMouseState(state);
  const auto focused = getFocusState(state);
  const auto active = getActiveState(state);
  const auto selected = getSelectionState(state);
  const auto checked = getCheckState(state);

  result += "{ ";
  result += "mouse: " + mouseStateToString(mouse) + ", ";
  result += "focus: " + focusStateToString(focused) + ", ";
  result += "active: " + activeStateToString(active) + ", ";
  result += "selected: " + selectionStateToString(selected) + ", ";
  result += "checked: " + checkStateToString(checked);
  result += " }";

  return result;
}
} // namespace oclero::qlementine
