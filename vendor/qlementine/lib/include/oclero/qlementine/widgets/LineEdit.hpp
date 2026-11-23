// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>
#include <oclero/qlementine/style/Theme.hpp>
#include <oclero/qlementine/Common.hpp>

#include <QLabel>
#include <QLineEdit>
#include <QIcon>

namespace oclero::qlementine {
/// A QLineEdit that draws a search icon
class QLEMENTINE_EXPORT LineEdit : public QLineEdit {
  Q_OBJECT

  Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
  Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)

public:
  explicit LineEdit(QWidget* parent = nullptr);
  ~LineEdit() override = default;

  const QIcon& icon() const;
  Q_SLOT void setIcon(const QIcon& icon);

  void setUseMonoSpaceFont(bool useMonoSpaceFont);
  bool useMonoSpaceFont() const;

  Status status() const;
  Q_SLOT void setStatus(Status status);

Q_SIGNALS:
  void iconChanged();
  void statusChanged();

protected:
  void paintEvent(QPaintEvent* evt) override;
  bool event(QEvent* e) override;

private:
  QPixmap getPixmap() const;
  void updateFont();

  QIcon _icon;
  bool _useMonospaceFont{ false };
  Status _status{ Status::Default };
};
} // namespace oclero::qlementine
