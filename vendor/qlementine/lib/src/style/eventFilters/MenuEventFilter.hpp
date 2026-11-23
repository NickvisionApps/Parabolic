// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/utils/MenuUtils.hpp>

#include <QCoreApplication>
#include <QEvent>
#include <QGuiApplication>
#include <QObject>
#include <QMenu>
#include <QMenuBar>
#include <QScreen>
#include <QTimer>
#include <QMouseEvent>
#include <QPointer>

#include <algorithm>

namespace oclero::qlementine {
class MenuEventFilter : public QObject {
public:
  explicit MenuEventFilter(QMenu* menu)
    : QObject(menu)
    , _menu(menu) {
    menu->installEventFilter(this);
  }

  ~MenuEventFilter() override = default;

  bool eventFilter(QObject*, QEvent* evt) override {
    switch (evt->type()) {
      case QEvent::Type::Show: {
        _mousePressed = false;

        // Place the QMenu correctly by making up for the drop shadow margins.
        // It'll be reset before every show, so we can safely move it every time.
        // Submenus should already be placed correctly, so there's no need to translate their geometry.
        // Also, make up for the menu item padding so the texts are aligned.
        const auto isMenuBarMenu = qobject_cast<QMenuBar*>(_menu->parentWidget()) != nullptr;
        const auto isSubMenu = qobject_cast<QMenu*>(_menu->parentWidget()) != nullptr;
        const auto alignForMenuBar = isMenuBarMenu && !isSubMenu;
        const auto* qlementineStyle = qobject_cast<QlementineStyle*>(_menu->style());
        const auto menuItemHPadding = qlementineStyle ? qlementineStyle->theme().spacing : 0;
        const auto menuDropShadowWidth = qlementineStyle ? qlementineStyle->theme().spacing : 0;
        const auto menuOriginalPos = _menu->pos();
        const auto menuBarTranslation = alignForMenuBar ? QPoint(-menuItemHPadding, 0) : QPoint(0, 0);
        const auto shadowTranslation = QPoint(-menuDropShadowWidth, -menuDropShadowWidth);
        auto menuNewPos = menuOriginalPos + menuBarTranslation + shadowTranslation;

        if (alignForMenuBar) {
          const auto* menuBar = qobject_cast<QMenuBar*>(_menu->parentWidget());
          const auto actionRect = menuBar->actionGeometry(_menu->menuAction());
          const auto anchor = menuBar->mapToGlobal(actionRect.center());
          const auto* screen = QGuiApplication::screenAt(anchor);
          if (!screen) {
            screen = menuBar->screen();
          }
          if (screen) {
            menuNewPos.setX(std::max(menuNewPos.x(), screen->availableGeometry().left() - menuDropShadowWidth));
          }
        }

        // Menus have weird sizing bugs when moving them from this event.
        // We have to wait for the event loop to be processed before setting the final position.
        const auto menuSize = _menu->size();
        if (menuSize != QSize(0, 0)) {
          _menu->resize(0, 0); // Hide the menu for now until we can set the position.
          QTimer::singleShot(0, _menu, [this, menuNewPos, menuSize]() {
            _menu->move(menuNewPos);
            _menu->resize(menuSize);
          });
        }
      } break;
      case QEvent::Type::MouseButtonPress: {
        _mousePressed = true;
        const auto* mouseEvt = static_cast<QMouseEvent*>(evt);
        const auto mousePos = mouseEvt->pos();
        if (const auto* action = _menu->actionAt(mousePos)) {
          if (action->isSeparator() || !action->isEnabled() || action->property("qlementine_flashing").toBool()) {
            return true;
          }
        } else if (_menu->rect().contains(mousePos)) {
          return true;
        }
      } break;
      case QEvent::Type::MouseButtonRelease: {
        if (!_mousePressed) {
          if (evt == _mouseEventToNotFilter.get()) {
            // Let it go to the widget.
            return false;
          }
          return true; // ignore
        }
        _mousePressed = false;
        const auto* mouseEvt = static_cast<QMouseEvent*>(evt);
        const auto mousePos = mouseEvt->pos();
        auto* action = _menu->actionAt(mousePos);
        // A click in a scrolled/clipped menu can miss every action rect: fall back to the highlighted action.
        if (action == nullptr && _menu->rect().contains(mousePos)) {
          action = _menu->activeAction();
        }
        if (action != nullptr) {
          if (action->isSeparator() || !action->isEnabled() || action->property("qlementine_flashing").toBool())
            return true;

          if (action->menu() == nullptr) {
            flashAction(action, _menu, [this, action]() {
              // We send a manually-built mouse event to click on the menu item.
              const auto menuItemCenter = _menu->actionGeometry(action).center();
              _mouseEventToNotFilter.reset(new QMouseEvent(QEvent::MouseButtonRelease, menuItemCenter,
                _menu->mapToGlobal(menuItemCenter), Qt::LeftButton, Qt::NoButton, Qt::NoModifier));
              QCoreApplication::sendEvent(_menu, _mouseEventToNotFilter.get());
              _mouseEventToNotFilter.reset();
            });
            return true;
          }
        } else if (_menu->rect().contains(mousePos)) {
          return true;
        }
      } break;
      default:
        break;
    }

    return false;
  }

private:
  QMenu* _menu{ nullptr };

  // This is roughly copied from how QMenuPrivate::mouseDown detects whether the mouse is down.
  bool _mousePressed{ false };
  // Please do not dereference it, this is just to close properly the QMenu.
  std::unique_ptr<QEvent> _mouseEventToNotFilter{};
};
} // namespace oclero::qlementine
