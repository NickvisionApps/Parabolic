// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/Popover.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/utils/PrimitiveUtils.hpp>
#include <oclero/qlementine/utils/ImageUtils.hpp>
#include <oclero/qlementine/utils/GeometryUtils.hpp>

#include <QApplication>
#include <QBitmap>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <QStyle>
#include <QStylePainter>
#include <QTimer>

#include <array>

namespace oclero::qlementine {
// Values taken arbitrarly, to make it feel smooth and natural.
constexpr auto openAnimationDurationFactor = .75;
constexpr auto closeAnimationDurationFactor = .9;

constexpr auto defaultFramePadding = 16;

#ifdef __APPLE__
const bool Popover::_shouldDrawDropShadow = false;
#else
const bool Popover::_shouldDrawDropShadow = true;
#endif

// Sert de conteneur au contenu du Popover.
class Popover::PopoverFrame : public QWidget {
private:
  std::function<void()> _onResize;

public:
  explicit PopoverFrame(QWidget* parent)
    : QWidget(parent) {
    // Transparent background.
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    setBackgroundRole(QPalette::NoRole);

    // Layout where the content will be.
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(defaultFramePadding, defaultFramePadding, defaultFramePadding, defaultFramePadding);
    setLayout(layout);
  }

  ~PopoverFrame() override = default;

  void onResize(const std::function<void()>&& cb) {
    _onResize = cb;
    if (_onResize) {
      _onResize();
    }
  }

  void resizeEvent(QResizeEvent* e) override {
    QWidget::resizeEvent(e);
    callResize();
  }

