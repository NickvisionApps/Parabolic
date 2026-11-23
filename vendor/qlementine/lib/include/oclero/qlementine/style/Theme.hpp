// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <optional>

#include <oclero/qlementine/qlementine_export.h>
#include <oclero/qlementine/Common.hpp>

#include <QString>
#include <QRgb>
#include <QJsonDocument>
#include <QColor>
#include <QFont>
#include <QSize>
#include <QPalette>

namespace oclero::qlementine {
/// Metadata about a Qlementine Theme.
struct ThemeMeta {
  QString name;
  QString version;
  QString author;

  inline bool operator==(const ThemeMeta& other) const {
    return name == other.name && version == other.version && author == other.author;
  }
  inline bool operator!=(const ThemeMeta& other) const {
    return !(*this == other);
  }
};

/// Color and sizes configuration for a Qlementine Theme.
class QLEMENTINE_EXPORT Theme {
public: // Ctor.
  Theme();

  static std::optional<Theme> fromJsonPath(const QString& jsonPath);
  static std::optional<Theme> fromJsonString(const QString& jsonString);
  static std::optional<Theme> fromJsonDoc(const QJsonDocument& jsonDoc);
  static Theme makeLight();
  static Theme makeDark();
  static Theme makeLegacyDark();

  /// Gets a copy of this theme where every primary color (and the focus color) is derived from the accent color.
  Theme withAccentColor(const QColor& accentColor) const;

  Theme(Theme const& other) = default;
  Theme(Theme&& other) noexcept = default;
  virtual ~Theme() = default;

public: // Operators.
  Theme& operator=(Theme const& other) = default;
  Theme& operator=(Theme&& other) = default;
  bool operator==(const Theme& other) const;
  bool operator!=(const Theme& other) const;

public: // Values.
  ThemeMeta meta;

  QColor backgroundColorMain1{ 0xffffff };
  QColor backgroundColorMain2{ 0xf3f3f3 };
  QColor backgroundColorMain3{ 0xe3e3e3 };
  QColor backgroundColorMain4{ 0xdcdcdc };
  QColor backgroundColorMainTransparent{ QRgba64::fromArgb32(0x00fafafa) };

  QColor backgroundColorWorkspace{ 0xb7b7b7 };
  QColor backgroundColorTabBar{ 0xb7b7b7 };

  QColor neutralColor{ 0xe1e1e1 };
  QColor neutralColorHovered{ 0xdadada };
  QColor neutralColorPressed{ 0xd2d2d2 };
  QColor neutralColorDisabled{ 0xeeeeee };
  QColor neutralColorTransparent{ QRgba64::fromArgb32(0x00E1E1E1) };

  QColor focusColor{ QRgba64::fromArgb32(0x6640a9ff) };

  QColor primaryColor{ 0x1890ff };
  QColor primaryColorHovered{ 0x2c9dff };
  QColor primaryColorPressed{ 0x40a9ff };
  QColor primaryColorDisabled{ 0xd1e9ff };
  QColor primaryColorTransparent{ QRgba64::fromArgb32(0x001890FF) };

  QColor primaryColorForeground{ 0xffffff };
  QColor primaryColorForegroundHovered{ 0xffffff };
  QColor primaryColorForegroundPressed{ 0xffffff };
  QColor primaryColorForegroundDisabled{ 0xecf6ff };
  QColor primaryColorForegroundTransparent{ QRgba64::fromArgb32(0x00ffffff) };

  QColor primaryAlternativeColor{ 0x106ef9 };
  QColor primaryAlternativeColorHovered{ 0x107bfd };
  QColor primaryAlternativeColorPressed{ 0x108bfd };
  QColor primaryAlternativeColorDisabled{ 0xa9d6ff };
  QColor primaryAlternativeColorTransparent{ QRgba64::fromArgb32(0x001875ff) };

  QColor secondaryColor{ 0x404040 };
  QColor secondaryColorHovered{ 0x333333 };
  QColor secondaryColorPressed{ 0x262626 };
  QColor secondaryColorDisabled{ 0xd4d4d4 };
  QColor secondaryColorTransparent{ QRgba64::fromArgb32(0x00404040) };

  QColor secondaryColorForeground{ 0xffffff };
  QColor secondaryColorForegroundHovered{ 0xffffff };
  QColor secondaryColorForegroundPressed{ 0xffffff };
  QColor secondaryColorForegroundDisabled{ 0xededed };
  QColor secondaryColorForegroundTransparent{ QRgba64::fromArgb32(0x00ffffff) };

  QColor secondaryAlternativeColor{ 0x909090 };
  QColor secondaryAlternativeColorHovered{ 0x747474 };
  QColor secondaryAlternativeColorPressed{ 0x828282 };
  QColor secondaryAlternativeColorDisabled{ 0xc3c3c3 };
  QColor secondaryAlternativeColorTransparent{ QRgba64::fromArgb32(0x00909090) };

