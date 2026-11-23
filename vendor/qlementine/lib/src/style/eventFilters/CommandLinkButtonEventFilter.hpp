// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/animation/WidgetAnimationManager.hpp>
#include <oclero/qlementine/utils/StateUtils.hpp>
#include <oclero/qlementine/utils/PrimitiveUtils.hpp>

#include <QPointer>
#include <QPainter>
#include <QCommandLinkButton>
#include <QEvent>

namespace oclero::qlementine {
class CommandLinkButtonEventFilter : public QObject {
public:
  CommandLinkButtonEventFilter(QlementineStyle* style, WidgetAnimationManager& animManager, QCommandLinkButton* button)
    : QObject(button)
    , _style(style)
    , _animManager(animManager)
    , _button(button) {}

  ~CommandLinkButtonEventFilter() override = default;

  bool eventFilter(QObject*, QEvent* evt) override {
    if (evt->type() == QEvent::Paint && _style) {
      // Draw the button by ourselves to bypass QLineEditIconButton::paintEvent in qlineedit_p.cpp:353
      const auto enabled = _button->isEnabled();
      const auto hovered = _button->underMouse();
      const auto pressed = _button->isDown();
      const auto mouse = getMouseState(pressed, hovered, enabled);
      const auto& theme = _style->theme();
      const auto rect = _button->rect();
      const auto spacing = theme.spacing;
      const auto hPadding = spacing * 2;
      const auto fgRect = rect.marginsRemoved(QMargins(hPadding, 0, hPadding, 0));
      const auto& bgColor = _style->toolButtonBackgroundColor(mouse, ColorRole::Secondary);
      const auto& currentBgColor = _animManager.animateBackgroundColor(_button, bgColor, theme.animationDuration);
      const auto radius = theme.borderRadius;

      const auto iconSize = theme.iconSize;
      const auto pixmap = getPixmap(_button->icon(), iconSize, mouse, CheckState::NotChecked, _button);
      const auto pixmapX = fgRect.x();
      const auto pixmapY = fgRect.y() + (fgRect.height() - iconSize.height()) / 2;
      const auto pixmapRect = QRect{ { pixmapX, pixmapY }, iconSize };

      QPainter p(_button);
      p.setRenderHint(QPainter::Antialiasing, true);
      p.setRenderHint(QPainter::SmoothPixmapTransform, true);
      p.setPen(Qt::NoPen);
      p.setBrush(currentBgColor);
      p.drawRoundedRect(rect, radius, radius);
      p.drawPixmap(pixmapRect, pixmap);

      evt->accept();
      return true;
    }

    return false;
  }

private:
  QPointer<QlementineStyle> _style;
  WidgetAnimationManager& _animManager;
  QCommandLinkButton* _button{ nullptr };
};
} // namespace oclero::qlementine