  bool event(QEvent* e) override {
    const auto result = QWidget::event(e);
    const auto type = e->type();
    switch (type) {
      case QEvent::LayoutRequest:
      case QEvent::DevicePixelRatioChange:
        callResize();
        break;
      default:
        break;
    }
    return result;
  }

private:
  void callResize() {
    if (_onResize) {
      _onResize();
    }
  }
};

Popover::Popover(QWidget* parent)
  : QWidget(parent) {
  setAttribute(Qt::WA_TranslucentBackground, true);
  setAttribute(Qt::WA_OpaquePaintEvent, true);
  setAttribute(Qt::WA_NoSystemBackground, true);
  setWindowFlag(Qt::WindowType::Popup, true);
  setWindowFlag(Qt::WindowType::FramelessWindowHint, true);

  if (_shouldDrawDropShadow) {
    // We draw the drop shadow ourselves.
    setWindowFlag(Qt::WindowType::NoDropShadowWindowHint, true);
  } else {
    // Let the system draw the drop shadow.
    setWindowFlag(Qt::WindowType::NoDropShadowWindowHint, false);
  }

  setBackgroundRole(QPalette::NoRole);
  setAutoFillBackground(false);
  // Seems to have no effect with Qt::WindowType::Popup.
  setWindowModality(Qt::WindowModality::NonModal);
  setFocusPolicy(Qt::FocusPolicy::NoFocus);

  const auto& palette = this->palette();
  _backgroundColor = palette.color(QPalette::ColorGroup::Active, QPalette::ColorRole::Window);
  _borderColor = palette.color(QPalette::ColorGroup::Active, QPalette::ColorRole::Button);

  const auto* style = this->style();

  // Popover layout, used to place the content and keep space for the drop shadow.
  auto* popoverLayout = new QVBoxLayout(this);
  popoverLayout->setSpacing(0);
  popoverLayout->setContentsMargins(0, 0, 0, 0);
  setLayout(popoverLayout);
  updateDropShadowMargins();

  // Layout.
  _frame = new Popover::PopoverFrame(this);
  _frame->onResize([this]() {
    updateFrameMask();
  });
  _frameLayout = static_cast<QVBoxLayout*>(_frame->layout());
  popoverLayout->addWidget(_frame);

  // Opacity animation.
  const auto opacityAnimDuration =
    _animated ? style->styleHint(QStyle::SH_Widget_Animation_Duration) * openAnimationDurationFactor : 0;
  constexpr auto startOpacity = 0.;
  const auto startOpacityVar = QVariant::fromValue<double>(startOpacity);
  _opacityAnimation.setDuration(opacityAnimDuration);
  _opacityAnimation.setStartValue(startOpacityVar);
  _opacityAnimation.setEndValue(startOpacityVar);
  setWindowOpacity(startOpacity);
  QObject::connect(&_opacityAnimation, &QVariantAnimation::valueChanged, this, [this]() {
    const auto currentOpacity = _opacityAnimation.currentValue().toDouble();
    setWindowOpacity(currentOpacity);
  });
  QObject::connect(&_opacityAnimation, &QVariantAnimation::finished, this, [this]() {
    if (_opened) {
      Q_EMIT opened();
    } else {
      Q_EMIT closed();
      hide();
    }
  });

  // Timer to prevent the popup to re-open again and again when clicking on the anchor widget.
  _clickTimer.setSingleShot(true);
  _clickTimer.setInterval(style->styleHint(QStyle::SH_Widget_Animation_Duration) * closeAnimationDurationFactor);

  // Allows to define properties with CSS.
  ensurePolished();
}

Popover::~Popover() {}

bool Popover::manualPositioning() const {
  return _manualPositioning;
}

void Popover::setManualPositioning(bool value) {
  if (value != _manualPositioning) {
    _manualPositioning = value;
    if (isVisible()) {
      updatePopoverGeometry();
    }
    Q_EMIT manualPositioningChanged();
  }
}

void Popover::setManualPositioningCallback(const std::function<QPoint(const QSize&)>& cb) {
  _manualPositioningCb = cb;
  if (isVisible()) {
    updatePopoverGeometry();
  }
}

QSize Popover::popoverFrameSize() const {
  return _frame ? _frame->sizeHint() : QSize{};
}

Popover::Position Popover::preferredPosition() const {
  return _preferredPosition;
}

void Popover::setPreferredPosition(Position position) {
  if (position != _preferredPosition) {
    _preferredPosition = position;
    if (isVisible()) {
      updatePopoverGeometry();
    }
    Q_EMIT preferredPositionChanged();
  }
}

Popover::Alignment Popover::preferredAlignment() const {
  return _preferredAlignment;
}

void Popover::setPreferredAlignment(Alignment alignment) {
  if (alignment != _preferredAlignment) {
    _preferredAlignment = alignment;
    if (isVisible()) {
      updatePopoverGeometry();
    }
    Q_EMIT preferredAlignmentChanged();
  }
}

QWidget* Popover::contentWidget() const {
  return _content;
}

void Popover::setContentWidget(QWidget* widget) {
  if (widget != _content) {
    // Check if there is no parenting issues.
    if (_anchorWidget && widget->isAncestorOf(_anchorWidget)) {
      qWarning() << "The Popover's anchor widget cannot be a child of the Popover's content widget.";
      return;
    }

    if (_content) {
      _content = nullptr;

      // Empty layout.
      if (auto* layoutItem = _frameLayout->takeAt(0)) {
        delete layoutItem->widget();
        delete layoutItem;
      }
    }

    _content = widget;

    if (_content) {
      _content->setParent(_frame);
      _content->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      _frameLayout->addWidget(_content);
    }

    if (isVisible()) {
      updatePopoverGeometry();
    }
    Q_EMIT contentWidgetChanged();
  }
}

bool Popover::isOpened() const {
  return _opened;
}

bool Popover::isClosed() const {
  return !isOpened();
}

void Popover::setOpened(bool opened) {
  if (opened != _opened) {
    _opened = opened;
    if (_opened) {
      Q_EMIT aboutToOpen();
      updatePopoverGeometry();
      show();
      startAnimation();
    } else {
      Q_EMIT aboutToClose();
      startAnimation();
    }

    Q_EMIT openedChanged();
  }
}

void Popover::setClosed(bool closed) {
  setOpened(!closed);
}

QMargins Popover::padding() const {
  return _frameLayout->contentsMargins();
}

void Popover::setPadding(const QMargins& padding) {
  if (padding != _frameLayout->contentsMargins()) {
    _frameLayout->setContentsMargins(padding);
    Q_EMIT paddingChanged();
    if (isVisible()) {
      updatePopoverGeometry();
    }
  }
}

QMargins Popover::screenPadding() const {
  return _screenPadding;
}

void Popover::setScreenPadding(const QMargins& padding) {
  if (padding != _screenPadding) {
    _screenPadding = padding;
    Q_EMIT screenPaddingChanged();
    if (isVisible()) {
      updatePopoverGeometry();
    }
  }
}

QWidget* Popover::anchorWidget() const {
  return _anchorWidget;
}

void Popover::setAnchorWidget(QWidget* widget) {
  if (widget != _anchorWidget) {
    // Register to new anchor widget.
    _anchorWidget = widget;
    if (isVisible()) {
      updatePopoverGeometry();
    }
    Q_EMIT anchorWidgetChanged();
  }
}

int Popover::verticalSpacing() const {
  return _verticalSpacing;
}

void Popover::setVerticalSpacing(int spacing) {
  if (spacing != _verticalSpacing) {
    _verticalSpacing = spacing;
    if (isVisible()) {
      updatePopoverGeometry();
    }
    Q_EMIT verticalSpacingChanged();
  }
}

int Popover::horizontalSpacing() const {
  return _horizontalSpacing;
}

void Popover::setHorizontalSpacing(int spacing) {
  if (spacing != _horizontalSpacing) {
    _horizontalSpacing = spacing;
    if (isVisible()) {
      updatePopoverGeometry();
    }
    Q_EMIT horizontalSpacingChanged();
  }
}

const QColor& Popover::dropShadowColor() const {
  return _dropShadowColor;
}

void Popover::setDropShadowColor(const QColor& color) {
  if (color != _dropShadowColor) {
    _dropShadowColor = color;
    updateDropShadowCache();
    Q_EMIT dropShadowColorChanged();
    update();
  }
}

qreal Popover::radius() const {
  return _radius;
}

void Popover::setRadius(qreal radius) {
  if (radius != _radius) {
    _radius = radius;
    Q_EMIT radiusChanged();
    updateGeometry();
    update();
  }
}

qreal Popover::borderWidth() const {
  return _borderWidth;
}

void Popover::setBorderWidth(qreal value) {
  if (value != _borderWidth) {
    _borderWidth = value;
    Q_EMIT borderWidthChanged();
    update();
  }
}

qreal Popover::dropShadowRadius() const {
  return _dropShadowRadius;
}

void Popover::setDropShadowRadius(qreal radius) {
  if (radius != _dropShadowRadius) {
    _dropShadowRadius = radius;
    Q_EMIT dropShadowRadiusChanged();
    // Reset the cache.
    _dropShadowCache.frameSize = {};
    _dropShadowCache.shadowPixmap = {};
    // Update everything.
    updateDropShadowMargins();
    updateGeometry();
    update();
  }
}

const QPointF& Popover::dropShadowOffset() const {
  return _dropShadowOffset;
}

void Popover::setDropShadowOffset(const QPointF& offset) {
  if (offset != _dropShadowOffset) {
    _dropShadowOffset = offset;
    Q_EMIT dropShadowOffsetChanged();
    // Reset the cache.
    _dropShadowCache.frameSize = {};
    _dropShadowCache.shadowPixmap = {};
    // Update everything.
    updateDropShadowMargins();
    updateGeometry();
    update();
  }
}

bool Popover::canBeOverAnchor() const {
  return _canBeOverAnchor;
}

void Popover::setCanBeOverAnchor(bool value) {
  if (value != _canBeOverAnchor) {
    _canBeOverAnchor = value;
    Q_EMIT canBeOverAnchorChanged();
    updateGeometry();
  }
}

bool Popover::deleteContentAfterClosing() const {
  return _deleteContentAfterClosing;
}

void Popover::setDeleteContentAfterClosing(bool value) {
  if (value != _deleteContentAfterClosing) {
    _deleteContentAfterClosing = value;
    Q_EMIT deleteContentAfterClosingChanged();
  }
}

const QColor& Popover::backgroundColor() const {
  return _backgroundColor;
}

void Popover::setBackgroundColor(const QColor& color) {
  if (color != _backgroundColor) {
    _backgroundColor = color;
    Q_EMIT backgroundColorChanged();
    update();
  }
}

const QColor& Popover::borderColor() const {
  return _borderColor;
}

void Popover::setBorderColor(const QColor& color) {
  if (color != _borderColor) {
    _borderColor = color;
    Q_EMIT borderColorChanged();
    update();
  }
}

bool Popover::contentMaskEnabled() const {
  return _contentMaskEnabled;
}

void Popover::setContentMaskEnabled(bool enabled) {
  if (enabled != _contentMaskEnabled) {
    _contentMaskEnabled = enabled;
    Q_EMIT contentMaskEnabledChanged();
  }
}

void Popover::openPopover() {
  setOpened(true);
  updatePopoverGeometry();
}

void Popover::closePopover() {
  setOpened(false);
}

void Popover::togglePopover() {
  if (isOpened()) {
    closePopover();
  } else {
    openPopover();
  }
}

void Popover::adjustSizeToContent() {
  if (_content) {
    _content->ensurePolished();
    _content->updateGeometry();
  }
  _frame->adjustSize();
  adjustSize();
}

void Popover::paintEvent(QPaintEvent*) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setRenderHint(QPainter::SmoothPixmapTransform, true);

