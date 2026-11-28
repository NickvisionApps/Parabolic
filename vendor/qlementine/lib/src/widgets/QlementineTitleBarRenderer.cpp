// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include "TitleBarRenderer.hpp"

#include <oclero/qlementine/style/Theme.hpp>
#include <oclero/qlementine/utils/ColorUtils.hpp>

#include <QGuiApplication>
#include <QIcon>
#include <QPaintDevice>
#include <QPainter>
#include <QPainterPath>

#include <algorithm>
#include <cmath>

namespace oclero::qlementine {
namespace {
// Metrics similar to Windows 11 caption buttons.
constexpr auto buttonWidth = 46;

class QlementineTitleBarRenderer final : public TitleBarRenderer {
public:
  int barHeight(const Theme& theme) const override {
    return theme.titleBarHeight;
  }

  QSize captionButtonSize(CaptionButton, const Theme& theme) const override {
    return QSize(buttonWidth, theme.titleBarHeight);
  }

  int captionButtonSpacing() const override {
    return 0;
  }

  QMargins captionButtonMargins() const override {
    return QMargins();
  }

  void paintBackground(QPainter& painter, const QRect& rect, const Theme& theme, const QColor& background,
    bool /*windowActive*/, bool /*maximized*/) const override {
    painter.fillRect(rect, background);

    // Bottom border.
    painter.fillRect(QRect(rect.left(), rect.bottom() + 1 - theme.borderWidth, rect.width(), theme.borderWidth),
      theme.titleBarBorderColor);
  }

  void paintTitle(
    QPainter& painter, const QRect& rect, const QString& title, const Theme& theme, bool windowActive) const override {
    const auto textRect = rect.adjusted(theme.spacing, 0, -theme.spacing, 0);
    if (title.isEmpty() || textRect.width() <= 0) {
      return;
    }

    const auto& foreground = theme.titleBarForegroundColor;
    painter.save();
    painter.setFont(theme.fontRegular);
    painter.setPen(windowActive ? foreground : colorWithAlpha(foreground, foreground.alpha() / 2));
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter,
      painter.fontMetrics().elidedText(title, Qt::ElideRight, textRect.width()));
    painter.restore();
  }

  void paintCaptionButton(QPainter& painter, const QRect& rect, CaptionButton button, const CaptionButtonState& state,
    const Theme& theme) const override {
    const auto isClose = button == CaptionButton::Close;
    const auto interacted = state.hovered || state.pressed;
    const auto& foreground = theme.titleBarForegroundColor;

    // Background and glyph colors.
    auto glyphColor =
      state.windowActive || interacted ? foreground : colorWithAlpha(foreground, foreground.alpha() / 2);
    if (interacted) {
      if (isClose) {
        painter.fillRect(rect, colorWithAlpha(QColor(0xC42B1C), state.pressed ? 0xe6 : 0xff));
        glyphColor = colorWithAlpha(QColor(Qt::white), state.pressed ? 0xb3 : 0xff);
      } else {
        painter.fillRect(rect, colorWithAlpha(foreground, state.pressed ? 0x14 : 0x1e));
      }
    }

    // Lines are snapped to the device pixel grid to stay crisp.
    const auto dpr = painter.device()->devicePixelRatioF();
    const auto devicePixels = std::max(1., std::round(dpr));
    const auto lineWidth = devicePixels / dpr;
    const auto snap = [dpr, devicePixels](qreal value) {
      return (std::floor(value * dpr) + devicePixels / 2.) / dpr;
    };
    const auto center = QPointF(rect.x() + rect.width() / 2., rect.y() + rect.height() / 2.);
    const auto half = 5.;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(glyphColor, lineWidth, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));

    switch (button) {
      case CaptionButton::Minimize:
        painter.drawLine(
          QPointF(snap(center.x() - half), snap(center.y())), QPointF(snap(center.x() + half), snap(center.y())));
        break;
      case CaptionButton::Maximize:
        if (state.maximized) {
          const auto left = snap(center.x() - half);
          const auto right = snap(center.x() + half);
          const auto top = snap(center.y() - half);
          const auto bottom = snap(center.y() + half);
          const auto offset = 2.;

          // Front window.
          painter.drawRect(QRectF(QPointF(left, top + offset), QPointF(right - offset, bottom)));

          // Visible part of the back window.
          QPainterPath path;
          path.moveTo(left + offset, top + offset);
          path.lineTo(left + offset, top);
          path.lineTo(right, top);
          path.lineTo(right, bottom - offset);
          path.lineTo(right - offset, bottom - offset);
          painter.drawPath(path);
        } else {
          painter.drawRect(QRectF(QPointF(snap(center.x() - half), snap(center.y() - half)),
            QPointF(snap(center.x() + half), snap(center.y() + half))));
        }
        break;
      case CaptionButton::Close:
        painter.drawLine(QPointF(snap(center.x() - half), snap(center.y() - half)),
          QPointF(snap(center.x() + half), snap(center.y() + half)));
        painter.drawLine(QPointF(snap(center.x() + half), snap(center.y() - half)),
          QPointF(snap(center.x() - half), snap(center.y() + half)));
        break;
      case CaptionButton::Menu: {
        const auto icon = QGuiApplication::windowIcon();
        if (!icon.isNull()) {
          const auto iconExtent = theme.iconSize.width();
          icon.paint(&painter,
            QRect(qRound(center.x() - iconExtent / 2.), qRound(center.y() - iconExtent / 2.), iconExtent, iconExtent));
        } else {
          for (const auto dy : { -half + 1., 0., half - 1. }) {
            painter.drawLine(QPointF(snap(center.x() - half), snap(center.y() + dy)),
              QPointF(snap(center.x() + half), snap(center.y() + dy)));
          }
        }
        break;
      }
    }
    painter.restore();
  }
};
} // namespace

std::unique_ptr<TitleBarRenderer> createQlementineTitleBarRenderer() {
  return std::make_unique<QlementineTitleBarRenderer>();
}
} // namespace oclero::qlementine
