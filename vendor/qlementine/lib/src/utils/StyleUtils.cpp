// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/utils/StyleUtils.hpp>

#include <QAbstractButton>
#include <QComboBox>
#include <QMenuBar>
#include <QScrollBar>
#include <QSplitterHandle>
#include <QTabBar>
#include <QAbstractSlider>
#include <QLineEdit>
#include <QAbstractSpinBox>
#include <qabstractitemview.h>
#include <QGroupBox>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QToolButton>
#include <QPlainTextEdit>
#include <QToolButton>

#include <oclero/qlementine/widgets/ColorButton.hpp>

namespace oclero::qlementine {
bool shouldHaveHoverEvents(const QWidget* w) {
  return qobject_cast<const QAbstractButton*>(w) || qobject_cast<const QComboBox*>(w)
         || qobject_cast<const QMenuBar*>(w) || qobject_cast<const QScrollBar*>(w)
         || qobject_cast<const QSplitterHandle*>(w) || qobject_cast<const QTabBar*>(w)
         || qobject_cast<const QAbstractSlider*>(w) || qobject_cast<const QLineEdit*>(w)
         || qobject_cast<const QAbstractSpinBox*>(w) || qobject_cast<const QAbstractItemView*>(w)
         || qobject_cast<const QGroupBox*>(w) || w->inherits("QDockSeparator") || w->inherits("QDockWidgetSeparator");
}

bool shouldHaveMouseTracking(const QWidget* w) {
  return qobject_cast<const QAbstractItemView*>(w);
}

bool shouldHaveBoldFont(const QWidget* w) {
  return qobject_cast<const QPushButton*>(w) || qobject_cast<const QToolButton*>(w);
}

bool shouldHaveExternalFocusFrame(const QWidget* w) {
  // Special case for QPlainTextEdit. Need to be handled before QAbstractScrollArea.
  if (const auto* plainTextEdit = qobject_cast<const QPlainTextEdit*>(w)) {
    return plainTextEdit->focusPolicy() != Qt::NoFocus && plainTextEdit->frameShape() == QFrame::StyledPanel
           && plainTextEdit->frameShadow() != QFrame::Plain;
  }
  if (const auto* textEdit = qobject_cast<const QTextEdit*>(w)) {
    return textEdit->focusPolicy() != Qt::NoFocus && textEdit->frameShape() == QFrame::StyledPanel
           && textEdit->frameShadow() != QFrame::Plain;
  }

  // Special case for all other widgets inheriting QAbstractScrollArea.
  if (qobject_cast<const QAbstractScrollArea*>(w)) {
    return false;
  }

  if (const auto* lineEdit = qobject_cast<const QLineEdit*>(w)) {
    return lineEdit->focusPolicy() != Qt::NoFocus
           && (lineEdit->hasFrame() || qobject_cast<QComboBox*>(lineEdit->parentWidget()) != nullptr);
  }

  return (w && qobject_cast<const QAbstractButton*>(w) && !qobject_cast<const QTabBar*>(w->parentWidget()))
         || qobject_cast<const QComboBox*>(w) || qobject_cast<const QLineEdit*>(w)
         || (!qobject_cast<const QScrollBar*>(w) && qobject_cast<const QAbstractSlider*>(w))
         || qobject_cast<const QGroupBox*>(w);
}

bool shouldHaveTabFocus(const QWidget* w) {
  return w && (w->focusPolicy() == Qt::StrongFocus || w->focusPolicy() == Qt::ClickFocus)
         && (qobject_cast<const QAbstractButton*>(w) || qobject_cast<const QGroupBox*>(w))
         && !qobject_cast<const QToolButton*>(w);
}

bool shouldHaveNoFocus(const QWidget* w) {
  return qobject_cast<const QToolButton*>(w);
}

bool shouldNotBeVerticallyCompressed(const QWidget* w) {
  return qobject_cast<const QAbstractButton*>(w) || qobject_cast<const QComboBox*>(w)
         || qobject_cast<const QLineEdit*>(w) || qobject_cast<const QAbstractSpinBox*>(w);
}

std::tuple<int, int> getHPaddings(const bool hasIcon, const bool hasText, const bool hasIndicator, const int padding) {
  if (hasText) {
    if (!hasIcon && hasIndicator)
      return { padding * 2, padding };
    else if (hasIcon && !hasIndicator)
      return { padding, padding * 2 };
    else if (hasIcon && hasIndicator)
      return { padding, padding };
    else
      return { padding * 2, padding * 2 };
  }
  return { padding, padding };
}

bool shouldNotHaveWheelEvents(const QWidget* w) {
  return (!qobject_cast<const QScrollBar*>(w) && qobject_cast<const QAbstractSlider*>(w))
         || qobject_cast<const QAbstractSpinBox*>(w);
}

int getTabIndex(const QStyleOptionTab* optTab, const QWidget* parentWidget) {
  if (const auto* optTabV4 = qstyleoption_cast<const QStyleOptionTab*>(optTab)) {
    return optTabV4->tabIndex;
  }

  if (const auto* tabBar = qobject_cast<const QTabBar*>(parentWidget)) {
    return tabBar->tabAt(optTab->rect.topLeft());
  }

  return -1;
}

int getTabCount(const QWidget* parentWidget) {
  if (const auto* tabBar = qobject_cast<const QTabBar*>(parentWidget)) {
    return tabBar->count();
  }

  return -1;
}
} // namespace oclero::qlementine
