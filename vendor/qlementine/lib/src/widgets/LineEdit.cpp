// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/LineEdit.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/QlementineStyleOption.hpp>
#include <oclero/qlementine/utils/PrimitiveUtils.hpp>
#include <oclero/qlementine/utils/ImageUtils.hpp>

#include <QPainter>
#include <QEvent>
#include <QFontDatabase>

namespace oclero::qlementine {
LineEdit::LineEdit(QWidget* parent)
  : QLineEdit(parent) {}

const QIcon& LineEdit::icon() const {
  return _icon;
}

void LineEdit::setIcon(const QIcon& icon) {
  _icon = icon;
  Q_EMIT iconChanged();
  update();
  if (_icon.isNull()) {
    setTextMargins(0, 0, 0, 0);
  } else {
    const auto spacing = style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
    const auto iconExtent = style()->pixelMetric(QStyle::PM_SmallIconSize);
    setTextMargins(spacing + iconExtent, 0, 0, 0);
  }
}

void LineEdit::setUseMonoSpaceFont(bool useMonoSpaceFont) {
  _useMonospaceFont = useMonoSpaceFont;
  ensurePolished();
  updateFont();
}

bool LineEdit::useMonoSpaceFont() const {
  return _useMonospaceFont;
}

void LineEdit::paintEvent(QPaintEvent* evt) {
  QLineEdit::paintEvent(evt);

  // Draw icon.
  const auto iconExtent = style()->pixelMetric(QStyle::PM_SmallIconSize);
  const auto spacing = style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
  const auto iconSize = QSize{ iconExtent, iconExtent };
  const auto pixmap = this->getPixmap();
  const auto pixmapX = spacing;
  const auto pixmapY = (height() - iconSize.height()) / 2;
  const auto pixmapRect = QRect{ { pixmapX, pixmapY }, iconSize };

  QPainter p(this);
  p.setRenderHint(QPainter::SmoothPixmapTransform, true);
  p.drawPixmap(pixmapRect, pixmap);
}

bool LineEdit::event(QEvent* e) {
  if (e->type() == QEvent::Type::PaletteChange) {
    update();
  }
  return QLineEdit::event(e);
}

QPixmap LineEdit::getPixmap() const {
  const auto iconExtent = style()->pixelMetric(QStyle::PM_SmallIconSize);
  const auto iconSize = QSize{ iconExtent, iconExtent };

  const auto* qlementineStyle = qobject_cast<QlementineStyle*>(style());
  const auto autoIconColor = qlementineStyle ? qlementineStyle->autoIconColor(this) : AutoIconColor::None;
  if (autoIconColor != AutoIconColor::None) {
    const auto pixmap = qlementine::getPixmap(_icon, iconSize, MouseState::Normal, CheckState::NotChecked, this);
    const auto colorGroup = isEnabled() ? QPalette::ColorGroup::Normal : QPalette::ColorGroup::Disabled;
    const auto& color = palette().color(colorGroup, QPalette::ColorRole::Text);
    const auto colorizedPixmap = qlementine::getColorizedPixmap(pixmap, color);
    return colorizedPixmap;
  } else {
    const auto mouse = isEnabled() ? MouseState::Normal : MouseState::Disabled;
    const auto pixmap = qlementine::getPixmap(_icon, iconSize, mouse, CheckState::NotChecked, this);
    return pixmap;
  }
}

void LineEdit::updateFont() {
  const auto* style = this->style();
  const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style);

  if (_useMonospaceFont) {
    if (qlementineStyle) {
      setFont(qlementineStyle->theme().fontMonospace);
    } else {
      const auto systemFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
      setFont(systemFont);
    }
  } else {
    if (qlementineStyle) {
      setFont(qlementineStyle->theme().fontRegular);
    } else {
      const auto systemFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
      setFont(systemFont);
    }
  }
}

Status LineEdit::status() const {
  return _status;
}

void LineEdit::setStatus(Status status) {
  if (status != _status) {
    _status = status;
    update();
    Q_EMIT statusChanged();
  }
}
} // namespace oclero::qlementine
