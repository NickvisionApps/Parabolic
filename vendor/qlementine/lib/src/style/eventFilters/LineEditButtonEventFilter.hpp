// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/animation/WidgetAnimationManager.hpp>
#include <oclero/qlementine/utils/StateUtils.hpp>
#include <oclero/qlementine/utils/PrimitiveUtils.hpp>

#include <QToolButton>
#include <QPointer>
#include <QLineEdit>
#include <QPainter>
#include <QMoveEvent>

namespace oclero::qlementine {
class LineEditButtonEventFilter : public QObject {
public:
  LineEditButtonEventFilter(QlementineStyle* style, WidgetAnimationManager& animManager, QToolButton* button)
    : QObject(button)
    , _style(style)
    , _animManager(animManager)
    , _button(button) {
    // Qt doesn't emit this signal so we emit it by ourselves.
    if (auto* parent = button->parentWidget()) {
      if (const auto* lineEdit = qobject_cast<QLineEdit*>(parent)) {
        QObject::connect(_button, &QAbstractButton::clicked, lineEdit, &QLineEdit::returnPressed);
      }
    }
  }

  ~LineEditButtonEventFilter() override = default;

protected:
  bool eventFilter(QObject*, QEvent* evt) override {
    if (!_style) {
      return false;
    }

    switch (evt->type()) {
      case QEvent::Resize:
        // Prevent resizing from qlineedit_p.cpp:540
        evt->ignore();
        return true;
      case QEvent::Move: {
        // Prevent moving from qlineedit_p.cpp:540
        evt->ignore();
        // Instead, place the button by ourselves.
        const auto* moveEvent = static_cast<QMoveEvent*>(evt);
        const auto proposedX = moveEvent->pos().x();
        const auto* parentLineEdit = _button->parentWidget();
        const auto parentRect = parentLineEdit->rect();
        const auto& theme = _style ? _style->theme() : Theme{};
        const auto buttonH = theme.controlHeightMedium;
        const auto buttonW = buttonH;
        const auto spacing = theme.spacing / 2;
        const auto buttonY = parentRect.y() + (parentRect.height() - buttonH) / 2;
        auto buttonX = 0;

        auto isLeading = false;
        if (proposedX < (parentRect.width() / 2)) {
          isLeading = true;
        }

        const auto isRTL = parentLineEdit->layoutDirection() == Qt::RightToLeft;

        if ((isLeading && !isRTL) || (!isLeading && isRTL)) {
          buttonX = parentRect.x() + spacing;
        } else {
          buttonX = parentRect.x() + parentRect.width() - buttonW - spacing;
        }
        _button->setGeometry(buttonX, buttonY, buttonW, buttonH);
        return true;
      } break;
      case QEvent::Paint: {
        // Draw the button by ourselves to bypass QLineEditIconButton::paintEvent in qlineedit_p.cpp:353
        const auto enabled = _button->isEnabled();
        if (!enabled) {
          evt->accept();
          return true;
        }

        const auto hovered = _button->underMouse();
        const auto pressed = _button->isDown();
        const auto mouse = getMouseState(pressed, hovered, enabled);
        const auto& theme = _style ? _style->theme() : Theme{};
        const auto rect = _button->rect();
        const auto palette = _button->style()->standardPalette();

        const auto& bgColor = _style ? _style->toolButtonBackgroundColor(mouse, ColorRole::Secondary)
                                     : palette.color(getPaletteColorGroup(mouse), QPalette::ColorRole::ButtonText);
        const auto& fgColor = _style ? _style->toolButtonForegroundColor(mouse, ColorRole::Secondary)
                                     : palette.color(getPaletteColorGroup(mouse), QPalette::ColorRole::Button);
        const auto animationDuration = _style ? _style->theme().animationDuration : 0;
        const auto& currentBgColor = _animManager.animateBackgroundColor(_button, bgColor, animationDuration);
        const auto& currentFgColor = _animManager.animateForegroundColor(_button, fgColor, animationDuration);

        // Get opacity animated in qlinedit_p.cpp:436
        const auto opacity = _button->property(QByteArrayLiteral("opacity")).toDouble();

        const auto circleH = theme.controlHeightMedium;
        const auto circleW = circleH;
        const auto circleX = rect.x() + (rect.width() - circleW) / 2;
        const auto circleY = rect.y() + (rect.height() - circleH) / 2;
        const auto circleRect = QRect(QPoint{ circleX, circleY }, QSize{ circleW, circleH });

        const auto pixmap = getPixmap(_button->icon(), theme.iconSize, mouse, CheckState::NotChecked, _button);
        const auto autoIconColor = _style ? _style->autoIconColor(_button) : AutoIconColor::None;
        const auto& colorizedPixmap = _style->getColorizedPixmap(pixmap, autoIconColor, currentFgColor, currentFgColor);
        const auto pixmapX = circleRect.x() + (circleRect.width() - theme.iconSize.width()) / 2;
        const auto pixmapY = circleRect.y() + (circleRect.height() - theme.iconSize.height()) / 2;
        const auto pixmapRect = QRect{ { pixmapX, pixmapY }, theme.iconSize };

        QPainter p(_button);
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setOpacity(opacity);
        p.setPen(Qt::NoPen);

        // Background.
        p.setBrush(currentBgColor);
        p.drawEllipse(circleRect);

        // Foreground.
        p.drawPixmap(pixmapRect, colorizedPixmap);

        evt->accept();
        return true;
      } break;
      default:
        break;
    }

    return false;
  }

private:
  QPointer<QlementineStyle> _style;
  WidgetAnimationManager& _animManager;
  QToolButton* _button{ nullptr };
};
} // namespace oclero::qlementine
