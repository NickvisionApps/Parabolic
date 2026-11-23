// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/utils/ImageUtils.hpp>
#include <oclero/qlementine/utils/PrimitiveUtils.hpp>

#include <oclero/qlementine/utils/BlurUtils.hpp>

#include <QPixmap>
#include <QLatin1Char>
#include <QLatin1String>
#include <QPixmapCache>
#include <QSvgRenderer>
#include <QImageReader>

#include <cmath>
#include <algorithm>

namespace qtprivate {
// Taken from qpixmapfilter.cpp, line 946, Qt 5.15.2
// Grayscales the image to dest (could be same). If rect isn't defined
// destination image size is used to determine the dimension of grayscaling
// process.
static void grayscale(const QImage& image, QImage& dest, const QRect& rect = QRect()) {
  QRect destRect = rect;
  QRect srcRect = rect;

  if (rect.isNull()) {
    srcRect = dest.rect();
    destRect = dest.rect();
  }
  if (&image != &dest) {
    destRect.moveTo(QPoint(0, 0));
  }

  const unsigned int* data = (const unsigned int*) image.bits();
  unsigned int* outData = (unsigned int*) dest.bits();

  if (dest.size() == image.size() && image.rect() == srcRect) {
    // A bit faster loop for grayscaling everything.
    int pixels = dest.width() * dest.height();
    for (int i = 0; i < pixels; ++i) {
      int val = qGray(data[i]);
      outData[i] = qRgba(val, val, val, qAlpha(data[i]));
    }
  } else {
    int yd = destRect.top();
    for (int y = srcRect.top(); y <= srcRect.bottom() && y < image.height(); y++) {
      data = (const unsigned int*) image.scanLine(y);
      outData = (unsigned int*) dest.scanLine(yd++);
      int xd = destRect.left();
      for (int x = srcRect.left(); x <= srcRect.right() && x < image.width(); x++) {
        int val = qGray(data[x]);
        outData[xd++] = qRgba(val, val, val, qAlpha(data[x]));
      }
    }
  }
}
} // namespace qtprivate

namespace oclero::qlementine {

QImage colorizeImage(QPixmap const& input, QColor const& color) {
  if (input.isNull())
    return {};

  // Convert input QPixmap to QImage, because it is better for fast pixel manipulation.
  const auto imageSize = input.size();
  auto inputImage = input.toImage();
  inputImage = std::move(inputImage).convertToFormat(QImage::Format_ARGB32);

  // Create output QImage with same format and size as input QImage.
  auto outputImage = QImage(imageSize, inputImage.format());
  const auto outputRgb = color.rgba();
  const auto outputR = qRed(outputRgb);
  const auto outputG = qGreen(outputRgb);
  const auto outputB = qBlue(outputRgb);
  const auto outputAf = qAlpha(outputRgb) / 255.;

  // Modify the pixels.
  for (auto x = 0; x < imageSize.width(); ++x) {
    for (auto y = 0; y < imageSize.height(); ++y) {
      const auto inputPixel = inputImage.pixel(x, y);
      const auto inputA = qAlpha(inputPixel);
      const auto outputA = static_cast<int>(inputA * outputAf);
      const auto outputPixel = qRgba(outputR, outputG, outputB, outputA);
      outputImage.setPixel(x, y, outputPixel);
    }
  }
  // Set the pixel ratio.
  outputImage.setDevicePixelRatio(inputImage.devicePixelRatioF());

  return outputImage;
}

QPixmap colorizePixmap(QPixmap const& input, QColor const& color) {
  // Convert output QImage to QPixmap.
  return QPixmap::fromImage(colorizeImage(input, color));
}

QPixmap tintPixmap(QPixmap const& input, QColor const& color) {
  if (input.isNull())
    return {};

  // QImage is made for faster pixel manipulation.
  auto inputImage = input.toImage();
  const auto format = inputImage.hasAlphaChannel() ? QImage::Format_ARGB32_Premultiplied : QImage::Format_RGB32;
  inputImage = std::move(inputImage).convertToFormat(format);

  auto outputImage = QImage(inputImage.size(), inputImage.format());
  outputImage.setDevicePixelRatio(inputImage.devicePixelRatioF());

  // Convert to gray scale, then apply the color over with a Screen composition mode.
  QPainter outputPainter(&outputImage);
  qtprivate::grayscale(inputImage, outputImage, inputImage.rect());
  outputPainter.setCompositionMode(QPainter::CompositionMode_Screen);
  outputPainter.fillRect(inputImage.rect(), color);
  outputPainter.end();

  // Keep the alpha.
  if (inputImage.hasAlphaChannel()) {
    Q_ASSERT(outputImage.format() == QImage::Format_ARGB32_Premultiplied);
    QPainter maskPainter(&outputImage);
    maskPainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    maskPainter.drawImage(0, 0, inputImage);
  }

  return QPixmap::fromImage(outputImage);
}

QPixmap getColorizedPixmap(const QPixmap& input, const QColor& color) {
  return getCachedPixmap(input, color, ColorizeMode::Colorize);
}

QPixmap getTintedPixmap(const QPixmap& input, const QColor& color) {
  return getCachedPixmap(input, color, ColorizeMode::Tint);
}

QString getColorizedPixmapKey(QPixmap const& pixmap, QColor const& color) {
  return QString("qlementine_color_%1_%2").arg(toHex(pixmap.cacheKey()), toHex(color.rgba()));
}

QString getTintedPixmapKey(QPixmap const& pixmap, QColor const& color) {
  return QString("qlementine_tint_%1_%2").arg(toHex(pixmap.cacheKey()), toHex(color.rgba()));
}

QPixmap getCachedPixmap(QPixmap const& input, QColor const& color, ColorizeMode mode) {
  if (input.isNull())
    return input;

  const auto tint = mode == ColorizeMode::Tint;
  // Look if pixmap already exists in cache.
  const auto& pixmapKey = tint ? getTintedPixmapKey(input, color) : getColorizedPixmapKey(input, color);
  QPixmap pixmapInCache;
  const auto found = QPixmapCache::find(pixmapKey, &pixmapInCache);

  // Add colorized pixmap to cache, if not present.
  if (!found) {
    const auto& newPixmap = tint ? tintPixmap(input, color) : colorizePixmap(input, color);
    const auto successfulInsert = QPixmapCache::insert(pixmapKey, newPixmap);
    if (successfulInsert) {
      QPixmapCache::find(pixmapKey, &pixmapInCache);
    }
  }

  // Returns the new pixmap, or the input if any error.
  return pixmapInCache.isNull() ? input : pixmapInCache;
}

QPixmap makePixmapFromSvg(const QString& svgPath, const QSize& size) {
  if (svgPath.isEmpty())
    return {};

  QSvgRenderer renderer(svgPath);
  QPixmap pixmap(size);
  pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing, true);
  renderer.render(&painter, pixmap.rect());
  return pixmap;
}

