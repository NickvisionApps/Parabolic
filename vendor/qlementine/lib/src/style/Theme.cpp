// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/style/Theme.hpp>
#include <oclero/qlementine/utils/PrimitiveUtils.hpp>
#include <oclero/qlementine/utils/FontUtils.hpp>
#include <oclero/qlementine/utils/ColorUtils.hpp>

#include <QColor>
#include <QFile>
#include <QFontDatabase>
#include <QJsonObject>
#include <QScreen>
#include <QVector>
#include <QGuiApplication>

#include <algorithm>
#include <optional>

namespace oclero::qlementine {
namespace {
#define TRY_GET_COLOR_ATTRIBUTE(JSON_OBJ, NAME) tryGetColor(JSON_OBJ, QStringLiteral(#NAME), NAME)
#define TRY_GET_BOOL_ATTRIBUTE(JSON_OBJ, NAME) tryGetBool(JSON_OBJ, QStringLiteral(#NAME), NAME)
#define TRY_GET_INT_ATTRIBUTE(JSON_OBJ, NAME) tryGetInt(JSON_OBJ, QStringLiteral(#NAME), NAME)
#define TRY_GET_DOUBLE_ATTRIBUTE(JSON_OBJ, NAME) tryGetDouble(JSON_OBJ, QStringLiteral(#NAME), NAME)

#define SET_COLOR(JSON_OBJ, NAME) setColor(JSON_OBJ, QStringLiteral(#NAME), NAME)
#define SET_BOOL(JSON_OBJ, NAME) setBool(JSON_OBJ, QStringLiteral(#NAME), NAME)
#define SET_INT(JSON_OBJ, NAME) setInt(JSON_OBJ, QStringLiteral(#NAME), NAME)
#define SET_DOUBLE(JSON_OBJ, NAME) setDouble(JSON_OBJ, QStringLiteral(#NAME), NAME)

std::optional<QColor> tryGetColorRecursive(QJsonObject const& jsonObj, QString const& key, int maxRecursiveCalls = 1) {
  if (jsonObj.contains(key)) {
    const auto variant = jsonObj.value(key).toVariant();

    // Try parse a QColor.
    const auto color = tryGetColorFromVariant(variant);
    if (color.has_value()) {
      return color.value();
    }

    // Check if the value is a reference to another value.
    if (variant.userType() == QMetaType::QString && maxRecursiveCalls > 0) {
      const auto variantString = variant.toString();
      if (variantString != key) {
        return tryGetColorRecursive(jsonObj, variantString, maxRecursiveCalls - 1);
      }
    }
  }

  return {};
}

void tryGetColor(QJsonObject const& jsonObj, QString const& key, QColor& target) {
  if (const auto opt = tryGetColorRecursive(jsonObj, key)) {
    target = opt.value();
  }
}

QString tryGetString(QJsonObject const& jsonObj, QString const& key, QString const& defaultValue) {
  if (jsonObj.contains(key)) {
    const auto variant = jsonObj.value(key).toVariant();
    if (variant.isValid() && variant.canConvert<QString>()) {
      return variant.toString();
    }
  }
  return defaultValue;
}

std::optional<bool> tryGetBoolRecursive(QJsonObject const& jsonObj, QString const& key, int maxRecursiveCalls = 1) {
  if (jsonObj.contains(key)) {
    const auto variant = jsonObj.value(key).toVariant();

    // Check if the value is a reference to another value.
    if (variant.userType() == QMetaType::QString && maxRecursiveCalls > 0) {
      const auto variantString = variant.toString();
      if (variantString != key) {
        return tryGetBoolRecursive(jsonObj, variantString, maxRecursiveCalls - 1);
      }
    }

    // Try parse a bool.
    if (variant.isValid() && variant.canConvert<bool>()) {
      return variant.toBool();
    }
  }
  return {};
}

void tryGetBool(QJsonObject const& jsonObj, QString const& key, bool& target) {
  if (const auto opt = tryGetBoolRecursive(jsonObj, key); opt.has_value()) {
    target = opt.value();
  }
}

std::optional<int> tryGetIntRecursive(QJsonObject const& jsonObj, QString const& key, int maxRecursiveCalls = 1) {
  if (jsonObj.contains(key)) {
    const auto variant = jsonObj.value(key).toVariant();

    // Check if the value is a reference to another value.
    if (variant.userType() == QMetaType::QString && maxRecursiveCalls > 0) {
      const auto variantString = variant.toString();
      if (variantString != key) {
        return tryGetIntRecursive(jsonObj, variantString, maxRecursiveCalls - 1);
      }
    }

    // Try parse an int.
    if (variant.isValid() && variant.canConvert<int>()) {
      return variant.toInt();
    }
  }
  return {};
}

void tryGetInt(QJsonObject const& jsonObj, QString const& key, int& target) {
  if (const auto opt = tryGetIntRecursive(jsonObj, key); opt.has_value()) {
    target = opt.value();
  }
}

std::optional<double> tryGetDoubleRecursive(QJsonObject const& jsonObj, QString const& key, int maxRecursiveCalls = 1) {
  if (jsonObj.contains(key)) {
    const auto variant = jsonObj.value(key).toVariant();

    // Check if the value is a reference to another value.
    if (variant.userType() == QMetaType::QString && maxRecursiveCalls > 0) {
      const auto variantString = variant.toString();
      if (variantString != key) {
        return tryGetDoubleRecursive(jsonObj, variantString, maxRecursiveCalls - 1);
      }
    }

    // Try parse an int.
    if (variant.isValid() && variant.canConvert<double>()) {
      return variant.toDouble();
    }
  }
  return {};
}

void tryGetDouble(QJsonObject const& jsonObj, QString const& key, double& target) {
  if (const auto opt = tryGetDoubleRecursive(jsonObj, key); opt.has_value()) {
    target = opt.value();
  }
}

QJsonDocument readJsonDoc(QString const& jsonPath) {
  QFile jsonFile(jsonPath);
  if (jsonFile.open(QIODevice::ReadOnly)) {
    const auto fileContents = jsonFile.readAll();
    QJsonParseError jsonParseError{};
    const auto jsonDoc = QJsonDocument::fromJson(fileContents, &jsonParseError);
    if (jsonParseError.error == QJsonParseError::ParseError::NoError && !jsonDoc.isEmpty()) {
      return jsonDoc;
    }
  }

  return {};
}

void setColor(QJsonObject& jsonObj, const QString& key, const QColor& value) {
  jsonObj.insert(key, toHexRGBA(value));
}

void setBool(QJsonObject& jsonObj, const QString& key, bool value) {
  jsonObj.insert(key, value);
}

void setInt(QJsonObject& jsonObj, const QString& key, int value) {
  jsonObj.insert(key, value);
}

void setDouble(QJsonObject& jsonObj, const QString& key, double value) {
  jsonObj.insert(key, value);
}

bool jsonObjHasKey(const QJsonObject& obj, const QString& key) {
  return obj.find(key) != obj.end();
}

bool jsonObjHasAllKeys(const QJsonObject& obj, const QVector<QString>& keys) {
  for (const auto& key : keys) {
    if (!jsonObjHasKey(obj, key))
      return false;
  }
  return true;
}

QColor adjustLightness(const QColor& color, qreal delta) {
  auto hsl = color.toHsl();
  hsl.setHslF(hsl.hslHueF(), hsl.hslSaturationF(), std::clamp(hsl.lightnessF() + delta, 0., 1.), color.alphaF());
  return hsl.toRgb();
}

int resolveFontPixelSize(const QFont& font) {
  if (font.pixelSize() > 0) {
    return std::max(1, font.pixelSize());
  }

  const auto dpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
  return std::max(1, qRound(pointSizeToPixelSize(font.pointSizeF(), dpi)));
}
} // namespace

Theme::Theme() {
  initializeFonts();
  initializePalette();
}

std::optional<Theme> Theme::fromJsonPath(const QString& jsonPath) {
  return fromJsonDoc(readJsonDoc(jsonPath));
}

std::optional<Theme> Theme::fromJsonString(const QString& jsonString) {
  QJsonParseError jsonParseError{};
  const auto jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &jsonParseError);
  if (jsonParseError.error != QJsonParseError::ParseError::NoError || jsonDoc.isEmpty()) {
    return {};
  }
  return fromJsonDoc(jsonDoc);
}

