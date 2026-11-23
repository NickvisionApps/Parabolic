// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/Expander.hpp>
#include <oclero/qlementine/utils/WidgetUtils.hpp>

#include <QStyle>
#include <QEvent>

namespace oclero::qlementine {
constexpr auto animationDurationFactor = 1.;

Expander::Expander(QWidget* parent)
  : QWidget(parent) {
  setFocusPolicy(Qt::NoFocus);

  if (_orientation == Qt::Vertical) {
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  } else {
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  }

  // Initialize animation.
  const auto animationDuration = style()->styleHint(QStyle::SH_Widget_Animation_Duration);
  _animation.setStartValue(QVariant::fromValue<int>(0));
  _animation.setEndValue(QVariant::fromValue<int>(0));
  _animation.setDuration(animationDuration * animationDurationFactor);
  _animation.setEasingCurve(QEasingCurve::Type::OutCubic);
  QObject::connect(&_animation, &QVariantAnimation::valueChanged, this, [this]() {
    updateGeometry();
  });

  QObject::connect(&_animation, &QVariantAnimation::finished, this, [this]() {
    if (_expanded) {
      Q_EMIT didExpand();
    } else {
      Q_EMIT didShrink();
    }
  });
}

QSize Expander::sizeHint() const {
  const auto contentSizeHint = _content ? _content->sizeHint() : QSize{ 0, 0 };
  const auto isVertical = _orientation == Qt::Orientation::Vertical;
  const auto currentValue = _animation.currentValue().toInt();

  if (isVertical) {
    const auto finalValue = _expanded ? contentSizeHint.height() : 0;
    const auto w = contentSizeHint.width();
    const auto h = _animation.state() == QVariantAnimation::Running ? currentValue : finalValue;
    return { w, h };
  } else {
    const auto finalValue = _expanded ? contentSizeHint.width() : 0;
    const auto h = contentSizeHint.height();
    const auto w = _animation.state() == QVariantAnimation::Running ? currentValue : finalValue;
    return { w, h };
  }
}

bool Expander::event(QEvent* e) {
  const auto type = e->type();
  if (type == QEvent::Type::LayoutRequest) {
    updateGeometry();
    updateContentGeometry();
  }
  return QWidget::event(e);
}

void Expander::resizeEvent(QResizeEvent* e) {
  QWidget::resizeEvent(e);
  updateContentGeometry();
}

bool Expander::eventFilter(QObject* o, QEvent* e) {
  const auto type = e->type();
  if (type == QEvent::Type::Resize) {
    updateGeometry();
  }
  return QWidget::eventFilter(o, e);
}

void Expander::updateContentGeometry() {
  if (_content) {
    _content->ensurePolished();
    const auto& availableSize = size();
    const auto contentSizeHint = _content->sizeHint();
    const auto isVertical = _orientation == Qt::Orientation::Vertical;
    const auto w = isVertical ? availableSize.width() : contentSizeHint.width();
    const auto h = isVertical ? contentSizeHint.height() : availableSize.height();
    const auto visible = isVertical ? w > 0 : h > 0;
    _content->setVisible(visible);
    _content->setGeometry(0, 0, w, h);
  }
}

bool Expander::expanded() const {
  return _expanded;
}

void Expander::setExpanded(bool expanded) {
  if (expanded != _expanded) {
    if (_content) {
      _content->setVisible(true);
    }
    _expanded = expanded;

    if (_expanded) {
      Q_EMIT aboutToExpand();
    } else {
      oclero::qlementine::clearFocus(this, true);
      Q_EMIT aboutToShrink();
    }

    const auto isVertical = _orientation == Qt::Orientation::Vertical;
    const auto current = isVertical ? height() : width();
    const auto contentSizeHint = _content ? _content->sizeHint() : QSize{ 0, 0 };
    const auto target = _content && _expanded ? (isVertical ? contentSizeHint.height() : contentSizeHint.width()) : 0;
    const auto animationDuration = isVisible() ? style()->styleHint(QStyle::SH_Widget_Animation_Duration) : 0;
    _animation.stop();
    _animation.setDuration(animationDuration * animationDurationFactor);
    _animation.setStartValue(QVariant::fromValue<int>(current));
    _animation.setEndValue(QVariant::fromValue<int>(target));
    _animation.start();

    Q_EMIT expandedChanged();
  }
}

void Expander::toggleExpanded() {
  setExpanded(!expanded());
}

Qt::Orientation Expander::orientation() const {
  return _orientation;
}

void Expander::setOrientation(Qt::Orientation orientation) {
  if (_orientation != orientation) {
    _orientation = orientation;

    if (_orientation == Qt::Vertical) {
      setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    } else {
      setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    }

    updateGeometry();
    Q_EMIT orientationChanged();
  }
}

QWidget* Expander::content() const {
  return _content;
}

void Expander::setContent(QWidget* content) {
  if (content != _content) {
    if (_content) {
      _content->removeEventFilter(this);
      delete _content;
    }

    _content = content;
    if (_content) {
      if (_orientation == Qt::Orientation::Vertical) {
        _content->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Ignored);
      } else {
        _content->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::MinimumExpanding);
      }

      _content->setParent(this);
      _content->installEventFilter(this);
      _content->setVisible(_expanded);
    }
    updateGeometry();
    Q_EMIT contentChanged();
  }
}
} // namespace oclero::qlementine