QPixmap makePixmapFromSvg(const QString& backgroundSvgPath, const QColor& backgroundColor,
  const QString& foregroundSvgPath, const QColor& foregroundColor, const QSize& size) {
  const auto bgPixmap = makePixmapFromSvg(backgroundSvgPath, size);
  const auto fgPixmap = makePixmapFromSvg(foregroundSvgPath, size);
  const auto coloredBgPixmap = colorizePixmap(bgPixmap, backgroundColor);
  const auto coloredFgPixmap = colorizePixmap(fgPixmap, foregroundColor);

  QPixmap pixmap(size);
  pixmap.fill(Qt::transparent);

  QPainter p(&pixmap);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setRenderHint(QPainter::SmoothPixmapTransform, true);
  p.drawPixmap(0, 0, coloredBgPixmap);
  p.drawPixmap(0, 0, coloredFgPixmap);

  return pixmap;
}

QPixmap makeRoundedPixmap(QPixmap const& input, double radius) {
  return makeRoundedPixmap(input, radius, radius, radius, radius);
}

QPixmap makeRoundedPixmap(QPixmap const& input, const RadiusesF& radiuses) {
  return makeRoundedPixmap(input, radiuses.topLeft, radiuses.topRight, radiuses.bottomRight, radiuses.bottomLeft);
}

QPixmap makeRoundedPixmap(
  QPixmap const& input, double topLeft, double topRight, double bottomRight, double bottomLeft) {
  if (input.isNull())
    return {};

  QPixmap result(input.size());
  result.fill(Qt::transparent);

  QPainter p(&result);

  // Mask.
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setRenderHint(QPainter::SmoothPixmapTransform, true);
  drawRoundedRect(&p, result.rect(), Qt::white, { topLeft, topRight, bottomRight, bottomLeft });
  p.setCompositionMode(QPainter::CompositionMode_SourceIn);
  // Draw input pixmap over.
  p.drawPixmap(result.rect(), input);

  result.setDevicePixelRatio(input.devicePixelRatio());
  return result;
}

