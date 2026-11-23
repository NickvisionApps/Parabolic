// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/resources/ResourceInitialization.hpp>

#include <qglobal.h>

// This must be done outside of any namespace.
void qlementineResourceInitialization() {
  // Loads the QRC content.
  Q_INIT_RESOURCE(qlementine);
  Q_INIT_RESOURCE(qlementine_font_inter);
  Q_INIT_RESOURCE(qlementine_font_roboto);
}

namespace oclero::qlementine::resources {
void initializeResources() {
  qlementineResourceInitialization();
}
} // namespace oclero::qlementine::resources
