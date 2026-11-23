// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <functional>

#include <oclero/qlementine/qlementine_export.h>

class QMenu;
class QAction;

namespace oclero::qlementine {
QLEMENTINE_EXPORT QMenu* getTopLevelMenu(QMenu* menu);

QLEMENTINE_EXPORT void flashAction(QAction* action, QMenu* menu, const std::function<void()>& onAnimationFinished);
} // namespace oclero::qlementine
