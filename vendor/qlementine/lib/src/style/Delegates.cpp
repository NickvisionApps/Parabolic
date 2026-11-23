// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/style/Delegates.hpp>

#include <oclero/qlementine/utils/StateUtils.hpp>
#include <oclero/qlementine/utils/ImageUtils.hpp>
#include <oclero/qlementine/utils/PrimitiveUtils.hpp>
#include <oclero/qlementine/utils/FontUtils.hpp>
#include <oclero/qlementine/utils/ColorUtils.hpp>

#include <QPainter>
#include <QPainterStateGuard>
#include <QTreeView>

namespace oclero::qlementine {
ComboBoxDelegate::ComboBoxDelegate(QWidget* widget, QlementineStyle& style)
  : QItemDelegate(widget)
  , _widget(widget)
  , _qlementineStyle(&style) {}

void ComboBoxDelegate::paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& idx) const {
  if (!_qlementineStyle) {
    QItemDelegate::paint(p, opt, idx);
    return;
  }

  QPainterStateGuard stateGuard(p);
  p->setRenderHint(QPainter::Antialiasing, true);
  p->setRenderHint(QPainter::SmoothPixmapTransform, true);
  const auto& theme = _qlementineStyle ? _qlementineStyle->theme() : Theme{};

  const auto viewIsTreeView = qobject_cast<const QTreeView*>(opt.widget);
  const auto isSeparator = idx.data(Qt::AccessibleDescriptionRole).toString() == QLatin1String("separator");
  const auto contentMargin = _qlementineStyle->pixelMetric(QStyle::PM_MenuHMargin);
  const auto contentRect = opt.rect.marginsRemoved({ contentMargin, 0, contentMargin, 0 });

  if (isSeparator) {
    const auto& rect = contentRect;
    const auto& color =
      _qlementineStyle ? _qlementineStyle->toolBarSeparatorColor() : Theme().secondaryAlternativeColorDisabled;
    const auto lineW = theme.borderWidth;
    constexpr auto padding = 0;
    const auto x = rect.x() + (rect.width() - lineW) / 2.;
    const auto y1 = static_cast<double>(rect.y() + padding);
    const auto y2 = static_cast<double>(rect.y() + rect.height() - padding);
    p->setBrush(Qt::NoBrush);
    p->setPen(QPen(color, lineW, Qt::SolidLine, Qt::FlatCap));
    p->drawLine(QPointF{ x, y1 }, QPointF{ x, y2 });
  } else {
    const auto mouse = getComboBoxItemMouseState(opt.state);
    const auto& rect = contentRect;

    // Background.
    auto bgRect = contentRect;
    if (viewIsTreeView) {
      const auto viewPadding = theme.spacing / 2;
      bgRect.setLeft(viewPadding);
    }
    const auto hPadding = theme.spacing;
    const auto& bgColor =
      _qlementineStyle ? _qlementineStyle->menuItemBackgroundColor(mouse) : Theme().primaryColorTransparent;
    const auto radius = _qlementineStyle->theme().borderRadius - contentMargin / 2;
    p->setPen(Qt::NoPen);
    p->setBrush(bgColor);
    p->drawRoundedRect(bgRect, radius, radius);

    // Foreground.
    const auto fgRect = rect.marginsRemoved(QMargins{ hPadding, 0, hPadding, 0 });
    if (fgRect.isEmpty()) {
      return;
    }

    const auto selected = getSelectionState(opt.state);
    const auto active = getActiveState(opt.state);
    const auto focus = selected == SelectionState::Selected ? FocusState::Focused : FocusState::NotFocused;
    auto availableW = fgRect.width();
    auto availableX = fgRect.x();
    const auto& fgData = idx.data(Qt::ForegroundRole);
    auto fgColor = QColor(_qlementineStyle ? _qlementineStyle->menuItemForegroundColor(mouse) : Theme().secondaryColor);
    if (fgData.isValid()) {
      fgColor = fgData.value<QColor>();
    }

    // Arrow that indicates that the tree's branch is open/closed.
    // It is already drawn by the QTreeView when the item is not the current one.
    if (viewIsTreeView && opt.state.testFlag(QStyle::State_Selected)) {
      const auto hasChildren = opt.state.testFlag(QStyle::State_Children);
      if (hasChildren) {
        const auto indentation = _qlementineStyle ? _qlementineStyle->pixelMetric(QStyle::PM_TreeViewIndentation) : 0;
        const auto branchIsOpen = opt.state.testFlag(QStyle::State_Open);
        const auto arrowSize = theme.iconSize;
        const auto arrowX = rect.x() - indentation;
        const auto arrowY = rect.y() + (rect.height() - arrowSize.height()) / 2;
        const auto arrowRect = QRect(QPoint{ arrowX, arrowY }, arrowSize);
        p->setBrush(Qt::NoBrush);
        p->setPen(QPen(fgColor, 1.01, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        if (branchIsOpen) {
          qlementine::drawArrowDown(arrowRect, p);
        } else {
          qlementine::drawArrowRight(arrowRect, p);
        }
      }
    }

    // Icon.
    const auto iconVariant = idx.data(Qt::DecorationRole);
    const auto& icon =
      iconVariant.isValid() && iconVariant.userType() == QMetaType::QIcon ? iconVariant.value<QIcon>() : QIcon{};
    if (availableW > 0 && !icon.isNull()) {
      const auto spacing = theme.spacing;
      const auto& iconSize = opt.decorationSize; // Get icon size.
      const auto pixmap = getPixmap(icon, iconSize, mouse, CheckState::NotChecked, _widget);
      const auto* qlementineStyle = qobject_cast<QlementineStyle*>(_widget->style());
      const auto autoIconColor = qlementineStyle ? qlementineStyle->autoIconColor(_widget) : AutoIconColor::None;
      const auto pixmapPixelRatio = pixmap.devicePixelRatio();
      const auto pixmapW = pixmapPixelRatio != 0 ? static_cast<int>((qreal) pixmap.width() / pixmapPixelRatio) : 0;
      const auto pixmapH = pixmapPixelRatio != 0 ? static_cast<int>((qreal) pixmap.height() / pixmapPixelRatio) : 0;
      const auto pixmapX = availableX + (iconSize.width() - pixmapW) / 2; // Center the icon in the rect.
      const auto pixmapY = fgRect.y() + (fgRect.height() - pixmapH) / 2;
      const auto pixmapRect = QRect{ pixmapX, pixmapY, pixmapW, pixmapH };
      availableW -= iconSize.width() + spacing;
      availableX += iconSize.width() + spacing;

      if (mouse == MouseState::Disabled && autoIconColor == AutoIconColor::None) {
        // Change only the icon's tint and opacity, so it looks disabled.
        const auto& sourceOverBgColor = qlementineStyle ? qlementineStyle->listItemBackgroundColor(
                                                            MouseState::Normal, selected, focus, active, idx, _widget)
                                                        : Theme().neutralColorTransparent;
        const auto premultipiedColor = getColorSourceOver(sourceOverBgColor, fgColor);
        const auto& tintedPixmap = getTintedPixmap(pixmap, premultipiedColor);
        const auto opacity = selected == SelectionState::Selected ? 1. : 0.25;
        const auto backupOpacity = p->opacity();
        p->setOpacity(opacity * backupOpacity);
        p->drawPixmap(pixmapRect, tintedPixmap);
        p->setOpacity(backupOpacity);
      } else {
        // Actually color the whole icon if needed.
        const auto& colorizedPixmap =
          qlementineStyle ? qlementineStyle->getColorizedPixmap(pixmap, autoIconColor, fgColor, fgColor) : pixmap;
        p->drawPixmap(pixmapRect, colorizedPixmap);
      }
    }

    // Text.
    const auto textVariant = idx.data(Qt::DisplayRole);
    const auto& text =
      textVariant.isValid() && textVariant.userType() == QMetaType::QString ? textVariant.value<QString>() : QString{};
    if (availableW > 0 && !text.isEmpty()) {
      const auto& fm = opt.fontMetrics;
      const auto elidedText = fm.elidedText(text, Qt::ElideRight, availableW);
      const auto textX = availableX;
      const auto textRect = QRect{ textX, fgRect.y(), availableW, fgRect.height() };
      const auto textFlags = Qt::AlignVCenter | Qt::AlignBaseline | Qt::TextSingleLine | Qt::AlignLeft;
      p->setBrush(Qt::NoBrush);
      p->setPen(fgColor);
      p->drawText(textRect, textFlags, elidedText, nullptr);
    }
  }
}

QSize ComboBoxDelegate::sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& idx) const {
  if (!_qlementineStyle) {
    return QItemDelegate::sizeHint(opt, idx);
  }

  const auto& theme = _qlementineStyle ? _qlementineStyle->theme() : Theme{};

  const auto isSeparator = idx.data(Qt::AccessibleDescriptionRole).toString() == QLatin1String("separator");
  if (isSeparator) {
    const auto h = theme.spacing + theme.borderWidth;
    return QSize{ h, h };
  } else {
    const auto contentMargin = _qlementineStyle->pixelMetric(QStyle::PM_MenuHMargin);
    const auto hPadding = theme.spacing;
    const auto vPadding = theme.spacing;
    const auto iconSize = theme.iconSize;
    const auto spacing = theme.spacing;
    const auto& fm = opt.fontMetrics;

    const auto textVariant = idx.data(Qt::DisplayRole);
    const auto& text =
      textVariant.isValid() && textVariant.userType() == QMetaType::QString ? textVariant.value<QString>() : QString{};
    const auto iconVariant = idx.data(Qt::DecorationRole);
    const auto& icon =
      iconVariant.isValid() && iconVariant.userType() == QMetaType::QIcon ? iconVariant.value<QIcon>() : QIcon{};
    const auto textW = qlementine::textWidth(fm, text);
    const auto iconW = !icon.isNull() ? iconSize.width() + spacing : 0;
    const auto w = std::max(0, contentMargin * 2 + hPadding + iconW + textW + hPadding);
    const auto h = std::max(theme.controlHeightMedium, std::max(iconSize.height(), vPadding) + vPadding);
    return QSize{ w, h };
  }
}
} // namespace oclero::qlementine
