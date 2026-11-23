// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QPushButton>

namespace oclero::qlementine {
class Popover;

/// A Button that looks like a ComboBox but opens a Popover.
class QLEMENTINE_EXPORT PopoverButton : public QPushButton {
  Q_OBJECT

public:
  explicit PopoverButton(QWidget* parent = nullptr);
  PopoverButton(const QString& text, QWidget* parent = nullptr);
  PopoverButton(const QString& text, const QIcon& icon, QWidget* parent = nullptr);
  ~PopoverButton() override = default;

public:
  QWidget* popoverContentWidget() const;
  void setPopoverContentWidget(QWidget* widget);
  Q_SIGNAL void popoverContentWidgetChanged();

  Popover* popover() const;

public Q_SLOTS:
  void setPopoverOpened(bool opened);

Q_SIGNALS:
  void popoverOpenedChanged(bool opened);

protected:
  void paintEvent(QPaintEvent* e) override;

private:
  Popover* _popover{ nullptr };
};
} // namespace oclero::qlementine
