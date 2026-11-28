// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include "PolishUtils.hpp"

#include "EventFilters.hpp"

#include <oclero/qlementine/animation/WidgetAnimationManager.hpp>
#include <oclero/qlementine/style/Delegates.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/utils/StyleUtils.hpp>
#include <oclero/qlementine/utils/WidgetUtils.hpp>

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QScrollArea>
#include <QSpinBox>
#include <QTabBar>
#include <QTextEdit>
#include <QToolButton>

namespace oclero::qlementine {
void polishToolTip(const QlementineStyle& style, QWidget* widget) {
  if (widget->inherits("QTipLabel")) {
    auto palette = widget->palette();
    palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::ToolTipBase, style.toolTipBackgroundColor());
    palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::ToolTipText, style.toolTipForegroundColor());
    palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::WindowText, style.toolTipForegroundColor());
    widget->setPalette(palette);
  }
#ifndef _WIN32
  // Currently we only support tooltips with rounded corners on MacOS.
  // More investigation is need to make it work on Windows.
  if (widget->inherits("QTipLabel")) {
    // TODO: turn this into addAlphaChannel
    widget->setBackgroundRole(QPalette::NoRole);
    widget->setAutoFillBackground(false);
    widget->setAttribute(Qt::WA_TranslucentBackground, true);
    widget->setAttribute(Qt::WA_NoSystemBackground, true);
    widget->setAttribute(Qt::WA_OpaquePaintEvent, false);
  }
#else
  Q_UNUSED(widget)
#endif
}

void polishLineEditIconButton(QlementineStyle& style, WidgetAnimationManager& animations, QWidget* widget,
  PolishedWidgetRegistry::Entry& entry, bool eventFiltersInstalled) {
  if (!widget->inherits("QLineEditIconButton"))
    return;

  if (!eventFiltersInstalled) {
    auto* eventFilter = new LineEditButtonEventFilter(&style, animations, qobject_cast<QToolButton*>(widget));
    widget->installEventFilter(eventFilter);
    entry.eventFilters.push_back(eventFilter);
  }
  widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  // Fix hardcoded width in qlineedit_p.cpp:493
  widget->setFixedSize(style.theme().controlHeightMedium, style.theme().controlHeightMedium);
}

void polishCommonWidget(QWidget* widget) {
  // Prevent the following warning:
  // QWidget::setMinimumSize: (/QAbstractButton) Negative sizes (0,-1) are not possible
  if (qobject_cast<QAbstractButton*>(widget) && widget->minimumSize() == QSize(0, -1)) {
    widget->setMinimumSize(0, 1);
  }

  // Font.
  if (shouldHaveBoldFont(widget)) {
    auto font = QFont{ widget->font() };
    font.setBold(true);
    widget->setFont(font);
  }

  // Enable hover state.
  if (shouldHaveHoverEvents(widget)) {
    widget->setAttribute(Qt::WA_Hover, true);
    widget->setAttribute(Qt::WA_OpaquePaintEvent, false);
  }
  if (shouldHaveMouseTracking(widget)) {
    widget->setMouseTracking(true);
  }

  // Hijack the default focus policy for buttons.
  if (shouldHaveTabFocus(widget)) {
    widget->setFocusPolicy(Qt::TabFocus);
  } else if (shouldHaveNoFocus(widget)) {
    widget->setFocusPolicy(Qt::NoFocus);
  }
}

void polishExternalFocusFrame(
  QWidget* widget, PolishedWidgetRegistry& registry, PolishedWidgetRegistry::Entry& entry, bool eventFiltersInstalled) {
  // QFocusFrame is used to draw focus outside of the widget's bound.
  if (shouldHaveExternalFocusFrame(widget) && !eventFiltersInstalled) {
    auto* eventFilter = new WidgetWithFocusFrameEventFilter(widget);
    registry.addEventFilter(widget, entry, eventFilter);
  }
}

