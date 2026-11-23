// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QCommandLinkButton>

namespace oclero::qlementine {
class QStyleOptionCommandLinkButton;

/// A QCommandLinkButton that just doesn't force its size to Windows Vista's
/// system-default CommandLinkButtton size. It actually lets content decide.
class QLEMENTINE_EXPORT CommandLinkButton : public QCommandLinkButton {
public:
  explicit CommandLinkButton(QWidget* parent = nullptr);
  explicit CommandLinkButton(const QString& text, QWidget* parent = nullptr);
  explicit CommandLinkButton(const QString& text, const QString& description, QWidget* parent = nullptr);
  explicit CommandLinkButton(
    const QIcon& icon, const QString& text, const QString& description, QWidget* parent = nullptr);
  ~CommandLinkButton() override;

public:
  QSize sizeHint() const override;
  int heightForWidth(int) const override;
  bool hasHeightForWidth() const override;

protected:
  void paintEvent(QPaintEvent*) override;
  using QCommandLinkButton::initStyleOption;

  virtual void initStyleOption(QStyleOptionCommandLinkButton* option) const;

private:
  void updateIconSize();
};
} // namespace oclero::qlementine
