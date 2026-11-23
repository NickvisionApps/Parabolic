// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QWidget>

namespace oclero::qlementine {
/// A row of dots that shows the current page among a number of pages, such as in a carousel.
/// Clicking a dot selects its page.
class QLEMENTINE_EXPORT PageDots : public QWidget {
  Q_OBJECT

  Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
  Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

public:
  explicit PageDots(QWidget* parent = nullptr);
  ~PageDots() override = default;

  int count() const;
  Q_SLOT void setCount(int count);

  int currentIndex() const;
  Q_SLOT void setCurrentIndex(int index);

  QSize minimumSizeHint() const override;
  QSize sizeHint() const override;

Q_SIGNALS:
  void countChanged();
  void currentIndexChanged();

protected:
  void paintEvent(QPaintEvent* evt) override;
  void mousePressEvent(QMouseEvent* evt) override;

private:
  int indexAtPos(const QPoint& pos) const;

  int _count{ 0 };
  int _currentIndex{ 0 };
};
} // namespace oclero::qlementine
