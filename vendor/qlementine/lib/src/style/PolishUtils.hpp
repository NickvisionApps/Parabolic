// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "PolishedWidgetRegistry.hpp"

class QObject;
class QWidget;

namespace oclero::qlementine {
class QlementineStyle;
class WidgetAnimationManager;

void polishToolTip(QWidget* widget);

void polishLineEditIconButton(QlementineStyle& style, WidgetAnimationManager& animations, QWidget* widget,
  PolishedWidgetRegistry::Entry& entry, bool eventFiltersInstalled);

void polishCommonWidget(QWidget* widget);

void polishExternalFocusFrame(
  QWidget* widget, PolishedWidgetRegistry& registry, PolishedWidgetRegistry::Entry& entry, bool eventFiltersInstalled);

void polishMenu(
  QWidget* widget, PolishedWidgetRegistry& registry, PolishedWidgetRegistry::Entry& entry, bool eventFiltersInstalled);

void polishComboBoxPopup(QWidget* widget, const QlementineStyle& style);

void polishVerticalCompression(QWidget* widget);

void polishMouseWheelBlocker(
  QWidget* widget, PolishedWidgetRegistry& registry, PolishedWidgetRegistry::Entry& entry, bool eventFiltersInstalled);

void polishComboBox(QlementineStyle& style, QWidget* widget);

void polishTabBar(
  QWidget* widget, PolishedWidgetRegistry& registry, PolishedWidgetRegistry::Entry& entry, bool eventFiltersInstalled);

void polishMessageBox(QWidget* widget, const QlementineStyle& style);

void polishScrollArea(QWidget* widget);

void polishSlider(QWidget* widget);

void polishTextEditors(
  QWidget* widget, PolishedWidgetRegistry& registry, PolishedWidgetRegistry::Entry& entry, bool eventFiltersInstalled);

void polishLineEditMenus(
  QWidget* widget, PolishedWidgetRegistry& registry, PolishedWidgetRegistry::Entry& entry, bool eventFiltersInstalled);

void polishDestructionTracking(QWidget* widget, QObject* parent, PolishedWidgetRegistry& registry,
  PolishedWidgetRegistry::Entry& entry, bool eventFiltersInstalled);

void unpolishWidget(QlementineStyle& style, QWidget* widget, PolishedWidgetRegistry& registry);
} // namespace oclero::qlementine
