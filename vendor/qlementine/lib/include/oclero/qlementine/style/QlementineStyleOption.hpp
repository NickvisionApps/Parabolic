// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/utils/RadiusesF.hpp>

#include <QStyleOption>
#include <QtTypeTraits>

namespace oclero::qlementine {
enum class StyleOptionTypeExt {
  SO_RoundedButton = QStyleOption::SO_CustomBase + 1,
  SO_FocusRoundedRect = QStyleOption::SO_CustomBase + 2,
  SO_CommandLinkButton = QStyleOption::SO_CustomBase + 3,
};

/// Allows to customize the radius of the focus border.
class QStyleOptionFocusRoundedRect : public QStyleOptionFocusRect {
public:
  enum StyleOptionType { Type = qToUnderlying(StyleOptionTypeExt::SO_FocusRoundedRect) };

  RadiusesF radiuses;
  int hMargin{ 0 };
  int vMargin{ 0 };
  QColor borderColor;

  QStyleOptionFocusRoundedRect() {
    type = Type;
  }

  ~QStyleOptionFocusRoundedRect() = default;

  static QStyleOptionFocusRoundedRect fromBase(QStyleOption const& opt, QRect const& rect, RadiusesF const& radiuses) {
    QStyleOptionFocusRoundedRect newOpt;
    newOpt.QStyleOption::operator=(opt);
    newOpt.radiuses = radiuses;
    newOpt.rect = rect;
    return newOpt;
  }

  QStyleOptionFocusRoundedRect(const QStyleOptionFocusRoundedRect& other)
    : QStyleOptionFocusRoundedRect() {
    *this = other;
  }

  QStyleOptionFocusRoundedRect& operator=(const QStyleOptionFocusRoundedRect&) = default;
};

/// Allows to customize the radius of a button.
class QStyleOptionRoundedButton : public QStyleOptionButton {
public:
  enum StyleOptionType { Type = qToUnderlying(StyleOptionTypeExt::SO_RoundedButton) };

  RadiusesF radiuses{ 0. };

  QStyleOptionRoundedButton() {
    type = Type;
  }

  ~QStyleOptionRoundedButton() = default;

  QStyleOptionRoundedButton(const QStyleOptionRoundedButton& other)
    : QStyleOptionButton(other)
    , radiuses(other.radiuses) {
    type = Type;
  }

  QStyleOptionRoundedButton& operator=(const QStyleOptionRoundedButton&) = default;
};

/// Adds the ability to have a second line of text in the button.
class QStyleOptionCommandLinkButton : public QStyleOptionButton {
public:
  enum StyleOptionType { Type = qToUnderlying(StyleOptionTypeExt::SO_CommandLinkButton) };

  QString description;

  QStyleOptionCommandLinkButton() {
    type = Type;
  }

  ~QStyleOptionCommandLinkButton() = default;

  explicit QStyleOptionCommandLinkButton(const QStyleOptionButton& other)
    : QStyleOptionButton(other) {
    type = Type;
  }

  QStyleOptionCommandLinkButton(const QStyleOptionCommandLinkButton& other)
    : QStyleOptionCommandLinkButton() {
    *this = other;
  }

  QStyleOptionCommandLinkButton& operator=(const QStyleOptionCommandLinkButton&) = default;
};
} // namespace oclero::qlementine
