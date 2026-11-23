// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <QWidget>
#include <QColor>

#include <oclero/qlementine/qlementine_export.h>
#include <oclero/qlementine/Common.hpp>

namespace oclero::qlementine {
class ColorButton;
class LineEdit;

class QLEMENTINE_EXPORT ColorEditor : public QWidget {
  Q_OBJECT

  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(ColorMode colorMode READ colorMode WRITE setColorMode NOTIFY colorModeChanged)

public:
  explicit ColorEditor(QWidget* parent = nullptr);
  explicit ColorEditor(const QColor& color, QWidget* parent = nullptr);
  ~ColorEditor() override = default;

  const QColor& color() const;
  void setColor(const QColor& color);

  ColorMode colorMode() const;
  void setColorMode(ColorMode mode);

Q_SIGNALS:
  void colorChanged();
  void colorModeChanged();

private:
  void setup(const QColor& initialColor);
  void syncLineEditFromButton();

  ColorButton* _colorButton{};
  LineEdit* _lineEdit{};
};
} // namespace oclero::qlementine
