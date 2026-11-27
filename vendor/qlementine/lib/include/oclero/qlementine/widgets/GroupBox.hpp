// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QGroupBox>

namespace oclero::qlementine {
class Label;

/// A QGroupBox that can also show a description under its title.
/// The description is word-wrapped, and the frame and the contents of the group box start below it.
class QLEMENTINE_EXPORT GroupBox : public QGroupBox {
  Q_OBJECT

  Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)

public:
  explicit GroupBox(QWidget* parent = nullptr);
  explicit GroupBox(const QString& title, QWidget* parent = nullptr);
  ~GroupBox() override = default;

  QString description() const;
  Q_SLOT void setDescription(const QString& description);

  /// Height of the description when it is laid out in the given width. 0 if there is no description.
  int descriptionHeight(int width) const;

Q_SIGNALS:
  void descriptionChanged();

protected:
  void changeEvent(QEvent* e) override;
  void resizeEvent(QResizeEvent* e) override;
  void showEvent(QShowEvent* e) override;

private:
  void updateContentsMargins();
  void updateDescriptionGeometry();

private:
  Label* _descriptionLabel{ nullptr };
};
} // namespace oclero::qlementine
