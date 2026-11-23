// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QString>
#include <QFontMetrics>

namespace oclero::qlementine {

QLEMENTINE_EXPORT double pointSizeToPixelSize(double pointSize, double dpi);

QLEMENTINE_EXPORT double pixelSizeToPointSize(double pixelSize, double dpi);

/**
 * @brief An utility to centralize the calls to QFontMetrics.
 * @param fm The current QFontMetrics to use.
 * @param text The text to compute the width for.
 * @return The width of the text in logical pixels.
 */
QLEMENTINE_EXPORT int textWidth(const QFontMetrics& fm, const QString& text);
} // namespace oclero::qlementine
