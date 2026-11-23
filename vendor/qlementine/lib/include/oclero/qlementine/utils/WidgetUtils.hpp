// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QWidget>
#include <QScreen>
#include <QGuiApplication>

namespace oclero::qlementine {
QLEMENTINE_EXPORT QWidget* makeVerticalLine(QWidget* parentWidget, int maxHeight = -1);
QLEMENTINE_EXPORT QWidget* makeHorizontalLine(QWidget* parentWidget, int maxWidth = -1);

QLEMENTINE_EXPORT void centerWidget(QWidget* widget, QWidget* host = nullptr);

QLEMENTINE_EXPORT qreal getDpi(const QWidget* widget);

QLEMENTINE_EXPORT QWindow* getWindow(const QWidget* widget);

QLEMENTINE_EXPORT void clearFocus(QWidget* widget, bool recursive);

template<class T>
T* findFirstParentOfType(QWidget* child) {
  auto* parent = child;

  while (parent != nullptr) {
    parent = parent->parentWidget();
    if (auto* typedPArent = qobject_cast<T*>(parent)) {
      return typedPArent;
    }
  }

  return nullptr;
}
} // namespace oclero::qlementine
