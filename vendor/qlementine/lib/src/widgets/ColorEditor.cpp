// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/ColorEditor.hpp>

#include <oclero/qlementine/widgets/LineEdit.hpp>
#include <oclero/qlementine/widgets/ColorButton.hpp>
#include <oclero/qlementine/utils/ColorUtils.hpp>

#include <QHBoxLayout>

namespace oclero::qlementine {

ColorEditor::ColorEditor(QWidget* parent)
  : QWidget(parent) {
  setup(Qt::black);
}

ColorEditor::ColorEditor(const QColor& color, QWidget* parent)
  : QWidget(parent) {
  setup(color);
}

void ColorEditor::setup(const QColor& initialColor) {
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  auto* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  // Button to see and choose a RGBA color.
  _colorButton = new ColorButton(this);
  _colorButton->setColor(initialColor);
  layout->addWidget(_colorButton);

  // LineEdit with an hex representation of the RGBA color.
  _lineEdit = new LineEdit(this);
  _lineEdit->setPlaceholderText(QStringLiteral("#RRGGBBAA"));
  _lineEdit->setMaxLength(9);
  _lineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  _lineEdit->ensurePolished();
  _lineEdit->setFixedWidth(_lineEdit->minimumSizeHint().width());
  syncLineEditFromButton();
  layout->addWidget(_lineEdit);

  // Connect widgets together.
  QObject::connect(_colorButton, &ColorButton::colorChanged, this, [this]() {
    syncLineEditFromButton();
    Q_EMIT colorChanged();
  });

  QObject::connect(_colorButton, &ColorButton::colorModeChanged, this, [this]() {
    syncLineEditFromButton();
    Q_EMIT colorModeChanged();
  });


  QObject::connect(_lineEdit, &QLineEdit::editingFinished, this, [this]() {
    const auto colorOptional = qlementine::tryGetColorFromHexaString(_lineEdit->text());
    const auto validString = colorOptional.has_value();
    _lineEdit->setStatus(validString ? qlementine::Status::Default : qlementine::Status::Error);
    if (validString) {
      QSignalBlocker blocker(_colorButton);
      _colorButton->setColor(colorOptional.value());

      // Ensure the lineEdit's text is correctly formatted (lowercase, etc.).
      syncLineEditFromButton();
    }
  });
}

const QColor& ColorEditor::color() const {
  return _colorButton->color();
}

void ColorEditor::setColor(const QColor& color) {
  _colorButton->setColor(color);
  // Sync even if the color hasn't changed, to clear any previous wrong string.
  syncLineEditFromButton();
}


ColorMode ColorEditor::colorMode() const {
  return _colorButton->colorMode();
}

void ColorEditor::setColorMode(ColorMode mode) {
  _colorButton->setColorMode(mode);
}

void ColorEditor::syncLineEditFromButton() {
  QSignalBlocker blocker(_lineEdit);
  switch (_colorButton->colorMode()) {
    case ColorMode::RGB:
      _lineEdit->setText(qlementine::toHexRGB(_colorButton->color()));
      break;
    case ColorMode::RGBA:
      _lineEdit->setText(qlementine::toHexRGBA(_colorButton->color()));
      break;
    default:
      break;
  }
}
} // namespace oclero::qlementine
