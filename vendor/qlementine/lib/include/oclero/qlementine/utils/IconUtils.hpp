// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QIcon>
#include <QColor>

namespace oclero::qlementine {
struct QLEMENTINE_EXPORT IconTheme {
  QColor normal;
  QColor disabled;
  QColor checkedNormal;
  QColor checkedDisabled;

  explicit IconTheme(const QColor& normal);
  IconTheme(const QColor& normal, const QColor& disabled);
  IconTheme(const QColor& normal, const QColor& disabled, const QColor& checkedNormal, QColor checkedDisabled);

  const QColor& color(QIcon::Mode mode, QIcon::State state) const;
};

/// Makes an icon from the file located at the path in parameter. Fixes the standard Qt behavior.
[[maybe_unused]] QLEMENTINE_EXPORT QIcon makeIconFromSvg(const QString& svgPath, const QSize& size);

/// Makes an icon from the file located at the path in parameter and colorizes the QPixmaps. Fixes the standard Qt behavior.
[[maybe_unused]] QLEMENTINE_EXPORT QIcon makeIconFromSvg(
  const QString& svgPath, const IconTheme& iconTheme, const QSize& size = QSize(16, 16));

/// Makes an icon from SVG data in memory. Fixes the standard Qt behavior.
[[maybe_unused]] QLEMENTINE_EXPORT QIcon makeIconFromSvgData(const QByteArray& svgData, const QSize& size);

/// Makes an icon from SVG data in memory and colorizes the QPixmaps. Fixes the standard Qt behavior.
[[maybe_unused]] QLEMENTINE_EXPORT QIcon makeIconFromSvgData(
  const QByteArray& svgData, const IconTheme& iconTheme, const QSize& size = QSize(16, 16));
} // namespace oclero::qlementine
