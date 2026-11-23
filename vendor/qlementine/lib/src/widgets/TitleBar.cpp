// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/TitleBar.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/Theme.hpp>
#include <oclero/qlementine/widgets/AbstractItemListWidget.hpp>
#include <oclero/qlementine/widgets/IconWidget.hpp>
#include <oclero/qlementine/widgets/NotificationBadge.hpp>
#include <oclero/qlementine/widgets/StatusBadgeWidget.hpp>

#include "TitleBarRenderer.hpp"

#include <QApplication>
#include <QEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QLCDNumber>
#include <QLabel>
#include <QMenuBar>
#include <QMouseEvent>
#include <QPainter>
#include <QPointer>
#include <QProgressBar>
#include <QTabBar>
#include <QToolBar>

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

namespace oclero::qlementine {
namespace {
class CallbackEventFilter : public QObject {
public:
  explicit CallbackEventFilter(std::function<void(QObject*, QEvent*)>&& callback)
    : _callback(std::move(callback)) {}

  bool eventFilter(QObject* watched, QEvent* evt) override {
    _callback(watched, evt);
    return false;
  }

private:
  std::function<void(QObject*, QEvent*)> _callback;
};
} // namespace

struct TitleBar::Impl {
  explicit Impl(TitleBar& titleBar)
    : owner(titleBar) {}

  const Theme& theme() const {
    const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(owner.style());
    if (qlementineStyle) {
      return qlementineStyle->theme();
    }
    static const Theme fallbackTheme = Theme::makeLight();
    return fallbackTheme;
  }

  int groupWidth(const QList<CaptionButton>& buttons) const {
    auto width = 0;
    for (const auto button : buttons) {
      width += renderer->captionButtonSize(button, theme()).width();
    }
    return buttons.isEmpty() ? 0 : width + renderer->captionButtonSpacing() * (static_cast<int>(buttons.size()) - 1);
  }

  int buttonsWidth() const {
    if (!captionButtonsVisible) {
      return 0;
    }
    const auto margins = renderer->captionButtonMargins();
    return groupWidth(buttonLayout.left) + groupWidth(buttonLayout.right) + margins.left() + margins.right();
  }

  QRect contentArea() const {
    auto left = contentAfterLeading ? std::max(leftInset, leadingWidth) : leftInset;
    auto right = owner.width() - rightInset;
    const auto gap = theme().spacing;
    for (const auto button : buttonLayout.left) {
      const auto rect = owner.captionButtonRect(button);
      if (!rect.isEmpty()) {
        left = std::max(left, rect.right() + 1 + gap);
      }
    }
    for (const auto button : buttonLayout.right) {
      const auto rect = owner.captionButtonRect(button);
      if (!rect.isEmpty()) {
        right = std::min(right, rect.left() - gap);
      }
    }
    return QRect(left, 0, std::max(0, right - left), owner.height());
  }

  void updateLayout() {
    const auto area = contentArea();
    if (!contentWidget) {
      container->setGeometry(QRect(area.left(), 0, 0, area.height()));
      return;
    }
    const auto expanding = contentWidget->sizePolicy().horizontalPolicy() == QSizePolicy::Expanding;
    const auto width =
      expanding ? area.width() : std::min(area.width(), std::max(0, contentWidget->sizeHint().width()));
    container->setGeometry(QRect(area.left(), 0, width, area.height()));
  }

  void refreshWindowState() {
    const auto* window = owner.window();
    const auto newMaximized = window->isMaximized() || window->isFullScreen();
    const auto newActive = window->isActiveWindow();
    if (newMaximized != maximized || newActive != active) {
      maximized = newMaximized;
      active = newActive;
      owner.update();
    }
  }

  void trackWindow() {
    auto* window = owner.window();
    if (window == trackedWindow.data()) {
      return;
    }
    if (trackedWindow) {
      trackedWindow->removeEventFilter(eventFilter.get());
    }
    trackedWindow = window;
    window->installEventFilter(eventFilter.get());
    refreshWindowState();
  }

