// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>
#include <oclero/qlementine/widgets/AbstractItemListWidget.hpp>

namespace oclero::qlementine {
/// A TabBar with tabs like on Android.
class QLEMENTINE_EXPORT NavigationBar : public AbstractItemListWidget {
public:
  using AbstractItemListWidget::AbstractItemListWidget;
  ~NavigationBar() override = default;

protected: // Inherited via AbstractItemListWidget
  const QColor& getBgColor(const Theme& theme) const override;
  const QColor& getItemBgColor(MouseState mouse, const Theme& theme) const override;
  const QColor& getItemFgColor(MouseState mouse, bool selected, const Theme& theme) const override;
  const QColor& getItemBadgeBgColor(MouseState mouse, bool selected, const Theme& theme) const override;
  const QColor& getItemBadgeFgColor(MouseState mouse, bool selected, const Theme& theme) const override;
  QMargins getItemPadding() const override;
  double getItemRadius() const override;
  QMargins getPadding() const override;
  int getSpacing() const override;
  int getItemMinimumHeight() const override;
  void drawCurrentItemIndicator(QPainter& p) const override;
  QFont labelFont() const override;
};
} // namespace oclero::qlementine
