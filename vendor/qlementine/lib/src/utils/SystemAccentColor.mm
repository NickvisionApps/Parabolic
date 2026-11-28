// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include "SystemAccentColor.hpp"

#include <AppKit/AppKit.h>

namespace oclero::qlementine {
QColor systemAccentColor(bool dark) {
  __block QColor result;
  NSAppearance* appearance = [NSAppearance appearanceNamed:dark ? NSAppearanceNameDarkAqua : NSAppearanceNameAqua];
  [appearance performAsCurrentDrawingAppearance:^{
    NSColor* color = [[NSColor controlAccentColor] colorUsingColorSpace:[NSColorSpace sRGBColorSpace]];
    result = QColor::fromRgbF(color.redComponent, color.greenComponent, color.blueComponent);
  }];
  return result;
}
} // namespace oclero::qlementine
