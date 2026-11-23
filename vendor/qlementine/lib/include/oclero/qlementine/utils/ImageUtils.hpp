// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>
#include <oclero/qlementine/utils/RadiusesF.hpp>

#include <QPixmap>
#include <QString>
#include <QColor>

#include <iomanip>
#include <sstream>
#include <type_traits>

namespace oclero::qlementine {
/// Converts an integer to its std::string hex form.
template<typename T, typename std::enable_if<std::is_integral<T>::value, T>::type* = nullptr>
std::string to_hex(T const i, bool const prefix = false) {
  std::stringstream stream;
  if (prefix) {
    stream << "0x";
  }
  stream << std::setfill('0') << std::setw(sizeof(T) * 2) << std::hex << i;
  return stream.str();
}

/// Convets an integer to its QString hex form.
template<typename T, typename std::enable_if<std::is_integral<T>::value, T>::type* = nullptr>
QString toHex(T const i, bool const prefix = false) {
  return QString::fromStdString(to_hex(i, prefix));
}

/// Basically colorize the QPixmap and returns a QImage.
QLEMENTINE_EXPORT QImage colorizeImage(QPixmap const& input, QColor const& color);

/// Basically colorize the QPixmap.
QLEMENTINE_EXPORT QPixmap colorizePixmap(QPixmap const& input, QColor const& color);

/// Tints the QPixmap, preserving contrast between shades.
QLEMENTINE_EXPORT QPixmap tintPixmap(QPixmap const& input, QColor const& color);

/// Looks for a colorized version of the input pixmap in the cache.
/// If not existing, generates it and adds it to the cache.
/// Else, just returns the existing pixmap.
/// If any error, returns the input pixmap.
QLEMENTINE_EXPORT QPixmap getColorizedPixmap(QPixmap const& input, QColor const& color);

/// Looks for a tinted version of the input pixmap in the cache.
/// If not existing, generates it and adds it to the cache.
/// Else, just returns the existing pixmap.
/// If any error, returns the input pixmap.
QLEMENTINE_EXPORT QPixmap getTintedPixmap(QPixmap const& input, QColor const& color);

/// Gets the key to use in the QPixmapCache.
QLEMENTINE_EXPORT QString getColorizedPixmapKey(QPixmap const& pixmap, QColor const& color);

/// Gets the key to use in the QPixmapCache.
QLEMENTINE_EXPORT QString getTintedPixmapKey(QPixmap const& pixmap, QColor const& color);

/// Type of effect applied to colorize the image.
enum class ColorizeMode {
  // Replaces all {R,G,B} values with another, thus loosing luminance, but preserve alpha.
  // Useful for flat symbolic icons.
  Colorize,
  // Applies an {R,G,B} tint, but keeps luminance and alpha.
  // Useful for complex icons with multiple colors and shades.
  Tint,
};

/// Specifies which color to apply when colorizing an icon.
enum class AutoIconColor {
  // No automatic icon recolorization.
  None,
  // Recolorize with the foreground color.
  ForegroundColor,
  // Recolorize with the current text color, which might be different from the foreground color
  // if the text color was customized in the palette.
  TextColor,
};

/// Gets the pixmap in the cache, or creates it if not yet there.
QLEMENTINE_EXPORT QPixmap getCachedPixmap(QPixmap const& input, QColor const& color, ColorizeMode mode);

/// Makes a QPixmap from the file located at the path in parameter at the desired size.
QLEMENTINE_EXPORT QPixmap makePixmapFromSvg(const QString& svgPath, const QSize& size);

/// Makes a QPixmap from the file located at the path in parameter at the desired size.
QLEMENTINE_EXPORT QPixmap makePixmapFromSvg(const QString& backgroundSvgPath, const QColor& backgroundSvgColor,
  const QString& foregroundSvgPath, const QColor& foregroundSvgColor, const QSize& size);

/// Makes a QPixmap with rounded corners.
QLEMENTINE_EXPORT QPixmap makeRoundedPixmap(QPixmap const& input, double radius);

/// Makes a QPixmap with rounded corners.
QLEMENTINE_EXPORT QPixmap makeRoundedPixmap(QPixmap const& input, const RadiusesF& radiuses);

/// Makes a QPixmap with rounded corners.
QLEMENTINE_EXPORT QPixmap makeRoundedPixmap(
  QPixmap const& input, double topLeft, double topRight, double bottomRight, double bottomLeft);

/// Makes a pixmap that fits the requested size.
QLEMENTINE_EXPORT QPixmap makeFitPixmap(QPixmap const& input, const QSize& size);

/// Gets the aspect ratio (width by height) of the image, without loading it into memory.
QLEMENTINE_EXPORT double getImageAspectRatio(QString const& path);

/// Gets a version of the pixmap with padding around.
QLEMENTINE_EXPORT QImage getExtendedImage(QPixmap const& input, int padding);

/// Gets a version of the image with padding around.
QLEMENTINE_EXPORT QImage getExtendedImage(QImage const& input, int padding);

/// Gets a blurred version of the input pixmap
QLEMENTINE_EXPORT QPixmap getBlurredPixmap(QPixmap const& input, double blurRadius);

/// Gets a drop shadow for the input pixmap (i.e. a blurred colorized version).
QLEMENTINE_EXPORT QPixmap getDropShadowPixmap(QPixmap const& input, double blurRadius, QColor const& color = Qt::black);

/// Gets a drop shadow for a QRect.
QLEMENTINE_EXPORT QPixmap getDropShadowPixmap(
  QSize const& size, double borderRadius, double blurRadius, QColor const& color = Qt::black);

/// Calculates the necessary space for a blurred image.
QLEMENTINE_EXPORT int blurRadiusNecessarySpace(const double blurRadius);
} // namespace oclero::qlementine

Q_DECLARE_METATYPE(oclero::qlementine::AutoIconColor);
