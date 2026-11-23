#include <oclero/qlementine/utils/LayoutUtils.hpp>

#include <QWidget>
#include <QStyle>

namespace oclero {
namespace qlementine {
QMargins getLayoutMargins(const QWidget* widget) {
  if (const auto* style = widget ? widget->style() : nullptr) {
    const auto left = style->pixelMetric(QStyle::PM_LayoutLeftMargin);
    const auto top = style->pixelMetric(QStyle::PM_LayoutTopMargin);
    const auto right = style->pixelMetric(QStyle::PM_LayoutRightMargin);
    const auto bottom = style->pixelMetric(QStyle::PM_LayoutBottomMargin);
    return { left, top, right, bottom };
  }
  return { 0, 0, 0, 0 };
}

int getLayoutHSpacing(const QWidget* widget) {
  if (const auto* style = widget ? widget->style() : nullptr) {
    return style->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
  }
  return 0;
}

int getLayoutVSpacing(const QWidget* widget) {
  if (const auto* style = widget ? widget->style() : nullptr) {
    return style->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
  }
  return 0;
}

std::tuple<int, QMargins> getVLayoutProps(const QWidget* widget) {
  return std::tuple{ getLayoutVSpacing(widget), getLayoutMargins(widget) };
}

std::tuple<int, QMargins> getHLayoutProps(const QWidget* widget) {
  return std::tuple{ getLayoutHSpacing(widget), getLayoutMargins(widget) };
}

std::tuple<int, int, QMargins> getFormLayoutProps(const QWidget* widget) {
  return std::tuple{ getLayoutVSpacing(widget), getLayoutHSpacing(widget), getLayoutMargins(widget) };
}

void clearLayout(QLayout* layout) {
  while (auto* item = layout->takeAt(0)) {
    if (auto* widget = item->widget()) {
      delete widget;
    } else if (auto* item_layout = item->layout()) {
      clearLayout(item_layout);
    }
    delete item;
  }
}
} // namespace qlementine
} // namespace oclero