Theme Theme::makeLight() {
  const auto themeOpt = fromJsonPath(QStringLiteral(":/qlementine/resources/themes/light.json"));
  return themeOpt.value_or(Theme{});
}

Theme Theme::makeLegacyDark() {
  const auto themeOpt = fromJsonPath(QStringLiteral(":/qlementine/resources/themes/legacy-dark.json"));
  return themeOpt.value_or(Theme{});
}

Theme Theme::makeDark() {
  const auto themeOpt = fromJsonPath(QStringLiteral(":/qlementine/resources/themes/dark.json"));
  return themeOpt.value_or(Theme{});
}

Theme Theme::withAccentColor(const QColor& accentColor) const {
  Theme theme(*this);
  if (!accentColor.isValid())
    return theme;

  const QColor base(accentColor.rgb());
  const auto foreground =
    getContrastRatio(base, Qt::white) >= getContrastRatio(base, Qt::black) ? QColor(Qt::white) : QColor(Qt::black);
  const auto alternative = adjustLightness(base, -0.06);

  // Primary.
  theme.primaryColor = base;
  theme.primaryColorHovered = adjustLightness(base, 0.04);
  theme.primaryColorPressed = adjustLightness(base, 0.08);
  theme.primaryColorDisabled = getColorSourceOver(backgroundColorMain2, colorWithAlphaF(base, 0.18));
  theme.primaryColorTransparent = colorWithAlpha(base, 0);

  // Foreground.
  theme.primaryColorForeground = foreground;
  theme.primaryColorForegroundHovered = foreground;
  theme.primaryColorForegroundPressed = foreground;
  theme.primaryColorForegroundDisabled =
    getColorSourceOver(theme.primaryColorDisabled, colorWithAlphaF(foreground, 0.35));
  theme.primaryColorForegroundTransparent = colorWithAlpha(foreground, 0);

  // Alternative.
  theme.primaryAlternativeColor = alternative;
  theme.primaryAlternativeColorHovered = adjustLightness(alternative, 0.04);
  theme.primaryAlternativeColorPressed = adjustLightness(alternative, 0.08);
  theme.primaryAlternativeColorDisabled = getColorSourceOver(backgroundColorMain2, colorWithAlphaF(alternative, 0.15));
  theme.primaryAlternativeColorTransparent = colorWithAlpha(alternative, 0);

  // Focus.
  theme.focusColor = colorWithAlpha(base, 0x66);

  theme.initializePalette();
  return theme;
}

