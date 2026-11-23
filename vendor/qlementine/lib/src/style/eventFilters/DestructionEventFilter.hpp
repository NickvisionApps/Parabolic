// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <functional>
#include <utility>

#include <QEvent>
#include <QObject>
#include <QWidget>
#include <QPointer>

namespace oclero::qlementine {
class DestructionEventFilter : public QObject {
  Q_OBJECT

public:
  using DestructionCallback = std::function<void(QWidget*, QObject*)>;

  DestructionEventFilter(QWidget* widget, QObject* parent, DestructionCallback callback)
    : QObject(parent) // Parent to QlementineStyle, not the widget!
    , _widget(widget)
    , _callback(std::move(callback)) {}
  ~DestructionEventFilter() override = default;

protected:
  bool eventFilter(QObject* watched, QEvent* event) override {
    if (event->type() == QEvent::Destroy && watched == _widget) {
      auto* widget = _widget.data();
      auto callback = std::move(_callback);
      _callback = {};
      _widget = nullptr;
      watched->removeEventFilter(this);
      if (callback) {
        callback(widget, this);
      }
      deleteLater();
      return false;
    }
    return QObject::eventFilter(watched, event);
  }

private:
  QPointer<QWidget> _widget;
  DestructionCallback _callback;
};
} // namespace oclero::qlementine
