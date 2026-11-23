// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT
#include <oclero/qlementine/widgets/NotificationBadge.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>

#include <QStyle>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QLayout>

#include <algorithm>

namespace oclero::qlementine {
NotificationBadge::NotificationBadge(QWidget* parent)
  : QWidget(parent) {
  setFocusPolicy(Qt::NoFocus);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setAttribute(Qt::WA_TransparentForMouseEvents);

  // Font
  const auto* qlementine = qobject_cast<const QlementineStyle*>(style());
  if (qlementine) {
    auto font = qlementine->fontForTextRole(qlementine::TextRole::Caption);
    font.setBold(true);
    setFont(font);
  }

  // Colors.
  _backgroundColor =
    qlementine ? qlementine->statusColor(qlementine::Status::Error, qlementine::MouseState::Normal) : Qt::red;
  _foregroundColor = qlementine
                       ? qlementine->statusColorForeground(qlementine::Status::Error, qlementine::MouseState::Normal)
                       : Qt::white;
}

void NotificationBadge::setWidget(QWidget* widget) {
  if (widget != _widget) {
    // Reset.
    if (_widget) {
      _widget->removeEventFilter(this);
    }
    if (_widgetParent) {
      _widgetParent->removeEventFilter(this);

      if (auto* layout = _widgetParent->layout()) {
        layout->removeWidget(this);
      }
    }

    _widget = nullptr;
    _widgetParent = nullptr;
    setParent(nullptr);
    hide();

    // Set new one.
    if (widget && !widget->isWindow() && widget->parentWidget()->windowType() != Qt::SubWindow) {
      _widget = widget;
      _widget->installEventFilter(this);

      _widgetParent = _widget->parentWidget();
      if (_widgetParent) {
        _widgetParent->installEventFilter(this);
      }

      // Modify the parent to ensure the widget is not clipped.
      setParent(_widgetParent ? _widgetParent : _widget);

      // Update the position.
      QTimer::singleShot(0, [this]() {
        if (_widget->isVisible()) {
          show();
          updatePosition();
        }
      });
    }
  }
}

QWidget* NotificationBadge::widget() const {
  return _widget;
}

const QString& NotificationBadge::text() const {
  return _text;
}

void NotificationBadge::setText(const QString& text) {
  const auto simplifiedText = text.simplified();
  if (simplifiedText != _text) {
    _text = simplifiedText;
    Q_EMIT textChanged();
    updateGeometry();
  }
}

const QColor& NotificationBadge::foregroundColor() const {
  return _foregroundColor;
}

void NotificationBadge::setForegroundColor(const QColor& color) {
  if (color != _foregroundColor) {
    _foregroundColor = color;
    Q_EMIT foregroundColorChanged();
    update();
  }
}

const QColor& NotificationBadge::backgroundColor() const {
  return _backgroundColor;
}

void NotificationBadge::setBackgroundColor(const QColor& color) {
  if (color != _backgroundColor) {
    _backgroundColor = color;
    Q_EMIT backgroundColorChanged();
    update();
  }
}

const QPoint& NotificationBadge::relativePosition() const {
  return _relativePos;
}

void NotificationBadge::setRelativePosition(const QPoint& pos) {
  if (pos != _relativePos) {
    _relativePos = pos;
    Q_EMIT relativePositionChanged();
    updatePosition();
  }
}

void NotificationBadge::setRelativePosition(int x, int y) {
  setRelativePosition({ x, y });
}

const QMargins& NotificationBadge::padding() const {
  return _padding;
}

void NotificationBadge::setPadding(const QMargins& padding) {
  if (padding != _padding) {
    _padding = padding;
    Q_EMIT paddingChanged();
    updateGeometry();
    update();
    updatePosition();
  }
}

QSize NotificationBadge::minimumSizeHint() const {
  const auto* style = this->style();
  const auto defaultIconExtent = style ? style->pixelMetric(QStyle::PM_ButtonIconSize) : 16;
  const auto defaultExtent = defaultIconExtent / 2;

  if (_text.isEmpty()) {
    return { defaultExtent, defaultExtent };
  } else {
    const auto font = this->font();
    const auto fm = QFontMetrics{ font };
    const auto textSize = fm.boundingRect(_text);
    const auto h = textSize.height() + _padding.top() + _padding.bottom();
    const auto w = std::max(h, textSize.width() + _padding.left() + _padding.right());
    return { w, h };
  }
}

QSize NotificationBadge::sizeHint() const {
  return minimumSizeHint();
}

void NotificationBadge::paintEvent(QPaintEvent*) {
  QPainter p(this);

  // Background.
  const auto rect = this->rect();
  constexpr auto defaultRadius = 8;
  const auto radius = std::min(defaultRadius, std::min(rect.height(), rect.width()));
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setBrush(_backgroundColor);
  p.setPen(Qt::NoPen);
  p.drawRoundedRect(rect, radius, radius);

  // Foreground.
  if (!_text.isEmpty()) {
    const auto font = this->font();
    p.setFont(font);
    p.setPen(_foregroundColor);
    p.drawText(rect, _text, { Qt::AlignCenter });
  }
}

bool NotificationBadge::eventFilter(QObject* obj, QEvent* evt) {
  const auto type = evt->type();
  switch (type) {
    case QEvent::Type::Resize:
    case QEvent::Type::Move:
      updatePosition();
      break;
    case QEvent::Type::ParentChange:
      if (obj == _widget) {
        _widgetParent = _widget->parentWidget();
        updatePosition();
      }
      break;
    case QEvent::Type::Show:
      if (obj == _widget) {
        show();
        updatePosition();
      }
      break;
    case QEvent::Type::Hide:
      if (obj == _widget) {
        hide();
      }
      break;
    case QEvent::Type::ZOrderChange:
      if (obj == _widget) {
        raise();
      }
      break;
    case QEvent::Destroy:
      setWidget(nullptr);
    default:
      break;
  }

  return false;
}

bool NotificationBadge::event(QEvent* evt) {
  const auto type = evt->type();
  switch (type) {
    case QEvent::FontChange:
      updateGeometry();
      update();
      updatePosition();
      break;
    default:
      break;
  }
  return QWidget::event(evt);
}

void NotificationBadge::updatePosition() {
  if (!_widget || !_widgetParent) {
    return;
  }

  const auto widgetRect = _widget->geometry();
  const auto badgeSize = sizeHint();

  const auto x = widgetRect.x() + widgetRect.width() - badgeSize.width() + _relativePos.x();
  const auto y = widgetRect.y() + _relativePos.y();

  setGeometry(x, y, badgeSize.width(), badgeSize.height());
}
} // namespace oclero::qlementine