std::optional<Theme> Theme::fromJsonDoc(const QJsonDocument& jsonDoc) {
  Theme theme;
  if (theme.initializeFromJson(jsonDoc)) {
    theme.initializeFonts();
    theme.initializePalette();
    return theme;
  }
  return std::nullopt;
}

void Theme::initializeFonts() {
  // Fonts.
  const auto defaultFont =
    useSystemFonts ? QFontDatabase::systemFont(QFontDatabase::GeneralFont) : QFont(QStringLiteral("Inter"));
  const auto fixedFont =
    useSystemFonts ? QFontDatabase::systemFont(QFontDatabase::FixedFont) : QFont(QStringLiteral("Roboto Mono"));
  const auto titleFont =
    useSystemFonts ? QFontDatabase::systemFont(QFontDatabase::TitleFont) : QFont(QStringLiteral("Inter Display"));

  fontRegular = defaultFont;
  fontRegular.setWeight(QFont::Weight::Normal);
  if (useSystemFonts && !fontSizeWasOverriden) {
    fontSize = resolveFontPixelSize(defaultFont);
  }
  fontRegular.setPixelSize(std::max(1, fontSize));

  fontBold = defaultFont;
  fontBold.setWeight(QFont::Weight::Bold);
  fontBold.setPixelSize(std::max(1, fontSize));

  fontH1 = titleFont;
  fontH1.setWeight(QFont::Weight::Bold);
  fontH1.setPixelSize(std::max(1, fontSizeH1));

  fontH2 = titleFont;
  fontH2.setWeight(QFont::Weight::Bold);
  fontH2.setPixelSize(std::max(1, fontSizeH2));

  fontH3 = titleFont;
  fontH3.setWeight(QFont::Weight::Bold);
  fontH3.setPixelSize(std::max(1, fontSizeH3));

  fontH4 = titleFont;
  fontH4.setWeight(QFont::Weight::Bold);
  fontH4.setPixelSize(std::max(1, fontSizeH4));

  fontH5 = titleFont;
  fontH5.setWeight(QFont::Weight::Bold);
  fontH5.setPixelSize(std::max(1, fontSizeH5));

  fontCaption = defaultFont;
  fontCaption.setWeight(QFont::Weight::Normal);
  if (useSystemFonts && !fontSizeS1WasOverriden) {
    fontSizeS1 = resolveFontPixelSize(defaultFont);
  }
  fontCaption.setPixelSize(std::max(1, fontSizeS1));

  fontMonospace = fixedFont;
  fontMonospace.setWeight(QFont::Weight::Normal);
  if (useSystemFonts && !fontSizeMonospaceWasOverriden) {
    fontSizeMonospace = resolveFontPixelSize(fixedFont);
  }
  fontMonospace.setPixelSize(std::max(1, fontSizeMonospace));
}

