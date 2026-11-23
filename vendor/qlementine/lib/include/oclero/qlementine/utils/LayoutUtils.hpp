// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QMargins>
#include <QLayout>

#include <tuple>

class QWidget;

namespace oclero::qlementine {
/// Retrieves the widget's QStyle margins.
QLEMENTINE_EXPORT QMargins getLayoutMargins(const QWidget* widget);

/// Retrieves the widget's QStyle horizontal spacing.
QLEMENTINE_EXPORT int getLayoutHSpacing(const QWidget* widget);

/// Retrieves the widget's QStyle vertical spacing.
QLEMENTINE_EXPORT int getLayoutVSpacing(const QWidget* widget);

/// Retrieves the widget's QStyle horizontal spacing and margins.
QLEMENTINE_EXPORT std::tuple<int, QMargins> getHLayoutProps(const QWidget* widget);

/// Retrieves the widget's QStyle vertical spacing and margins.
QLEMENTINE_EXPORT std::tuple<int, QMargins> getVLayoutProps(const QWidget* widget);

/// Retrieves the widget's QStyle vertical/horizontal spacings and margins.
QLEMENTINE_EXPORT std::tuple<int, int, QMargins> getFormLayoutProps(const QWidget* widget);

/// Remove and deletes all the elements in the layout.
QLEMENTINE_EXPORT void clearLayout(QLayout* layout);
} // namespace oclero::qlementine
