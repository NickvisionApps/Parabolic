// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/style/QlementineStyle.hpp>

#include <QEvent>
#include <QObject>
#include <QMenu>
#include <QPointer>
#include <QAbstractSpinBox>
#include <QLineEdit>
#include <QPlainTextEdit>

namespace oclero::qlementine {
// Helper to modify the icons.
class LineEditMenuIconsBehavior : public QObject {
  QPointer<QMenu> _menu{ nullptr };
  bool _menuCustomized{ false };

  enum class IconListMode {
    None,
    LineEdit, // Use of QLineEdit and QPlainTextEdit.
    ReadOnlyLineEdit,
    SpinBox,
  };

  static std::vector<QIcon> iconList(IconListMode mode) {
    const auto* qlem = oclero::qlementine::appStyle();
    if (!qlem)
      return {};

    // The order follows the one defined QLineEdit.cpp and QSpinBox.cpp (Qt6).
    switch (mode) {
      case IconListMode::LineEdit:
        return {
          QIcon(), // Separator
          qlem->makeThemedIconFromName("edit-undo"),
          qlem->makeThemedIconFromName("edit-redo"),
          QIcon(), // Separator
          qlem->makeThemedIconFromName("edit-cut"),
          qlem->makeThemedIconFromName("edit-copy"),
          qlem->makeThemedIconFromName("edit-paste"),
          qlem->makeThemedIconFromName("edit-delete"),
          QIcon(), // Separator
          qlem->makeThemedIconFromName("edit-select-all"),
        };
      case IconListMode::ReadOnlyLineEdit:
        return {
          QIcon(), // Separator
          qlem->makeThemedIconFromName("edit-copy"),
          QIcon(), // Separator
          qlem->makeThemedIconFromName("edit-select-all"),
        };
      case IconListMode::SpinBox:
        return {
          QIcon(), // Separator
          qlem->makeThemedIconFromName("edit-undo"),
          qlem->makeThemedIconFromName("edit-redo"),
          QIcon(), // Separator
          qlem->makeThemedIconFromName("edit-cut"),
          qlem->makeThemedIconFromName("edit-copy"),
          qlem->makeThemedIconFromName("edit-paste"),
          qlem->makeThemedIconFromName("edit-delete"),
          QIcon(), // Separator
          QIcon(), // Separator
          qlem->makeThemedIconFromName("edit-select-all"),
          QIcon(), // Separator
          qlem->makeThemedIconFromName("go-up"),
          qlem->makeThemedIconFromName("go-down"),
        };
      default:
        return {};
    }
  }

  static IconListMode getMode(const QMenu* menu) {
    if (const auto* menuParent = menu->parent()) {
      if (qobject_cast<const QAbstractSpinBox*>(menuParent->parent())) {
        return IconListMode::SpinBox;
      } else if (const auto* line_edit = qobject_cast<const QLineEdit*>(menuParent)) {
        return line_edit->isReadOnly() ? IconListMode::ReadOnlyLineEdit : IconListMode::LineEdit;
      } else if (const auto* menuParentParent = menuParent ? menuParent->parent() : nullptr) {
        if (const auto* plainTextEdit = qobject_cast<const QPlainTextEdit*>(menuParentParent)) {
          return plainTextEdit->isReadOnly() ? IconListMode::ReadOnlyLineEdit : IconListMode::LineEdit;
        }
      }
    }
    return IconListMode::None;
  }

  void customizeMenu() {
    const auto actions = _menu->findChildren<QAction*>();
    if (!actions.empty()) {
      const auto icons = iconList(getMode(_menu));
      if (!icons.empty()) {
        for (auto i = 0; i < static_cast<int>(icons.size()) && i < static_cast<int>(actions.size()); ++i) {
          if (auto* action = actions.at(i)) {
            action->setIcon(icons.at(i));
          }
        }
      }
    }
    _menu->adjustSize();
  }

public:
  explicit LineEditMenuIconsBehavior(QMenu* menu)
    : QObject(menu)
    , _menu(menu) {
    // Hack pour modifier les icones du menu contextuel des line edit.
    QObject::connect(_menu, &QMenu::aboutToShow, this, [this]() {
      if (!_menuCustomized) {
        customizeMenu();
        _menuCustomized = true;
      }
    });
  }

  ~LineEditMenuIconsBehavior() override = default;
};

class LineEditMenuEventFilter : public QObject {
public:
  explicit LineEditMenuEventFilter(QWidget* parent)
    : QObject(parent) {
    assert(parent);
    // Might be a menu's submenu.
    if (auto* menu = qobject_cast<QMenu*>(parent)) {
      new LineEditMenuIconsBehavior(menu);
    } else {
      // The QLineEdit.
      parent->installEventFilter(this);
    }
  }

  ~LineEditMenuEventFilter() override = default;

protected:
  bool eventFilter(QObject*, QEvent* evt) override {
    if (evt->type() == QEvent::ChildPolished) {
      constexpr auto propertyName = "qlementine_tweak_menu_icons";
      auto* child = static_cast<QChildEvent*>(evt)->child();

      const auto tweaked = child->property(propertyName).toBool();
      if (!tweaked) {
        child->setProperty(propertyName, true);

        // QLineEdit child of QSpinBox.
        if (auto* lineEdit = qobject_cast<QLineEdit*>(child)) {
          lineEdit->installEventFilter(this);

        }
        // Qmenu that needs tweaking.
        else if (auto* menu = qobject_cast<QMenu*>(child)) {
          new LineEditMenuIconsBehavior(menu);

          // Forward auto icon color mode from parent to the menu.
          if (const auto* menuParent = menu->parentWidget()) {
            if (const auto* style = qobject_cast<oclero::qlementine::QlementineStyle*>(menuParent->style())) {
              const auto autoIconColor = style->autoIconColor(menuParent);
              QlementineStyle::setAutoIconColor(menu, autoIconColor);
            }
          }
        }
        // Case of a QPlainTextEdit (inherits QAbstractScrollArea).
        else if (child->objectName() == "qt_scrollarea_viewport") {
          if (auto* childWidget = qobject_cast<QWidget*>(child)) {
            childWidget->installEventFilter(this);
          }
        }
      }
    }

    return false;
  }
};
} // namespace oclero::qlementine
