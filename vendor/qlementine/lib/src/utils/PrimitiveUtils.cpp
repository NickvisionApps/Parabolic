// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/utils/PrimitiveUtils.hpp>
#include <oclero/qlementine/utils/ImageUtils.hpp>
#include <oclero/qlementine/utils/StateUtils.hpp>
#include <oclero/qlementine/utils/FontUtils.hpp>
#include <oclero/qlementine/utils/ColorUtils.hpp>
#include <oclero/qlementine/utils/WidgetUtils.hpp>

#include <QTextLayout>
#include <QTextLine>
#include <QPaintDevice>
#include <QPainter>
#include <QPainterStateGuard>
#include <QPainterPath>
#include <QPixmapCache>
#include <QWindow>
#include <QApplication>

#include <cmath>

static constexpr auto QLEMENTINE_PI_4 = 3.14159265358979323846 / 4.;

namespace oclero::qlementine {
namespace {
double getLength(const double x, const double y) {
  return std::sqrt(x * x + y * y);
}

QPointF getColinearVector(const QPointF& point, double partLength, double vectorX, double vectorY) {
  const auto vectorLength = getLength(vectorX, vectorY);
  const auto factor = vectorLength == 0 ? 0 : partLength / vectorLength;
  return { point.x() - vectorX * factor, point.y() - vectorY * factor };
}

struct AngleRadius {
  double radius{ 0. }; // px
  double startAngle{ 0. }; // Radians
  double sweepAngle{ 0. }; // Radians
  QPointF centerPoint{ 0., 0. };
  QPointF startPoint{ 0., 0. };
  QPointF endPoint{ 0., 0. };
  QPointF translation{ 0., 0. };
};

AngleRadius getAngleRadius(const QPointF& p1, const QPointF& angularPoint, const QPointF& p2, double radius) {
  // Vector 1.
  const auto v1_x = angularPoint.x() - p1.x();
  const auto v1_y = angularPoint.y() - p1.y();

  // Vector 2.
  const auto v2_x = angularPoint.x() - p2.x();
  const auto v2_y = angularPoint.y() - p2.y();

  // The length of segment between angular point and the
  // points of intersection with the circle of a given radius.
  const auto vectorsAngle = (std::atan2(v1_y, v1_x) - std::atan2(v2_y, v2_x));
  const auto tan = std::tan(vectorsAngle / 2.);
  auto segment = radius / tan;

  // We have to choose a smaller radius if there is not enough space available.
  const auto length1 = getLength(v1_x, v1_y);
  const auto length2 = getLength(v2_x, v2_y);
  const auto length = std::min(length1, length2);
  if (segment > length) {
    segment = length;
    radius = length * tan;
  }

  // Points of intersection (vectors are colinear).
  auto startPoint = getColinearVector(angularPoint, segment, v1_x, v1_y);
  auto endPoint = getColinearVector(angularPoint, segment, v2_x, v2_y);

  // Coordinates of the circle center.
  const auto dx = angularPoint.x() * 2. - startPoint.x() - endPoint.x();
  const auto dy = angularPoint.y() * 2. - startPoint.y() - endPoint.y();
  const auto d = getLength(segment, radius);
  const auto circleCenter = getColinearVector(angularPoint, d, dx, dy);

  // StartAngle and EndAngle of arc.
  auto startAngle = -std::atan2(startPoint.y() - circleCenter.y(), startPoint.x() - circleCenter.x());
  auto endAngle = -std::atan2(endPoint.y() - circleCenter.y(), endPoint.x() - circleCenter.x());

  // Sweep angle.
  auto sweepAngle = endAngle - startAngle;
  if (sweepAngle > 0.) {
    startAngle = 2 * QLEMENTINE_PI + startAngle;
    sweepAngle = endAngle - startAngle;
  }

  // Translation need to center in the rect.
  const auto pointOnCircle =
    getColinearVector(circleCenter, radius, circleCenter.x() - angularPoint.x(), circleCenter.y() - angularPoint.y());
  const auto translationX = 2 * static_cast<int>(angularPoint.x() - pointOnCircle.x());
  const auto translationY = 2 * static_cast<int>(angularPoint.y() - pointOnCircle.y());
  const auto translation = QPoint(translationX, translationY);

  constexpr auto radiansToDegrees = 180. / QLEMENTINE_PI;

  return AngleRadius{
    radius,
    startAngle * radiansToDegrees,
    sweepAngle * radiansToDegrees,
    circleCenter,
    startPoint,
    endPoint,
    translation,
  };
}
}; // namespace

double getPixelRatio(QWidget const* w) {
  auto* const window = w && w->window() ? w->window()->windowHandle() : nullptr;
  const auto pixelRatio = window ? window->devicePixelRatio() : 1.;
  return pixelRatio;
}

std::tuple<QString, QString> getMenuLabelAndShortcut(QString const& text) {
  // TODO
  // text format is something like this "Menu text\tShortcut".
  const auto elements = text.split('\t', Qt::KeepEmptyParts);
  const auto& label = !elements.empty() ? elements.first() : QString("");
  const auto& shortcut = elements.size() > 1 ? elements.at(1) : QString("");
  return { label, shortcut };
}

void drawEllipseBorder(QPainter* p, QRectF const& rect, QColor const& color, qreal const borderWidth) {
  QPainterStateGuard stateGuard(p);
  const auto halfBorderW = borderWidth / 2.;
  const auto borderRect = rect.marginsRemoved({ halfBorderW, halfBorderW, halfBorderW, halfBorderW });
  p->setPen(QPen(color, borderWidth, Qt::SolidLine));
  p->setBrush(Qt::NoBrush);
  p->drawEllipse(borderRect);
}

QPainterPath getMultipleRadiusesRectPath(QRectF const& rect, RadiusesF const& radiuses) {
  QPainterPath path;
  const auto w = static_cast<qreal>(rect.width());
  const auto h = static_cast<qreal>(rect.height());
  const auto x = static_cast<qreal>(rect.x());
  const auto y = static_cast<qreal>(rect.y());

  // Ensure angle validity.
  const auto half = std::max(w / 2., h / 2.);
  const auto topLeft = std::min(half, std::max(0., radiuses.topLeft * 2));
  const auto topRight = std::min(half, std::max(0., radiuses.topRight * 2));
  const auto bottomRight = std::min(half, std::max(0., radiuses.bottomRight * 2));
  const auto bottomLeft = std::min(half, std::max(0., radiuses.bottomLeft * 2));

  // Top-left.
  path.moveTo(x, y + topLeft);
  if (topLeft > 0.) {
    path.arcTo(x, y, topLeft, topLeft, 180., -90.);
  }

  // Top-right.
  if (topRight > 0.) {
    path.lineTo(x + w - topRight, y);
    path.arcTo(x + w - topRight, y, topRight, topRight, 90., -90.);
  } else {
    path.lineTo(x + w, y);
  }

  // Bottom-right.
  if (bottomRight > 0.) {
    path.lineTo(x + w, y + h - bottomRight);
    path.arcTo(x + w - bottomRight, y + h - bottomRight, bottomRight, bottomRight, 0., -90.);
  } else {
    path.lineTo(x + w, y + h);
  }

  // Bottom-left.
  if (bottomLeft > 0.) {
    path.lineTo(x + bottomLeft, y + h);
    path.arcTo(x, y + h - bottomLeft, bottomLeft, bottomLeft, 270., -90.);
  } else {
    path.lineTo(x, y + h);
  }
  path.closeSubpath();

  return path;
}

void drawRoundedRect(QPainter* p, QRectF const& rect, QBrush const& brush, qreal const radius) {
  QPainterStateGuard stateGuard(p);
  if (radius < 0.1) {
    p->fillRect(rect, brush);
  } else {
    p->setRenderHint(QPainter::RenderHint::Antialiasing, true);
    p->setPen(Qt::NoPen);
    p->setBrush(brush);
    p->drawRoundedRect(rect, radius, radius);
  }
}

void drawRoundedRect(QPainter* p, QRectF const& rect, QBrush const& brush, RadiusesF const& radiuses) {
  QPainterStateGuard stateGuard(p);
  if (radiuses.hasSameRadius()) {
    drawRoundedRect(p, rect, brush, radiuses.topLeft);
  } else {
    const auto path = getMultipleRadiusesRectPath(rect, radiuses);
    p->setRenderHint(QPainter::Antialiasing, true);
    p->setPen(Qt::NoPen);
    p->setBrush(brush);
    p->drawPath(path);
  }
}

void drawRoundedRect(QPainter* p, QRect const& rect, QBrush const& brush, qreal const radius) {
  QPainterStateGuard stateGuard(p);
  if (radius < 0.1) {
    p->fillRect(rect, brush);
  } else {
    p->setRenderHint(QPainter::RenderHint::Antialiasing, true);
    p->setPen(Qt::NoPen);
    p->setBrush(brush);
    p->drawRoundedRect(rect, radius, radius);
  }
}

void drawRoundedRect(QPainter* p, QRect const& rect, QBrush const& brush, RadiusesF const& radiuses) {
  QPainterStateGuard stateGuard(p);
  if (radiuses.hasSameRadius()) {
    drawRoundedRect(p, rect, brush, radiuses.topLeft);
  } else {
    const auto path = getMultipleRadiusesRectPath(rect, radiuses);
    p->setRenderHint(QPainter::Antialiasing, true);
    p->setPen(Qt::NoPen);
    p->setBrush(brush);
    p->drawPath(path);
  }
}

void drawRoundedRectBorder(
  QPainter* p, QRectF const& rect, QColor const& color, qreal const borderWidth, qreal const radius) {
  QPainterStateGuard stateGuard(p);
  if (borderWidth > 0) {
    p->setRenderHint(QPainter::RenderHint::Antialiasing);
    p->setPen(QPen{ color, borderWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin });
    p->setBrush(Qt::NoBrush);
    const auto halfBorderW = borderWidth / 2.;
    const auto borderRect = rect.marginsRemoved({ halfBorderW, halfBorderW, halfBorderW, halfBorderW });
    const auto borderRadius = radius - halfBorderW;
    if (borderRadius < 0.1) {
      p->drawRect(borderRect);
    } else {
      p->drawRoundedRect(borderRect, borderRadius, borderRadius);
    }
  }
}

void drawRoundedRectBorder(
  QPainter* p, QRect const& rect, QColor const& color, qreal const borderWidth, qreal const radius) {
  drawRoundedRectBorder(p, rect.toRectF(), color, borderWidth, radius);
}

void drawRoundedRectBorder(
  QPainter* p, QRectF const& rect, QColor const& color, qreal const borderWidth, RadiusesF const& radiuses) {
  QPainterStateGuard stateGuard(p);
  if (borderWidth > 0) {
    if (radiuses.hasSameRadius()) {
      drawRoundedRectBorder(p, rect, color, borderWidth, radiuses.topLeft);
    } else {
      p->setRenderHint(QPainter::RenderHint::Antialiasing);
      p->setPen(QPen{ color, borderWidth, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin });
      p->setBrush(Qt::NoBrush);

      const auto halfBorderW = borderWidth / 2.;
      const auto borderRect = rect.marginsRemoved({ halfBorderW, halfBorderW, halfBorderW, halfBorderW });
      const auto borderRadiuses = radiuses - halfBorderW;
      if (borderRadiuses < 0.1) {
        p->drawRect(borderRect);
      } else {
        const auto path = getMultipleRadiusesRectPath(borderRect, borderRadiuses);
        p->drawPath(path);
      }
    }
  }
}

void drawRoundedRectBorder(
  QPainter* p, QRect const& rect, QColor const& color, qreal const borderWidth, RadiusesF const& radiuses) {
  drawRoundedRectBorder(p, QRectF(rect), color, borderWidth, radiuses);
}

void drawRectBorder(QPainter* p, QRect const& rect, QColor const& color, qreal const borderWidth) {
  drawRectBorder(p, QRectF(rect), color, borderWidth);
}

void drawRectBorder(QPainter* p, QRectF const& rect, QColor const& color, qreal const borderWidth) {
  QPainterStateGuard stateGuard(p);
  if (borderWidth > 0) {
    p->setRenderHint(QPainter::RenderHint::Antialiasing);
    p->setPen(QPen{ color, borderWidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin });
    p->setBrush(Qt::NoBrush);
    const auto halfBorderW = borderWidth / 2.;
    const auto borderRect = rect.marginsRemoved({ halfBorderW, halfBorderW, halfBorderW, halfBorderW });
    p->drawRect(borderRect);
  }
}

void drawRoundedTriangle(QPainter* p, QRectF const& rect, qreal const radius) {
  QPainterStateGuard stateGuard(p);
  const auto w = rect.width();
  const auto h = rect.height();
  const auto x = rect.x();
  const auto y = rect.y();

  p->setRenderHint(QPainter::Antialiasing, true);

  const auto p1 = QPointF(x + w / 2., y);
  const auto p2 = QPointF(x + w, y + h);
  const auto p3 = QPointF(x, y + h);

  /* The angles correspond to the following points:
       P1
      /  \
     /    \
    P3----P2
  */
  const auto angle1 = getAngleRadius(p3, p1, p2, radius);
  const auto angle2 = getAngleRadius(p1, p2, p3, radius);
  const auto angle3 = getAngleRadius(p2, p3, p1, radius);

  QPainterPath path;
  const auto diameter = radius * 2.;
  path.moveTo(angle1.startPoint);
  path.arcTo(QRectF(angle1.centerPoint.x() - radius, angle1.centerPoint.y() - radius, diameter, diameter),
    angle1.startAngle, angle1.sweepAngle);

  path.lineTo(angle2.startPoint);
  path.arcTo(QRectF(angle2.centerPoint.x() - radius, angle2.centerPoint.y() - radius, diameter, diameter),
    angle2.startAngle, angle2.sweepAngle);

  path.lineTo(angle3.startPoint);
  path.arcTo(QRectF(angle3.centerPoint.x() - radius, angle3.centerPoint.y() - radius, diameter, diameter),
    angle3.startAngle, angle3.sweepAngle);

  path.lineTo(angle1.startPoint);
  path.closeSubpath();

  const auto tr_x = (angle1.translation.x() + angle2.translation.x() + angle3.translation.x());
  const auto tr_y = (angle1.translation.y() + angle2.translation.y() + angle3.translation.y());

  p->translate(tr_x, tr_y);
  p->drawPath(path);
  p->translate(-tr_x, -tr_y);
}

void drawCheckerboard(
  QPainter* p, const QRectF& rect, const QColor& darkColor, const QColor& lightColor, const qreal cellWidth) {
  QPainterStateGuard stateGuard(p);
  const auto hCellCount = rect.width() / cellWidth;
  const auto vCellCount = rect.height() / cellWidth;

  p->setPen(Qt::NoPen);
  for (auto i = 0; i < hCellCount; ++i) {
    for (auto j = 0; j < vCellCount; ++j) {
      const auto& cellColor = (i + j) % 2 == 0 ? darkColor : lightColor;
      const auto cellX = rect.x() + i * cellWidth;
      const auto cellY = rect.y() + j * cellWidth;
      const auto cellW = std::max(1., hCellCount - i) * cellWidth;
      const auto cellH = std::max(1., vCellCount - j) * cellWidth;
      const auto squareRect = QRectF(cellX, cellY, cellW, cellH);
      p->setBrush(cellColor);
      p->drawRect(squareRect);
    }
  }
}

void drawProgressBarValueRect(QPainter* p, QRect const& rect, QColor const& color, qreal min, qreal max, qreal value,
  qreal const radius, bool inverted, Qt::Orientation orientation) {
  const auto ratio = (max != min) ? (value - min) / (max - min) : 0;
  const auto valueRect = [&]() {
    if (orientation == Qt::Horizontal) {
      const auto w = static_cast<int>(rect.width() * ratio);
      return QRect{ inverted ? rect.x() + rect.width() - w : rect.x(), rect.y(), w, rect.height() };
    }
    const auto h = static_cast<int>(rect.height() * ratio);
    return QRect{ rect.x(), inverted ? rect.y() + rect.height() - h : rect.y(), rect.width(), h };
  }();

  QPainterPath clipPath;
  clipPath.addRoundedRect(rect, radius, radius);
  QPainterStateGuard stateGuard(p);
  {
    p->setClipPath(clipPath);
    p->fillRect(valueRect, color);
  }
}

void drawColorMark(QPainter* p, QRect const& rect, const QColor& color, const QColor& borderColor, int borderWidth) {
  QPainterStateGuard stateGuard(p);
  const auto circleDiameter = rect.height();
  const auto markRect = QRect((rect.width() - circleDiameter) / 2, 0, circleDiameter, circleDiameter);

  p->setRenderHint(QPainter::Antialiasing, true);

  // Draw checkerboard if the color has semi-transparency.
  if (color.alphaF() < 1.) {
    static const auto darkCellColor = QColor(220, 220, 220);
    static const auto lightCellColor = QColor(255, 255, 255);
    constexpr auto cellWidth = 8;

    QPainterPath clipPath;
    clipPath.addEllipse(markRect);
    QPainterStateGuard clipStateGuard(p);
    {
      p->setClipPath(clipPath);
      drawCheckerboard(p, markRect, darkCellColor, lightCellColor, cellWidth);
    }
  }

  // Draw background.
  p->setPen(Qt::NoPen);
  p->setBrush(color);
  p->drawEllipse(markRect);

  // Draw border for when contrast is not high enough.
  borderWidth = std::max(1, borderWidth);
  drawEllipseBorder(p, markRect, borderColor, borderWidth * 1.5); // get better readability.
}

void drawColorMarkBorder(QPainter* p, QRect const& rect, const QColor& borderColor, int borderWidth) {
  const auto circleDiameter = rect.height();
  const auto markRect = QRect(int((rect.width() - circleDiameter) * 0.5), 0, circleDiameter, circleDiameter);
  drawEllipseBorder(p, markRect, borderColor, borderWidth * 1.5); // get better readability.
}

void drawDebugRect(const QRect& rect, QPainter* p) {
  p->fillRect(rect, QColor(255, 0, 0, 32));
}


QPainterPath getMenuIndicatorPath(const QRect& rect) {
  const auto w = rect.width();
  const auto h = rect.width();
  const auto x = rect.x();
  const auto y = rect.y();
  constexpr auto intendedSize = 16.;

  const auto p1 = QPointF{ (3.5 / intendedSize) * w + x, (6.5 / intendedSize) * h + y };
  const auto p2 = QPointF{ (8. / intendedSize) * w + x, (11. / intendedSize) * h + y };
  const auto p3 = QPointF{ (12.5 / intendedSize) * w + x, (6.5 / intendedSize) * h + y };

  QPainterPath indicatorPath;
  indicatorPath.moveTo(p1);
  indicatorPath.lineTo(p2);
  indicatorPath.lineTo(p3);
  return indicatorPath;
}


void drawComboBoxIndicator(const QRect& rect, QPainter* p) {
  const auto w = rect.width();
  const auto h = rect.width();
  const auto x = rect.x();
  const auto y = rect.y();
  constexpr auto intendedSize = 16.;

  const auto p1 = QPointF{ (5.5 / intendedSize) * w + x, (5.5 / intendedSize) * h + y };
  const auto p2 = QPointF{ (8. / intendedSize) * w + x, (3. / intendedSize) * h + y };
  const auto p3 = QPointF{ (10.5 / intendedSize) * w + x, (5.5 / intendedSize) * h + y };
  QPainterPath indicatorPath1;
  indicatorPath1.moveTo(p1);
  indicatorPath1.lineTo(p2);
  indicatorPath1.lineTo(p3);
  p->drawPath(indicatorPath1);

  const auto p4 = QPointF{ (5.5 / intendedSize) * w + x, (10.5 / intendedSize) * h + y };
  const auto p5 = QPointF{ (8. / intendedSize) * w + x, (13. / intendedSize) * h + y };
  const auto p6 = QPointF{ (10.5 / intendedSize) * w + x, (10.5 / intendedSize) * h + y };
  QPainterPath indicatorPath2;
  indicatorPath2.moveTo(p4);
  indicatorPath2.lineTo(p5);
  indicatorPath2.lineTo(p6);
  p->drawPath(indicatorPath2);
}

void drawCheckBoxIndicator(const QRect& rect, QPainter* p, qreal progress) {
  const auto w = rect.width();
  const auto h = rect.width();
  const auto x = rect.x();
  const auto y = rect.y();
  constexpr auto intendedSize = 16.;

  const auto p1 = QPointF{ (4.5 / intendedSize) * w + x, (8.5 / intendedSize) * h + y };
  const auto p2 = QPointF{ (7 / intendedSize) * w + x, (11. / intendedSize) * h + y };
  const auto p3 = QPointF{ (11.5 / intendedSize) * w + x, (5.5 / intendedSize) * h + y };

  QPainterPath indicatorPath;
  indicatorPath.moveTo(p1);
  indicatorPath.lineTo(p2);
  indicatorPath.lineTo(p3);

  // Draw only a certain percentage of the path.
  if (1. - progress > 0.01) {
    const auto lastPoint = indicatorPath.pointAtPercent(progress);
    QPainterPath trimmedPath;
    trimmedPath.moveTo(p1);
    if (progress < 0.5) {
      trimmedPath.lineTo(lastPoint);
    } else {
      trimmedPath.lineTo(p2);
      trimmedPath.lineTo(lastPoint);
    }
    indicatorPath = trimmedPath;
  }

  p->drawPath(indicatorPath);
}

void drawPartiallyCheckedCheckBoxIndicator(const QRect& rect, QPainter* p, qreal progress) {
  const auto w = rect.width();
  const auto h = rect.width();
  const auto x = rect.x();
  const auto y = rect.y();
  constexpr auto intendedSize = 16.;

  const auto p1 = QPointF{ (4. / intendedSize) * w + x, (8. / intendedSize) * h + y };
  const auto p2 = QPointF{ (12. / intendedSize) * w + x, (8. / intendedSize) * h + y };

  QPainterPath indicatorPath;
  indicatorPath.moveTo(p1);
  indicatorPath.lineTo(p2);

  // Draw only a certain percentage of the path.
  if (1. - progress > 0.01) {
    const auto lastPoint = indicatorPath.pointAtPercent(progress);
    QPainterPath trimmedPath;
    trimmedPath.moveTo(p1);
    trimmedPath.lineTo(lastPoint);
    indicatorPath = trimmedPath;
  }

  p->drawPath(indicatorPath);
}

void drawRadioButtonIndicator(const QRect& rect, QPainter* p, qreal progress) {
  constexpr auto intendedRatio = 8. / 16.;

  const auto indicatorW = rect.width() * intendedRatio * progress;
  const auto indicatorH = rect.height() * intendedRatio * progress;
  const auto ellipseRect = QRectF{ rect.x() + (rect.width() - indicatorW) / 2.,
    rect.y() + (rect.height() - indicatorH) / 2., indicatorW, indicatorH };
  p->drawEllipse(ellipseRect);
}

void drawSpinBoxArrowIndicator(const QRect& rect, QPainter* p, QAbstractSpinBox::ButtonSymbols buttonSymbol,
  QStyle::SubControl subControl, QSize const& iconSize) {
  if (buttonSymbol == QAbstractSpinBox::NoButtons)
    return;

  const auto iconSizeW = std::min(rect.width(), iconSize.width());
  const auto iconSizeH = std::min(rect.height(), iconSize.height());
  const auto iconRectX = rect.x() + (rect.width() - iconSizeW) / 2;
  const auto iconRectY = rect.y() + (rect.height() - iconSizeH) / 2;
  const auto iconRect = QRect(iconRectX, iconRectY, iconSizeW, iconSizeH);

  const auto x = iconRect.x();
  const auto y = iconRect.y();
  const auto w = iconRect.width();
  const auto h = iconRect.height();

  if (buttonSymbol == QAbstractSpinBox::PlusMinus) {
    if (subControl == QStyle::SC_SpinBoxUp) {
      const auto p1 = QPointF(x + w / 2, y); //NOLINT (we do want integer division here)
      const auto p2 = QPointF(x + w / 2, y + h); //NOLINT (we do want integer division here)
      p->drawLine(p1, p2);

      const auto p3 = QPointF(x, y + h / 2); //NOLINT (we do want integer division here)
      const auto p4 = QPointF(x + w, y + h / 2); //NOLINT (we do want integer division here)
      p->drawLine(p3, p4);
    } else if (subControl == QStyle::SC_SpinBoxDown) {
      const auto p1 = QPointF(x, y + h / 2); //NOLINT (we do want integer division here)
      const auto p2 = QPointF(x + w, y + h / 2); //NOLINT (we do want integer division here)
      p->drawLine(p1, p2);
    }
  } else if (buttonSymbol == QAbstractSpinBox::UpDownArrows) {
    constexpr auto intendedSize = 8.;
    if (subControl == QStyle::SC_SpinBoxUp) {
      const auto p1 = QPointF{ (1. / intendedSize) * w + x, (5. / intendedSize) * h + y };
      const auto p2 = QPointF{ (4. / intendedSize) * w + x, (2. / intendedSize) * h + y };
      const auto p3 = QPointF{ (7. / intendedSize) * w + x, (5. / intendedSize) * h + y };

      QPainterPath path;
      path.moveTo(p1);
      path.lineTo(p2);
      path.lineTo(p3);
      p->drawPath(path);
    } else if (subControl == QStyle::SC_SpinBoxDown) {
      const auto p1 = QPointF{ (1. / intendedSize) * w + x, (3. / intendedSize) * h + y };
      const auto p2 = QPointF{ (4. / intendedSize) * w + x, (6. / intendedSize) * h + y };
      const auto p3 = QPointF{ (7. / intendedSize) * w + x, (3. / intendedSize) * h + y };

      QPainterPath path;
      path.moveTo(p1);
      path.lineTo(p2);
      path.lineTo(p3);
      p->drawPath(path);
    }
  }
}

void drawArrowRight(const QRect& rect, QPainter* p) {
  const auto w = rect.width();
  const auto h = rect.width();
  const auto x = rect.x();
  const auto y = rect.y();
  constexpr auto intendedSize = 16.;

  const auto p1 = QPointF{ (6.5 / intendedSize) * w + x, (4.5 / intendedSize) * h + y };
  const auto p2 = QPointF{ (10. / intendedSize) * w + x, (8. / intendedSize) * h + y };
  const auto p3 = QPointF{ (6.5 / intendedSize) * w + x, (11.5 / intendedSize) * h + y };
  QPainterPath path;
  path.moveTo(p1);
  path.lineTo(p2);
  path.lineTo(p3);
  p->drawPath(path);
}

void drawArrowLeft(const QRect& rect, QPainter* p) {
  const auto w = rect.width();
  const auto h = rect.width();
  const auto x = rect.x();
  const auto y = rect.y();
  constexpr auto intendedSize = 16.;

  const auto p1 = QPointF{ (9.5 / intendedSize) * w + x, (4.5 / intendedSize) * h + y };
  const auto p2 = QPointF{ (6. / intendedSize) * w + x, (8. / intendedSize) * h + y };
  const auto p3 = QPointF{ (9.5 / intendedSize) * w + x, (11.5 / intendedSize) * h + y };
  QPainterPath path;
  path.moveTo(p1);
  path.lineTo(p2);
  path.lineTo(p3);
  p->drawPath(path);
}

void drawArrowDown(const QRect& rect, QPainter* p) {
  const auto w = rect.width();
  const auto h = rect.width();
  const auto x = rect.x();
  const auto y = rect.y();
  constexpr auto intendedSize = 16.;

  const auto p1 = QPointF{ (4.5 / intendedSize) * w + x, (6.5 / intendedSize) * h + y };
  const auto p2 = QPointF{ (8. / intendedSize) * w + x, (10. / intendedSize) * h + y };
  const auto p3 = QPointF{ (11.5 / intendedSize) * w + x, (6.5 / intendedSize) * h + y };
  QPainterPath path;
  path.moveTo(p1);
  path.lineTo(p2);
  path.lineTo(p3);
  p->drawPath(path);
}

void drawArrowUp(const QRect& rect, QPainter* p) {
  const auto w = rect.width();
  const auto h = rect.width();
  const auto x = rect.x();
  const auto y = rect.y();
  constexpr auto intendedSize = 16.;

  const auto p1 = QPointF{ (4.5 / intendedSize) * w + x, (10. / intendedSize) * h + y };
  const auto p2 = QPointF{ (8. / intendedSize) * w + x, (6.5 / intendedSize) * h + y };
  const auto p3 = QPointF{ (11.5 / intendedSize) * w + x, (10. / intendedSize) * h + y };
  QPainterPath path;
  path.moveTo(p1);
  path.lineTo(p2);
  path.lineTo(p3);
  p->drawPath(path);
}

void drawSubMenuIndicator(const QRect& rect, QPainter* p) {
  const auto w = rect.width();
  const auto h = rect.width();
  const auto x = rect.x();
  const auto y = rect.y();
  constexpr auto intendedSize = 16.;

  const auto p1 = QPointF{ (10.5 / intendedSize) * w + x, (4.5 / intendedSize) * h + y };
  const auto p2 = QPointF{ (14. / intendedSize) * w + x, (8. / intendedSize) * h + y };
  const auto p3 = QPointF{ (10.5 / intendedSize) * w + x, (11.5 / intendedSize) * h + y };
  QPainterPath path;
  path.moveTo(p1);
  path.lineTo(p2);
  path.lineTo(p3);
  p->drawPath(path);
}

void drawDoubleArrowRightIndicator(const QRect& rect, QPainter* p) {
  const auto w = rect.width();
  const auto h = rect.width();
  const auto x = rect.x();
  const auto y = rect.y();
  constexpr auto intendedSize = 16.;

  {
    const auto p1 = QPointF{ (4. / intendedSize) * w + x, (4. / intendedSize) * h + y };
    const auto p2 = QPointF{ (8. / intendedSize) * w + x, (8. / intendedSize) * h + y };
    const auto p3 = QPointF{ (4. / intendedSize) * w + x, (12. / intendedSize) * h + y };
    QPainterPath arrowPath1;
    arrowPath1.moveTo(p1);
    arrowPath1.lineTo(p2);
    arrowPath1.lineTo(p3);
    p->drawPath(arrowPath1);
  }

  {
    const auto p4 = QPointF{ (9. / intendedSize) * w + x, (4. / intendedSize) * h + y };
    const auto p5 = QPointF{ (13. / intendedSize) * w + x, (8. / intendedSize) * h + y };
    const auto p6 = QPointF{ (9. / intendedSize) * w + x, (12. / intendedSize) * h + y };
    QPainterPath arrowPath2;
    arrowPath2.moveTo(p4);
    arrowPath2.lineTo(p5);
    arrowPath2.lineTo(p6);
    p->drawPath(arrowPath2);
  }
}

void drawToolBarExtensionIndicator(const QRect& rect, QPainter* p) {
  const auto w = rect.width();
  const auto h = rect.width();
  const auto x = rect.x();
  const auto y = rect.y();
  constexpr auto intendedSize = 16.;

  {
    const auto p1 = QPointF{ (5.5 / intendedSize) * w + x, (5.5 / intendedSize) * h + y };
    const auto p2 = QPointF{ (8. / intendedSize) * w + x, (8. / intendedSize) * h + y };
    const auto p3 = QPointF{ (5.5 / intendedSize) * w + x, (10.5 / intendedSize) * h + y };
    QPainterPath arrowPath1;
    arrowPath1.moveTo(p1);
    arrowPath1.lineTo(p2);
    arrowPath1.lineTo(p3);
    p->drawPath(arrowPath1);
  }

  {
    const auto p4 = QPointF{ (9.5 / intendedSize) * w + x, (5.5 / intendedSize) * h + y };
    const auto p5 = QPointF{ (12. / intendedSize) * w + x, (8. / intendedSize) * h + y };
    const auto p6 = QPointF{ (9.5 / intendedSize) * w + x, (10.5 / intendedSize) * h + y };
    QPainterPath arrowPath2;
    arrowPath2.moveTo(p4);
    arrowPath2.lineTo(p5);
    arrowPath2.lineTo(p6);
    p->drawPath(arrowPath2);
  }
}

void drawCloseIndicator(const QRect& rect, QPainter* p) {
  const auto w = rect.width();
  const auto h = rect.width();
  const auto x = rect.x();
  const auto y = rect.y();
  constexpr auto intendedSize = 16.;

  {
    const auto p1 = QPointF{ (4. / intendedSize) * w + x, (4. / intendedSize) * h + y };
    const auto p2 = QPointF{ (12. / intendedSize) * w + x, (12. / intendedSize) * h + y };
    p->drawLine(p1, p2);
  }
  {
    const auto p3 = QPointF{ (12. / intendedSize) * w + x, (4. / intendedSize) * h + y };
    const auto p4 = QPointF{ (4. / intendedSize) * w + x, (12. / intendedSize) * h + y };
    p->drawLine(p3, p4);
  }
}

void drawTreeViewIndicator(const QRect& rect, QPainter* p, bool open) {
  if (open) {
    drawArrowDown(rect, p);
  } else {
    drawArrowRight(rect, p);
  }
}

void drawCalendarIndicator(const QRect& rect, QPainter* p, const QColor& color) {
  QPainterStateGuard stateGuard(p);
  constexpr auto defaultSize = 16.;
  constexpr auto defaultPenWidth = 1.01;
  constexpr auto defaultRadius = 2.5;
  constexpr auto defaultDayRadius = 0.5;
  const auto sizeRatio = rect.size().width() / defaultSize;
  const auto penWidth = sizeRatio * defaultPenWidth;
  const auto radius = sizeRatio * defaultRadius;
  p->setRenderHint(QPainter::Antialiasing, true);

  // Draw spiral-bound notebook shape.
  p->setBrush(Qt::NoBrush);
  p->setPen(QPen{ color, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin });
  drawRoundedRectBorder(p,
    QRectF{ rect.x() + sizeRatio * 1., rect.y() + sizeRatio * 1., sizeRatio * 14, sizeRatio * 14 }, Qt::black, penWidth,
    radius);
  p->drawLine(sizeRatio * QPointF{ 1.5, 5.5 }, sizeRatio * QPointF{ 14.5, 5.5 });
  p->drawLine(sizeRatio * QPointF{ 4.5, 0.5 }, sizeRatio * QPointF{ 4.5, 2.5 });
  p->drawLine(sizeRatio * QPointF{ 11.5, 0.5 }, sizeRatio * QPointF{ 11.5, 2.5 });

  // Draw days.
  p->setPen(Qt::NoPen);
  p->setBrush(color);
  const auto dayBlockSize = sizeRatio * QSizeF{ 2., 2. };
  p->drawRoundedRect(
    QRectF{ rect.topLeft() + sizeRatio * QPointF{ 4., 7. }, dayBlockSize }, defaultDayRadius, defaultDayRadius);
  p->drawRoundedRect(
    QRectF{ rect.topLeft() + sizeRatio * QPointF{ 7., 7. }, dayBlockSize }, defaultDayRadius, defaultDayRadius);
  p->drawRoundedRect(
    QRectF{ rect.topLeft() + sizeRatio * QPointF{ 10., 7. }, dayBlockSize }, defaultDayRadius, defaultDayRadius);
  p->drawRoundedRect(
    QRectF{ rect.topLeft() + sizeRatio * QPointF{ 4., 10. }, dayBlockSize }, defaultDayRadius, defaultDayRadius);
  p->drawRoundedRect(
    QRectF{ rect.topLeft() + sizeRatio * QPointF{ 7., 10. }, dayBlockSize }, defaultDayRadius, defaultDayRadius);
}

void drawGripIndicator(const QRect& rect, QPainter* p, const QColor& color, Qt::Orientation orientation) {
  QPainterStateGuard stateGuard(p);
  constexpr auto defaultSize = 16.;
  constexpr auto defaultBulletDiameter = 2.;

  p->setPen(Qt::NoPen);
  p->setBrush(color);

  const auto wRatio = rect.width() / defaultSize;
  const auto hRatio = rect.height() / defaultSize;
  const auto bulletSize = QSize(defaultBulletDiameter * wRatio, defaultBulletDiameter * hRatio);

  if (orientation == Qt::Vertical) {
    p->drawEllipse(QRect(rect.topLeft() + QPoint(5. * wRatio, 3. * hRatio), bulletSize));
    p->drawEllipse(QRect(rect.topLeft() + QPoint(5. * wRatio, 7. * hRatio), bulletSize));
    p->drawEllipse(QRect(rect.topLeft() + QPoint(5. * wRatio, 11. * hRatio), bulletSize));
    p->drawEllipse(QRect(rect.topLeft() + QPoint(9. * wRatio, 3. * hRatio), bulletSize));
    p->drawEllipse(QRect(rect.topLeft() + QPoint(9. * wRatio, 7. * hRatio), bulletSize));
    p->drawEllipse(QRect(rect.topLeft() + QPoint(9. * wRatio, 11. * hRatio), bulletSize));
  } else {
    p->drawEllipse(QRect(rect.topLeft() + QPoint(3. * wRatio, 5. * hRatio), bulletSize));
    p->drawEllipse(QRect(rect.topLeft() + QPoint(7. * wRatio, 5. * hRatio), bulletSize));
    p->drawEllipse(QRect(rect.topLeft() + QPoint(11. * wRatio, 5. * hRatio), bulletSize));
    p->drawEllipse(QRect(rect.topLeft() + QPoint(3. * wRatio, 9. * hRatio), bulletSize));
    p->drawEllipse(QRect(rect.topLeft() + QPoint(7. * wRatio, 9. * hRatio), bulletSize));
    p->drawEllipse(QRect(rect.topLeft() + QPoint(11. * wRatio, 9. * hRatio), bulletSize));
  }
}

void drawRadioButton(QPainter* p, const QRect& rect, QColor const& bgColor, const QColor& borderColor,
  QColor const& fgColor, const qreal borderWidth, qreal progress) {
  QPainterStateGuard stateGuard(p);
  // Background.
  p->setRenderHint(QPainter::RenderHint::Antialiasing);
  p->setPen(Qt::NoPen);
  p->setBrush(bgColor);
  // To avoid ugly visual artifacts in the rounded corners, we cheat by reducing a bit the size
  const auto& ellipseRect =
    borderWidth > 0.1
      ? QRectF(rect).marginsRemoved(QMarginsF(borderWidth / 2., borderWidth / 2., borderWidth / 2., borderWidth / 2.))
      : rect;
  p->drawEllipse(ellipseRect);

  // Border.
  if (borderWidth > 0.1) {
    drawEllipseBorder(p, rect, borderColor, borderWidth);
  }

  // Foreground.
  if (progress > 0.01) {
    p->setBrush(fgColor);
    p->setPen(Qt::NoPen);
    drawRadioButtonIndicator(rect, p, progress);
  }
}

void drawCheckButton(QPainter* p, const QRect& rect, qreal radius, const QColor& bgColor, const QColor& borderColor,
  QColor const& fgColor, const qreal borderWidth, qreal progress, CheckState checkState) {
  QPainterStateGuard stateGuard(p);
  // Background.
  p->setRenderHint(QPainter::RenderHint::Antialiasing);
  if (radius < 1) {
    p->fillRect(rect, bgColor);
  } else {
    p->setPen(Qt::NoPen);
    p->setBrush(bgColor);
    p->setRenderHint(QPainter::RenderHint::Antialiasing);
    // To avoid ugly visual artifacts in the rounded corners, we cheat by reducing a bit the size
    const auto& buttonRect =
      borderWidth > 0.1
        ? QRectF(rect).marginsRemoved(QMarginsF(borderWidth / 2., borderWidth / 2., borderWidth / 2., borderWidth / 2.))
        : rect;
    p->drawRoundedRect(buttonRect, radius, radius);
  }

  // Border.
  if (borderWidth > 0.1) {
    drawRoundedRectBorder(p, rect, borderColor, borderWidth, radius);
  }

  // Foreground.
  if (progress > 0.01) {
    constexpr auto checkThickness = 2.;
    p->setBrush(Qt::NoBrush);
    p->setPen(QPen{ fgColor, checkThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin });
    if (checkState == CheckState::Checked) {
      drawCheckBoxIndicator(rect, p, progress);
    } else if (checkState == CheckState::Indeterminate) {
      drawPartiallyCheckedCheckBoxIndicator(rect, p, progress);
    }
  }
}

void drawItemForeground(QPainter* p, const QRect& rect, const QPixmap& iconPixmap, const QString& text,
  const QFontMetrics& fontMetrics, QColor const& textColor, const int spacing, Qt::Alignment const alignment,
  const QString& secondaryText, QColor const& secondaryTextColor, bool const useMnemonic, bool const showMnemonic,
  Qt::TextElideMode const elideMode) {
  QPainterStateGuard stateGuard(p);
  p->setRenderHint(QPainter::SmoothPixmapTransform, true);
  p->setRenderHint(QPainter::Antialiasing, true);

  const auto hasIcon = !iconPixmap.isNull();
  const auto hasText = !text.isEmpty();
  const auto hasSecondaryText = !secondaryText.isEmpty();
  const auto centered = alignment.testFlag(Qt::AlignHCenter);
  const auto rightAligned = alignment.testFlag(Qt::AlignRight);

  // Compute actual content rect.
  const auto iconW = hasIcon ? iconPixmap.width() : 0;
  const auto textW = hasText ? fontMetrics.size(Qt::TextShowMnemonic, text).width() : 0;
  const auto secondaryTextW = hasSecondaryText ? fontMetrics.size(Qt::TextShowMnemonic, secondaryText).width() : 0;
  const auto iconSpacing = hasIcon && hasText ? spacing : 0;
  const auto secondaryTextSpacing = hasSecondaryText ? spacing : 0;
  const auto contentW = centered || rightAligned
                          ? std::min(rect.width(), iconW + iconSpacing + textW + secondaryTextSpacing + secondaryTextW)
                          : rect.width();

  auto contentX = rect.x();
  if (alignment.testFlag(Qt::AlignHCenter)) {
    contentX = rect.x() + (rect.width() - contentW) / 2;
  } else if (alignment.testFlag(Qt::AlignRight)) {
    contentX = rect.x() + rect.width() - contentW;
  }
  const auto contentRect = QRect{ contentX, rect.y(), contentW, rect.height() };

  auto availableWidth = contentW;

  // Draw icon.
  if (hasIcon) {
    // Get pixmap size.
    const auto pixmapPixelRatio = iconPixmap.devicePixelRatio();
    const auto pixmapW = pixmapPixelRatio != 0 ? (int) ((qreal) iconPixmap.width() / pixmapPixelRatio) : 0;
    const auto pixmapH = pixmapPixelRatio != 0 ? (int) ((qreal) iconPixmap.height() / pixmapPixelRatio) : 0;
    const auto pixmapX = contentRect.x();
    const auto pixmapY = contentRect.y() + (contentRect.height() - pixmapH) / 2;
    const auto pixmapRect = QRect{ pixmapX, pixmapY, pixmapW, pixmapH };

    p->drawPixmap(pixmapRect, iconPixmap);

    availableWidth -= pixmapW + iconSpacing;
  }

  // Draw secondary text.
  if (hasSecondaryText && availableWidth > secondaryTextW) {
    const auto secondaryTextX = contentRect.x() + contentRect.width() - secondaryTextW;
    const auto secondaryTextRect = QRect{ secondaryTextX, contentRect.y(), secondaryTextW, contentRect.height() };
    constexpr auto textFlags =
      Qt::AlignVCenter | Qt::AlignBaseline | Qt::TextSingleLine | Qt::AlignRight | Qt::TextHideMnemonic;
    availableWidth -= secondaryTextW + secondaryTextSpacing;
    p->setPen(secondaryTextColor);
    p->drawText(secondaryTextRect, textFlags, secondaryText);
  }

  // Draw text.
  if (hasText && availableWidth > 0) {
    const auto textActualW = availableWidth;
    const auto elidedText = fontMetrics.elidedText(text, elideMode, textActualW);
    const auto textX = contentRect.x() + iconW + iconSpacing;
    const auto textRect = QRect{ textX, contentRect.y(), textW, contentRect.height() };
    auto textFlags = Qt::AlignVCenter | Qt::AlignBaseline | Qt::TextSingleLine | Qt::TextShowMnemonic;
    if (alignment.testFlag(Qt::AlignLeft) || alignment.testFlag(Qt::AlignRight)) {
      textFlags |= Qt::AlignLeft;
    } else if (!hasIcon) {
      textFlags |= Qt::AlignHCenter;
    }
    if (useMnemonic) {
      textFlags |= Qt::TextShowMnemonic;
    }
    if (!showMnemonic) {
      textFlags |= Qt::TextHideMnemonic;
    }

    p->setPen(textColor);
    p->drawText(textRect, int(textFlags), elidedText, nullptr);
  }
}

void drawMenuSeparator(QPainter* p, const QRect& rect, QColor const& color, const int thickness) {
  const auto x = rect.x();
  const auto w = rect.width();
  const auto h = std::max(1, thickness);
  const auto y = rect.y() + (rect.height() - h) / 2;
  p->fillRect(QRect{ x, y, w, h }, color);
}

int getTickInterval(int tickInterval, int singleStep, int pageStep, int min, int max, int sliderLength) {
  if (tickInterval <= 0) {
    tickInterval = singleStep;
    const auto posInterval = QStyle::sliderPositionFromValue(min, max, tickInterval, sliderLength);
    const auto posZero = QStyle::sliderPositionFromValue(min, max, 0, sliderLength);
    if (posInterval - posZero < 3) {
      tickInterval = pageStep;
    }
  }
  if (tickInterval <= 0) {
    tickInterval = 1;
  }
  return tickInterval;
}

void drawSliderTickMarks(QPainter* p, QRect const& tickmarksRect, QColor const& tickColor, const int min, const int max,
  const int interval, const int tickThickness, const int singleStep, const int pageStep, Qt::Orientation orientation,
  bool upsideDown) {
  QPainterStateGuard stateGuard(p);
  const auto horizontal = orientation == Qt::Horizontal;
  const auto sliderLength = horizontal ? tickmarksRect.width() : tickmarksRect.height();
  const auto tickInterval = getTickInterval(interval, singleStep, pageStep, min, max, sliderLength);

  p->setPen(QPen(tickColor, tickThickness, Qt::SolidLine, Qt::FlatCap));
  p->setBrush(Qt::NoBrush);

  auto v = min;
  while (v <= max) {
    const auto offset = QStyle::sliderPositionFromValue(min, max, v, sliderLength, upsideDown);
    if (horizontal) {
      const auto x = std::min(tickmarksRect.right(), tickmarksRect.left() + offset);
      p->drawLine(x, tickmarksRect.top(), x, tickmarksRect.bottom() + 1);
    } else {
      const auto y = std::min(tickmarksRect.bottom(), tickmarksRect.top() + offset);
      p->drawLine(tickmarksRect.left(), y, tickmarksRect.right() + 1, y);
    }
    v += tickInterval;
  }
}

void drawDialTickMarks(QPainter* p, QRect const& tickmarksRect, QColor const& tickColor, const int min, const int max,
  const int tickThickness, const int tickLength, const int singleStep, const int pageStep, const int minArcLength) {
  QPainterStateGuard stateGuard(p);
  p->setRenderHint(QPainter::Antialiasing, true);
  p->setPen(QPen(tickColor, tickThickness, Qt::SolidLine, Qt::FlatCap));
  p->setBrush(Qt::NoBrush);

  const auto centerX = tickmarksRect.x() + tickmarksRect.width() / 2;
  const auto centerY = tickmarksRect.y() + tickmarksRect.height() / 2;
  const auto r1 = tickmarksRect.width() / 2;
  const auto r2 = r1 - tickLength;

  const auto anglePerStep = max != min ? 6. * QLEMENTINE_PI_4 / (max - min) : 0.;
  const auto arcPerSteep = anglePerStep * r1;
  const auto tickInterval = arcPerSteep > minArcLength ? singleStep : pageStep;

  auto v = static_cast<qreal>(min);
  while (v <= max) {
    const auto ratio = max != min ? (v - min) / (max - min) : 0;
    const auto angle = (ratio - 1.) * 6. * QLEMENTINE_PI_4 + QLEMENTINE_PI_4;
    const auto cosinus = std::cos(angle);
    const auto sinus = std::sin(angle);

    const auto x1 = centerX + r1 * cosinus;
    const auto y1 = centerY + r1 * sinus;

    const auto x2 = centerX + r2 * cosinus;
    const auto y2 = centerY + r2 * sinus;

    p->drawLine(QPointF{ x1, y1 }, QPointF{ x2, y2 });
    v += tickInterval;
  }
}

void drawDial(QPainter* p, QRect const& dialRect, int min, int max, double value, QColor const& bgColor,
  QColor const& handleColor, QColor const& grooveColor, QColor const& valueColor, QColor const& markColor,
  const int grooveThickness, const int markLength, const int markThickness) {
  QPainterStateGuard stateGuard(p);
  constexpr auto totalAngleDegrees = 360;
  constexpr auto deadAngleDegrees = 90;
  constexpr auto angleSpreadDegrees = totalAngleDegrees - deadAngleDegrees;
  constexpr auto startAngleDegrees = angleSpreadDegrees - 45;
  constexpr auto qtAnglePrecision = 16; // Do not change (cf Qt documentation).
  constexpr auto angleSpreadRadians = 6. * QLEMENTINE_PI_4;
  constexpr auto startAngleRadians = QLEMENTINE_PI_4;

  // Background.
  p->setRenderHint(QPainter::Antialiasing, true);
  p->setPen(Qt::NoPen);
  p->setBrush(bgColor);
  p->drawEllipse(dialRect);

  // Value line.
  const auto halfGrooveThickness = grooveThickness / 2.;
  const auto arcRect = QRectF(dialRect).marginsRemoved(
    { halfGrooveThickness, halfGrooveThickness, halfGrooveThickness, halfGrooveThickness });
  constexpr auto startAngle = startAngleDegrees * qtAnglePrecision;
  const auto ratio = max != min ? (double) (value - min) / (max - min) : 0.;
  const auto angleLength = -(angleSpreadDegrees * ratio) * qtAnglePrecision;
  p->setBrush(Qt::NoBrush);
  p->setPen(QPen(grooveColor, grooveThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  p->setPen(QPen(valueColor, grooveThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  p->drawArc(arcRect, startAngle, int(angleLength));

  // Cheat a bit: draw a pie below to avoid that nasty imprecise antialiasing: it soften the colors.
  if (value > min) {
    p->setPen(Qt::NoPen);
    p->setBrush(valueColor);
    const auto circlePerimeter = QLEMENTINE_PI * dialRect.width();
    const auto cropLength = (halfGrooveThickness / circlePerimeter) * deadAngleDegrees;
    const auto shiftAngle = cropLength * qtAnglePrecision;
    p->drawPie(dialRect, startAngle, int(angleLength + shiftAngle));
  }

  // Rounded triangle to delimit the run of the line.
  p->setPen(Qt::NoPen);
  p->setBrush(bgColor);
  p->drawPie(dialRect, startAngle, deadAngleDegrees * qtAnglePrecision);

  // Front.
  const auto dialFrontRect =
    dialRect.marginsRemoved({ grooveThickness, grooveThickness, grooveThickness, grooveThickness });
  p->setPen(Qt::NoPen);
  p->setBrush(handleColor);
  p->drawEllipse(dialFrontRect);

  // Mark.
  const auto markRect = QRectF{ dialFrontRect };
  const auto markAngleRadians = startAngleRadians + (ratio - 1.) * angleSpreadRadians;
  const auto markAngleCosinus = std::cos(markAngleRadians);
  const auto markAngleSinus = std::sin(markAngleRadians);
  const auto p1Radius = (markRect.width() - 2.) / 2.;
  const auto p2Radius = p1Radius - markLength;
  const auto p1 = QPointF{ p1Radius * markAngleCosinus, p1Radius * markAngleSinus } + markRect.center();
  const auto p2 = QPointF{ p2Radius * markAngleCosinus, p2Radius * markAngleSinus } + markRect.center();
  p->setBrush(Qt::NoBrush);
  p->setPen(QPen(markColor, markThickness, Qt::SolidLine, Qt::FlatCap));
  p->drawLine(p1, p2);
}

QPainterPath getTabPath(QRect const& rect, const RadiusesF& radiuses) {
  QPainterPath path;
  const auto w = static_cast<qreal>(rect.width());
  const auto h = static_cast<qreal>(rect.height());
  const auto x = static_cast<qreal>(rect.x());
  const auto y = static_cast<qreal>(rect.y());

  // Ensure angle validity.
  const auto half = std::min(w / 2., h / 2.);
  const auto topLeft = std::min(half, std::max(0., radiuses.topLeft * 2));
  const auto topRight = std::min(half, std::max(0., radiuses.topRight * 2));
  const auto bottomRight = std::min(half, std::max(0., radiuses.bottomRight * 2));
  const auto bottomLeft = std::min(half, std::max(0., radiuses.bottomLeft * 2));

  // Top-left.
  path.moveTo(x, y + topLeft);
  if (topLeft > 0.) {
    path.arcTo(x, y, topLeft, topLeft, 180., -90.);
  }

  // Top-right.
  if (topRight > 0.) {
    path.lineTo(x + w - topRight, y);
    path.arcTo(x + w - topRight, y, topRight, topRight, 90., -90.);
  } else {
    path.lineTo(x + w, y);
  }

  // Bottom-right.
  if (bottomRight > 0.) {
    path.lineTo(x + w, y + h - bottomRight);
    path.arcTo(x + w, y + h - bottomRight, bottomRight, bottomRight, 180., 90.);
  } else {
    path.lineTo(x + w, y + h);
  }

  // Bottom-left.
  if (bottomLeft > 0.) {
    path.lineTo(x - bottomLeft, y + h);
    path.arcTo(x - bottomLeft, y + h - bottomLeft, bottomLeft, bottomLeft, 270., 90.);
  } else {
    path.lineTo(x, y + h);
  }
  path.closeSubpath();

  return path;
}

QPainterPath getTrapezoidTabPath(QRect const& rect, qreal slant, qreal radius) {
  QPainterPath path;
  const auto w = static_cast<qreal>(rect.width());
  const auto h = static_cast<qreal>(rect.height());
  const auto x = static_cast<qreal>(rect.x());
  const auto y = static_cast<qreal>(rect.y());

  // Ensure geometry validity.
  const auto effectiveSlant = std::min(std::max(0., slant), w / 2.);
  const auto edgeLength = std::sqrt(effectiveSlant * effectiveSlant + h * h);
  const auto effectiveRadius = std::min({ std::max(0., radius), (w / 2.) - effectiveSlant, edgeLength / 2. });
  const auto edgeDirection = edgeLength > 0. ? QPointF(effectiveSlant / edgeLength, -h / edgeLength) : QPointF();

  // Bottom-left, then up the left edge.
  const auto topLeft = QPointF(x + effectiveSlant, y);
  path.moveTo(x, y + h);
  path.lineTo(topLeft - edgeDirection * effectiveRadius);

  // Top-left.
  path.quadTo(topLeft, topLeft + QPointF(effectiveRadius, 0.));

  // Top-right.
  const auto topRight = QPointF(x + w - effectiveSlant, y);
  path.lineTo(topRight - QPointF(effectiveRadius, 0.));
  path.quadTo(topRight, topRight + QPointF(-edgeDirection.x(), edgeDirection.y()) * -effectiveRadius);

  // Bottom-right.
  path.lineTo(x + w, y + h);
  path.closeSubpath();

  return path;
}

void drawTab(QPainter* p, QRect const& rect, const RadiusesF& radius, const QColor& bgColor, bool drawShadow,
  const QColor& shadowColor) {
  drawTab(p, getTabPath(rect, radius), bgColor, drawShadow, shadowColor);
}

void drawTab(QPainter* p, const QPainterPath& path, const QColor& bgColor, bool drawShadow, const QColor& shadowColor) {
  QPainterStateGuard stateGuard(p);
  if (drawShadow) {
    drawTabShadow(p, path, shadowColor);
  }
  p->setRenderHint(QPainter::Antialiasing, true);
  p->setPen(Qt::NoPen);
  p->setBrush(bgColor);
  p->drawPath(path);
}

void drawTabShadow(QPainter* p, QRect const& rect, const RadiusesF& radius, const QColor& color) {
  drawTabShadow(p, getTabPath(rect, radius), color);
}

void drawTabShadow(QPainter* p, const QPainterPath& path, const QColor& color) {
  QPainterStateGuard stateGuard(p);
  p->setRenderHint(QPainter::SmoothPixmapTransform, true);
  // Draw the tab in a temporary buffer.
  const auto pathRect = path.boundingRect().toAlignedRect();
  QPixmap pathPixmap(pathRect.size());
  {
    pathPixmap.fill(Qt::transparent);
    QPainter pixmapPainter(&pathPixmap);
    pixmapPainter.setRenderHint(QPainter::Antialiasing, true);
    pixmapPainter.setPen(Qt::NoPen);
    pixmapPainter.setBrush(Qt::black);
    pixmapPainter.drawPath(path.translated(-pathRect.x(), -pathRect.y()));
  }

  // Get the blurred version of the temporary buffer.
  constexpr auto blurRadius = 4.;
  constexpr auto shadowX = 0;
  constexpr auto shadowY = blurRadius / 2;
  const auto shadowPixmap = getDropShadowPixmap(pathPixmap, blurRadius, color);

  // Draw the shadow buffer.
  const auto deltaX = (shadowPixmap.width() - pathRect.width()) / 2 + shadowX;
  const auto deltaY = (shadowPixmap.height() - pathRect.height()) / 2 + shadowY - blurRadius;
  const auto shadowRect = QRect(QPoint(pathRect.x() - deltaX, pathRect.y() - deltaY), shadowPixmap.size());

  p->setCompositionMode(QPainter::CompositionMode::CompositionMode_Multiply);
  p->drawPixmap(shadowRect, shadowPixmap);
}

void drawElidedMultiLineText(QPainter& p, const QRect& rect, const QString& text, const QPaintDevice* paintDevice) {
  // Do layout.
  const auto fontMetrics = p.fontMetrics();
  const auto leading = fontMetrics.leading();
  const auto maxWidth = rect.width();
  const auto maxHeight = rect.height();
  QTextLayout textLayout(text, p.font(), paintDevice);
  textLayout.setCacheEnabled(true);
  auto height = 0.;
  auto lastLine = -1;
  textLayout.beginLayout();
  while (true) {
    auto line = textLayout.createLine();
    if (!line.isValid())
      break;

    line.setLineWidth(maxWidth);
    height += leading;
    line.setPosition(QPointF(0, height));
    height += line.height();
    if (height <= maxHeight) {
      ++lastLine;
    }
  }
  textLayout.endLayout();

  // Draw lines.
  const auto lineCount = textLayout.lineCount();
  for (auto i = 0; i <= lastLine; ++i) {
    const auto& line = textLayout.lineAt(i);

    const auto lineRect = QRect(rect.topLeft() + line.position().toPoint(), QSize(maxWidth, int(line.height())));
    if ((i < lastLine || lastLine == lineCount - 1) && line.naturalTextWidth() < maxWidth) {
      line.draw(&p, rect.topLeft());
    } else {
      const auto lineText = removeTrailingWhitespaces(textLayout.text().mid(line.textStart(), line.textLength()));
      const auto ellipsis = QString("…");
      const auto ellipsisWidth = qlementine::textWidth(fontMetrics, ellipsis);
      auto elidedLineText = removeTrailingWhitespaces(
        fontMetrics.elidedText(lineText, Qt::TextElideMode::ElideRight, maxWidth - ellipsisWidth, Qt::TextSingleLine));
      if (!elidedLineText.endsWith(ellipsis)) {
        elidedLineText = removeTrailingWhitespaces(elidedLineText);
        elidedLineText += ellipsis;
      }
      constexpr auto textFlags = Qt::AlignLeft | Qt::TextSingleLine;
      p.drawText(lineRect, textFlags, elidedLineText);
    }
  }
}

QString removeTrailingWhitespaces(const QString& str) {
  auto n = str.size() - 1;
  for (; n >= 0; --n) {
    if (!str.at(n).isSpace()) {
      return str.left(n + 1);
    }
  }
  return {};
}

QString displayedShortcutString(const QKeySequence& shortcut) {
  auto shortcutStr = shortcut.toString(QKeySequence::SequenceFormat::NativeText);
  shortcutStr.replace(QApplication::translate("QShortcut", "Left"), QChar(0x2190));
  shortcutStr.replace(QApplication::translate("QShortcut", "Right"), QChar(0x2192));
  shortcutStr.replace(QApplication::translate("QShortcut", "Up"), QChar(0x2191));
  shortcutStr.replace(QApplication::translate("QShortcut", "Down"), QChar(0x2193));
  return shortcutStr;
}

void drawShortcut(QPainter& p, const QKeySequence& shortcut, const QRect& rect, const Theme& theme, bool enabled,
  Qt::Alignment alignment) {
  QPainterStateGuard stateGuard(&p);
  const auto shortcutStr = displayedShortcutString(shortcut);
  if (shortcutStr.isEmpty())
    return;

  p.setRenderHint(QPainter::Antialiasing, true);
  constexpr auto radius = 3.;
  constexpr auto borderW = 1;
  const auto& font = theme.fontRegular;
  const auto fm = QFontMetrics(font);
  const auto parts = shortcutStr.split('+', Qt::SkipEmptyParts, Qt::CaseInsensitive);
  const auto spacing = theme.spacing / 2;
  const auto paddingV = theme.spacing / 4;
  const auto paddingH = theme.spacing / 2;
  const auto padding = QMargins(paddingH, paddingV, paddingH, paddingV);
  const auto minimumTextW = fm.capHeight() + 2 * borderW; // Ensure (approximatively) a square.
  const auto bgMargins = QMargins(borderW, borderW, borderW, static_cast<int>(borderW * radius));
  auto x = 0;

  for (const auto& part : parts) {
    const auto textRect = QRect(0, 0, std::max(minimumTextW, qlementine::textWidth(fm, part)), fm.height());
    const auto fgRect = textRect.marginsAdded(padding);
    const auto bgRect = fgRect.marginsAdded(bgMargins);
    const auto delta = textRect.topLeft() - bgRect.topLeft();

    // Handle alignment inside the bounding rect.
    auto translation = QPoint{ rect.x() + x, rect.y() } + delta;
    if (alignment.testFlag(Qt::AlignHCenter)) {
      translation.rx() += (rect.width() - bgRect.width()) / 2;
    }
    if (alignment.testFlag(Qt::AlignVCenter)) {
      translation.ry() += (rect.height() - bgRect.height()) / 2;
    }

    // Background.
    p.setBrush(enabled ? theme.secondaryAlternativeColor : theme.secondaryAlternativeColorDisabled);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(bgRect.translated(translation), radius + borderW, radius + borderW);

    // Foreground.
    p.setBrush(enabled ? theme.backgroundColorMain2 : theme.backgroundColorMain3);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(fgRect.translated(translation), radius, radius);

    // Text.
    constexpr auto textFlags = Qt::AlignCenter | Qt::TextSingleLine | Qt::TextHideMnemonic;
    p.setBrush(Qt::NoBrush);
    p.setPen(enabled ? theme.secondaryColor : theme.secondaryColorDisabled);
    p.drawText(textRect.translated(translation), textFlags, part);

    x += bgRect.width() + spacing;
  }
}

QSize shortcutSizeHint(const QKeySequence& shortcut, const Theme& theme) {
  const auto shortcutStr = displayedShortcutString(shortcut);
  if (shortcutStr.isEmpty())
    return { 0, 0 };

  constexpr auto radius = 3.;
  constexpr auto borderW = 1;
  const auto& font = theme.fontRegular;
  const auto fm = QFontMetrics(font);
  const auto parts = shortcutStr.split('+', Qt::SkipEmptyParts, Qt::CaseInsensitive);

  const auto spacing = theme.spacing / 2;
  const auto paddingV = theme.spacing / 4;
  const auto paddingH = theme.spacing / 2;
  const auto padding = QMargins(paddingH, paddingV, paddingH, paddingV);
  const auto bgMargins = QMargins(borderW, borderW, borderW, static_cast<int>(borderW * radius));
  const auto minimumTextW = fm.capHeight() + 2 * borderW; // Ensure (approximatively) a square.
  auto w = 0;
  auto h = fm.height() + paddingV * 2 + bgMargins.top() + bgMargins.bottom();

  // Add items widths.
  for (const auto& part : parts) {
    const auto textRect = QRect(0, 0, std::max(minimumTextW, qlementine::textWidth(fm, part)), fm.height());
    const auto fgRect = textRect.marginsAdded(padding);
    const auto bgRect = fgRect.marginsAdded(bgMargins);
    w += bgRect.width();
  }

  // Add spacing between items.
  w += parts.empty() ? 0 : (parts.count() - 1) * spacing;

  return QSize{ w, h };
}

QPixmap getPixmap(
  QIcon const& icon, const QSize& iconSize, MouseState const mouse, CheckState const checked, const QWidget* widget) {
  const auto iconMode = getIconMode(mouse);
  const auto iconState = getIconState(checked);
  const auto devicePixelRatio = widget ? widget->devicePixelRatio() : qApp->devicePixelRatio();
  // Qt icon pixmap cache is broken when devicePixelRatio > 1.0.
  auto cacheKey = QString("qlementine_icon_pixmap_%1_%2_%3_%4_%5_%6")
                    .arg(icon.cacheKey())
                    .arg(iconSize.width())
                    .arg(iconSize.height())
                    .arg(devicePixelRatio)
                    .arg(static_cast<int>(iconMode))
                    .arg(static_cast<int>(iconState));
  QPixmap pixmap;
  if (QPixmapCache::find(cacheKey, &pixmap)) {
    return pixmap;
  }
  pixmap = icon.pixmap(iconSize, devicePixelRatio, iconMode, iconState);
  QPixmapCache::insert(cacheKey, pixmap);
  return pixmap;
}

QRect drawIcon(const QRect& rect, QPainter* p, const QIcon& icon, const MouseState mouse, const CheckState checked,
  const QWidget* widget, bool colorize, const QColor& color) {
  QPainterStateGuard stateGuard(p);
  p->setRenderHint(QPainter::SmoothPixmapTransform, true);
  if (rect.isEmpty() || icon.isNull()) {
    return { rect.x(), rect.y(), 0, 0 };
  }

  // Get pixmap to draw.
  const auto iconSize = rect.size();
  const auto& pixmap = getPixmap(icon, iconSize, mouse, checked, widget);
  const auto& targetPixmap = colorize ? getColorizedPixmap(pixmap, color) : pixmap;

  if (targetPixmap.isNull()) {
    return { rect.x(), rect.y(), 0, 0 };
  }

  // Get rect to draw the pixmap in.
  // The pixmap may be smaller than the requested size, so we center it in the rect by default.
  const auto targetPixelRatio = targetPixmap.devicePixelRatio();
  const auto targetW = static_cast<int>((qreal) targetPixmap.width() / (targetPixelRatio));
  const auto targetH = static_cast<int>((qreal) targetPixmap.height() / (targetPixelRatio));
  const auto targetX = rect.x() + (rect.width() - targetW) / 2;
  const auto targetY = rect.y() + (rect.height() - targetH) / 2;
  const auto targetRect = QRect{ targetX, targetY, targetW, targetH };

  // Draw the pixmap.
  p->drawPixmap(targetRect, targetPixmap);

  // Return the actual rect where the pixmap was drawn.
  return targetRect;
}

void updateUncheckableButtonIconPixmap(
  QIcon& icon, const QSize& size, QlementineStyle const& style, const PixmapMakerFunc& func) {
  if (!func)
    return;

  constexpr auto role = ColorRole::Secondary;

  for (const auto factor : { 1.0, 2.0 }) {
    const auto scaledSize = size * factor;

    auto pixmapNormal = func(scaledSize, style.toolButtonForegroundColor(MouseState::Normal, role));
    pixmapNormal.setDevicePixelRatio(factor);
    icon.addPixmap(pixmapNormal, QIcon::Mode::Normal);

    auto pixmapMouseOver = func(scaledSize, style.toolButtonForegroundColor(MouseState::Hovered, role));
    pixmapMouseOver.setDevicePixelRatio(factor);
    icon.addPixmap(pixmapMouseOver, QIcon::Mode::Selected);

    auto pixmapPressed = func(scaledSize, style.toolButtonForegroundColor(MouseState::Pressed, role));
    pixmapPressed.setDevicePixelRatio(factor);
    icon.addPixmap(pixmapPressed, QIcon::Mode::Active);

    auto pixmapDisabled = func(scaledSize, style.toolButtonForegroundColor(MouseState::Disabled, role));
    pixmapDisabled.setDevicePixelRatio(factor);
    icon.addPixmap(pixmapDisabled, QIcon::Mode::Disabled);
  }
}

void updateCheckIcon(QIcon& icon, QSize const& size, QlementineStyle const& style) {
  const auto role = ColorRole::Primary;

  for (const auto factor : { 1.0, 2.0 }) {
    const auto scaledSize = size * factor;

    auto pixmapNormal = makeCheckPixmap(scaledSize, style.buttonForegroundColor(MouseState::Normal, role));
    pixmapNormal.setDevicePixelRatio(factor);

    auto pixmapMouseOver = makeCheckPixmap(scaledSize, style.buttonForegroundColor(MouseState::Hovered, role));
    pixmapMouseOver.setDevicePixelRatio(factor);

    auto pixmapPressed = makeCheckPixmap(scaledSize, style.buttonForegroundColor(MouseState::Pressed, role));
    pixmapPressed.setDevicePixelRatio(factor);

    auto pixmapDisabled = makeCheckPixmap(scaledSize, style.buttonForegroundColor(MouseState::Disabled, role));
    pixmapDisabled.setDevicePixelRatio(factor);

    QPixmap uncheckedPixmap{ scaledSize };
    uncheckedPixmap.fill(Qt::transparent);
    uncheckedPixmap.setDevicePixelRatio(factor);

    icon.addPixmap(pixmapNormal, QIcon::Mode::Normal, QIcon::State::On);
    icon.addPixmap(pixmapMouseOver, QIcon::Mode::Selected, QIcon::State::On);
    icon.addPixmap(pixmapPressed, QIcon::Mode::Active, QIcon::State::On);
    icon.addPixmap(pixmapDisabled, QIcon::Mode::Disabled, QIcon::State::On);

    icon.addPixmap(uncheckedPixmap, QIcon::Mode::Normal, QIcon::State::Off);
    icon.addPixmap(uncheckedPixmap, QIcon::Mode::Selected, QIcon::State::Off);
    icon.addPixmap(uncheckedPixmap, QIcon::Mode::Active, QIcon::State::Off);
    icon.addPixmap(uncheckedPixmap, QIcon::Mode::Disabled, QIcon::State::Off);
  }
}

QPixmap makeClearButtonPixmap(QSize const& size, QColor const& fgColor) {
  const auto w = size.width();
  const auto h = size.width();
  constexpr auto intendedSize = 16.;

  const auto p1 = QPointF{ (4.7 / intendedSize) * w, (4.7 / intendedSize) * h };
  const auto p2 = QPointF{ (11.3 / intendedSize) * w, (11.3 / intendedSize) * h };

  const auto p3 = QPointF{ (4.7 / intendedSize) * w, (11.3 / intendedSize) * h };
  const auto p4 = QPointF{ (11.3 / intendedSize) * w, (4.7 / intendedSize) * h };

  const auto penWidth = (1.4 / intendedSize) * h;
  QPixmap pixmap{ size };
  pixmap.fill(Qt::transparent);
  QPainter p{ &pixmap };
  p.setRenderHint(QPainter::Antialiasing, true);

  p.setBrush(Qt::NoBrush);
  p.setPen(QPen{ fgColor, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin });
  p.drawLine(p1, p2);
  p.drawLine(p3, p4);

  return pixmap;
}

QPixmap makeCloseButtonPixmap(QSize const& size, QColor const& fgColor) {
  const auto w = size.width();
  const auto h = size.width();
  constexpr auto intendedSize = 16.;

  const auto p1 = QPointF{ (3.5 / intendedSize) * w, (3.5 / intendedSize) * h };
  const auto p2 = QPointF{ (12.5 / intendedSize) * w, (12.5 / intendedSize) * h };

  const auto p3 = QPointF{ (3.5 / intendedSize) * w, (12.5 / intendedSize) * h };
  const auto p4 = QPointF{ (12.5 / intendedSize) * w, (3.5 / intendedSize) * h };

  const auto penWidth = (1.6 / intendedSize) * h;
  QPixmap pixmap{ size };
  pixmap.fill(Qt::transparent);
  QPainter p{ &pixmap };
  p.setRenderHint(QPainter::Antialiasing, true);

  p.setBrush(Qt::NoBrush);
  p.setPen(QPen{ fgColor, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin });
  p.drawLine(p1, p2);
  p.drawLine(p3, p4);

  return pixmap;
}

QPixmap makeCheckPixmap(QSize const& size, QColor const& color) {
  const QRect checkRect{ 0, 0, size.width(), size.height() };

  QPixmap pixmap{ size };
  pixmap.fill(Qt::transparent);

  constexpr auto defaultSize = 16.;
  constexpr auto defaultPenWidth = 2.;
  const auto penWidth = (size.width() / defaultSize) * defaultPenWidth;

  QPainter p{ &pixmap };
  p.setBrush(Qt::NoBrush);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setPen(QPen{ color, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin });

  drawCheckBoxIndicator(checkRect, &p, 1.0);

  return pixmap;
}

QPixmap makeCalendarPixmap(QSize const& size, QColor const& color) {
  QPixmap pixmap{ size };
  pixmap.fill(Qt::transparent);
  QPainter p{ &pixmap };
  drawCalendarIndicator(QRect{ QPoint{ 0, 0 }, size }, &p, color);

  return pixmap;
}

QPixmap makeDoubleArrowRightPixmap(QSize const& size, QColor const& color) {
  const QRect rect{ 0, 0, size.width(), size.height() };

  QPixmap pixmap{ size };
  pixmap.fill(Qt::transparent);

  constexpr auto defaultSize = 16.;
  constexpr auto defaultPenWidth = 1.5;
  const auto penWidth = (size.width() / defaultSize) * defaultPenWidth;

  QPainter p{ &pixmap };
  p.setBrush(Qt::NoBrush);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setPen(QPen{ color, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin });

  drawDoubleArrowRightIndicator(rect, &p);

  return pixmap;
}

QPixmap makeToolBarExtensionPixmap(QSize const& size, QColor const& color) {
  const QRect rect{ 0, 0, size.width(), size.height() };

  QPixmap pixmap{ size };
  pixmap.fill(Qt::transparent);

  constexpr auto defaultSize = 16.;
  constexpr auto defaultPenWidth = 1.01;
  const auto penWidth = (size.width() / defaultSize) * defaultPenWidth;

  QPainter p{ &pixmap };
  p.setBrush(Qt::NoBrush);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setPen(QPen{ color, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin });

  drawToolBarExtensionIndicator(rect, &p);

  return pixmap;
}

QPixmap makeArrowLeftPixmap(QSize const& size, QColor const& color) {
  const QRect rect{ 0, 0, size.width(), size.height() };

  QPixmap pixmap{ size };
  pixmap.fill(Qt::transparent);

  constexpr auto defaultSize = 16.;
  constexpr auto defaultPenWidth = 1.01;
  const auto penWidth = (size.width() / defaultSize) * defaultPenWidth;

  QPainter p{ &pixmap };
  p.setBrush(Qt::NoBrush);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setPen(QPen{ color, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin });

  drawArrowLeft(rect, &p);
  return pixmap;
}

QPixmap makeArrowRightPixmap(QSize const& size, QColor const& color) {
  const QRect rect{ 0, 0, size.width(), size.height() };

  QPixmap pixmap{ size };
  pixmap.fill(Qt::transparent);

  constexpr auto defaultSize = 16.;
  constexpr auto defaultPenWidth = 1.01;
  const auto penWidth = (size.width() / defaultSize) * defaultPenWidth;

  QPainter p{ &pixmap };
  p.setBrush(Qt::NoBrush);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setPen(QPen{ color, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin });

  drawArrowRight(rect, &p);
  return pixmap;
}

QPixmap makeMessageBoxWarningPixmap(QSize const& size, QColor const& bgColor, QColor const& fgColor) {
  const auto bgSvgPath = QStringLiteral(":/qlementine/resources/icons/messagebox_warning_bg.svg");
  const auto fgSvgPath = QStringLiteral(":/qlementine/resources/icons/messagebox_warning_fg.svg");
  const auto pixmap = makePixmapFromSvg(bgSvgPath, bgColor, fgSvgPath, fgColor, size);
  return pixmap;
}

QPixmap makeMessageBoxCriticalPixmap(QSize const& size, QColor const& bgColor, QColor const& fgColor) {
  const auto bgSvgPath = QStringLiteral(":/qlementine/resources/icons/messagebox_critical_bg.svg");
  const auto fgSvgPath = QStringLiteral(":/qlementine/resources/icons/messagebox_critical_fg.svg");
  const auto pixmap = makePixmapFromSvg(bgSvgPath, bgColor, fgSvgPath, fgColor, size);
  return pixmap;
}

QPixmap makeMessageBoxQuestionPixmap(QSize const& size, QColor const& bgColor, QColor const& fgColor) {
  const auto bgSvgPath = QStringLiteral(":/qlementine/resources/icons/messagebox_question_bg.svg");
  const auto fgSvgPath = QStringLiteral(":/qlementine/resources/icons/messagebox_question_fg.svg");
  const auto pixmap = makePixmapFromSvg(bgSvgPath, bgColor, fgSvgPath, fgColor, size);
  return pixmap;
}

QPixmap makeMessageBoxInformationPixmap(QSize const& size, QColor const& bgColor, QColor const& fgColor) {
  const auto bgSvgPath = QStringLiteral(":/qlementine/resources/icons/messagebox_information_bg.svg");
  const auto fgSvgPath = QStringLiteral(":/qlementine/resources/icons/messagebox_information_fg.svg");
  const auto pixmap = makePixmapFromSvg(bgSvgPath, bgColor, fgSvgPath, fgColor, size);
  return pixmap;
}

void updateMessageBoxWarningIcon(QIcon& icon, QSize const& size, Theme const& theme) {
  const auto& bgColor = theme.statusColorWarning;
  const auto& fgColor = theme.statusColorForeground;

  const auto& bgColorDisabled = theme.statusColorWarningDisabled;
  const auto& fgColorDisabled = theme.statusColorForegroundDisabled;

  for (const auto factor : { 1., 2. }) {
    const auto scaledSize = size * factor;
    auto pixmap = makeMessageBoxWarningPixmap(scaledSize, bgColor, fgColor);
    pixmap.setDevicePixelRatio(factor);
    icon.addPixmap(pixmap, QIcon::Mode::Normal, QIcon::State::Off);
    icon.addPixmap(pixmap, QIcon::Mode::Active, QIcon::State::Off);
    icon.addPixmap(pixmap, QIcon::Mode::Selected, QIcon::State::Off);
    icon.addPixmap(pixmap, QIcon::Mode::Normal, QIcon::State::On);
    icon.addPixmap(pixmap, QIcon::Mode::Active, QIcon::State::On);
    icon.addPixmap(pixmap, QIcon::Mode::Selected, QIcon::State::On);

    auto disbledPixmap = makeMessageBoxWarningPixmap(scaledSize, bgColorDisabled, fgColorDisabled);
    disbledPixmap.setDevicePixelRatio(factor);
    icon.addPixmap(disbledPixmap, QIcon::Mode::Disabled, QIcon::State::Off);
    icon.addPixmap(disbledPixmap, QIcon::Mode::Disabled, QIcon::State::On);
  }
}

void updateMessageBoxCriticalIcon(QIcon& icon, QSize const& size, Theme const& theme) {
  const auto& bgColor = theme.statusColorError;
  const auto& fgColor = theme.statusColorForeground;

  const auto& bgColorDisabled = theme.statusColorErrorDisabled;
  const auto& fgColorDisabled = theme.statusColorForegroundDisabled;

  for (const auto factor : { 1., 2. }) {
    const auto scaledSize = size * factor;
    auto pixmap = makeMessageBoxCriticalPixmap(scaledSize, bgColor, fgColor);
    pixmap.setDevicePixelRatio(factor);
    icon.addPixmap(pixmap, QIcon::Mode::Normal, QIcon::State::Off);
    icon.addPixmap(pixmap, QIcon::Mode::Active, QIcon::State::Off);
    icon.addPixmap(pixmap, QIcon::Mode::Selected, QIcon::State::Off);
    icon.addPixmap(pixmap, QIcon::Mode::Normal, QIcon::State::On);
    icon.addPixmap(pixmap, QIcon::Mode::Active, QIcon::State::On);
    icon.addPixmap(pixmap, QIcon::Mode::Selected, QIcon::State::On);

    auto disbledPixmap = makeMessageBoxCriticalPixmap(scaledSize, bgColorDisabled, fgColorDisabled);
    disbledPixmap.setDevicePixelRatio(factor);
    icon.addPixmap(disbledPixmap, QIcon::Mode::Disabled, QIcon::State::Off);
    icon.addPixmap(disbledPixmap, QIcon::Mode::Disabled, QIcon::State::On);
  }
}

void updateMessageBoxQuestionIcon(QIcon& icon, QSize const& size, Theme const& theme) {
  const auto& bgColor = theme.statusColorInfo;
  const auto& fgColor = theme.statusColorForeground;

  const auto& bgColorDisabled = theme.statusColorInfoDisabled;
  const auto& fgColorDisabled = theme.statusColorForegroundDisabled;

  for (const auto factor : { 1., 2. }) {
    const auto scaledSize = size * factor;
    auto pixmap = makeMessageBoxQuestionPixmap(scaledSize, bgColor, fgColor);
    pixmap.setDevicePixelRatio(factor);
    icon.addPixmap(pixmap, QIcon::Mode::Normal, QIcon::State::Off);
    icon.addPixmap(pixmap, QIcon::Mode::Active, QIcon::State::Off);
    icon.addPixmap(pixmap, QIcon::Mode::Selected, QIcon::State::Off);
    icon.addPixmap(pixmap, QIcon::Mode::Normal, QIcon::State::On);
    icon.addPixmap(pixmap, QIcon::Mode::Active, QIcon::State::On);
    icon.addPixmap(pixmap, QIcon::Mode::Selected, QIcon::State::On);

    auto disbledPixmap = makeMessageBoxQuestionPixmap(scaledSize, bgColorDisabled, fgColorDisabled);
    disbledPixmap.setDevicePixelRatio(factor);
    icon.addPixmap(disbledPixmap, QIcon::Mode::Disabled, QIcon::State::Off);
    icon.addPixmap(disbledPixmap, QIcon::Mode::Disabled, QIcon::State::On);
  }
}

void updateMessageBoxInformationIcon(QIcon& icon, QSize const& size, Theme const& theme) {
  const auto& bgColor = theme.statusColorInfo;
  const auto& fgColor = theme.statusColorForeground;

  const auto& bgColorDisabled = theme.statusColorInfoDisabled;
  const auto& fgColorDisabled = theme.statusColorForegroundDisabled;

  for (const auto factor : { 1., 2. }) {
    const auto scaledSize = size * factor;
    auto pixmap = makeMessageBoxInformationPixmap(scaledSize, bgColor, fgColor);
    pixmap.setDevicePixelRatio(factor);
    icon.addPixmap(pixmap, QIcon::Mode::Normal, QIcon::State::Off);
    icon.addPixmap(pixmap, QIcon::Mode::Active, QIcon::State::Off);
    icon.addPixmap(pixmap, QIcon::Mode::Selected, QIcon::State::Off);
    icon.addPixmap(pixmap, QIcon::Mode::Normal, QIcon::State::On);
    icon.addPixmap(pixmap, QIcon::Mode::Active, QIcon::State::On);
    icon.addPixmap(pixmap, QIcon::Mode::Selected, QIcon::State::On);

    auto disbledPixmap = makeMessageBoxInformationPixmap(scaledSize, bgColorDisabled, fgColorDisabled);
    disbledPixmap.setDevicePixelRatio(factor);
    icon.addPixmap(disbledPixmap, QIcon::Mode::Disabled, QIcon::State::Off);
    icon.addPixmap(disbledPixmap, QIcon::Mode::Disabled, QIcon::State::On);
  }
}
} // namespace oclero::qlementine
