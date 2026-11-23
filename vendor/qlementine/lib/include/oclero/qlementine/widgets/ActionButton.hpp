// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QPushButton>

namespace oclero::qlementine {

class QLEMENTINE_EXPORT ActionButton : public QPushButton {
  Q_OBJECT

public:
  explicit ActionButton(QWidget* parent = nullptr);
  ~ActionButton() override;

public:
  void setAction(QAction* action);
  void updateFromAction();

private:
  QAction* _action{ nullptr };
};
} // namespace oclero::qlementine
