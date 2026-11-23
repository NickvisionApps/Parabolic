// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/utils/StateUtils.hpp>

#include <QObject>
#include <QAbstractScrollArea>
#include <QEvent>
#include <QStyleOption>
#include <QPainter>

namespace oclero::qlementine {
// Works for both QTextEdit and QPlainTextEdit
class TextEditEventFilter : public QObject {
public:
  explicit TextEditEventFilter(QAbstractScrollArea* textEdit)
    : QObject(textEdit)
    , _textEdit(textEdit) {}

  ~TextEditEventFilter() override = default;

  bool eventFilter(QObject*, QEvent* evt) override {
    const auto type = evt->type();
    switch (type) {
      case QEvent::Enter:
      case QEvent::Leave:
        _textEdit->update();
        break;
      case QEvent::Paint: {
        const auto* qlementineStyle = qobject_cast<QlementineStyle*>(_textEdit->style());
        if (!qlementineStyle)
          break;

        const auto frameShape = _textEdit->frameShape();
        switch (frameShape) {
          case QFrame::Shape::StyledPanel: {
            QStyleOptionFrame opt;
            opt.initFrom(_textEdit);
            opt.rect = _textEdit->rect();
            QPainter p(_textEdit);
            qlementineStyle->drawPrimitive(QStyle::PE_PanelLineEdit, &opt, &p, _textEdit);
          } break;
          case QFrame::Shape::Panel: {
            const auto mouse = getMouseState(_textEdit->hasFocus(), _textEdit->underMouse(), _textEdit->isEnabled());
            const auto& bgColor = qlementineStyle->textFieldBackgroundColor(mouse, Status::Default);
            const auto rect = _textEdit->rect();
            QPainter p(_textEdit);
            p.fillRect(rect, bgColor);
          } break;
          default:
            break;
        }
      } break;
      default:
        break;
    }

    return false;
  }

private:
  QAbstractScrollArea* _textEdit{ nullptr };
};
} // namespace oclero::qlementine
