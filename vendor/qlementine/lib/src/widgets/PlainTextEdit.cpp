// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/PlainTextEdit.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>

namespace oclero::qlementine {
PlainTextEdit::PlainTextEdit(QWidget* parent)
  : QPlainTextEdit(parent) {}

QSize PlainTextEdit::minimumSizeHint() const {
  if (const auto* qlementineStyle = qobject_cast<QlementineStyle*>(style())) {
    const auto& theme = qlementineStyle->theme();
    const auto w = theme.controlDefaultWidth;
    const auto h = theme.controlHeightMedium * 2;
    return QSize(w, h);
  }
  return QPlainTextEdit::minimumSizeHint();
}

QSize PlainTextEdit::sizeHint() const {
  return minimumSizeHint();
}

void PlainTextEdit::setUseMonoSpaceFont(bool useMonoSpaceFont) {
  _useMonospaceFont = useMonoSpaceFont;
  ensurePolished();
  updateFont();
}

bool PlainTextEdit::useMonoSpaceFont() const {
  return _useMonospaceFont;
}

Status PlainTextEdit::status() const {
  return _status;
}

void PlainTextEdit::setStatus(Status status) {
  if (status != _status) {
    _status = status;
    update();
    Q_EMIT statusChanged();
  }
}

void PlainTextEdit::updateFont() {
  const auto* style = this->style();
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style);

  if (_useMonospaceFont) {
    if (qlementineStyle) {
      setFont(qlementineStyle->theme().fontMonospace);
    } else {
      const auto systemFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
      setFont(systemFont);
    }
  } else {
    if (qlementineStyle) {
      setFont(qlementineStyle->theme().fontRegular);
    } else {
      const auto systemFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
      setFont(systemFont);
    }
  }
}


} // namespace oclero::qlementine
