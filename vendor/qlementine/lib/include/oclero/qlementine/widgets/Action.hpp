// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QAction>
#include <functional>

namespace oclero::qlementine {
class QLEMENTINE_EXPORT Action : public QAction {
  Q_OBJECT

  Q_PROPERTY(bool shortcutEditable READ shortcutEditable WRITE setShortcutEditable NOTIFY shortcutEditableChanged)
  Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
  Q_PROPERTY(QKeySequence userShortcut READ userShortcut WRITE setUserShortcut NOTIFY userShortcutChanged)

public:
  explicit Action(QObject* parent = nullptr);
  explicit Action(const QString& text, QObject* parent = nullptr);
  explicit Action(const QIcon& icon, const QString& text, QObject* parent = nullptr);
  explicit Action(const QKeySequence& shortcut, QObject* parent = nullptr);
  explicit Action(const QKeySequence& shortcut, Qt::ShortcutContext shortcutContext, QObject* parent = nullptr);
  explicit Action(const QString& text, const QKeySequence& shortcut, QObject* parent = nullptr);
  explicit Action(const QString& text, const QKeySequence& shortcut, const Qt::ShortcutContext shortcutContext,
    QObject* parent = nullptr);
  explicit Action(const QIcon& icon, const QString& text, const QKeySequence& shortcut, QObject* parent = nullptr);
  explicit Action(const QIcon& icon, const QString& text, const QKeySequence& shortcut,
    const Qt::ShortcutContext shortcutContext, QObject* parent = nullptr);
  ~Action() override = default;

  void setCallback(const std::function<void()>& cb);
  void setEnabledPredicate(const std::function<bool()>& cb);
  void setCheckedPredicate(const std::function<bool()>& cb);
  void setCheckablePredicate(const std::function<bool()>& cb);
  void setVisiblePredicate(const std::function<bool()>& cb);

  void updateEnabled();
  void updateChecked();
  void updateCheckable();
  void updateVisible();
  void update();

  bool shortcutEditable() const;
  void setShortcutEditable(bool editable);
  Q_SIGNAL void shortcutEditableChanged();

  const QKeySequence& userShortcut() const;
  void setUserShortcut(const QKeySequence&);
  Q_SIGNAL void userShortcutChanged();

  void resetShortcut();

  bool shortcutEditedByUser() const;
  Q_SIGNAL void shortcutEditedByUserChanged();

  const QString& description() const;
  void setDescription(const QString& description);
  Q_SIGNAL void descriptionChanged();

private:
  bool _shortcutEditable{ false };
  bool _shortcutEditedByUser{ false };
  QString _description;
  QKeySequence _defaultShortcut;
  QKeySequence _userShortcut;
  QMetaObject::Connection _triggerdConnection;
  std::function<void()> _triggeredCb;
  std::function<bool()> _updateEnabledCb;
  std::function<bool()> _updateCheckedCb;
  std::function<bool()> _updateCheckableCb;
  std::function<bool()> _updateVisibleCb;
};
} // namespace oclero::qlementine

Q_DECLARE_METATYPE(QAction*)
Q_DECLARE_METATYPE(oclero::qlementine::Action*)