void Theme::initializePalette() {
  // Shades.
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Window, backgroundColorMain2);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Dark, backgroundColorMain3);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Mid, backgroundColorMain3);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Midlight, backgroundColorMain2);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Light, backgroundColorMain2);

  // ItemViews.
  // Compute color without alpha, to avoid color blending when the QTreeView is animating.
  const auto& itemViewBase = backgroundColorMain1;
  const auto itemViewAlternate =
    getColorSourceOver(itemViewBase, colorWithAlpha(neutralColorDisabled, neutralColorDisabled.alpha() / 2));
  const auto itemViewDisabled = getColorSourceOver(backgroundColorMain1, neutralColorDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Base, itemViewBase);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, itemViewDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::AlternateBase, itemViewAlternate);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, itemViewDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::NoRole, backgroundColorMainTransparent);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::NoRole, backgroundColorMainTransparent);

  // Tooltips.
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::ToolTipBase, secondaryColor);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::ToolTipText, secondaryColorForeground);

  // Highlight.
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Highlight, primaryColor);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Highlight, primaryColorDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::HighlightedText, primaryColorForeground);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::HighlightedText, primaryColorDisabled);

  // Text.
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Text, secondaryColor);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, secondaryColorDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::WindowText, secondaryColor);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, secondaryColorDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::PlaceholderText, secondaryColorDisabled);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, secondaryColorDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Link, primaryColor);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Link, secondaryColorDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::LinkVisited, primaryColor);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::LinkVisited, secondaryColorDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::BrightText, secondaryAlternativeColor);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, secondaryAlternativeColorDisabled);

  // Buttons.
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::ButtonText, secondaryColorForeground);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, secondaryColorForegroundDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Button, neutralColor);
  palette.setColor(QPalette::ColorGroup::Normal, QPalette::ColorRole::Button, neutralColor);
  palette.setColor(QPalette::ColorGroup::Current, QPalette::ColorRole::Button, neutralColorHovered);
  palette.setColor(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, neutralColorPressed);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, neutralColorDisabled);
}