QPixmap makeFitPixmap(QPixmap const& input, const QSize& size) {
  if (input.isNull())
    return {};

  QPixmap result(size);
  result.fill(Qt::transparent);

  QPainter p(&result);
  const auto scaledInput =
    input.scaled(size, Qt::AspectRatioMode::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
  const auto x = (result.width() - scaledInput.width()) / 2.;
  const auto y = (result.height() - scaledInput.height()) / 2.;

  p.setRenderHint(QPainter::Antialiasing, true);
  p.setRenderHint(QPainter::SmoothPixmapTransform, true);
  p.drawPixmap(int(x), int(y), scaledInput);

  return result;
}

double getImageAspectRatio(QString const& path) {
  if (path.isEmpty())
    return 1.0;

  const QImageReader reader(path);
  const auto size = reader.size();
  const auto aspectRatio = size.width() / static_cast<double>(size.height());
  return aspectRatio;
}

QImage getExtendedImage(QPixmap const& input, int padding) {
  return getExtendedImage(input.toImage(), padding);
}

QImage getExtendedImage(QImage const& input, int padding) {
  if (input.isNull())
    return {};
  const auto pxRatio = input.devicePixelRatioF();
  // The padding is in logical pixels, so we need to convert it to physical pixels.
  const auto actualPadding = static_cast<int>(std::ceil(std::max(padding, 0) * pxRatio));
  const auto actualExtension = 2 * actualPadding;
  const auto actualSize = input.size();
  const auto extendedSize = QSize(actualSize.width() + actualExtension, actualSize.height() + actualExtension);
  // Compute the extended image.
  QImage inputImage(extendedSize, QImage::Format_ARGB32_Premultiplied);
  {
    const auto start = actualPadding;
    const auto endWidth = extendedSize.width() - actualPadding;
    const auto endHeight = extendedSize.height() - actualPadding;
    for (auto i = 0; i < extendedSize.width(); ++i) {
      for (auto j = 0; j < extendedSize.height(); ++j) {
        if (i < start || i >= endWidth || j < start || j >= endHeight) {
          // Set pixel to transparent.
          inputImage.setPixel(i, j, qRgba(0, 0, 0, 0));
        } else {
          // Copy pixel from input image over the tranparent pixel.
          const auto pixel = input.pixel(i - actualPadding, j - actualPadding);
          if (input.format() == QImage::Format_ARGB32_Premultiplied) {
            // If the input image is already premultiplied, just copy the pixel.
            inputImage.setPixel(i, j, pixel);
          } else {
            // If the input image is not premultiplied, we need to premultiply it.
            const auto alpha = qAlpha(pixel);
            const auto red = qRed(pixel);
            const auto green = qGreen(pixel);
            const auto blue = qBlue(pixel);
            const auto premultipliedPixel = qRgba(red * alpha / 255, green * alpha / 255, blue * alpha / 255, alpha);
            inputImage.setPixel(i, j, premultipliedPixel);
          }
        }
      }
    }
  }
  inputImage.setDevicePixelRatio(pxRatio);
  return inputImage;
}

QImage getBlurredImage(const QImage& inputImage, double blurRadius) {
  if (inputImage.isNull())
    return {};

  auto input = inputImage.copy();
  auto output = inputImage.copy();
  auto* inputData = input.bits();
  auto* outputData = output.bits();
  constexpr auto channelCount = 4; // ARGB
  const auto sigma = blurRadius * inputImage.devicePixelRatioF() / oclero::qlementine::pixelToSigma;
  // Since fast_gaussian_blur does an unnecessary std::swap, the actual result is in input.
  fast_gaussian_blur(inputData, outputData, input.width(), input.height(), channelCount, sigma);
  input.setDevicePixelRatio(inputImage.devicePixelRatioF());
  return input;
}

QPixmap getBlurredPixmap(QPixmap const& input, double blurRadius) {
  if (input.isNull())
    return {};

  const auto inputImage = input.toImage();
  return QPixmap::fromImage(getBlurredImage(inputImage, blurRadius));
}

QPixmap getDropShadowPixmap(QPixmap const& input, double blurRadius, QColor const& color) {
  if (input.isNull())
    return {};

  if (blurRadius * input.devicePixelRatioF() < .5) {
    QPixmap result(input.size());
    result.fill(Qt::transparent);
    result.setDevicePixelRatio(input.devicePixelRatioF());
    return result;
  }

  const auto colorizedImage = colorizeImage(input, color);
  const auto padding = blurRadiusNecessarySpace(blurRadius); // Padding for one side, in logical pixels.
  const auto extendedImage = getExtendedImage(colorizedImage, padding);
  const auto shadowImage = getBlurredImage(extendedImage, blurRadius);
  auto output = QPixmap::fromImage(shadowImage);
  output.setDevicePixelRatio(input.devicePixelRatioF());
  return output;
}

QPixmap getDropShadowPixmap(QSize const& size, double borderRadius, double blurRadius, QColor const& color) {
  if (size.isEmpty())
    return {};

  // Create a pixmap with the same size as the input.
  // The pixmap is already colorized with the desired color.
  QPixmap colorizedImage(size);
  colorizedImage.fill(Qt::transparent);
  {
    QPainter p(&colorizedImage);
    drawRoundedRect(&p, QRect{ QPoint(0, 0), size }, color, borderRadius);
  }

  // Create a blurred version of the colorized image.
  const auto padding = blurRadiusNecessarySpace(blurRadius) * 2;
  const auto extendedImage = getExtendedImage(colorizedImage, padding);
  const auto shadowImage = getBlurredImage(extendedImage, blurRadius);
  auto output = QPixmap::fromImage(shadowImage);
  output.setDevicePixelRatio(colorizedImage.devicePixelRatioF());
  return output;
}

int blurRadiusNecessarySpace(const double blurRadius) {
  return static_cast<int>(std::ceil(blurRadius));
}
} // namespace oclero::qlementine