  QColor statusColorSuccess{ 0x2bb5a0 };
  QColor statusColorSuccessHovered{ 0x3cbfab };
  QColor statusColorSuccessPressed{ 0x4ecdb9 };
  QColor statusColorSuccessDisabled{ 0xd5f0ec };
  QColor statusColorInfo{ 0x1ba8d5 };
  QColor statusColorInfoHovered{ 0x1eb5e5 };
  QColor statusColorInfoPressed{ 0x29c0f0 };
  QColor statusColorInfoDisabled{ 0xc7eaf5 };
  QColor statusColorWarning{ 0xfbc064 };
  QColor statusColorWarningHovered{ 0xffcf6c };
  QColor statusColorWarningPressed{ 0xffd880 };
  QColor statusColorWarningDisabled{ 0xfeefd8 };
  QColor statusColorError{ 0xe96b72 };
  QColor statusColorErrorHovered{ 0xf47c83 };
  QColor statusColorErrorPressed{ 0xff9197 };
  QColor statusColorErrorDisabled{ 0xf9dadc };
  QColor statusColorForeground{ 0xffffff };
  QColor statusColorForegroundHovered{ 0xffffff };
  QColor statusColorForegroundPressed{ 0xffffff };
  QColor statusColorForegroundDisabled{ QRgba64::fromArgb32(0x99ffffff) };

  QColor shadowColor1{ QRgba64::fromArgb32(0x20000000) };
  QColor shadowColor2{ QRgba64::fromArgb32(0x40000000) };
  QColor shadowColor3{ QRgba64::fromArgb32(0x60000000) };
  QColor shadowColorTransparent{ QRgba64::fromArgb32(0x00000000) };

  QColor borderColor{ 0xd3d3d3 };
  QColor borderColorHovered{ 0xb3b3b3 };
  QColor borderColorPressed{ 0xa3a3a3 };
  QColor borderColorDisabled{ 0xe9e9e9 };
  QColor borderColorTransparent{ QRgba64::fromArgb32(0x00d3d3d3) };

  QColor titleBarForegroundColor{ 0x404040 };
  QColor titleBarBorderColor{ 0xd3d3d3 };

  QColor semiTransparentColor1{ QRgba64::fromArgb32(0x0000000) };
  QColor semiTransparentColor2{ QRgba64::fromArgb32(0x19000000) };
  QColor semiTransparentColor3{ QRgba64::fromArgb32(0x21000000) };
  QColor semiTransparentColor4{ QRgba64::fromArgb32(0x28000000) };
  QColor semiTransparentColorTransparent{ QRgba64::fromArgb32(0x00000000) };

  bool useSystemFonts{ false };

  int fontSize{ 12 };
  int fontSizeMonospace{ 13 };
  int fontSizeH1{ 34 };
  int fontSizeH2{ 26 };
  int fontSizeH3{ 22 };
  int fontSizeH4{ 18 };
  int fontSizeH5{ 14 };
  int fontSizeS1{ 10 };
  int animationDuration{ 192 };
  int focusAnimationDuration{ 384 };
  int sliderAnimationDuration{ 96 };
  double borderRadius{ 6. };
  double checkBoxBorderRadius{ 4. };
  double menuItemBorderRadius{ 4. };
  double menuBarItemBorderRadius{ 2. };
  int borderWidth{ 1 };
  int controlHeightLarge{ 28 };
  int controlHeightMedium{ 24 };
  int controlHeightSmall{ 16 };
  int controlDefaultWidth{ 96 };
  int dialMarkLength{ 8 };
  int dialMarkThickness{ 2 };
  int dialTickLength{ 4 };
  int dialTickSpacing{ 4 };
  int dialGrooveThickness{ 4 };
  int focusBorderWidth{ 2 };
  QSize iconSize{ 16, 16 };
  QSize iconSizeMedium{ 24, 24 };
  QSize iconSizeLarge{ 24, 24 };
  QSize iconSizeExtraSmall{ 12, 12 };
  int sliderTickSize{ 3 };
  int sliderTickSpacing{ 2 };
  int sliderTickThickness{ 1 };
  int sliderGrooveHeight{ 4 };
  int progressBarGrooveHeight{ 6 };
  int spacing{ 8 };
  int scrollBarThicknessFull{ 12 };
  int scrollBarThicknessSmall{ 6 };
  int scrollBarMargin{ 0 };
  int tabBarPaddingTop{ 4 };
  int tabBarTabMaxWidth{ 0 };
  int tabBarTabMinWidth{ 0 };
  int titleBarHeight{ 32 };

  bool fontSizeWasOverriden{ false };
  bool fontSizeMonospaceWasOverriden{ false };
  bool fontSizeS1WasOverriden{ false };

  QFont fontRegular;
  QFont fontBold;
  QFont fontH1;
  QFont fontH2;
  QFont fontH3;
  QFont fontH4;
  QFont fontH5;
  QFont fontCaption;
  QFont fontMonospace;

  QPalette palette;

public:
  QJsonDocument toJson() const;

private:
  void initializeFonts();
  void initializePalette();
  bool initializeFromJson(QJsonDocument const& jsonDoc);
};
} // namespace oclero::qlementine
