// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/Label.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/QlementineStyleOption.hpp>
#include <QApplication>

namespace oclero::qlementine {
Label::Label(QWidget* parent)
  : QLabel(parent) {
  updatePaletteFromTheme();
  qApp->installEventFilter(this);
}

Label::Label(const QString& text, QWidget* parent)
  : QLabel(text, parent) {
  updatePaletteFromTheme();
  qApp->installEventFilter(this);
}

Label::Label(const QString& text, TextRole role, QWidget* parent)
  : QLabel(text, parent)
  , _role(role) {
  updatePaletteFromTheme();
  qApp->installEventFilter(this);
}

Label::~Label() = default;

TextRole Label::role() const {
  return _role;
}

void Label::setRole(TextRole role) {
  if (role != _role) {
    _role = role;
    // Change text font, size and color.
    updatePaletteFromTheme();
    Q_EMIT roleChanged();
  }
}

bool Label::event(QEvent* e) {
  // Ensure the label repaints when the theme changes.
  if (e->type() == QEvent::Type::PaletteChange && !_changingPaletteFlag) {
    _changingPaletteFlag = true;
    updatePaletteFromTheme();
    _changingPaletteFlag = false;
  }
  return QLabel::event(e);
}

bool Label::eventFilter(QObject* obj, QEvent* e) {
  if (e->type() == QEvent::Type::PaletteChange) {
    updatePaletteFromTheme();
  }
  return QLabel::eventFilter(obj, e);
}

void Label::updatePaletteFromTheme() {
  if (const auto* qlementineStyle = qobject_cast<QlementineStyle*>(style())) {
    const auto& font = qlementineStyle->fontForTextRole(_role);
    const auto palette = qlementineStyle->paletteForTextRole(_role);
    setFont(font);
    setPalette(palette);
    updateGeometry();
    update(contentsRect());
  }
}
} // namespace oclero::qlementine
