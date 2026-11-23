// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include <QWidget>

#include <oclero/qlementine/qlementine_export.h>
#include <oclero/qlementine/style/Theme.hpp>

namespace oclero::qlementine {
class QLEMENTINE_EXPORT ThemeEditor : public QWidget {
  Q_OBJECT

public:
  explicit ThemeEditor(QWidget* parent = nullptr);
  ~ThemeEditor() override;

public:
  const Theme& theme() const;
  void setTheme(const Theme& theme);

Q_SIGNALS:
  void themeChanged(const oclero::qlementine::Theme& theme);

private:
  struct Impl;
  std::unique_ptr<Impl> _impl;
};
} // namespace oclero::qlementine
