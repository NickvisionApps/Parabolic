// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QWidget>
#include <QVariantAnimation>

namespace oclero::qlementine {
/// A modern progress ring, either indeterminate (animated) or determinate (showing a value).
class QLEMENTINE_EXPORT LoadingRing : public QWidget {
  Q_OBJECT

  Q_PROPERTY(bool indeterminate READ indeterminate WRITE setIndeterminate NOTIFY indeterminateChanged)
  Q_PROPERTY(int minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
  Q_PROPERTY(int maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
  Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)

public:
  explicit LoadingRing(QWidget* parent = nullptr);
  ~LoadingRing() override = default;

  bool indeterminate() const;
  Q_SLOT void setIndeterminate(bool);

  int minimum() const;
  Q_SLOT void setMinimum(int);

  int maximum() const;
  Q_SLOT void setMaximum(int);

  int value() const;
  Q_SLOT void setValue(int);

  QSize minimumSizeHint() const override;
  QSize sizeHint() const override;

Q_SIGNALS:
  void indeterminateChanged();
  void minimumChanged();
  void maximumChanged();
  void valueChanged();

protected:
  void paintEvent(QPaintEvent* evt) override;
  void showEvent(QShowEvent* evt) override;
  void hideEvent(QHideEvent* evt) override;

private:
  void updateAnimation();

private:
  QVariantAnimation _animation;
  bool _indeterminate{ true };
  int _minimum{ 0 };
  int _maximum{ 100 };
  int _value{ 0 };
};
} // namespace oclero::qlementine
