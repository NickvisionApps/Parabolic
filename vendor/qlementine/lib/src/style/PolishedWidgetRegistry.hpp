// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <QList>
#include <QMap>
#include <QSize>
#include <Qt>

class QObject;
class QWidget;

namespace oclero::qlementine {
class PolishedWidgetRegistry {
public:
  struct Entry {
    QList<QObject*> eventFilters;
    Qt::FocusPolicy focusPolicy{ Qt::NoFocus };
    QSize minimumSize;
    bool hover{ false };
    bool opaquePaintEvent{ false };
    bool mouseTracking{ false };
  };

  Entry& ensure(QWidget* widget);
  const Entry* find(QWidget* widget) const;
  bool hasEventFilters(QWidget* widget) const;

  void addEventFilter(QWidget* widget, Entry& entry, QObject* eventFilter);
  void rememberEventFilter(Entry& entry, QObject* eventFilter);
  void removeAndDeleteEventFilters(QWidget* widget);
  void forgetDestroyedWidget(QWidget* widget, QObject* destructionEventFilter);

private:
  QMap<QWidget*, Entry> _widgets;
};
} // namespace oclero::qlementine
