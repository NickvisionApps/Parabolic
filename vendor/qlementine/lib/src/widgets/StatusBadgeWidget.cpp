// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/StatusBadgeWidget.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>

namespace oclero::qlementine {
StatusBadgeWidget::StatusBadgeWidget(QWidget* parent)
  : StatusBadgeWidget(StatusBadge::Info, StatusBadgeSize::Medium, parent) {}

StatusBadgeWidget::StatusBadgeWidget(StatusBadge badge, QWidget* parent)
  : StatusBadgeWidget(badge, StatusBadgeSize::Medium, parent) {}

StatusBadgeWidget::StatusBadgeWidget(StatusBadge badge, StatusBadgeSize badgeSize, QWidget* parent)
  : QWidget(parent)
  , _badgeSize(badgeSize)
  , _badge(badge) {
  setFocusPolicy(Qt::NoFocus);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

StatusBadge StatusBadgeWidget::badge() const {
  return _badge;
}
void StatusBadgeWidget::setBadge(StatusBadge badge) {
  if (badge != _badge) {
    _badge = badge;
    update();
    Q_EMIT badgeChanged();
  }
}

StatusBadgeSize StatusBadgeWidget::badgeSize() const {
  return _badgeSize;
}

void StatusBadgeWidget::setBadgeSize(StatusBadgeSize size) {
  if (size != _badgeSize) {
    _badgeSize = size;
    updateGeometry();
    update();
    Q_EMIT badgeSizeChanged();
  }
}

QSize StatusBadgeWidget::sizeHint() const {
  const auto* style = this->style();
  const auto* qlementineStyle = qobject_cast<const qlementine::QlementineStyle*>(style);

  auto extent = 0;
  switch (_badgeSize) {
    case StatusBadgeSize::Small:
      extent =
        qlementineStyle ? qlementineStyle->theme().iconSize.height() : style->pixelMetric(QStyle::PM_ButtonIconSize);
      break;
    case StatusBadgeSize::Medium:
      extent = qlementineStyle ? qlementineStyle->theme().iconSizeMedium.height()
                               : style->pixelMetric(QStyle::PM_LargeIconSize);
      break;
    default:
      extent = qlementineStyle ? qlementineStyle->theme().iconSizeMedium.height()
                               : style->pixelMetric(QStyle::PM_LargeIconSize);
      break;
  }

  return { extent, extent };
}

void StatusBadgeWidget::paintEvent(QPaintEvent* /*e*/) {
  QPainter p(this);
  const auto* style = this->style();
  const auto* qlementineStyle = qobject_cast<const qlementine::QlementineStyle*>(style);
  const auto& theme = qlementineStyle ? qlementineStyle->theme() : qlementine::Theme{};
  const auto opacity = isEnabled() ? 1.0 : 0.35;
  p.setOpacity(opacity);
  qlementine::drawStatusBadge(&p, rect(), _badge, _badgeSize, theme);
}
} // namespace oclero::qlementine
