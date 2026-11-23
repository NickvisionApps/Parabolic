// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/utils/IconUtils.hpp>

#include <oclero/qlementine/utils/ImageUtils.hpp>

#include <QSvgRenderer>
#include <QPainter>

namespace oclero::qlementine {
namespace {
// Helper function to generate icon from SVG renderer with optional colorization.
QIcon makeIconFromRenderer(QSvgRenderer& svgRenderer, const QSize& size, const IconTheme* iconTheme = nullptr) {
  if (size.isEmpty())
    return {};

  QIcon icon;
  svgRenderer.setAspectRatioMode(Qt::AspectRatioMode::KeepAspectRatio);

  for (const auto pxRatio : { 1., 2. }) {
    QPixmap pixmap(size * pxRatio);
    pixmap.fill(Qt::transparent);
    {
      QPainter painter(&pixmap);
      painter.setRenderHint(QPainter::Antialiasing, true);
      svgRenderer.render(&painter, pixmap.rect());
    }
    pixmap.setDevicePixelRatio(pxRatio);

    for (const auto iconMode : { QIcon::Normal, QIcon::Disabled, QIcon::Active, QIcon::Selected }) {
      for (const auto iconState : { QIcon::On, QIcon::Off }) {
        if (iconTheme) {
          const auto& fgColor = iconTheme->color(iconMode, iconState);
          const auto coloredPixmap = qlementine::getColorizedPixmap(pixmap, fgColor);
          icon.addPixmap(coloredPixmap, iconMode, iconState);
        } else {
          icon.addPixmap(pixmap, iconMode, iconState);
        }
      }
    }
  }

  return icon;
}
} // namespace

IconTheme::IconTheme(const QColor& normal, const QColor& disabled, const QColor& checkedNormal, QColor checkedDisabled)
  : normal(normal)
  , disabled(disabled)
  , checkedNormal(checkedNormal)
  , checkedDisabled(checkedDisabled) {}

IconTheme::IconTheme(const QColor& normal, const QColor& disabled)
  : IconTheme(normal, disabled, normal, disabled) {}

IconTheme::IconTheme(const QColor& normal)
  : IconTheme(normal, normal, normal, normal) {}

const QColor& IconTheme::color(QIcon::Mode mode, QIcon::State state) const {
  switch (mode) {
    case QIcon::Disabled:
      return state == QIcon::On ? checkedDisabled : disabled;
    case QIcon::Normal:
    case QIcon::Active:
    case QIcon::Selected:
    default:
      return state == QIcon::On ? checkedNormal : normal;
  }
}

QIcon makeIconFromSvg(const QString& svgPath, const QSize& size) {
  if (svgPath.isEmpty() || size.isEmpty())
    return {};

  QSvgRenderer svgRenderer(svgPath);
  return makeIconFromRenderer(svgRenderer, size);
}

QIcon makeIconFromSvg(const QString& svgPath, const IconTheme& iconTheme, const QSize& size) {
  if (svgPath.isEmpty() || size.isEmpty())
    return {};

  QSvgRenderer svgRenderer(svgPath);
  return makeIconFromRenderer(svgRenderer, size, &iconTheme);
}

QIcon makeIconFromSvgData(const QByteArray& svgData, const QSize& size) {
  if (svgData.isEmpty() || size.isEmpty())
    return {};

  QSvgRenderer svgRenderer(svgData);
  return makeIconFromRenderer(svgRenderer, size);
}

QIcon makeIconFromSvgData(const QByteArray& svgData, const IconTheme& iconTheme, const QSize& size) {
  if (svgData.isEmpty() || size.isEmpty())
    return {};

  QSvgRenderer svgRenderer(svgData);
  return makeIconFromRenderer(svgRenderer, size, &iconTheme);
}
} // namespace oclero::qlementine
