// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/Action.hpp>

namespace oclero::qlementine {
Action::Action(QObject* parent)
  : QAction(parent) {}

Action::Action(const QString& text, QObject* parent)
  : QAction(text, parent) {}

Action::Action(const QIcon& icon, const QString& text, QObject* parent)
  : QAction(icon, text, parent) {}

Action::Action(const QKeySequence& shortcut, QObject* parent)
  : QAction(parent) {
  setShortcut(shortcut);
  setShortcutContext(Qt::ApplicationShortcut);
}

Action::Action(const QKeySequence& shortcut, Qt::ShortcutContext shortcutContext, QObject* parent)
  : QAction(parent) {
  setShortcut(shortcut);
  setShortcutContext(shortcutContext);
}

Action::Action(
  const QString& text, const QKeySequence& shortcut, const Qt::ShortcutContext shortcutContext, QObject* parent)
  : QAction(text, parent) {
  setShortcut(shortcut);
  setShortcutContext(shortcutContext);
}

Action::Action(const QString& text, const QKeySequence& shortcut, QObject* parent)
  : QAction(text, parent) {
  setShortcut(shortcut);
  setShortcutContext(Qt::ApplicationShortcut);
}

Action::Action(const QIcon& icon, const QString& text, const QKeySequence& shortcut, QObject* parent)
  : QAction(icon, text, parent) {
  setShortcut(shortcut);
  setShortcutContext(Qt::ApplicationShortcut);
}

Action::Action(const QIcon& icon, const QString& text, const QKeySequence& shortcut,
  const Qt::ShortcutContext shortcutContext, QObject* parent)
  : QAction(icon, text, parent) {
  setShortcut(shortcut);
  setShortcutContext(shortcutContext);
}

void Action::setCallback(const std::function<void()>& cb) {
  _triggeredCb = cb;
  QObject::disconnect(_triggerdConnection);
  _triggerdConnection = QObject::connect(this, &QAction::triggered, this, _triggeredCb);
}

void Action::setEnabledPredicate(const std::function<bool()>& cb) {
  _updateEnabledCb = cb;
  updateEnabled();
}

void Action::setCheckedPredicate(const std::function<bool()>& cb) {
  _updateCheckedCb = cb;
  updateChecked();
}

void Action::setCheckablePredicate(const std::function<bool()>& cb) {
  _updateCheckableCb = cb;
  updateCheckable();
}

void Action::setVisiblePredicate(const std::function<bool()>& cb) {
  _updateVisibleCb = cb;
  updateVisible();
}

void Action::updateEnabled() {
  if (_updateEnabledCb) {
    setEnabled(_updateEnabledCb());
  }
}

void Action::updateChecked() {
  if (_updateCheckedCb) {
    setChecked(_updateCheckedCb());
  }
}

void Action::updateCheckable() {
  if (_updateCheckableCb) {
    setCheckable(_updateCheckableCb());
  }
}

void Action::updateVisible() {
  if (_updateVisibleCb) {
    setVisible(_updateVisibleCb());
  }
}

void Action::update() {
  updateEnabled();
  updateCheckable();
  updateChecked();
  updateVisible();
}

bool Action::shortcutEditable() const {
  return _shortcutEditable;
}

void Action::setShortcutEditable(bool editable) {
  if (editable != _shortcutEditable) {
    _shortcutEditable = editable;
    Q_EMIT shortcutEditableChanged();
    Q_EMIT changed();
  }
}

const QKeySequence& Action::userShortcut() const {
  return _userShortcut;
}

void Action::setUserShortcut(const QKeySequence& shortcut) {
  if (_shortcutEditable && shortcut != _userShortcut) {
    if (!_shortcutEditedByUser) {
      //  Save the default shortcut.
      _defaultShortcut = this->shortcut();
      _shortcutEditedByUser = true;
      Q_EMIT shortcutEditedByUserChanged();
    }

    _userShortcut = shortcut;

    // Set the new shortcut on the action.
    setShortcut(_userShortcut);

    Q_EMIT userShortcutChanged();
  }
}

void Action::resetShortcut() {
  if (_shortcutEditedByUser) {
    _userShortcut = {};
    _shortcutEditedByUser = false;

    Q_EMIT userShortcutChanged();
    Q_EMIT shortcutEditedByUserChanged();
    setShortcut(_defaultShortcut);
  }
}

bool Action::shortcutEditedByUser() const {
  return _shortcutEditedByUser;
}

const QString& Action::description() const {
  return _description;
}

void Action::setDescription(const QString& description) {
  if (description != _description) {
    _description = description;
    Q_EMIT descriptionChanged();
    Q_EMIT changed();
  }
}
} // namespace oclero::qlementine
