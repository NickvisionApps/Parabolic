// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QPushButton>

namespace oclero::qlementine {
class QLEMENTINE_EXPORT PillButton : public QPushButton {
  Q_OBJECT

  Q_PROPERTY(bool badgeVisible READ isBadgeVisible WRITE setBadgeVisible NOTIFY badgeVisibleChanged)

public:
  explicit PillButton(QWidget* parent = nullptr);
  explicit PillButton(const QString& text, QWidget* parent = nullptr);
  PillButton(const QIcon& icon, const QString& text, QWidget* parent = nullptr);
  ~PillButton() override = default;

  bool isBadgeVisible() const;
  Q_SLOT void setBadgeVisible(bool visible);

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

Q_SIGNALS:
  void badgeVisibleChanged();

protected:
  void paintEvent(QPaintEvent* e) override;

private:
  QFont pillFont() const;

  bool _badgeVisible{ false };
};
} // namespace oclero::qlementine