  const auto shapePixmap = getFrameShape();

  // Drop shadow.
  if (_shouldDrawDropShadow) {
    // Update cache if necessary.
    const auto frameSize = _frame->size();
    const auto frameX = _frame->x();
    const auto frameY = _frame->y();
    if (_dropShadowCache.frameSize != frameSize) {
      updateDropShadowCache();
    }
    const auto dropShadowSize = _dropShadowCache.shadowPixmap.deviceIndependentSize();
    const auto dropShadowX = frameX + (frameSize.width() - dropShadowSize.width()) / 2. + _dropShadowOffset.x();
    const auto dropShadowY = frameY + (frameSize.height() - dropShadowSize.height()) / 2. + _dropShadowOffset.y();

    const auto compModeBackup = p.compositionMode();
    p.setCompositionMode(QPainter::CompositionMode::CompositionMode_Multiply);
    p.drawPixmap(dropShadowX, dropShadowY, _dropShadowCache.shadowPixmap);
    p.setCompositionMode(compModeBackup);
  }

  // Frame.
  {
    const auto bgRect = _frame->rect().translated(_frame->mapTo(this, QPoint{ 0, 0 }));

    if (_backgroundColor.isValid()) {
      p.setPen(Qt::NoPen);
      p.setBrush(_backgroundColor);
      p.drawRoundedRect(bgRect, _radius, _radius);
    }

    if (_borderColor.isValid() && _borderWidth > 0.) {
      const auto half_border = _borderWidth / 2.;
      const auto border_rect = bgRect.toRectF().adjusted(half_border, half_border, -half_border, -half_border);
      const auto border_radius = _radius - half_border;
      p.setPen(QPen{ _borderColor, _borderWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin });
      p.setBrush(Qt::NoBrush);
      p.drawRoundedRect(border_rect, border_radius, border_radius);
    }
  }
}

