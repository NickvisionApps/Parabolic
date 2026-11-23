// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/utils/BadgeUtils.hpp>

#include <oclero/qlementine/utils/PrimitiveUtils.hpp>

#include <QPainterPath>
#include <QPainterStateGuard>

namespace oclero::qlementine {
/// Gets the background and foreground colors.
std::pair<const QColor, const QColor> getStatusBadgeColors(StatusBadge statusBadge, const Theme& theme) {
  switch (statusBadge) {
    case StatusBadge::Error:
      return std::make_pair(theme.statusColorError, theme.statusColorForeground);
    case StatusBadge::Success:
      return std::make_pair(theme.statusColorSuccess, theme.statusColorForeground);
    case StatusBadge::Warning:
      return std::make_pair(theme.statusColorWarning, theme.statusColorForeground);
    case StatusBadge::Info:
    default:
      return std::make_pair(theme.statusColorInfo, theme.statusColorForeground);
  }
}

std::pair<QSize, QSize> getStatusBadgeSizes(StatusBadgeSize statusBadgeSize, const Theme& theme) {
  switch (statusBadgeSize) {
    case StatusBadgeSize::Small:
      return std::make_pair(QSize{ theme.controlHeightSmall, theme.controlHeightSmall }, QSize{ 10, 10 });
    case StatusBadgeSize::Medium:
    default:
      return std::make_pair(QSize{ theme.controlHeightMedium, theme.controlHeightMedium }, theme.iconSize);
  }
}

/// Draws the icons by drawing QPainterPaths directly, instead of using SVG files.
void drawStatusBadgeIcon(QPainter* p, const QRect& rect, StatusBadge statusBadge, StatusBadgeSize statusBadgeSize,
  const QColor& color, qreal lineThickness) {
  QPainterStateGuard stateGuard(p);

  switch (statusBadge) {
    case StatusBadge::Success: {
      if (statusBadgeSize == StatusBadgeSize::Small) {
        p->setBrush(Qt::NoBrush);
        p->setPen(QPen(color, lineThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        const auto halfLineThickness = lineThickness * 0.5;
        const auto ellipseRect = QRectF(rect).marginsRemoved(
          QMarginsF{ halfLineThickness, halfLineThickness, halfLineThickness, halfLineThickness });
        p->drawEllipse(ellipseRect);

        const auto w = rect.width();
        const auto h = rect.width();
        const auto x = rect.x();
        const auto y = rect.y();
        constexpr auto intendedSize = 10.;

        {
          QPainterPath path;
          const auto p1 = QPointF{ (3. / intendedSize) * w + x, (5. / intendedSize) * h + y };
          const auto p2 = QPointF{ (4.5 / intendedSize) * w + x, (6.5 / intendedSize) * h + y };
          const auto p3 = QPointF{ (7. / intendedSize) * w + x, (4. / intendedSize) * h + y };
          path.moveTo(p1);
          path.lineTo(p2);
          path.lineTo(p3);
          p->drawPath(path);
        }
      } else {
        p->setBrush(Qt::NoBrush);
        p->setPen(QPen(color, lineThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        const auto halfLineThickness = lineThickness * 0.5;
        const auto ellipseRect = QRectF(rect).marginsRemoved(
          QMarginsF{ halfLineThickness, halfLineThickness, halfLineThickness, halfLineThickness });
        p->drawEllipse(ellipseRect);

        const auto w = rect.width();
        const auto h = rect.width();
        const auto x = rect.x();
        const auto y = rect.y();
        constexpr auto intendedSize = 16.;

        {
          QPainterPath path;
          const auto p1 = QPointF{ (5. / intendedSize) * w + x, (8.5 / intendedSize) * h + y };
          const auto p2 = QPointF{ (7. / intendedSize) * w + x, (10.5 / intendedSize) * h + y };
          const auto p3 = QPointF{ (11. / intendedSize) * w + x, (6.5 / intendedSize) * h + y };
          path.moveTo(p1);
          path.lineTo(p2);
          path.lineTo(p3);
          p->drawPath(path);
        }
      }
    } break;
    case StatusBadge::Info: {
      if (statusBadgeSize == StatusBadgeSize::Small) {
        p->setBrush(Qt::NoBrush);
        p->setPen(QPen(color, lineThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        const auto halfLineThickness = lineThickness * 0.5;
        const auto ellipseRect = QRectF(rect).marginsRemoved(
          QMarginsF{ halfLineThickness, halfLineThickness, halfLineThickness, halfLineThickness });
        p->drawEllipse(ellipseRect);

        const auto w = rect.width();
        const auto h = rect.width();
        const auto x = rect.x();
        const auto y = rect.y();
        constexpr auto intendedSize = 10.;

        {
          const auto p1 = QPointF{ (5. / intendedSize) * w + x, (5. / intendedSize) * h + y };
          const auto p2 = QPointF{ (5. / intendedSize) * w + x, (7. / intendedSize) * h + y };
          p->drawLine(p1, p2);
        }
        {
          const auto ellipseCenter = QPointF{ (5. / intendedSize) * w + x, (3. / intendedSize) * h + y };
          const auto ellipseRadius = (1.1 * lineThickness / intendedSize) * w;
          p->setPen(Qt::NoPen);
          p->setBrush(color);
          p->drawEllipse(ellipseCenter, ellipseRadius, ellipseRadius);
        }
      } else {
        p->setBrush(Qt::NoBrush);
        p->setPen(QPen(color, lineThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        const auto halfLineThickness = lineThickness * 0.5;
        const auto ellipseRect = QRectF(rect).marginsRemoved(
          QMarginsF{ halfLineThickness, halfLineThickness, halfLineThickness, halfLineThickness });
        p->drawEllipse(ellipseRect);

        const auto w = rect.width();
        const auto h = rect.width();
        const auto x = rect.x();
        const auto y = rect.y();
        constexpr auto intendedSize = 16.;

        {
          QPainterPath path;
          const auto p1 = QPointF{ (6.75 / intendedSize) * w + x, (7. / intendedSize) * h + y };
          const auto p2 = QPointF{ (8. / intendedSize) * w + x, (7. / intendedSize) * h + y };
          const auto p3 = QPointF{ (8. / intendedSize) * w + x, (12. / intendedSize) * h + y };
          path.moveTo(p1);
          path.lineTo(p2);
          path.lineTo(p3);
          p->drawPath(path);
        }
        {
          const auto p1 = QPointF{ (6.75 / intendedSize) * w + x, (12. / intendedSize) * h + y };
          const auto p2 = QPointF{ (9.25 / intendedSize) * w + x, (12. / intendedSize) * h + y };
          p->drawLine(p1, p2);
        }
        {
          const auto ellipseCenter = QPointF{ (8. / intendedSize) * w + x, (4. / intendedSize) * h + y };
          const auto ellipseRadius = (1.1 * lineThickness / intendedSize) * w;
          p->setPen(Qt::NoPen);
          p->setBrush(color);
          p->drawEllipse(ellipseCenter, ellipseRadius, ellipseRadius);
        }
      }
    } break;
    case StatusBadge::Warning: {
      if (statusBadgeSize == StatusBadgeSize::Small) {
        constexpr auto intendedSize = 10.;
        const auto w = rect.width();
        const auto h = rect.width();
        const auto x = rect.x();
        const auto y = rect.y();

        p->setBrush(Qt::NoBrush);
        p->setPen(QPen(color, lineThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        const auto triangleMargin = (1. / intendedSize) * w;
        const auto triangleDeltaY = (1.5 / intendedSize) * h;
        const auto triangleRadius = (1. / intendedSize) * h;
        const auto triangleRect =
          QRectF(rect)
            .marginsAdded(QMarginsF(triangleMargin, triangleMargin, triangleMargin, triangleMargin))
            .translated(QPointF(0., triangleDeltaY));
        qlementine::drawRoundedTriangle(p, triangleRect, triangleRadius);

        {
          const auto p1 = QPointF{ (5. / intendedSize) * w + x, (2.5 / intendedSize) * h + y };
          const auto p2 = QPointF{ (5. / intendedSize) * w + x, (6.5 / intendedSize) * h + y };
          p->drawLine(p1, p2);
        }
        {
          const auto ellipseCenter = QPointF{ (5. / intendedSize) * w + x, (9. / intendedSize) * h + y };
          const auto ellipseRadius = (1.1 * lineThickness / intendedSize) * w;
          p->setPen(Qt::NoPen);
          p->setBrush(color);
          p->drawEllipse(ellipseCenter, ellipseRadius, ellipseRadius);
        }
      } else {
        constexpr auto intendedSize = 16.;
        const auto w = rect.width();
        const auto h = rect.width();
        const auto x = rect.x();
        const auto y = rect.y();

        p->setBrush(Qt::NoBrush);
        p->setPen(QPen(color, lineThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        const auto triangleMargin = (1. / intendedSize) * w;
        const auto triangleDeltaY = (2.5 / intendedSize) * h;
        const auto triangleRadius = (2. / intendedSize) * h;
        const auto triangleRect =
          QRectF(rect)
            .marginsAdded(QMarginsF(triangleMargin, triangleMargin, triangleMargin, triangleMargin))
            .translated(QPointF(0., triangleDeltaY));
        qlementine::drawRoundedTriangle(p, triangleRect, triangleRadius);

        {
          const auto p1 = QPointF{ (8. / intendedSize) * w + x, (5.5 / intendedSize) * h + y };
          const auto p2 = QPointF{ (8. / intendedSize) * w + x, (9.5 / intendedSize) * h + y };
          p->drawLine(p1, p2);
        }
        {
          const auto ellipseCenter = QPointF{ (8. / intendedSize) * w + x, (12. / intendedSize) * h + y };
          const auto ellipseRadius = (1.1 * lineThickness / intendedSize) * w;
          p->setPen(Qt::NoPen);
          p->setBrush(color);
          p->drawEllipse(ellipseCenter, ellipseRadius, ellipseRadius);
        }
      }
    } break;
    case StatusBadge::Error: {
      if (statusBadgeSize == StatusBadgeSize::Small) {
        constexpr auto intendedSize = 10.;
        const auto w = rect.width();
        const auto h = rect.width();
        const auto x = rect.x();
        const auto y = rect.y();

        p->setBrush(Qt::NoBrush);
        p->setPen(QPen(color, lineThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        {
          QPainterPath path;
          const auto p1 = QPointF{ (3. / intendedSize) * w + x, (0.5 / intendedSize) * h + y };
          const auto p2 = QPointF{ (7. / intendedSize) * w + x, (0.5 / intendedSize) * h + y };
          const auto p3 = QPointF{ (9.5 / intendedSize) * w + x, (3. / intendedSize) * h + y };
          const auto p4 = QPointF{ (9.5 / intendedSize) * w + x, (7. / intendedSize) * h + y };
          const auto p5 = QPointF{ (7. / intendedSize) * w + x, (9.5 / intendedSize) * h + y };
          const auto p6 = QPointF{ (3. / intendedSize) * w + x, (9.5 / intendedSize) * h + y };
          const auto p7 = QPointF{ (0.5 / intendedSize) * w + x, (7. / intendedSize) * h + y };
          const auto p8 = QPointF{ (0.5 / intendedSize) * w + x, (3. / intendedSize) * h + y };
          path.moveTo(p1);
          path.lineTo(p2);
          path.lineTo(p3);
          path.lineTo(p4);
          path.lineTo(p5);
          path.lineTo(p6);
          path.lineTo(p7);
          path.lineTo(p8);
          path.closeSubpath();
          p->drawPath(path);
        }
        {
          const auto p1 = QPointF{ (3.5 / intendedSize) * w + x, (3.5 / intendedSize) * h + y };
          const auto p2 = QPointF{ (6.5 / intendedSize) * w + x, (6.5 / intendedSize) * h + y };
          p->drawLine(p1, p2);
        }
        {
          const auto p1 = QPointF{ (3.5 / intendedSize) * w + x, (6.5 / intendedSize) * h + y };
          const auto p2 = QPointF{ (6.5 / intendedSize) * w + x, (3.5 / intendedSize) * h + y };
          p->drawLine(p1, p2);
        }
      } else {
        constexpr auto intendedSize = 16.;
        const auto w = rect.width();
        const auto h = rect.width();
        const auto x = rect.x();
        const auto y = rect.y();

        p->setBrush(Qt::NoBrush);
        p->setPen(QPen(color, lineThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        {
          QPainterPath path;
          const auto p1 = QPointF{ (4.5 / intendedSize) * w + x, (0.5 / intendedSize) * h + y };
          const auto p2 = QPointF{ (11.5 / intendedSize) * w + x, (0.5 / intendedSize) * h + y };
          const auto p3 = QPointF{ (15.5 / intendedSize) * w + x, (4.5 / intendedSize) * h + y };
          const auto p4 = QPointF{ (15.5 / intendedSize) * w + x, (11.5 / intendedSize) * h + y };
          const auto p5 = QPointF{ (11.5 / intendedSize) * w + x, (15.5 / intendedSize) * h + y };
          const auto p6 = QPointF{ (4.5 / intendedSize) * w + x, (15.5 / intendedSize) * h + y };
          const auto p7 = QPointF{ (0.5 / intendedSize) * w + x, (11.5 / intendedSize) * h + y };
          const auto p8 = QPointF{ (0.5 / intendedSize) * w + x, (4.5 / intendedSize) * h + y };
          path.moveTo(p1);
          path.lineTo(p2);
          path.lineTo(p3);
          path.lineTo(p4);
          path.lineTo(p5);
          path.lineTo(p6);
          path.lineTo(p7);
          path.lineTo(p8);
          path.closeSubpath();
          p->drawPath(path);
        }
        {
          const auto p1 = QPointF{ (5.5 / intendedSize) * w + x, (5.5 / intendedSize) * h + y };
          const auto p2 = QPointF{ (10.5 / intendedSize) * w + x, (10.5 / intendedSize) * h + y };
          p->drawLine(p1, p2);
        }
        {
          const auto p1 = QPointF{ (10.5 / intendedSize) * w + x, (5.5 / intendedSize) * h + y };
          const auto p2 = QPointF{ (5.5 / intendedSize) * w + x, (10.5 / intendedSize) * h + y };
          p->drawLine(p1, p2);
        }
      }
    } break;
    default:
      break;
  }
}

void drawStatusBadge(
  QPainter* p, const QRect& rect, StatusBadge statusBadge, StatusBadgeSize size, const Theme& theme) {
  QPainterStateGuard stateGuard(p);
  const auto [bgColor, fgColor] = getStatusBadgeColors(statusBadge, theme);
  const auto [badgeSize, iconSize] = getStatusBadgeSizes(size, theme);

  const auto badgeRect = QRect{
    QPoint{ rect.x() + (rect.width() - badgeSize.width()) / 2, rect.y() + (rect.height() - badgeSize.height()) / 2 },
    badgeSize,
  };
  const auto iconRect = QRect{
    QPoint{ rect.x() + (rect.width() - iconSize.width()) / 2, rect.y() + (rect.height() - iconSize.height()) / 2 },
    iconSize,
  };
  const auto radius = badgeRect.height() / 4.;

  // Background.
  p->setRenderHint(QPainter::Antialiasing, true);
  p->setPen(Qt::NoPen);
  p->setBrush(bgColor);
  p->drawRoundedRect(badgeRect, radius, radius);

  // Foreground.
  const auto lineThickness = 1.0001;
  drawStatusBadgeIcon(p, iconRect, statusBadge, size, fgColor, lineThickness);
}
} // namespace oclero::qlementine
