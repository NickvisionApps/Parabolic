// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/utils/GeometryUtils.hpp>

namespace oclero::qlementine {
bool isPointInRoundedRect(const QPointF& point, const QRectF& rect, qreal cornerRadius) {
  // Optimisations.
  if (!rect.contains(point)) {
    return false;
  }
  if (cornerRadius <= 1.) {
    return true;
  }

  const auto diameter = cornerRadius * 2.;
  const auto rect_size = QSizeF(diameter, diameter);
  for (const auto& corner : {
         rect.topLeft(),
         rect.topRight() - QPointF(diameter, 0.),
         rect.bottomLeft() - QPointF(diameter, diameter),
         rect.bottomRight() - QPointF(0., diameter),
       }) {
    const auto cornerRect = QRectF(corner, rect_size);
    if (cornerRect.contains(point)) {
      const auto center = cornerRect.center();
      const auto inCircle = std::hypot(point.x() - center.x(), point.y() - center.y()) <= cornerRadius;
      return inCircle;
    }
  }

  return true;
}
} // namespace oclero::qlementine
