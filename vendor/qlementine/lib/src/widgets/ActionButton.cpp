// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/ActionButton.hpp>

#include <QAction>

namespace oclero::qlementine {

ActionButton::ActionButton(QWidget* parent)
  : QPushButton(parent) {
  setDefault(true);
  setFocusPolicy(Qt::NoFocus);
}

ActionButton::~ActionButton() = default;

void ActionButton::setAction(QAction* action) {
  // Disconnect from previous action if any
  if (_action && (_action != action)) {
    disconnect(_action, &QAction::changed, this, &ActionButton::updateFromAction);
    disconnect(this, &ActionButton::clicked, _action, &QAction::trigger);
  }

  _action = action;
  updateFromAction();
  connect(_action, &QAction::changed, this, &ActionButton::updateFromAction);
  connect(this, &ActionButton::clicked, _action, &QAction::trigger);
}

void ActionButton::updateFromAction() {
  setText(_action->text());
  setStatusTip(_action->statusTip());
  setToolTip(_action->toolTip());
  setIcon(_action->icon());
  setEnabled(_action->isEnabled());
  setCheckable(_action->isCheckable());
  setChecked(_action->isChecked());
}

} // namespace oclero::qlementine
