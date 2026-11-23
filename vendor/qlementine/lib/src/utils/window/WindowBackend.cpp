// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include "WindowBackend.hpp"

#include <oclero/qlementine/widgets/UnifiedWindow.hpp>

#include <QWindow>

namespace oclero::qlementine {
void WindowBackend::startMove() {
  if (auto* windowHandle = _window.windowHandle()) {
    windowHandle->startSystemMove();
  }
}

void WindowBackend::handleTitleBarDoubleClick() {
  if (_window.isMaximized()) {
    _window.showNormal();
  } else {
    _window.showMaximized();
  }
}
} // namespace oclero::qlementine