void polishMenu(
  QWidget* widget, PolishedWidgetRegistry& registry, PolishedWidgetRegistry::Entry& entry, bool eventFiltersInstalled) {
  if (auto* menu = qobject_cast<QMenu*>(widget)) {
    menu->setBackgroundRole(QPalette::NoRole);
    menu->setAutoFillBackground(false);
    menu->setAttribute(Qt::WA_TranslucentBackground, true);
    menu->setAttribute(Qt::WA_OpaquePaintEvent, false);
    menu->setAttribute(Qt::WA_NoSystemBackground, true);
    menu->setWindowFlag(Qt::FramelessWindowHint, true);
    menu->setWindowFlag(Qt::NoDropShadowWindowHint, true);
    menu->setProperty("_q_windowsDropShadow", false);

    // Place the QMenu correctly by making up for the drop shadow margins.
    if (!eventFiltersInstalled) {
      auto* eventFilter = new MenuEventFilter(menu);
      registry.rememberEventFilter(entry, eventFilter);
    }
  }
}

void polishComboBoxPopup(QWidget* widget, const QlementineStyle& style) {
  if (auto* itemView = qobject_cast<QAbstractItemView*>(widget)) {
    auto* popup = itemView->parentWidget();
    auto isComboBoxPopupContainer = popup && popup->inherits("QComboBoxPrivateContainer");
    if (isComboBoxPopupContainer) {
      popup->setAttribute(Qt::WA_TranslucentBackground, true);
      popup->setAttribute(Qt::WA_OpaquePaintEvent, false);
      popup->setAttribute(Qt::WA_NoSystemBackground, true);
      popup->setWindowFlag(Qt::FramelessWindowHint, true);
      popup->setWindowFlag(Qt::NoDropShadowWindowHint, true);
      popup->setProperty("_q_windowsDropShadow", false);

      // Same shadow as QMenu.
      const auto shadowWidth = style.theme().spacing;
      const auto borderWidth = style.theme().borderWidth;
      const auto margin = shadowWidth + borderWidth;
      popup->layout()->setContentsMargins(margin, margin, margin, margin);

      itemView->viewport()->setAutoFillBackground(false);
      auto* comboBox = findFirstParentOfType<QComboBox>(itemView);
      new ComboboxItemViewFilter(comboBox, itemView);
    }
  }
}

void polishVerticalCompression(QWidget* widget) {
  // Ensure widgets are not compressed vertically.
  // Some widgets like QCheckBox or QLineEdit are compressed when added to
  // QFormLayout.
  if (shouldNotBeVerticallyCompressed(widget)) {
    const auto minHeight = widget->minimumHeight();
    if (minHeight == 0 || minHeight == 1) {
      const auto heightHint = widget->sizeHint().height();
      if (heightHint > 0) {
        widget->setMinimumHeight(widget->sizeHint().height());
      }
    }
  }
}

void polishMouseWheelBlocker(
  QWidget* widget, PolishedWidgetRegistry& registry, PolishedWidgetRegistry::Entry& entry, bool eventFiltersInstalled) {
  if (shouldNotHaveWheelEvents(widget)) {
    if (widget->focusPolicy() == Qt::WheelFocus) {
      widget->setFocusPolicy(Qt::StrongFocus);
    }
    if (!eventFiltersInstalled) {
      auto* eventFilter = new MouseWheelBlockerEventFilter(widget);
      registry.addEventFilter(widget, entry, eventFilter);
    }
  }
}

void polishComboBox(QlementineStyle& style, QWidget* widget) {
  if (auto* comboBox = qobject_cast<QComboBox*>(widget)) {
    comboBox->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToContents);

    // Only replace the delegate if the combobox doesn't already have a custom one.
    // This preserves delegates set by third-party widgets.
    if (isDefaultItemDelegate(comboBox->itemDelegate())) {
      // Will define a delegate to stylize the QComboBox items,
      comboBox->setItemDelegate(new ComboBoxDelegate(comboBox, style));
      // Trigger the redefine when the QComboBox's view changes.
      new ComboboxFilter(comboBox);
    }
  }
}

void polishTabBar(
  QWidget* widget, PolishedWidgetRegistry& registry, PolishedWidgetRegistry::Entry& entry, bool eventFiltersInstalled) {
  if (auto* tabBar = qobject_cast<QTabBar*>(widget)) {
    if (!eventFiltersInstalled) {
      auto* eventFilter = new TabBarEventFilter(tabBar);
      registry.addEventFilter(tabBar, entry, eventFilter);
    }
  }
}

void polishMessageBox(QWidget* widget, const QlementineStyle& style) {
  if (auto* messageBox = qobject_cast<QMessageBox*>(widget)) {
    if (auto* textEdit = messageBox->findChild<QTextEdit*>()) {
      textEdit->document()->setDocumentMargin(style.theme().spacing * 2);
    }
  }
}

