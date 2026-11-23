// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QRectF>
#include <QPointF>

namespace oclero::qlementine {
/**
 * @brief Checks if the point is within the rounded rect surface
 * excluding the space between the square corners and rounded ones.
 * @param point The point to check.
 * @param rect The rect to check against.
 * @param cornerRadius The radius of every rect corner.
 * @return True if the point is within the bounds, false otherwise.
 */
QLEMENTINE_EXPORT bool isPointInRoundedRect(const QPointF& point, const QRectF& rect, qreal cornerRadius);
} // namespace oclero::qlementine