  void connectToStyle() {
    QObject::disconnect(themeConnection);
    const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(owner.style());
    if (qlementineStyle) {
      themeConnection = QObject::connect(qlementineStyle, &QlementineStyle::themeChanged, &owner, [this]() {
        owner.update();
      });
    }
  }

  TitleBar& owner;
  std::unique_ptr<TitleBarRenderer> renderer{ createQlementineTitleBarRenderer() };
  std::unique_ptr<CallbackEventFilter> eventFilter;
  QMetaObject::Connection themeConnection;
  QPointer<QWidget> trackedWindow;
  QWidget* container{ nullptr };
  QPointer<QWidget> contentWidget;
  std::vector<std::pair<QPointer<QWidget>, bool>> hitTestOverrides;

  QString title;
  TitleBarStyle style{ TitleBarStyle::Qlementine };
  bool captionButtonsVisible{ true };
  CaptionButtonLayout buttonLayout{ {}, { CaptionButton::Minimize, CaptionButton::Maximize, CaptionButton::Close } };
  int leftInset{ 0 };
  int rightInset{ 0 };
  int leadingWidth{ 0 };
  bool contentAfterLeading{ true };
  std::optional<CaptionButton> hoveredButton;
  std::optional<CaptionButton> pressedButton;
  std::optional<QPoint> dragStartPos;
  bool maximized{ false };
  bool active{ true };
};

TitleBar::TitleBar(QWidget* parent)
  : QWidget(parent)
  , _impl(std::make_unique<Impl>(*this)) {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setMouseTracking(true);

  // Container of the content widget.
  _impl->container = new QWidget(this);
  auto* containerLayout = new QHBoxLayout(_impl->container);
  containerLayout->setContentsMargins(0, 0, 0, 0);
  containerLayout->setSpacing(0);

  // Window state and layout tracking.
  _impl->eventFilter = std::make_unique<CallbackEventFilter>([this](QObject* watched, QEvent* evt) {
    const auto type = evt->type();
    if (watched == _impl->trackedWindow.data()) {
      if (type == QEvent::WindowStateChange || type == QEvent::ActivationChange || type == QEvent::WindowActivate
          || type == QEvent::WindowDeactivate) {
        _impl->refreshWindowState();
      }
    } else if (watched == _impl->container && type == QEvent::LayoutRequest) {
      _impl->updateLayout();
      updateGeometry();
    }
  });
  _impl->container->installEventFilter(_impl->eventFilter.get());

  _impl->connectToStyle();
  _impl->trackWindow();
}

TitleBar::~TitleBar() = default;

QString TitleBar::title() const {
  return _impl->title;
}

void TitleBar::setTitle(const QString& title) {
  if (title != _impl->title) {
    _impl->title = title;
    update();
    Q_EMIT titleChanged();
  }
}

QWidget* TitleBar::contentWidget() const {
  return _impl->contentWidget.data();
}

void TitleBar::setContentWidget(QWidget* widget) {
  if (widget == _impl->contentWidget.data()) {
    return;
  }

  if (_impl->contentWidget) {
    _impl->container->layout()->removeWidget(_impl->contentWidget.data());
    _impl->contentWidget->hide();
    _impl->contentWidget->setParent(nullptr);
  }

  _impl->contentWidget = widget;
  if (widget) {
    _impl->container->layout()->addWidget(widget);
  }
  _impl->updateLayout();
  updateGeometry();
}

TitleBarStyle TitleBar::titleBarStyle() const {
  return _impl->style;
}

void TitleBar::setTitleBarStyle(TitleBarStyle style) {
  if (style == _impl->style) {
    return;
  }

  _impl->style = style;
  _impl->renderer =
    style == TitleBarStyle::Adwaita ? createAdwaitaTitleBarRenderer() : createQlementineTitleBarRenderer();
  updateGeometry();
  _impl->updateLayout();
  update();
}

bool TitleBar::captionButtonsVisible() const {
  return _impl->captionButtonsVisible;
}

void TitleBar::setCaptionButtonsVisible(bool visible) {
  if (visible != _impl->captionButtonsVisible) {
    _impl->captionButtonsVisible = visible;
    updateGeometry();
    _impl->updateLayout();
    update();
    Q_EMIT captionButtonsVisibleChanged();
  }
}

CaptionButtonLayout TitleBar::captionButtonLayout() const {
  return _impl->buttonLayout;
}

void TitleBar::setCaptionButtonLayout(const CaptionButtonLayout& layout) {
  _impl->buttonLayout = layout;
  updateGeometry();
  _impl->updateLayout();
  update();
}

int TitleBar::leftInset() const {
  return _impl->leftInset;
}

int TitleBar::rightInset() const {
  return _impl->rightInset;
}

void TitleBar::setSystemInsets(int left, int right) {
  if (left != _impl->leftInset || right != _impl->rightInset) {
    _impl->leftInset = left;
    _impl->rightInset = right;
    updateGeometry();
    _impl->updateLayout();
    update();
  }
}

int TitleBar::leadingWidth() const {
  return _impl->leadingWidth;
}

bool TitleBar::contentAfterLeading() const {
  return _impl->contentAfterLeading;
}

void TitleBar::setContentAfterLeading(bool after) {
  if (after != _impl->contentAfterLeading) {
    _impl->contentAfterLeading = after;
    _impl->updateLayout();
    update();
  }
}

void TitleBar::setLeadingWidth(int width) {
  width = std::max(0, width);
  if (width != _impl->leadingWidth) {
    _impl->leadingWidth = width;
    _impl->updateLayout();
    update();
  }
}

void TitleBar::setHitTestVisible(QWidget* widget, bool visible) {
  auto& overrides = _impl->hitTestOverrides;
  overrides.erase(std::remove_if(overrides.begin(), overrides.end(),
                    [widget](const auto& entry) {
                      return entry.first.isNull() || entry.first.data() == widget;
                    }),
    overrides.end());
  if (widget) {
    overrides.emplace_back(widget, visible);
  }
}

bool TitleBar::isDragArea(const QPoint& pos) const {
  if (!rect().contains(pos) || captionButtonAt(pos).has_value()) {
    return false;
  }

  // Interactive children of the content widget.
  const auto* container = _impl->container;
  if (!container->isHidden() && container->geometry().contains(pos)) {
    const auto* child = container->childAt(container->mapFrom(this, pos));
    for (const auto* widget = child; widget && widget != container; widget = widget->parentWidget()) {
      for (const auto& entry : _impl->hitTestOverrides) {
        if (entry.first.data() == widget) {
          return !entry.second;
        }
      }
    }

    // Widgets with items: only the empty space is draggable.
    for (const auto* widget = child; widget && widget != container; widget = widget->parentWidget()) {
      if (const auto* tabBar = qobject_cast<const QTabBar*>(widget)) {
        return child == tabBar && tabBar->tabAt(tabBar->mapFrom(this, pos)) == -1;
      }
      if (const auto* menuBar = qobject_cast<const QMenuBar*>(widget)) {
        return child == menuBar && menuBar->actionAt(menuBar->mapFrom(this, pos)) == nullptr;
      }
      if (const auto* toolBar = qobject_cast<const QToolBar*>(widget)) {
        return child == toolBar && toolBar->actionAt(toolBar->mapFrom(this, pos)) == nullptr;
      }
      if (const auto* itemList = qobject_cast<const AbstractItemListWidget*>(widget)) {
        return child == itemList && itemList->itemAtPos(itemList->mapFrom(this, pos)) == -1;
      }
    }

    // Display-only widgets and plain containers don't handle the mouse, the interactive widgets inside a container
    // are returned by childAt().
    if (child == nullptr) {
      return true;
    }
    const auto* metaObject = child->metaObject();
    return metaObject == &QWidget::staticMetaObject || metaObject == &QFrame::staticMetaObject
           || qobject_cast<const QLabel*>(child) || qobject_cast<const QProgressBar*>(child)
           || qobject_cast<const QLCDNumber*>(child) || qobject_cast<const IconWidget*>(child)
           || qobject_cast<const StatusBadgeWidget*>(child) || qobject_cast<const NotificationBadge*>(child);
  }

  return true;
}

std::optional<CaptionButton> TitleBar::captionButtonAt(const QPoint& pos) const {
  for (const auto button :
    { CaptionButton::Close, CaptionButton::Minimize, CaptionButton::Maximize, CaptionButton::Menu }) {
    if (captionButtonRect(button).contains(pos)) {
      return button;
    }
  }
  return std::nullopt;
}

QRect TitleBar::captionButtonRect(CaptionButton button) const {
  if (!_impl->captionButtonsVisible) {
    return {};
  }

  const auto& renderer = *_impl->renderer;
  const auto margins = renderer.captionButtonMargins();
  const auto spacing = renderer.captionButtonSpacing();
  auto buttonSize = renderer.captionButtonSize(button, _impl->theme());
  if (buttonSize.height() >= renderer.barHeight(_impl->theme())) {
    buttonSize.setHeight(height() - margins.top() - margins.bottom());
  }

  for (const auto isLeft : { true, false }) {
    const auto& buttons = isLeft ? _impl->buttonLayout.left : _impl->buttonLayout.right;
    const auto index = buttons.indexOf(button);
    if (index < 0) {
      continue;
    }

    auto x = isLeft ? _impl->leftInset + margins.left()
                    : width() - _impl->rightInset - margins.right() - _impl->groupWidth(buttons);
    for (auto i = 0; i < index; ++i) {
      x += renderer.captionButtonSize(buttons[i], _impl->theme()).width() + spacing;
    }
    const auto y = margins.top() + (height() - margins.top() - margins.bottom() - buttonSize.height()) / 2;
    return QRect(QPoint(x, y), buttonSize);
  }

  return {};
}

void TitleBar::setHoveredCaptionButton(std::optional<CaptionButton> button) {
  if (button != _impl->hoveredButton) {
    _impl->hoveredButton = button;
    update();
  }
}

void TitleBar::setPressedCaptionButton(std::optional<CaptionButton> button) {
  if (button != _impl->pressedButton) {
    _impl->pressedButton = button;
    update();
  }
}

QSize TitleBar::minimumSizeHint() const {
  auto height = _impl->renderer->barHeight(_impl->theme());
  if (qobject_cast<QToolBar*>(_impl->contentWidget.data())) {
    height = std::max(height, _impl->contentWidget->sizeHint().height());
  }
  return QSize(_impl->leftInset + _impl->rightInset + _impl->buttonsWidth(), height);
}

QSize TitleBar::sizeHint() const {
  const auto contentWidth = _impl->contentWidget ? std::max(0, _impl->contentWidget->sizeHint().width()) : 0;
  return minimumSizeHint() + QSize(contentWidth, 0);
}

bool TitleBar::event(QEvent* evt) {
  const auto result = QWidget::event(evt);
  switch (evt->type()) {
    case QEvent::ParentChange:
    case QEvent::Show:
      _impl->trackWindow();
      break;
    case QEvent::StyleChange:
      _impl->connectToStyle();
      updateGeometry();
      update();
      break;
    default:
      break;
  }
  return result;
}

void TitleBar::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);

  const auto& theme = _impl->theme();
  const auto& renderer = *_impl->renderer;

  // Background. It starts after the leading area, or at the left edge when the content is displayed over the leading
  // area, so that the whole row has the same background. A toolbar paints its own background, so the area under the
  // window controls keeps the leading area background in that case.
  const auto backgroundLeft = _impl->contentAfterLeading || qobject_cast<QToolBar*>(_impl->contentWidget.data())
                                ? _impl->leadingWidth
                                : 0;
  painter.save();
  painter.setClipRect(QRect(backgroundLeft, 0, std::max(0, width() - backgroundLeft), height()));
  renderer.paintBackground(painter, rect(), theme, _impl->active, _impl->maximized);
  painter.restore();

  // Title, in the space left by the content widget.
  const auto area = _impl->contentArea();
  const auto titleLeft = area.left() + (_impl->contentWidget ? _impl->container->width() : 0);
  renderer.paintTitle(painter, QRect(titleLeft, 0, std::max(0, area.right() + 1 - titleLeft), height()), _impl->title,
    theme, _impl->active);

  // Caption buttons.
  for (const auto button :
    { CaptionButton::Close, CaptionButton::Minimize, CaptionButton::Maximize, CaptionButton::Menu }) {
    const auto buttonRect = captionButtonRect(button);
    if (!buttonRect.isEmpty()) {
      const CaptionButtonState state{
        _impl->hoveredButton == button,
        _impl->pressedButton == button,
        _impl->active,
        _impl->maximized,
      };
      renderer.paintCaptionButton(painter, buttonRect, button, state, theme);
    }
  }
}