void polishScrollArea(QWidget* widget) {
  // Prevent ScrollArea to be focusable with Tab key.
  if (auto* scrollarea = qobject_cast<QScrollArea*>(widget)) {
    scrollarea->setFocusPolicy(Qt::NoFocus);
  }
}

void polishSlider(QWidget* widget) {
  // Make the QSlider horizontal by default.
  if (auto* slider = qobject_cast<QSlider*>(widget)) {
    slider->setOrientation(Qt::Orientation::Horizontal);
  }
}

void polishTextEditors(
  QWidget* widget, PolishedWidgetRegistry& registry, PolishedWidgetRegistry::Entry& entry, bool eventFiltersInstalled) {
  // Make the QPlainTextEdit have a frame by default.
  if (auto* plainTextEdit = qobject_cast<QPlainTextEdit*>(widget)) {
    if (!eventFiltersInstalled) {
      auto* eventFilter = new TextEditEventFilter(plainTextEdit);
      registry.addEventFilter(plainTextEdit, entry, eventFilter);
    }
    if (auto* viewport = plainTextEdit->findChild<QWidget*>(QStringLiteral("qt_scrollarea_viewport"))) {
      viewport->setAutoFillBackground(false);
    }
  }
  // Make the QTextEdit have a frame by default.
  if (auto* textEdit = qobject_cast<QTextEdit*>(widget)) {
    if (!eventFiltersInstalled) {
      auto* eventFilter = new TextEditEventFilter(textEdit);
      registry.addEventFilter(textEdit, entry, eventFilter);
    }
    if (auto* viewport = textEdit->findChild<QWidget*>(QStringLiteral("qt_scrollarea_viewport"))) {
      viewport->setAutoFillBackground(false);
    }
  }
}

void polishLineEditMenus(
  QWidget* widget, PolishedWidgetRegistry& registry, PolishedWidgetRegistry::Entry& entry, bool eventFiltersInstalled) {
  if (auto* lineEdit = qobject_cast<QLineEdit*>(widget)) {
    if (!eventFiltersInstalled) {
      auto* eventFilter = new LineEditMenuEventFilter(lineEdit);
      registry.addEventFilter(lineEdit, entry, eventFilter);
    }
  } else if (auto* spinBox = qobject_cast<QSpinBox*>(widget)) {
    if (!eventFiltersInstalled) {
      auto* eventFilter = new LineEditMenuEventFilter(spinBox);
      registry.addEventFilter(spinBox, entry, eventFilter);
    }
  } else if (auto* plainTextEdit = qobject_cast<QPlainTextEdit*>(widget)) {
    if (!eventFiltersInstalled) {
      auto* eventFilter = new LineEditMenuEventFilter(plainTextEdit);
      registry.addEventFilter(plainTextEdit, entry, eventFilter);
    }
  }
}

void polishDestructionTracking(QWidget* widget, QObject* parent, PolishedWidgetRegistry& registry,
  PolishedWidgetRegistry::Entry& entry, bool eventFiltersInstalled) {
  // Clean up when the widget is destroyed without unpolish() being called.
  // Using an event filter instead of signal to handle cases where blockSignals() is called.
  if (!eventFiltersInstalled) {
    auto* eventFilter = new DestructionEventFilter(
      widget, parent, [&registry](QWidget* destroyedWidget, QObject* destructionEventFilter) {
        registry.forgetDestroyedWidget(destroyedWidget, destructionEventFilter);
      });
    registry.addEventFilter(widget, entry, eventFilter);
  }
}

void unpolishWidget(QlementineStyle&, QWidget* widget, PolishedWidgetRegistry& registry) {
  if (const auto* entry = registry.find(widget)) {
    widget->setAttribute(Qt::WA_Hover, entry->hover);
    widget->setAttribute(Qt::WA_OpaquePaintEvent, entry->opaquePaintEvent);
    widget->setMouseTracking(entry->mouseTracking);
    widget->setFocusPolicy(entry->focusPolicy);
    widget->setMinimumSize(entry->minimumSize.expandedTo({ 0, 0 }));

    if (shouldHaveBoldFont(widget)) {
      auto font = QFont{ widget->font() };
      font.setBold(false);
      widget->setFont(font);
    }
  }

  registry.removeAndDeleteEventFilters(widget);
}
} // namespace oclero::qlementine