bool Theme::initializeFromJson(QJsonDocument const& jsonDoc) {
  if (!jsonDoc.isObject())
    return false;

  const auto jsonObj = jsonDoc.object();
  if (!jsonObj.isEmpty()) {
    // Parse metadata.
    auto const metaObj = jsonObj.value(QStringLiteral("meta")).toObject();
    if (!jsonObjHasAllKeys(metaObj, {
                                      QStringLiteral("name"),
                                      QStringLiteral("version"),
                                      QStringLiteral("author"),
                                    }))
      return false;

    meta = ThemeMeta{
      tryGetString(metaObj, QStringLiteral("name"), {}),
      tryGetString(metaObj, QStringLiteral("version"), {}),
      tryGetString(metaObj, QStringLiteral("author"), {}),
    };

    // Parse all values.
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, backgroundColorMain1);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, backgroundColorMain2);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, backgroundColorMain3);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, backgroundColorMain4);
    backgroundColorMainTransparent = colorWithAlpha(backgroundColorMain1, 0);

    TRY_GET_COLOR_ATTRIBUTE(jsonObj, backgroundColorWorkspace);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, backgroundColorTabBar);

    TRY_GET_COLOR_ATTRIBUTE(jsonObj, neutralColorDisabled);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, neutralColor);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, neutralColorHovered);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, neutralColorPressed);
    neutralColorTransparent = colorWithAlpha(neutralColorDisabled, 0);

    TRY_GET_COLOR_ATTRIBUTE(jsonObj, focusColor);

    TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryColor);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryColorHovered);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryColorPressed);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryColorDisabled);
    primaryColorTransparent = colorWithAlpha(primaryColor, 0);

    TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryColorForeground);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryColorForegroundHovered);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryColorForegroundPressed);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryColorForegroundDisabled);
    primaryColorForegroundTransparent = colorWithAlpha(primaryColorForeground, 0);

    TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryAlternativeColor);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryAlternativeColorHovered);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryAlternativeColorPressed);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryAlternativeColorDisabled);
    primaryAlternativeColorTransparent = colorWithAlpha(primaryAlternativeColor, 0);

    TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryColor);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryColorHovered);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryColorPressed);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryColorDisabled);
    secondaryColorTransparent = colorWithAlpha(secondaryColor, 0);

    TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryAlternativeColor);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryAlternativeColorHovered);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryAlternativeColorPressed);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryAlternativeColorDisabled);
    secondaryAlternativeColorTransparent = colorWithAlpha(secondaryAlternativeColor, 0);

    TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryColorForeground);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryColorForegroundHovered);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryColorForegroundPressed);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryColorForegroundDisabled);
    secondaryColorForegroundTransparent = colorWithAlpha(secondaryColorForeground, 0);

    TRY_GET_COLOR_ATTRIBUTE(jsonObj, titleBarForegroundColor);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, titleBarBorderColor);

    TRY_GET_COLOR_ATTRIBUTE(jsonObj, semiTransparentColor1);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, semiTransparentColor2);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, semiTransparentColor3);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, semiTransparentColor4);
    semiTransparentColorTransparent = colorWithAlpha(semiTransparentColor1, 0);

    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorSuccess);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorSuccessHovered);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorSuccessPressed);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorSuccessDisabled);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorInfo);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorInfoHovered);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorInfoPressed);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorInfoDisabled);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorWarning);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorWarningHovered);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorWarningPressed);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorWarningDisabled);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorError);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorErrorHovered);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorErrorPressed);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorErrorDisabled);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorForeground);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorForegroundHovered);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorForegroundPressed);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorForegroundDisabled);

    TRY_GET_COLOR_ATTRIBUTE(jsonObj, borderColor);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, borderColorHovered);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, borderColorPressed);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, borderColorDisabled);
    borderColorTransparent = colorWithAlpha(borderColor, 0);

    TRY_GET_COLOR_ATTRIBUTE(jsonObj, shadowColor1);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, shadowColor2);
    TRY_GET_COLOR_ATTRIBUTE(jsonObj, shadowColor3);
    shadowColorTransparent = colorWithAlpha(shadowColor1, 0);

    TRY_GET_BOOL_ATTRIBUTE(jsonObj, useSystemFonts);

    fontSizeWasOverriden = jsonObj.contains(QStringLiteral("fontSize"));
    fontSizeMonospaceWasOverriden = jsonObj.contains(QStringLiteral("fontSizeMonospace"));
    fontSizeS1WasOverriden = jsonObj.contains(QStringLiteral("fontSizeS1"));

    TRY_GET_INT_ATTRIBUTE(jsonObj, fontSize);
    TRY_GET_INT_ATTRIBUTE(jsonObj, fontSizeMonospace);
    TRY_GET_INT_ATTRIBUTE(jsonObj, fontSizeH1);
    TRY_GET_INT_ATTRIBUTE(jsonObj, fontSizeH2);
    TRY_GET_INT_ATTRIBUTE(jsonObj, fontSizeH3);
    TRY_GET_INT_ATTRIBUTE(jsonObj, fontSizeH4);
    TRY_GET_INT_ATTRIBUTE(jsonObj, fontSizeH5);
    TRY_GET_INT_ATTRIBUTE(jsonObj, fontSizeS1);
    TRY_GET_INT_ATTRIBUTE(jsonObj, animationDuration);
    TRY_GET_INT_ATTRIBUTE(jsonObj, focusAnimationDuration);
    TRY_GET_INT_ATTRIBUTE(jsonObj, sliderAnimationDuration);
    TRY_GET_DOUBLE_ATTRIBUTE(jsonObj, borderRadius);
    TRY_GET_DOUBLE_ATTRIBUTE(jsonObj, checkBoxBorderRadius);
    TRY_GET_DOUBLE_ATTRIBUTE(jsonObj, menuItemBorderRadius);
    TRY_GET_DOUBLE_ATTRIBUTE(jsonObj, menuBarItemBorderRadius);
    TRY_GET_INT_ATTRIBUTE(jsonObj, borderWidth);
    TRY_GET_INT_ATTRIBUTE(jsonObj, controlHeightLarge);
    TRY_GET_INT_ATTRIBUTE(jsonObj, controlHeightMedium);
    TRY_GET_INT_ATTRIBUTE(jsonObj, controlHeightSmall);
    TRY_GET_INT_ATTRIBUTE(jsonObj, controlDefaultWidth);
    TRY_GET_INT_ATTRIBUTE(jsonObj, dialMarkLength);
    TRY_GET_INT_ATTRIBUTE(jsonObj, dialMarkThickness);
    TRY_GET_INT_ATTRIBUTE(jsonObj, dialTickLength);
    TRY_GET_INT_ATTRIBUTE(jsonObj, dialTickSpacing);
    TRY_GET_INT_ATTRIBUTE(jsonObj, dialGrooveThickness);
    TRY_GET_INT_ATTRIBUTE(jsonObj, focusBorderWidth);

    auto iconExtent = 16;
    TRY_GET_INT_ATTRIBUTE(jsonObj, iconExtent);
    iconSize = QSize{ iconExtent, iconExtent };
    iconSizeMedium = iconSize * 1.5;
    iconSizeLarge = iconSize * 2;
    iconSizeExtraSmall = iconSize * 0.75;

    TRY_GET_INT_ATTRIBUTE(jsonObj, sliderTickSize);
    TRY_GET_INT_ATTRIBUTE(jsonObj, sliderTickSpacing);
    TRY_GET_INT_ATTRIBUTE(jsonObj, sliderTickThickness);
    TRY_GET_INT_ATTRIBUTE(jsonObj, sliderGrooveHeight);
    TRY_GET_INT_ATTRIBUTE(jsonObj, progressBarGrooveHeight);
    TRY_GET_INT_ATTRIBUTE(jsonObj, spacing);
    TRY_GET_INT_ATTRIBUTE(jsonObj, scrollBarThicknessFull);
    TRY_GET_INT_ATTRIBUTE(jsonObj, scrollBarThicknessSmall);
    TRY_GET_INT_ATTRIBUTE(jsonObj, scrollBarMargin);
    TRY_GET_INT_ATTRIBUTE(jsonObj, tabBarPaddingTop);
    TRY_GET_INT_ATTRIBUTE(jsonObj, tabBarTabMaxWidth);
    TRY_GET_INT_ATTRIBUTE(jsonObj, tabBarTabMinWidth);
    TRY_GET_INT_ATTRIBUTE(jsonObj, titleBarHeight);

    tabBarTabMaxWidth = std::max(0, tabBarTabMaxWidth);
    tabBarTabMinWidth = std::max(0, tabBarTabMinWidth);
    if (tabBarTabMinWidth > tabBarTabMaxWidth) {
      std::swap(tabBarTabMinWidth, tabBarTabMaxWidth);
    }
  }

  return true;
}

