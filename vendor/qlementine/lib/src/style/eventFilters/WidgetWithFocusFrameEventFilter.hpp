// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <QAbstractScrollArea>
#include <QFocusFrame>
#include <QPointer>
#include <QTimer>
#include <QEvent>

namespace oclero::qlementine {
class WidgetWithFocusFrameEventFilter : public QObject {
  Q_OBJECT
public:
  explicit WidgetWithFocusFrameEventFilter(QWidget* widget)
    : QObject(widget)
    , _widget(widget) {
    _focusFrame = new QFocusFrame(_widget);
  }

  ~WidgetWithFocusFrameEventFilter() override = default;

  bool eventFilter(QObject* watchedObject, QEvent* evt) override {
    if (watchedObject == _widget) {
      switch (evt->type()) {
        case QEvent::Paint:
          // Create the focus frame as late as possible to give
          // more chances to any parent (e.g. scrollarea) to already exist.
          // QEvent::Show isn't sufficient. We need to delay even more, so
          // waiting for the first QEvent::Paint is our only solution.
          if (!_added) {
            QTimer::singleShot(0, this, [this]() {
              attachFocusFrame();
            });
          }
          break;
        case QEvent::Show:
          // Defer to avoid mapTo() on a partially-connected hierarchy
          // after reparenting (same pattern as the Paint path above).
          if (_added) {
            QTimer::singleShot(0, this, [this]() {
              reattachFocusFrame();
            });
          }
          break;
        case QEvent::Hide:
          // Disconnect the focus frame when the widget is hidden (e.g.
          // during reparenting) to prevent stale mapTo() calls.
          if (_added) {
            detachFocusFrame();
          }
          break;
        default:
          break;
      }
    }

    return QObject::eventFilter(watchedObject, evt);
  }

private:
  void attachFocusFrame() {
    if (!_added && _widget && _focusFrame) {
      _added = true;
      _focusFrame->setWidget(_widget);
    }
  }

  void reattachFocusFrame() {
    if (_widget && _focusFrame) {
      _focusFrame->setWidget(nullptr);
      _focusFrame->setWidget(_widget);
    }
  }

  void detachFocusFrame() {
    if (_focusFrame) {
      _focusFrame->setWidget(nullptr);
    }
  }

  QPointer<QWidget> _widget;
  QPointer<QFocusFrame> _focusFrame;
  bool _added{ false };
};
} // namespace oclero::qlementine
