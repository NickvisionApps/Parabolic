// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <QEvent>
#include <QObject>
#include <QWidget>

namespace oclero::qlementine {
class MouseWheelBlockerEventFilter : public QObject {
public:
  explicit MouseWheelBlockerEventFilter(QWidget* widget)
    : QObject(widget)
    , _widget(widget) {}

  ~MouseWheelBlockerEventFilter() override = default;

  bool eventFilter(QObject*, QEvent* evt) override {
    if (evt->type() == QEvent::Wheel && !_widget->hasFocus()) {
      evt->ignore();
      return true;
    }

    return false;
  }

private:
  QWidget* _widget{ nullptr };
};
} // namespace oclero::qlementine
