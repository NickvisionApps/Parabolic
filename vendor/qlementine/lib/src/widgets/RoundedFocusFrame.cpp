// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/RoundedFocusFrame.hpp>

namespace oclero::qlementine {
const RadiusesF& RoundedFocusFrame::radiuses() const {
  return _radiuses;
}

void RoundedFocusFrame::setRadiuses(const RadiusesF& radiuses) {
  if (radiuses != _radiuses) {
    _radiuses = radiuses;
    Q_EMIT radiusesChanged();
    update();
  }
}
} // namespace oclero::qlementine