void Popover::mousePressEvent(QMouseEvent* e) {
  QWidget::mousePressEvent(e);

  if (hitboxContainsPoint(e->position())) {
    Q_EMIT pressed();
  } else {
    e->ignore();
    closePopover();
  }
}

void Popover::mouseReleaseEvent(QMouseEvent* e) {
  QWidget::mouseReleaseEvent(e);

  if (hitboxContainsPoint(e->position())) {
    Q_EMIT released();
  } else {
    e->ignore();
    closePopover();
  }
}

void Popover::hideEvent(QHideEvent* e) {
  QWidget::hideEvent(e);
  setOpened(false);
  if (_deleteContentAfterClosing && _content) {
    _content->deleteLater();
    _content = nullptr;
  }
}

void Popover::showEvent(QShowEvent* e) {
  QWidget::showEvent(e);
  setOpened(true);

  if (_anchorWidget) {
    // Qt bug: the widget stays in hover state.
    QEvent leaveEvent(QEvent::Type::Leave);
    QApplication::sendEvent(_anchorWidget, &leaveEvent);
    _anchorWidget->update();
  }
}

QMargins Popover::dropShadowMargins() const {
  return layout()->contentsMargins();
}

void Popover::updateDropShadowMargins() {
  if (_shouldDrawDropShadow) {
    const auto spaceForShadow = qlementine::blurRadiusNecessarySpace(_dropShadowRadius);
    const auto margins = QMargins{
      static_cast<int>(std::ceil(spaceForShadow - _dropShadowOffset.x())), // left
      static_cast<int>(std::ceil(spaceForShadow - _dropShadowOffset.y())), // top
      static_cast<int>(std::ceil(spaceForShadow + _dropShadowOffset.x())), // right
      static_cast<int>(std::ceil(spaceForShadow + _dropShadowOffset.y())), // bottom
    };
    layout()->setContentsMargins(margins);
  } else {
    layout()->setContentsMargins(0, 0, 0, 0);
  }
}

