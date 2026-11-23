// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <functional>

#include <oclero/qlementine/qlementine_export.h>

#include <QMenu>
#include <QAction>

namespace oclero::qlementine {
class QLEMENTINE_EXPORT Menu : public QMenu {
  Q_OBJECT

public:
  using QMenu::QMenu;
  ~Menu() override = default;

  void setEnabledPredicate(const std::function<bool()>& cb);
  void setVisiblePredicate(const std::function<bool()>& cb);

  void updateEnabled();
  void updateVisible();

  /// Updates visible and enabled properties by calling corresponding predicates.
  void updateProps();

private:
  std::function<bool()> _updateEnabledCb;
  std::function<bool()> _updateVisibleCb;
};
} // namespace oclero::qlementine
