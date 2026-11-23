// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <QMetaType>

namespace oclero::qlementine {
/// Color family to highlght or not the widget.
enum class ColorRole {
  Primary,
  Secondary,
};

/// Mouse interaction state.
enum class MouseState {
  Transparent,
  Normal,
  Hovered,
  Pressed,
  Disabled,
};

/// Is the widget checked or not.
enum class CheckState {
  NotChecked,
  Checked,
  Indeterminate,
};

/// Has the widget keyboard focus or not.
enum class FocusState {
  NotFocused,
  Focused,
};

/// Is the list element the current item.
enum class ActiveState {
  NotActive,
  Active,
};

/// Is the widget selected or not.
enum class SelectionState {
  NotSelected,
  Selected,
};

/// Does the ListView row need to be painted in alternate color or not.
enum class AlternateState {
  NotAlternate,
  Alternate,
};

/// Does the button is the default button.
enum class DefaultState {
  NotDefault,
  Default,
};

/// Feedback status displayed to the user.
enum class Status {
  Default,
  Info,
  Success,
  Warning,
  Error,
};

/// Role given to the text or QLabel.
enum class TextRole : int {
  Caption = -1,
  Default = 0,
  H1,
  H2,
  H3,
  H4,
  H5,
};

/// Color mode.
enum class ColorMode {
  RGB,
  RGBA,
};

} // namespace oclero::qlementine

Q_DECLARE_METATYPE(oclero::qlementine::Status);
Q_DECLARE_METATYPE(oclero::qlementine::ColorMode);