void Popover::updateDropShadowCache() {
  if (_shouldDrawDropShadow) {
    const auto framePixmap = getFrameShape();
    _dropShadowCache.frameSize = _frame->size();
    _dropShadowCache.shadowPixmap = qlementine::getDropShadowPixmap(framePixmap, _dropShadowRadius, _dropShadowColor);
  } else {
    _dropShadowCache.frameSize = { 0, 0 };
    _dropShadowCache.shadowPixmap = {};
  }
}

void Popover::updateFrameMask() {
  _frame->clearMask();
  // Prevents widgets from overflowing into rounded corners.
  // NB: There won't be any antialiasing because this is a limitation of Qt.
  if (!_frame->layout()->isEmpty() && _contentMaskEnabled) {
    const auto mask = getFrameMask();
    _frame->setMask(mask);
  }
}

void Popover::updatePopoverGeometry() {
  // First of all, ensure the popup fits its content.
  adjustSizeToContent();

  if (_manualPositioning) {
    // Only resizing, no positioning.
    const auto popoverFrameSize = _frame->sizeHint();
    const auto dropShadowMargins = this->dropShadowMargins();
    const auto popoverTotalSize = QSize(popoverFrameSize.width() + dropShadowMargins.left() + dropShadowMargins.right(),
      popoverFrameSize.height() + dropShadowMargins.top() + dropShadowMargins.bottom());
    Q_EMIT manualPositionRequested();
    auto topLeft = geometry().topLeft();
    if (_manualPositioningCb) {
      // Start from the frame geometry,
      // and get the total popup geometry by including the drop shadow margins.
      topLeft = _manualPositioningCb(popoverFrameSize);
      topLeft.rx() -= dropShadowMargins.left();
      topLeft.ry() -= dropShadowMargins.top();
    }
    const auto geometry = QRect(topLeft, popoverTotalSize);
    setGeometry(geometry);
  } else {
    // Check if the preferred position fits entirely on screen, or try another position until it works.
    const auto& priority = positionPriority(_preferredPosition);
    const auto screenGeometry =
      _anchorWidget ? _anchorWidget->screen()->availableGeometry() : screen()->availableGeometry();
    for (const auto position : priority) {
      auto geometry = getGeometryForPosition(position, _preferredAlignment);

      // Success: the popup fits on screen.
      const auto aboveAnchor =
        _anchorWidget
          ? _anchorWidget->rect().translated(_anchorWidget->mapToGlobal(QPoint{ 0, 0 })).intersects(geometry)
          : false;
      const auto allowed = aboveAnchor ? _canBeOverAnchor : true;
      if (screenGeometry.contains(geometry) && allowed) {
        // Now, translate it to include the drop shadow.
        const auto dropShadowMargins = this->dropShadowMargins();
        geometry.translate(-dropShadowMargins.left(), -dropShadowMargins.top());
        setGeometry(geometry);
        return;
      }
    }

    // If it is impossible to respect any position, show it centered on screen.
    const auto geometry = getFallbackGeometry();
    setGeometry(geometry);
  }
}

const std::array<Popover::Position, 4>& Popover::positionPriority(Position const position) {
  static const auto leftPriority =
    std::array<Position, 4>{ Position::Left, Position::Right, Position::Top, Position::Bottom };
  static const auto rightPriority =
    std::array<Position, 4>{ Position::Right, Position::Left, Position::Top, Position::Bottom };
  static const auto topPriority =
    std::array<Position, 4>{ Position::Top, Position::Bottom, Position::Left, Position::Right };
  static const auto bottomPriority =
    std::array<Position, 4>{ Position::Bottom, Position::Top, Position::Left, Position::Right };
  switch (position) {
    case Position::Left:
      return leftPriority;
    case Position::Right:
      return rightPriority;
    case Position::Top:
      return topPriority;
    case Position::Bottom:
    default:
      return bottomPriority;
  }
}