QJsonDocument Theme::toJson() const {
  QJsonObject jsonObj;

  // Metadata.
  QJsonObject metadataObj;
  metadataObj.insert("name", meta.name);
  metadataObj.insert("version", meta.version);
  metadataObj.insert("author", meta.author);
  jsonObj.insert("meta", metadataObj);

  // Colors.
  SET_COLOR(jsonObj, backgroundColorMain1);
  SET_COLOR(jsonObj, backgroundColorMain2);
  SET_COLOR(jsonObj, backgroundColorMain3);
  SET_COLOR(jsonObj, backgroundColorMain4);

  SET_COLOR(jsonObj, backgroundColorWorkspace);
  SET_COLOR(jsonObj, backgroundColorTabBar);

  SET_COLOR(jsonObj, neutralColor);
  SET_COLOR(jsonObj, neutralColorHovered);
  SET_COLOR(jsonObj, neutralColorPressed);
  SET_COLOR(jsonObj, neutralColorDisabled);

  SET_COLOR(jsonObj, focusColor);

  SET_COLOR(jsonObj, primaryColor);
  SET_COLOR(jsonObj, primaryColorHovered);
  SET_COLOR(jsonObj, primaryColorPressed);
  SET_COLOR(jsonObj, primaryColorDisabled);

  SET_COLOR(jsonObj, primaryColorForeground);
  SET_COLOR(jsonObj, primaryColorForegroundHovered);
  SET_COLOR(jsonObj, primaryColorForegroundPressed);
  SET_COLOR(jsonObj, primaryColorForegroundDisabled);

  SET_COLOR(jsonObj, primaryAlternativeColor);
  SET_COLOR(jsonObj, primaryAlternativeColorHovered);
  SET_COLOR(jsonObj, primaryAlternativeColorPressed);
  SET_COLOR(jsonObj, primaryAlternativeColorDisabled);

  SET_COLOR(jsonObj, secondaryColor);
  SET_COLOR(jsonObj, secondaryColorHovered);
  SET_COLOR(jsonObj, secondaryColorPressed);
  SET_COLOR(jsonObj, secondaryColorDisabled);

  SET_COLOR(jsonObj, secondaryColorForeground);
  SET_COLOR(jsonObj, secondaryColorForegroundHovered);
  SET_COLOR(jsonObj, secondaryColorForegroundPressed);
  SET_COLOR(jsonObj, secondaryColorForegroundDisabled);

  SET_COLOR(jsonObj, secondaryAlternativeColor);
  SET_COLOR(jsonObj, secondaryAlternativeColorHovered);
  SET_COLOR(jsonObj, secondaryAlternativeColorPressed);
  SET_COLOR(jsonObj, secondaryAlternativeColorDisabled);

  SET_COLOR(jsonObj, statusColorSuccess);
  SET_COLOR(jsonObj, statusColorSuccessHovered);
  SET_COLOR(jsonObj, statusColorSuccessPressed);
  SET_COLOR(jsonObj, statusColorSuccessDisabled);

  SET_COLOR(jsonObj, statusColorInfo);
  SET_COLOR(jsonObj, statusColorInfoHovered);
  SET_COLOR(jsonObj, statusColorInfoPressed);
  SET_COLOR(jsonObj, statusColorInfoDisabled);

  SET_COLOR(jsonObj, statusColorWarning);
  SET_COLOR(jsonObj, statusColorWarningHovered);
  SET_COLOR(jsonObj, statusColorWarningPressed);
  SET_COLOR(jsonObj, statusColorWarningDisabled);

  SET_COLOR(jsonObj, statusColorError);
  SET_COLOR(jsonObj, statusColorErrorHovered);
  SET_COLOR(jsonObj, statusColorErrorPressed);
  SET_COLOR(jsonObj, statusColorErrorDisabled);

  SET_COLOR(jsonObj, shadowColor1);
  SET_COLOR(jsonObj, shadowColor2);
  SET_COLOR(jsonObj, shadowColor3);

  SET_COLOR(jsonObj, borderColor);
  SET_COLOR(jsonObj, borderColorHovered);
  SET_COLOR(jsonObj, borderColorPressed);
  SET_COLOR(jsonObj, borderColorDisabled);

  SET_COLOR(jsonObj, titleBarForegroundColor);
  SET_COLOR(jsonObj, titleBarBorderColor);

  SET_COLOR(jsonObj, semiTransparentColor1);
  SET_COLOR(jsonObj, semiTransparentColor2);
  SET_COLOR(jsonObj, semiTransparentColor3);
  SET_COLOR(jsonObj, semiTransparentColor4);

  SET_BOOL(jsonObj, useSystemFonts);

  SET_INT(jsonObj, fontSize);
  SET_INT(jsonObj, fontSizeMonospace);
  SET_INT(jsonObj, fontSizeH1);
  SET_INT(jsonObj, fontSizeH2);
  SET_INT(jsonObj, fontSizeH3);
  SET_INT(jsonObj, fontSizeH4);
  SET_INT(jsonObj, fontSizeH5);
  SET_INT(jsonObj, fontSizeS1);
  SET_INT(jsonObj, animationDuration);
  SET_INT(jsonObj, focusAnimationDuration);
  SET_INT(jsonObj, sliderAnimationDuration);
  SET_DOUBLE(jsonObj, borderRadius);
  SET_DOUBLE(jsonObj, checkBoxBorderRadius);
  SET_DOUBLE(jsonObj, menuBarItemBorderRadius);
  SET_INT(jsonObj, borderWidth);
  SET_INT(jsonObj, controlHeightLarge);
  SET_INT(jsonObj, controlHeightMedium);
  SET_INT(jsonObj, controlHeightSmall);
  SET_INT(jsonObj, controlDefaultWidth);
  SET_INT(jsonObj, dialMarkLength);
  SET_INT(jsonObj, dialMarkThickness);
  SET_INT(jsonObj, dialTickLength);
  SET_INT(jsonObj, dialTickSpacing);
  SET_INT(jsonObj, dialGrooveThickness);
  SET_INT(jsonObj, focusBorderWidth);
  setInt(jsonObj, "iconSize", iconSize.width());
  setInt(jsonObj, "iconSizeMedium", iconSizeMedium.width());
  setInt(jsonObj, "iconSizeLarge", iconSizeLarge.width());
  setInt(jsonObj, "iconSizeExtraSmall", iconSizeExtraSmall.width());
  SET_INT(jsonObj, sliderTickSize);
  SET_INT(jsonObj, sliderTickSpacing);
  SET_INT(jsonObj, sliderTickThickness);
  SET_INT(jsonObj, sliderGrooveHeight);
  SET_INT(jsonObj, progressBarGrooveHeight);
  SET_INT(jsonObj, spacing);
  SET_INT(jsonObj, scrollBarThicknessFull);
  SET_INT(jsonObj, scrollBarThicknessSmall);
  SET_INT(jsonObj, scrollBarMargin);
  SET_INT(jsonObj, tabBarPaddingTop);
  SET_INT(jsonObj, tabBarTabMaxWidth);
  SET_INT(jsonObj, tabBarTabMinWidth);
  SET_INT(jsonObj, titleBarHeight);

  QJsonDocument jsonDoc;
  jsonDoc.setObject(jsonObj);
  return jsonDoc;
}

