// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/IconWidget.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/utils/ImageUtils.hpp>

#include <QPainter>

namespace oclero::qlementine {
IconWidget::IconWidget(QWidget* parent)
  : QWidget(parent) {
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setFocusPolicy(Qt::NoFocus);
  const auto* style = this->style();
  const auto extent = style ? style->pixelMetric(QStyle::PM_ButtonIconSize, nullptr, this) : 16;
  _iconSize = QSize{ extent, extent };
}

IconWidget::IconWidget(const QIcon& icon, QWidget* parent)
  : QWidget(parent)
  , _icon{ icon } {
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setFocusPolicy(Qt::NoFocus);
  const auto* style = this->style();
  const auto extent = style ? style->pixelMetric(QStyle::PM_ButtonIconSize, nullptr, this) : 16;
  _iconSize = QSize{ extent, extent };
}

IconWidget::IconWidget(const QIcon& icon, const QSize& size, QWidget* parent)
  : QWidget(parent)
  , _iconSize{ size }
  , _icon{ icon } {
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setFocusPolicy(Qt::NoFocus);
}

const QIcon& IconWidget::icon() const {
  return _icon;
}

void IconWidget::setIcon(const QIcon& icon) {
  _icon = icon;
  Q_EMIT iconChanged();
  update();
}

const QSize& IconWidget::iconSize() const {
  return _iconSize;
}

void IconWidget::setIconSize(const QSize& iconSize) {
  if (iconSize != _iconSize) {
    _iconSize = iconSize;
    Q_EMIT iconSizeChanged();
    updateGeometry();
    update();
  }
}

QSize IconWidget::sizeHint() const {
  const auto padding = contentsMargins();
  const auto w = _iconSize.width() + padding.left() + padding.right();
  const auto h = _iconSize.height() + padding.top() + padding.bottom();
  return { w, h };
}

void IconWidget::paintEvent(QPaintEvent*) {
  const auto* qlementineStyle = qobject_cast<QlementineStyle*>(style());
  const auto autoIconColor = qlementineStyle ? qlementineStyle->autoIconColor(this) : AutoIconColor::None;
  const auto iconMode =
    isEnabled() || autoIconColor != AutoIconColor::None ? QIcon::Mode::Normal : QIcon::Mode::Disabled;
  const auto pixmap = _icon.pixmap(_iconSize.height(), iconMode, QIcon::State::Off);
  if (pixmap.isNull())
    return;

  const auto& color = palette().color(isEnabled() ? QPalette::Normal : QPalette::Disabled, QPalette::Text);
  const auto& colorizedPixmap = autoIconColor != AutoIconColor::None ? getColorizedPixmap(pixmap, color) : pixmap;

  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setRenderHint(QPainter::SmoothPixmapTransform, true);
  const auto x = (width() - _iconSize.width()) / 2;
  const auto y = (height() - _iconSize.height()) / 2;
  p.drawPixmap(x, y, colorizedPixmap);
}
} // namespace oclero::qlementine