void TitleBar::resizeEvent(QResizeEvent* evt) {
  QWidget::resizeEvent(evt);
  _impl->updateLayout();
}

void TitleBar::mousePressEvent(QMouseEvent* evt) {
  const auto pos = evt->position().toPoint();
  if (evt->button() == Qt::LeftButton) {
    if (const auto button = captionButtonAt(pos)) {
      setPressedCaptionButton(button);
      evt->accept();
      return;
    }
    if (isDragArea(pos)) {
      _impl->dragStartPos = pos;
      evt->accept();
      return;
    }
  } else if (evt->button() == Qt::RightButton && isDragArea(pos)) {
    evt->accept();
    Q_EMIT menuRequested(evt->globalPosition().toPoint());
    return;
  }
  QWidget::mousePressEvent(evt);
}

void TitleBar::mouseMoveEvent(QMouseEvent* evt) {
  const auto pos = evt->position().toPoint();
  setHoveredCaptionButton(captionButtonAt(pos));

  // The move starts once the cursor has left the press position, so that a double-click stays possible.
  if (_impl->dragStartPos.has_value() && (evt->buttons() & Qt::LeftButton)
      && (pos - _impl->dragStartPos.value()).manhattanLength() > QApplication::startDragDistance()) {
    _impl->dragStartPos.reset();
    evt->accept();
    Q_EMIT moveRequested();
    return;
  }
  QWidget::mouseMoveEvent(evt);
}

