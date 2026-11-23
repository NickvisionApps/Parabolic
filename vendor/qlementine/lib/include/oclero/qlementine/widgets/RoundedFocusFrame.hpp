// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>
#include <oclero/qlementine/utils/RadiusesF.hpp>

#include <QFocusFrame>

namespace oclero::qlementine {
/// A rounded QFocusFrame.
class QLEMENTINE_EXPORT RoundedFocusFrame : public QFocusFrame {
  Q_OBJECT

  Q_PROPERTY(RadiusesF radiuses READ radiuses WRITE setRadiuses NOTIFY radiusesChanged)

public:
  using QFocusFrame::QFocusFrame;
  ~RoundedFocusFrame() override = default;

  const RadiusesF& radiuses() const;
  Q_SLOT void setRadiuses(const RadiusesF&);

Q_SIGNALS:
  void radiusesChanged();

private:
  RadiusesF _radiuses;
};
} // namespace oclero::qlementine
