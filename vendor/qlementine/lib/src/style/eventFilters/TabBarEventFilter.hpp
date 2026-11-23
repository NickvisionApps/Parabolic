// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <QEvent>
#include <QObject>
#include <QTabBar>
#include <QToolButton>
#include <QMouseEvent>
#include <QPointer>

#include <oclero/qlementine/style/QlementineStyle.hpp>

namespace oclero::qlementine {
class TabBarButtonEventFilter : public QObject {
public:
  explicit TabBarButtonEventFilter(QTabBar* tabBar)
    : QObject(tabBar)
    , _tabBar(tabBar) {}

  ~TabBarButtonEventFilter() override = default;

protected:
  bool eventFilter(QObject*, QEvent* evt) override {
    const auto type = evt->type();
    if (type == QEvent::Leave || type == QEvent::Enter) {
      if (_tabBar) {
        _tabBar->update();
      }
    }

    return false;
  }

private:
  QTabBar* _tabBar{ nullptr };
};

class TabBarEventFilter : public QObject {
public:
  TabBarEventFilter(QTabBar* tabBar)
    : QObject(tabBar)
    , _tabBar(tabBar) {
    // Tweak left/right buttons.
    const auto toolButtons = tabBar->findChildren<QToolButton*>();
    if (toolButtons.size() == 2) {
      auto* buttonEvtFilter = new TabBarButtonEventFilter(_tabBar);

      _leftButton = toolButtons.at(0);
      _leftButton->setFocusPolicy(Qt::NoFocus);
      _leftButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      QlementineStyle::setAutoIconColor(_leftButton, AutoIconColor::None);
      _leftButton->installEventFilter(buttonEvtFilter);

      _rightButton = toolButtons.at(1);
      _rightButton->setFocusPolicy(Qt::NoFocus);
      _rightButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      QlementineStyle::setAutoIconColor(_rightButton, AutoIconColor::None);
      _rightButton->installEventFilter(buttonEvtFilter);

      updateScrollButtonsSize();
    }
  }

  ~TabBarEventFilter() override = default;

  bool eventFilter(QObject*, QEvent* evt) override {
    const auto type = evt->type();
    const auto vertical = isVertical();

    if (type == QEvent::Resize) {
      // The shape of the QTabBar might have changed, and the scroll buttons must follow.
      updateScrollButtonsSize();
    } else if (type == QEvent::MouseButtonRelease) {
      const auto* mouseEvent = static_cast<QMouseEvent*>(evt);
      if (mouseEvent->button() == Qt::MouseButton::MiddleButton) {
        // Close tab.
        const auto tabIndex = _tabBar->tabAt(mouseEvent->pos());
        if (tabIndex != -1 && _tabBar->isTabVisible(tabIndex)) {
          evt->accept();
          Q_EMIT _tabBar->tabCloseRequested(tabIndex);
          return true;
        }
      } else if (mouseEvent->button() == Qt::MouseButton::RightButton) {
        // Tab context menu.
        const auto tabIndex = _tabBar->tabAt(mouseEvent->pos());
        if (tabIndex != -1 && _tabBar->isTabVisible(tabIndex)) {
          evt->accept();
          Q_EMIT _tabBar->customContextMenuRequested(mouseEvent->pos());
          return true;
        }
      }
    } else if (type == QEvent::Wheel) {
      const auto* wheelEvent = static_cast<QWheelEvent*>(evt);

      // Block scroll that is not along the QTabBar.
      const bool wheelVertical = qAbs(wheelEvent->angleDelta().y()) > qAbs(wheelEvent->angleDelta().x());
      if (wheelVertical != vertical) {
        evt->ignore();
        return true;
      }

      auto delta = vertical ? wheelEvent->pixelDelta().y() : wheelEvent->pixelDelta().x();

      // If delta is null, it might be because we are on MacOS, using a trackpad.
      // So let's use angleDelta instead.
      if (delta == 0) {
        delta = vertical ? wheelEvent->angleDelta().y() : wheelEvent->angleDelta().x();
      }

      // Scrolling up (positive vertical delta) goes back to the previous tabs.
      if (vertical) {
        delta = -delta;
      }

      // Invert the value if necessary.
      if (wheelEvent->inverted()) {
        delta = -delta;
      }

      if (delta > 0 && _rightButton) {
        // delta > 0 : scroll to the right.
        _rightButton->click();
        evt->accept();
        return true;
      } else if (delta < 0 && _leftButton) {
        // delta < 0 : scroll to the left.
        _leftButton->click();
        evt->accept();
        return true;
      } else {
        evt->ignore();
        return true;
      }
    } else if (type == QEvent::HoverMove) {
      const auto* mouseEvent = static_cast<QMouseEvent*>(evt);
      const auto begin = vertical ? _leftButton->y() : _leftButton->x();
      if ((vertical ? mouseEvent->pos().y() : mouseEvent->pos().x()) > begin) {
        _tabBar->update();
      }
    }

    return false;
  }

private:
  bool isVertical() const {
    const auto shape = _tabBar->shape();
    return shape == QTabBar::RoundedWest || shape == QTabBar::RoundedEast || shape == QTabBar::TriangularWest
           || shape == QTabBar::TriangularEast;
  }

  void updateScrollButtonsSize() {
    if (_leftButton) {
      _leftButton->setFixedSize(_leftButton->sizeHint());
    }
    if (_rightButton) {
      _rightButton->setFixedSize(_rightButton->sizeHint());
    }
  }

  QTabBar* _tabBar{ nullptr };
  QPointer<QToolButton> _leftButton{ nullptr };
  QPointer<QToolButton> _rightButton{ nullptr };
};
} // namespace oclero::qlementine
