// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>
#include <oclero/qlementine/Common.hpp>

#include <QPlainTextEdit>

namespace oclero::qlementine {
/// An improved QPlainTextEdit.
class QLEMENTINE_EXPORT PlainTextEdit : public QPlainTextEdit {
  Q_OBJECT

  Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)

public:
  explicit PlainTextEdit(QWidget* parent = nullptr);
  ~PlainTextEdit() override = default;

  QSize minimumSizeHint() const override;
  QSize sizeHint() const override;

  void setUseMonoSpaceFont(bool useMonoSpaceFont);
  bool useMonoSpaceFont() const;

  Status status() const;
  Q_SLOT void setStatus(Status status);

Q_SIGNALS:
  void statusChanged();

private:
  void updateFont();

  bool _useMonospaceFont{ false };
  Status _status{ Status::Default };
};
} // namespace oclero::qlementine