QRect Popover::getGeometryForPosition(Position const position, Alignment const alignment) const {
  // Take the content size as a basis, rather than the whole size that includes drop shadow.
  const auto popoverSize = _frame->sizeHint();
  const auto screenGeometry = _anchorWidget
                                ? _anchorWidget->screen()->availableGeometry().marginsRemoved(_screenPadding)
                                : screen()->availableGeometry().marginsRemoved(_screenPadding);
  const auto popoverFittedSize = QSize{
    std::min(screenGeometry.width(), popoverSize.width()),
    std::min(screenGeometry.height(), popoverSize.height()),
  };
  QRect result;

  if (_anchorWidget) {
    QPoint topLeft;
    switch (position) {
      case Position::Left: {
        switch (alignment) {
          case Alignment::Begin: {
            const auto anchorPoint = _anchorWidget->mapToGlobal(QPoint{ 0, 0 });
            const auto x = anchorPoint.x() - _horizontalSpacing - popoverFittedSize.width();
            const auto y = anchorPoint.y() + _verticalSpacing;
            topLeft = { x, y };
          } break;
          case Alignment::Center: {
            const auto anchorPoint = _anchorWidget->mapToGlobal(QPoint{ 0, _anchorWidget->height() / 2 });
            const auto x = anchorPoint.x() - _horizontalSpacing - popoverFittedSize.width();
            const auto y = anchorPoint.y() - popoverSize.height() / 2;
            topLeft = { x, y };
          } break;
          case Alignment::End: {
            const auto anchorPoint = _anchorWidget->mapToGlobal(QPoint{ 0, _anchorWidget->height() });
            const auto x = anchorPoint.x() - _horizontalSpacing - popoverFittedSize.width();
            const auto y = anchorPoint.y() - popoverSize.height() + _verticalSpacing;
            topLeft = { x, y };
          } break;
          default:
            break;
        }
      } break;
      case Position::Right: {
        switch (alignment) {
          case Alignment::Begin: {
            const auto anchorPoint = _anchorWidget->mapToGlobal(QPoint{ _anchorWidget->width(), 0 });
            const auto x = anchorPoint.x() + _horizontalSpacing;
            const auto y = anchorPoint.y() + _verticalSpacing;
            topLeft = { x, y };
          } break;
          case Alignment::Center: {
            const auto anchorPoint =
              _anchorWidget->mapToGlobal(QPoint{ _anchorWidget->width(), _anchorWidget->height() / 2 });
            const auto x = anchorPoint.x() + _horizontalSpacing;
            const auto y = anchorPoint.y() - popoverSize.height() / 2;
            topLeft = { x, y };
          } break;
          case Alignment::End: {
            const auto anchorPoint =
              _anchorWidget->mapToGlobal(QPoint{ _anchorWidget->width(), _anchorWidget->height() });
            const auto x = anchorPoint.x() + _horizontalSpacing;
            const auto y = anchorPoint.y() - popoverSize.height() + _verticalSpacing;
            topLeft = { x, y };
          } break;
          default:
            break;
        }
      } break;
      case Position::Top: {
        switch (alignment) {
          case Alignment::Begin: {
            const auto anchorPoint = _anchorWidget->mapToGlobal(QPoint{ 0, 0 });
            const auto x = anchorPoint.x() + _horizontalSpacing;
            const auto y = anchorPoint.y() - _verticalSpacing - popoverFittedSize.height();
            topLeft = { x, y };
          } break;
          case Alignment::Center: {
            const auto anchorPoint = _anchorWidget->mapToGlobal(QPoint{ _anchorWidget->width() / 2, 0 });
            const auto x = anchorPoint.x() - popoverFittedSize.width() / 2;
            const auto y = anchorPoint.y() - _verticalSpacing - popoverFittedSize.height();
            topLeft = { x, y };
          } break;
          case Alignment::End: {
            const auto anchorPoint = _anchorWidget->mapToGlobal(QPoint{ _anchorWidget->width(), 0 });
            const auto x = anchorPoint.x() + _horizontalSpacing;
            const auto y = anchorPoint.y() - _verticalSpacing - popoverFittedSize.height();
            topLeft = { x, y };
          } break;
          default:
            break;
        }
      } break;
      case Position::Bottom: {
        switch (alignment) {
          case Alignment::Begin: {
            const auto anchorPoint = _anchorWidget->mapToGlobal(QPoint{ 0, _anchorWidget->height() });
            const auto x = anchorPoint.x() + _horizontalSpacing;
            const auto y = anchorPoint.y() + _verticalSpacing;
            topLeft = { x, y };
          } break;
          case Alignment::Center: {
            const auto anchorPoint =
              _anchorWidget->mapToGlobal(QPoint{ _anchorWidget->width() / 2, _anchorWidget->height() });
            const auto x = anchorPoint.x() - popoverFittedSize.width() / 2;
            const auto y = anchorPoint.y() + _verticalSpacing;
            topLeft = { x, y };
          } break;
          case Alignment::End: {
            const auto anchorPoint =
              _anchorWidget->mapToGlobal(QPoint{ _anchorWidget->width(), _anchorWidget->height() });
            const auto x = anchorPoint.x() + _horizontalSpacing;
            const auto y = anchorPoint.y() + _verticalSpacing;
            topLeft = { x, y };
          } break;
          default:
            break;
        }
      } break;
      default:
        break;
    }
    result = QRect(topLeft, popoverFittedSize);
  } else {
    result = { QPoint{ _screenPadding.left(), _screenPadding.top() }, popoverFittedSize };
  }

  // Make sure the bounds fit the screen.
  const auto yDiff = (result.y() + result.height()) - (screenGeometry.y() + screenGeometry.height());
  if (yDiff > 0) {
    result.translate(0, -yDiff);
  } else if (result.y() < _screenPadding.top()) {
    result.translate(0, -result.y() + _screenPadding.top());
  }

  const auto xDiff = (result.x() + result.width()) - (screenGeometry.x() + screenGeometry.width());
  if (xDiff > 0) {
    result.translate(-xDiff, 0);
  } else if (result.x() < _screenPadding.left()) {
    result.translate(-result.x() + _screenPadding.left(), 0);
  }

  return result;
}

