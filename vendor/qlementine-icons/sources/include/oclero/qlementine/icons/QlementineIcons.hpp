// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <QString>

namespace oclero::qlementine::icons {
void initializeIconTheme();

QString fromFreeDesktop(const QString& freeDesktopName);
} // namespace oclero::qlementine::icons
