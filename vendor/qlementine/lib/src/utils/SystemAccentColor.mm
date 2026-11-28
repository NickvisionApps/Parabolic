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

SystemAccentWatcher& SystemAccentWatcher::instance() {
  static SystemAccentWatcher watcher;
  return watcher;
}

SystemAccentWatcher::SystemAccentWatcher()
  : QObject(nullptr) {
  NSDistributedNotificationCenter* center = [NSDistributedNotificationCenter defaultCenter];
  for (NSString* name in @[ @"AppleColorPreferencesChangedNotification", @"AppleInterfaceThemeChangedNotification" ]) {
    [center addObserverForName:name
                        object:nil
                         queue:[NSOperationQueue mainQueue]
                    usingBlock:^(NSNotification*) {
                      Q_EMIT changed();
                    }];
  }
}
} // namespace oclero::qlementine