bool Theme::operator==(const Theme& other) const {
  // All generated values are not used in this equality test, on purpose.
  // TODO Remove this and instead use spaceship operator when C++20 is available on macOS/XCode.
  // clang-format off
  return meta == other.meta
    && backgroundColorMain1 == other.backgroundColorMain1
    && backgroundColorMain2 == other.backgroundColorMain2
    && backgroundColorMain3 == other.backgroundColorMain3
    && backgroundColorMain4 == other.backgroundColorMain4

    && backgroundColorWorkspace == other.backgroundColorWorkspace

    && neutralColorDisabled == other.neutralColorDisabled
    && neutralColor == other.neutralColor
    && neutralColorHovered == other.neutralColorHovered
    && neutralColorPressed == other.neutralColorPressed

    && focusColor == other.focusColor

    && primaryColor == other.primaryColor
    && primaryColorHovered == other.primaryColorHovered
    && primaryColorPressed == other.primaryColorPressed
    && primaryColorDisabled == other.primaryColorDisabled

    && primaryColorForeground == other.primaryColorForeground
    && primaryColorForegroundHovered == other.primaryColorForegroundHovered
    && primaryColorForegroundPressed == other.primaryColorForegroundPressed
    && primaryColorForegroundDisabled == other.primaryColorForegroundDisabled

    && primaryAlternativeColor == other.primaryAlternativeColor
    && primaryAlternativeColorHovered == other.primaryAlternativeColorHovered
    && primaryAlternativeColorPressed == other.primaryAlternativeColorPressed
    && primaryAlternativeColorDisabled == other.primaryAlternativeColorDisabled

    && secondaryColor == other.secondaryColor
    && secondaryColorHovered == other.secondaryColorHovered
    && secondaryColorPressed == other.secondaryColorPressed
    && secondaryColorDisabled == other.secondaryColorDisabled

    && secondaryAlternativeColor == other.secondaryAlternativeColor
    && secondaryAlternativeColorHovered == other.secondaryAlternativeColorHovered
    && secondaryAlternativeColorPressed == other.secondaryAlternativeColorPressed
    && secondaryAlternativeColorDisabled == other.secondaryAlternativeColorDisabled

    && secondaryColorForeground == other.secondaryColorForeground
    && secondaryColorForegroundHovered == other.secondaryColorForegroundHovered
    && secondaryColorForegroundPressed == other.secondaryColorForegroundPressed
    && secondaryColorForegroundDisabled == other.secondaryColorForegroundDisabled

    && statusColorSuccess == other.statusColorSuccess
    && statusColorSuccessHovered == other.statusColorSuccessHovered
    && statusColorSuccessPressed == other.statusColorSuccessPressed
    && statusColorSuccessDisabled == other.statusColorSuccessDisabled
    && statusColorInfo == other.statusColorInfo
    && statusColorInfoHovered == other.statusColorInfoHovered
    && statusColorInfoPressed == other.statusColorInfoPressed
    && statusColorInfoDisabled == other.statusColorInfoDisabled
    && statusColorWarning == other.statusColorWarning
    && statusColorWarningHovered == other.statusColorWarningHovered
    && statusColorWarningPressed == other.statusColorWarningPressed
    && statusColorWarningDisabled == other.statusColorWarningDisabled
    && statusColorError == other.statusColorError
    && statusColorErrorHovered == other.statusColorErrorHovered
    && statusColorErrorPressed == other.statusColorErrorPressed
    && statusColorErrorDisabled == other.statusColorErrorDisabled
    && statusColorForeground == other.statusColorForeground
    && statusColorForegroundHovered == other.statusColorForegroundHovered
    && statusColorForegroundPressed == other.statusColorForegroundPressed
    && statusColorForegroundDisabled == other.statusColorForegroundDisabled

    && shadowColor1 == other.shadowColor1
    && shadowColor2 == other.shadowColor2
    && shadowColor3 == other.shadowColor3

    && borderColor == other.borderColor
    && borderColorHovered == other.borderColorHovered
    && borderColorPressed == other.borderColorPressed
    && borderColorDisabled == other.borderColorDisabled

    && titleBarForegroundColor == other.titleBarForegroundColor
    && titleBarBorderColor == other.titleBarBorderColor

    && useSystemFonts == other.useSystemFonts

    && animationDuration == other.animationDuration
    && focusAnimationDuration == other.focusAnimationDuration
    && sliderAnimationDuration == other.sliderAnimationDuration
    && borderRadius == other.borderRadius
    && checkBoxBorderRadius == other.checkBoxBorderRadius
    && menuItemBorderRadius == other.menuItemBorderRadius
    && menuBarItemBorderRadius == other.menuBarItemBorderRadius
    && borderWidth == other.borderWidth
    && controlHeightLarge == other.controlHeightLarge
    && controlHeightMedium == other.controlHeightMedium
    && controlHeightSmall == other.controlHeightSmall
    && controlDefaultWidth == other.controlDefaultWidth
    && dialMarkLength == other.dialMarkLength
    && dialMarkThickness == other.dialMarkThickness
    && dialTickLength == other.dialTickLength
    && dialTickSpacing == other.dialTickSpacing
    && dialGrooveThickness == other.dialGrooveThickness
    && focusBorderWidth == other.focusBorderWidth
    && iconSize == other.iconSize
    && iconSizeMedium == other.iconSizeMedium
    && iconSizeLarge == other.iconSizeLarge
    && iconSizeExtraSmall == other.iconSizeExtraSmall
    && sliderTickSize == other.sliderTickSize
    && sliderTickThickness == other.sliderTickThickness
    && sliderGrooveHeight == other.sliderGrooveHeight
    && progressBarGrooveHeight == other.progressBarGrooveHeight
    && spacing == other.spacing
    && scrollBarThicknessFull == other.scrollBarThicknessFull
    && scrollBarThicknessSmall == other.scrollBarThicknessSmall
    && scrollBarMargin == other.scrollBarMargin
    && tabBarPaddingTop == other.tabBarPaddingTop
    && titleBarHeight == other.titleBarHeight
    ;
  // clang-format on
}

bool Theme::operator!=(const Theme& other) const {
  return !(*this == other);
}

} // namespace oclero::qlementine