QRect Popover::getFallbackGeometry() const {
  const auto popoverSize = _frame->sizeHint();
  const auto screenGeometry = screen()->availableGeometry().marginsRemoved(_screenPadding);
  const auto popoverFittedSize = QSize{
    std::min(screenGeometry.width(), popoverSize.width()),
    std::min(screenGeometry.height(), popoverSize.height()),
  };

  const auto dropShadowMargins = layout()->contentsMargins();
  const auto x =
    screenGeometry.x() + (screenGeometry.width() - popoverFittedSize.width()) / 2 - dropShadowMargins.left();
  const auto y =
    screenGeometry.y() + (screenGeometry.height() - popoverFittedSize.height()) / 2 - dropShadowMargins.top();
  return { QPoint{ x, y }, popoverFittedSize };
}

void Popover::startAnimation() {
  const auto currentOpacity = _opacityAnimation.currentValue().toDouble();
  _opacityAnimation.stop();
  const auto duration = _animated ? style()->styleHint(QStyle::SH_Widget_Animation_Duration)
                                      * (_opened ? openAnimationDurationFactor : closeAnimationDurationFactor)
                                  : 0;
  _opacityAnimation.setDuration(duration);
  _opacityAnimation.setStartValue(QVariant::fromValue<double>(currentOpacity));
  _opacityAnimation.setEndValue(QVariant::fromValue<double>(_opened ? 1. : 0.));
  _opacityAnimation.start();
}

QPixmap Popover::getFrameShape() const {
  // Used for getting the initial shape the drop shadow is computed from.
  // Ensure the pixmap is the same size as the widget.
  const auto frameSize = _frame->size();
  const auto pixelRatio = _frame->devicePixelRatioF();
  const auto physicalSize = frameSize * pixelRatio;
  const auto physicalRect = QRect{ QPoint{ 0, 0 }, physicalSize };
  const auto phyicalRadius = _radius * pixelRatio;

  QPixmap result(physicalSize);
  result.fill(Qt::transparent);

  // Draw the frame.
  {
    QPainter p(&result);
    p.setRenderHint(QPainter::RenderHint::Antialiasing, true);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.drawRoundedRect(physicalRect, phyicalRadius, phyicalRadius);
  }

  result.setDevicePixelRatio(pixelRatio);

  return result;
}

QBitmap Popover::getFrameMask() const {
  // The mask doesn't to be pixel ratio aware.
  const auto logicalSize = _frame->size();
  const auto maskRect =
    QRectF(QPointF{ 0., 0. }, logicalSize).marginsRemoved({ _borderWidth, _borderWidth, _borderWidth, _borderWidth });
  const auto maskRadius = std::max(0., _radius - _borderWidth);
  QPixmap mask(logicalSize);
  {
    mask.fill(Qt::white);
    QPainter p(&mask);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.drawRoundedRect(maskRect, maskRadius, maskRadius);
  }

  const auto bitmap = QBitmap::fromPixmap(mask);
  return bitmap;
}

bool Popover::hitboxContainsPoint(const QPointF& pos) const {
  const auto& frameRect = _frame->geometry().toRectF();
  return qlementine::isPointInRoundedRect(pos, frameRect, _radius);
}
} // namespace oclero::qlementine
