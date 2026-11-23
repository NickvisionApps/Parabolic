// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/utils/RadiusesF.hpp>

namespace oclero::qlementine {
RadiusesF::RadiusesF(int radius)
  : RadiusesF(static_cast<double>(radius)) {}

RadiusesF::RadiusesF(int left, int right)
  : RadiusesF(static_cast<double>(left), static_cast<double>(right)) {}

RadiusesF::RadiusesF(int topLeft, int topRight, int bottomRight, int bottomLeft)
  : RadiusesF(static_cast<double>(topLeft), static_cast<double>(topRight), static_cast<double>(bottomRight),
      static_cast<double>(bottomLeft)) {}

RadiusesF::RadiusesF(double radius) {
  *this = radius;
}

RadiusesF::RadiusesF(double left, double right)
  : topLeft(left)
  , topRight(right)
  , bottomRight(right)
  , bottomLeft(left) {}


RadiusesF::RadiusesF(double topLeft, double topRight, double bottomRight, double bottomLeft)
  : topLeft(topLeft)
  , topRight(topRight)
  , bottomRight(bottomRight)
  , bottomLeft(bottomLeft) {}

bool RadiusesF::hasSameRadius() const {
  return topLeft == topRight && topLeft == bottomRight && topLeft == bottomLeft;
}

bool RadiusesF::hasDifferentRadius() const {
  return !hasSameRadius();
}

QDebug operator<<(QDebug debug, const RadiusesF& radiuses) {
  QDebugStateSaver saver(debug);
  debug.nospace() << "(" << radiuses.topLeft << ", " << radiuses.topRight << ", " << radiuses.bottomRight << ", "
                  << radiuses.bottomLeft << ")";
  return debug;
}

RadiusesF& RadiusesF::operator=(const double rhs) {
  topLeft = rhs;
  topRight = rhs;
  bottomRight = rhs;
  bottomLeft = rhs;
  return *this;
}

RadiusesF RadiusesF::operator+(const double rhs) const {
  return { topLeft + rhs, topRight + rhs, bottomRight + rhs, bottomLeft + rhs };
}

RadiusesF RadiusesF::operator-(const double rhs) const {
  return { topLeft - rhs, topRight - rhs, bottomRight - rhs, bottomLeft - rhs };
}

bool RadiusesF::operator<(const double rhs) const {
  return topLeft < rhs && topRight < rhs && bottomRight < rhs && bottomLeft < rhs;
}

bool RadiusesF::operator<=(const double rhs) const {
  return *this == rhs || *this < rhs;
}

bool RadiusesF::operator>(const double rhs) const {
  return topLeft > rhs && topRight > rhs && bottomRight > rhs && bottomLeft > rhs;
}

bool RadiusesF::operator>=(const double rhs) const {
  return *this == rhs || *this > rhs;
}

bool RadiusesF::operator==(const double rhs) const {
  return topLeft == rhs && topRight == rhs && bottomRight == rhs && bottomLeft == rhs;
}

bool RadiusesF::operator!=(const double rhs) const {
  return topLeft != rhs && topRight != rhs && bottomRight != rhs && bottomLeft != rhs;
}

bool RadiusesF::operator==(const RadiusesF& rhs) const {
  return topLeft == rhs.topLeft && topRight == rhs.topRight && bottomRight == rhs.bottomRight
         && bottomLeft == rhs.bottomLeft;
}

bool RadiusesF::operator!=(const RadiusesF& rhs) const {
  return !(*this == rhs);
}
} // namespace oclero::qlementine
