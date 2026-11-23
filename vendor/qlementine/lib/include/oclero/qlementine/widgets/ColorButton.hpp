// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <QAbstractButton>
#include <QColor>

#include <oclero/qlementine/qlementine_export.h>
#include <oclero/qlementine/Common.hpp>

namespace oclero::qlementine {
class QLEMENTINE_EXPORT ColorButton : public QAbstractButton {
  Q_OBJECT

  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(ColorMode colorMode READ colorMode WRITE setColorMode NOTIFY colorModeChanged)

public:
  explicit ColorButton(QWidget* parent = nullptr);
  explicit ColorButton(const QColor& color, QWidget* parent = nullptr);
  explicit ColorButton(const QColor& color, ColorMode mode, QWidget* parent = nullptr);
  ~ColorButton() override = default;

  const QColor& color() const;
  void setColor(const QColor& color);

  ColorMode colorMode() const;
  void setColorMode(ColorMode mode);

  QSize sizeHint() const override;

Q_SIGNALS:
  void colorChanged();
  void colorModeChanged();

protected:
  void paintEvent(QPaintEvent*) override;

private:
  void setup();
  QColor adaptColorToMode(const QColor& color) const;

  QColor _color;
  ColorMode _colorMode{ ColorMode::RGBA };
};
} // namespace oclero::qlementine
