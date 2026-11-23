// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>

#include <QLayout>
#include <QList>
#include <QStyle>

namespace oclero::qlementine {
/// A layout that arranges its items from left to right and wraps them to the next row when the width is exceeded.
/// Its height depends on its width.
class QLEMENTINE_EXPORT FlowLayout : public QLayout {
public:
  explicit FlowLayout(QWidget* parent = nullptr);
  ~FlowLayout() override;

  /// Spacing between the items of a same row. A negative value uses the spacing of the style.
  int horizontalSpacing() const;
  void setHorizontalSpacing(int spacing);

  /// Spacing between the rows. A negative value uses the spacing of the style.
  int verticalSpacing() const;
  void setVerticalSpacing(int spacing);

  void addItem(QLayoutItem* item) override;
  int count() const override;
  QLayoutItem* itemAt(int index) const override;
  QLayoutItem* takeAt(int index) override;
  Qt::Orientations expandingDirections() const override;
  bool hasHeightForWidth() const override;
  int heightForWidth(int width) const override;
  void setGeometry(const QRect& rect) override;
  QSize minimumSize() const override;
  QSize sizeHint() const override;

private:
  int doLayout(const QRect& rect, bool testOnly) const;
  int smartSpacing(QStyle::PixelMetric pixelMetric) const;

  QList<QLayoutItem*> _items;
  int _horizontalSpacing{ -1 };
  int _verticalSpacing{ -1 };
};
} // namespace oclero::qlementine
