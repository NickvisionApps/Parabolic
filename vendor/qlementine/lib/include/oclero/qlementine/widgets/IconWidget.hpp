// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QWidget>
#include <QIcon>

namespace oclero::qlementine {
/// A QWidget that displays a QIcon and paints the correct image according to its state.
class QLEMENTINE_EXPORT IconWidget : public QWidget {
  Q_OBJECT

  Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
  Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize NOTIFY iconSizeChanged)

public:
  explicit IconWidget(QWidget* parent = nullptr);
  IconWidget(const QIcon& icon, QWidget* parent = nullptr);
  IconWidget(const QIcon& icon, const QSize& size, QWidget* parent = nullptr);
  ~IconWidget() override = default;

  const QIcon& icon() const;
  Q_SLOT void setIcon(const QIcon& icon);

  const QSize& iconSize() const;
  Q_SLOT void setIconSize(const QSize& iconSize);

  QSize sizeHint() const override;

Q_SIGNALS:
  void iconChanged();
  void iconSizeChanged();

protected:
  void paintEvent(QPaintEvent* e) override;
  //void changeEvent(QEvent* e) override;

private:
  QSize _iconSize;
  QIcon _icon;
};
} // namespace oclero::qlementine
