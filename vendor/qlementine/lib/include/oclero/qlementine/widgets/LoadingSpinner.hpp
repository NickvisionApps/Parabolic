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
/// An animated loading spinner.
class QLEMENTINE_EXPORT LoadingSpinner : public QWidget {
  Q_OBJECT

  Q_PROPERTY(bool spinning READ spinning WRITE setSpinning NOTIFY spinningChanged)

public:
  explicit LoadingSpinner(QWidget* parent = nullptr);
  ~LoadingSpinner() override = default;

  bool spinning() const;
  Q_SLOT void setSpinning(bool);

  QSize minimumSizeHint() const override;
  QSize sizeHint() const override;

Q_SIGNALS:
  void spinningChanged();

protected:
  void paintEvent(QPaintEvent* evt) override;
  void timerEvent(QTimerEvent* evt) override;
  void showEvent(QShowEvent* evt) override;
  void hideEvent(QHideEvent* evt) override;

private:
  int _timerId{ -1 };
  bool _spinning{ false };
  int _i{ 0 };
};
} // namespace oclero::qlementine