void TitleBar::mouseReleaseEvent(QMouseEvent* evt) {
  _impl->dragStartPos.reset();
  if (evt->button() == Qt::LeftButton && _impl->pressedButton.has_value()) {
    const auto pressedButton = _impl->pressedButton.value();
    setPressedCaptionButton(std::nullopt);
    evt->accept();

    if (captionButtonAt(evt->position().toPoint()) == pressedButton) {
      switch (pressedButton) {
        case CaptionButton::Close:
          Q_EMIT closeRequested();
          break;
        case CaptionButton::Minimize:
          Q_EMIT minimizeRequested();
          break;
        case CaptionButton::Maximize:
          Q_EMIT maximizeRequested();
          break;
        case CaptionButton::Menu:
          Q_EMIT menuRequested(evt->globalPosition().toPoint());
          break;
      }
    }
    return;
  }
  QWidget::mouseReleaseEvent(evt);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent* evt) {
  const auto pos = evt->position().toPoint();
  if (evt->button() == Qt::LeftButton) {
    if (captionButtonAt(pos)) {
      mousePressEvent(evt);
      return;
    }
    if (isDragArea(pos)) {
      evt->accept();
      Q_EMIT doubleClicked();
      return;
    }
  }
  QWidget::mouseDoubleClickEvent(evt);
}

void TitleBar::leaveEvent(QEvent* evt) {
  QWidget::leaveEvent(evt);
  _impl->dragStartPos.reset();
  setHoveredCaptionButton(std::nullopt);
}
} // namespace oclero::qlementine
