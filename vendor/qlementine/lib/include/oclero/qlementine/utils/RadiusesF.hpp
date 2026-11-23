// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QDebug>

namespace oclero::qlementine {
// Handles radiuses for the 4 angles (topLeft, topRight, bottomRight, bottomLeft).
struct QLEMENTINE_EXPORT RadiusesF {
  double topLeft{ 0. };
  double topRight{ 0. };
  double bottomRight{ 0. };
  double bottomLeft{ 0. };

  RadiusesF() = default;
  explicit RadiusesF(int radius);
  RadiusesF(int left, int right);
  RadiusesF(int topLeft, int topRight, int bottomRight, int bottomLeft);
  explicit RadiusesF(double radius);
  RadiusesF(double left, double right);
  RadiusesF(double topLeft, double topRight, double bottomRight, double bottomLeft);

  bool hasSameRadius() const;
  bool hasDifferentRadius() const;

  RadiusesF& operator=(const double rhs);
  RadiusesF operator+(const double rhs) const;
  RadiusesF operator-(const double rhs) const;
  bool operator<(const double rhs) const;
  bool operator<=(const double rhs) const;
  bool operator>(const double rhs) const;
  bool operator>=(const double rhs) const;
  bool operator==(const double rhs) const;
  bool operator!=(const double rhs) const;
  bool operator==(const RadiusesF& rhs) const;
  bool operator!=(const RadiusesF& rhs) const;
};

QLEMENTINE_EXPORT QDebug operator<<(QDebug debug, const RadiusesF& radiuses);

} // namespace oclero::qlementine

Q_DECLARE_METATYPE(oclero::qlementine::RadiusesF);
