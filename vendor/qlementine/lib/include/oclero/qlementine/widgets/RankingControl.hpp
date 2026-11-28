// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QWidget>

namespace oclero::qlementine {
/// A row of stars that lets the user rank something.
/// The value can be set by clicking a star, hovering shows a preview, and the arrow keys change the value.
class QLEMENTINE_EXPORT RankingControl : public QWidget {
  Q_OBJECT

  Q_PROPERTY(int maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
  Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
  Q_PROPERTY(double placeholderValue READ placeholderValue WRITE setPlaceholderValue NOTIFY placeholderValueChanged)
  Q_PROPERTY(bool halfStepsEnabled READ halfStepsEnabled WRITE setHalfStepsEnabled NOTIFY halfStepsEnabledChanged)
  Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly NOTIFY readOnlyChanged)
  Q_PROPERTY(bool clearEnabled READ clearEnabled WRITE setClearEnabled NOTIFY clearEnabledChanged)

public:
  explicit RankingControl(QWidget* parent = nullptr);
  ~RankingControl() override = default;

  /// Number of stars. Defaults to 5.
  int maximum() const;
  Q_SLOT void setMaximum(int maximum);

  /// Current value, between 0 and maximum(). 0 means no ranking.
  double value() const;
  Q_SLOT void setValue(double value);

  /// Value shown, dimmed, while there is no ranking. 0 shows nothing.
  double placeholderValue() const;
  Q_SLOT void setPlaceholderValue(double placeholderValue);

  /// Whether half stars can be selected. Defaults to false.
  bool halfStepsEnabled() const;
  Q_SLOT void setHalfStepsEnabled(bool enabled);

  /// Whether the value can only be changed from code. Defaults to false.
  bool readOnly() const;
  Q_SLOT void setReadOnly(bool readOnly);

  /// Whether clicking the current value resets it to 0. Defaults to true.
  bool clearEnabled() const;
  Q_SLOT void setClearEnabled(bool enabled);

  QSize minimumSizeHint() const override;
  QSize sizeHint() const override;

Q_SIGNALS:
  void maximumChanged();
  void valueChanged(double value);
  void placeholderValueChanged();
  void halfStepsEnabledChanged();
  void readOnlyChanged();
  void clearEnabledChanged();

protected:
  void paintEvent(QPaintEvent* evt) override;
  void mouseMoveEvent(QMouseEvent* evt) override;
  void mousePressEvent(QMouseEvent* evt) override;
  void leaveEvent(QEvent* evt) override;
  void keyPressEvent(QKeyEvent* evt) override;

private:
  double valueAtPos(const QPoint& pos) const;

  int _maximum{ 5 };
  double _value{ 0. };
  double _placeholderValue{ 0. };
  double _hoverValue{ -1. };
  bool _halfStepsEnabled{ false };
  bool _readOnly{ false };
  bool _clearEnabled{ true };
};
} // namespace oclero::qlementine
