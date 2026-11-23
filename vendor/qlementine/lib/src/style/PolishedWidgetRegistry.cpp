// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include "PolishedWidgetRegistry.hpp"

#include <QObject>
#include <QWidget>

namespace oclero::qlementine {
PolishedWidgetRegistry::Entry& PolishedWidgetRegistry::ensure(QWidget* widget) {
  auto iter = _widgets.find(widget);
  if (iter == _widgets.end()) {
    Entry entry;
    entry.focusPolicy = widget->focusPolicy();
    entry.minimumSize = widget->minimumSize();
    entry.hover = widget->testAttribute(Qt::WA_Hover);
    entry.opaquePaintEvent = widget->testAttribute(Qt::WA_OpaquePaintEvent);
    entry.mouseTracking = widget->hasMouseTracking();
    iter = _widgets.insert(widget, entry);
  }
  return iter.value();
}

const PolishedWidgetRegistry::Entry* PolishedWidgetRegistry::find(QWidget* widget) const {
  const auto iter = _widgets.constFind(widget);
  return iter != _widgets.constEnd() ? &iter.value() : nullptr;
}

bool PolishedWidgetRegistry::hasEventFilters(QWidget* widget) const {
  const auto iter = _widgets.find(widget);
  return iter != _widgets.end() && !iter.value().eventFilters.empty();
}

void PolishedWidgetRegistry::addEventFilter(QWidget* widget, Entry& entry, QObject* eventFilter) {
  if (!widget || !eventFilter)
    return;

  widget->installEventFilter(eventFilter);
  rememberEventFilter(entry, eventFilter);
}

void PolishedWidgetRegistry::rememberEventFilter(Entry& entry, QObject* eventFilter) {
  if (eventFilter) {
    entry.eventFilters.push_back(eventFilter);
  }
}

void PolishedWidgetRegistry::removeAndDeleteEventFilters(QWidget* widget) {
  auto iter = _widgets.find(widget);
  if (iter == _widgets.end())
    return;

  const auto eventFilters = iter.value().eventFilters;
  for (auto* eventFilter : eventFilters) {
    if (eventFilter) {
      widget->removeEventFilter(eventFilter);
      delete eventFilter;
    }
  }

  _widgets.erase(iter);
}

void PolishedWidgetRegistry::forgetDestroyedWidget(QWidget* widget, QObject* destructionEventFilter) {
  if (!widget)
    return;

  auto iter = _widgets.find(widget);
  if (iter == _widgets.end())
    return;

  auto& eventFilters = iter.value().eventFilters;
  eventFilters.removeAll(destructionEventFilter);
  _widgets.erase(iter);
}
} // namespace oclero::qlementine
