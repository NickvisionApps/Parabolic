// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/UnifiedWindow.hpp>

#include <oclero/qlementine/style/QlementineStyle.hpp>

#include "utils/window/WindowBackend.hpp"

#include <QEvent>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPointer>
#include <QSpacerItem>
#include <QVBoxLayout>

#include <algorithm>

namespace oclero::qlementine {
namespace {
// Transparent-for-mouse overlay that re-draws the frame ring (corners and 1px border) on top of the content.
class FrameOverlay : public QWidget {
public:
  FrameOverlay(QWidget* parent, WindowBackend& backend)
    : QWidget(parent)
    , _backend(backend) {
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
  }

protected:
  void paintEvent(QPaintEvent*) override {
    const auto radius = _backend.cornerRadius();
    const auto frame = QRectF(rect().marginsRemoved(parentWidget()->contentsMargins()));

    // Ring between the frame rectangle and the rounded rectangle inset by 1px.
    auto inner = QPainterPath{};
    inner.addRoundedRect(frame.adjusted(1, 1, -1, -1), radius - 1, radius - 1);
    auto ring = QPainterPath{};
    ring.addRect(frame);
    ring = ring.subtracted(inner);

    // Erase the content in the ring, then let the backend draw the shadow, the border and the corners again.
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setClipPath(ring);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    _backend.paintFrame(painter);
  }

private:
  WindowBackend& _backend;
};

// Transparent-for-mouse overlay that draws the window background in the corners of the central widget, to round them,
// and a border along its sides. Only the inset sides are drawn: the other ones blend into the window.
class ContentCorners : public QWidget {
public:
  explicit ContentCorners(QWidget* parent)
    : QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
  }

  void setShape(int radius, Qt::Edges insetEdges) {
    if (radius != _radius || insetEdges != _insetEdges) {
      _radius = radius;
      _insetEdges = insetEdges;
      update();
    }
  }

protected:
  void paintEvent(QPaintEvent*) override {
    const auto radius = static_cast<qreal>(_radius);
    const auto top = _insetEdges.testFlag(Qt::TopEdge);
    const auto left = _insetEdges.testFlag(Qt::LeftEdge);
    const auto right = _insetEdges.testFlag(Qt::RightEdge);
    const auto bottom = _insetEdges.testFlag(Qt::BottomEdge);

    // A corner is rounded when both of its sides are inset.
    const auto topLeft = top && left ? radius : 0.;
    const auto topRight = top && right ? radius : 0.;
    const auto bottomLeft = bottom && left ? radius : 0.;
    const auto bottomRight = bottom && right ? radius : 0.;

    const auto full = QRectF(rect());
    auto shape = QPainterPath{};
    shape.moveTo(full.left() + topLeft, full.top());
    shape.lineTo(full.right() - topRight, full.top());
    if (topRight > 0.) {
      shape.arcTo(QRectF(full.right() - 2 * topRight, full.top(), 2 * topRight, 2 * topRight), 90, -90);
    }
    shape.lineTo(full.right(), full.bottom() - bottomRight);
    if (bottomRight > 0.) {
      shape.arcTo(
        QRectF(full.right() - 2 * bottomRight, full.bottom() - 2 * bottomRight, 2 * bottomRight, 2 * bottomRight), 0,
        -90);
    }
    shape.lineTo(full.left() + bottomLeft, full.bottom());
    if (bottomLeft > 0.) {
      shape.arcTo(QRectF(full.left(), full.bottom() - 2 * bottomLeft, 2 * bottomLeft, 2 * bottomLeft), 270, -90);
    }
    shape.lineTo(full.left(), full.top() + topLeft);
    if (topLeft > 0.) {
      shape.arcTo(QRectF(full.left(), full.top(), 2 * topLeft, 2 * topLeft), 180, -90);
    }
    shape.closeSubpath();
    auto corners = QPainterPath{};
    corners.addRect(full);
    corners = corners.subtracted(shape);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(corners, parentWidget()->palette().window());

    const auto* qlementineStyle = qobject_cast<const QlementineStyle*>(style());
    const auto borderWidth = qlementineStyle ? qlementineStyle->theme().borderWidth : 1;
    const auto borderColor = qlementineStyle ? qlementineStyle->theme().borderColor : palette().color(QPalette::Mid);
    const auto halfWidth = borderWidth / 2.;
    const auto inner = full.adjusted(halfWidth, halfWidth, -halfWidth, -halfWidth);
    const auto tl = std::max(0., topLeft - halfWidth);
    const auto tr = std::max(0., topRight - halfWidth);
    const auto bl = std::max(0., bottomLeft - halfWidth);
    const auto br = std::max(0., bottomRight - halfWidth);
    auto outline = QPainterPath{};
    if (top) {
      outline.moveTo(inner.left() + tl, inner.top());
      outline.lineTo(inner.right() - tr, inner.top());
    }
    if (right) {
      outline.moveTo(inner.right(), inner.top() + tr);
      outline.lineTo(inner.right(), inner.bottom() - br);
    }
    if (bottom) {
      outline.moveTo(inner.right() - br, inner.bottom());
      outline.lineTo(inner.left() + bl, inner.bottom());
    }
    if (left) {
      outline.moveTo(inner.left(), inner.bottom() - bl);
      outline.lineTo(inner.left(), inner.top() + tl);
    }
    if (tl > 0.) {
      outline.moveTo(inner.left(), inner.top() + tl);
      outline.arcTo(QRectF(inner.left(), inner.top(), 2 * tl, 2 * tl), 180, -90);
    }
    if (tr > 0.) {
      outline.moveTo(inner.right() - tr, inner.top());
      outline.arcTo(QRectF(inner.right() - 2 * tr, inner.top(), 2 * tr, 2 * tr), 90, -90);
    }
    if (br > 0.) {
      outline.moveTo(inner.right(), inner.bottom() - br);
      outline.arcTo(QRectF(inner.right() - 2 * br, inner.bottom() - 2 * br, 2 * br, 2 * br), 0, -90);
    }
    if (bl > 0.) {
      outline.moveTo(inner.left() + bl, inner.bottom());
      outline.arcTo(QRectF(inner.left(), inner.bottom() - 2 * bl, 2 * bl, 2 * bl), 270, -90);
    }
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(borderColor, borderWidth, Qt::SolidLine, Qt::FlatCap));
    painter.drawPath(outline);
  }

private:
  int _radius{ 0 };
  Qt::Edges _insetEdges;
};
} // namespace

struct UnifiedWindow::Impl {
  TitleBar* titleBar{ nullptr };
  QVBoxLayout* layout{ nullptr };
  QHBoxLayout* body{ nullptr };
  QSpacerItem* leadingSpacer{ nullptr };
  QPointer<QWidget> centralWidget;
  QPointer<QStatusBar> statusBar;
  bool statusBarBorderVisible{ true };
  QPointer<QWidget> leadingWidget;
  int leadingWidth{ 0 };
  int contentCornerRadius{ 0 };
  Qt::Edges contentInsetEdges{ Qt::TopEdge | Qt::LeftEdge };
  ContentCorners* corners{ nullptr };
  FrameOverlay* overlay{ nullptr };
  std::unique_ptr<WindowBackend> backend;
  bool shown{ false };

  void updateCorners() {
    if (!corners) {
      return;
    }
    if (!centralWidget || contentCornerRadius <= 0) {
      corners->hide();
      return;
    }
    auto edges = contentInsetEdges;
    if (statusBar && statusBar->isVisible()) {
      edges |= Qt::BottomEdge;
    }
    corners->setShape(contentCornerRadius, edges);
    corners->setGeometry(centralWidget->geometry());
    corners->show();
    corners->raise();
    if (overlay) {
      overlay->raise();
    }
  }

  void updateLeading(const QWidget& window) {
    auto width = 0;
    if (leadingWidget) {
      width = std::min(std::max(leadingWidget->sizeHint().width(), leadingWidget->minimumWidth()),
        leadingWidget->maximumWidth());

      // The window controls on the left must fit in the leading widget, even when it is narrow (compact sidebar).
      auto controlsWidth = titleBar->leftInset();
      for (const auto button : titleBar->captionButtonLayout().left) {
        const auto rect = titleBar->captionButtonRect(button);
        if (!rect.isEmpty()) {
          controlsWidth = std::max(controlsWidth, rect.right() + 1 + rect.left());
        }
      }
      width = std::max(width, controlsWidth);
      leadingWidget->setContentsMargins(0, titleBar->sizeHint().height(), 0, 0);
      const auto statusBarHeight = statusBar && statusBar->isVisibleTo(&window) ? statusBar->sizeHint().height() : 0;
      leadingWidget->setGeometry(0, 0, width, window.height() - statusBarHeight);
    }
    if (width != leadingWidth) {
      leadingWidth = width;
      leadingSpacer->changeSize(width, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);
      body->invalidate();
      titleBar->setLeadingWidth(width);
    }
  }
};

UnifiedWindow::UnifiedWindow(QWidget* parent)
  : QWidget(parent, Qt::Window)
  , _impl(std::make_unique<Impl>()) {
  // Layout: title bar on top, central widget below, with room at its left for the leading widget.
  _impl->layout = new QVBoxLayout(this);
  _impl->layout->setContentsMargins(0, 0, 0, 0);
  _impl->layout->setSpacing(0);
  _impl->titleBar = new TitleBar(this);
  _impl->titleBar->setContentAfterLeading(true);
  _impl->layout->addWidget(_impl->titleBar);
  _impl->body = new QHBoxLayout();
  _impl->body->setContentsMargins(0, 0, 0, 0);
  _impl->body->setSpacing(0);
  _impl->leadingSpacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);
  _impl->body->addSpacerItem(_impl->leadingSpacer);
  _impl->layout->addLayout(_impl->body, 1);

  // Native window configuration, before anything is shown.
  _impl->backend = createWindowBackend(*this);
  _impl->backend->prepareWindow();
  _impl->corners = new ContentCorners(this);
  _impl->corners->hide();
  _impl->overlay = new FrameOverlay(this, *_impl->backend);
  _impl->overlay->hide();

  // Title bar requests.
  connect(_impl->titleBar, &TitleBar::closeRequested, this, &QWidget::close);
  connect(_impl->titleBar, &TitleBar::minimizeRequested, this, &QWidget::showMinimized);
  connect(_impl->titleBar, &TitleBar::maximizeRequested, this, [this]() {
    if (isMaximized()) {
      showNormal();
    } else {
      showMaximized();
    }
  });
  connect(_impl->titleBar, &TitleBar::moveRequested, this, [this]() {
    _impl->backend->startMove();
  });
  connect(_impl->titleBar, &TitleBar::doubleClicked, this, [this]() {
    _impl->backend->handleTitleBarDoubleClick();
  });
  connect(_impl->titleBar, &TitleBar::menuRequested, this, [this](const QPoint& globalPos) {
    _impl->backend->showSystemMenu(globalPos);
  });
}

UnifiedWindow::~UnifiedWindow() {
  // Order: the overlay first (it calls into the backend), then the backend (it filters events on the window and the
  // title bar), then Qt deletes the remaining children.
  delete _impl->overlay;
  _impl->overlay = nullptr;
  _impl->backend.reset();
}

TitleBar* UnifiedWindow::titleBar() const {
  return _impl->titleBar;
}

QWidget* UnifiedWindow::centralWidget() const {
  return _impl->centralWidget.data();
}

void UnifiedWindow::setCentralWidget(QWidget* widget) {
  if (widget == _impl->centralWidget.data()) {
    return;
  }

  if (_impl->centralWidget) {
    _impl->body->removeWidget(_impl->centralWidget.data());
    _impl->centralWidget->hide();
    _impl->centralWidget->setParent(nullptr);
  }

  _impl->centralWidget = widget;
  if (widget) {
    _impl->body->addWidget(widget, 1);
    _impl->overlay->raise();
  }
  _impl->updateCorners();
}

QStatusBar* UnifiedWindow::statusBar() const {
  return _impl->statusBar.data();
}

void UnifiedWindow::setStatusBar(QStatusBar* statusBar) {
  if (statusBar == _impl->statusBar.data()) {
    return;
  }

  if (_impl->statusBar) {
    _impl->layout->removeWidget(_impl->statusBar.data());
    _impl->statusBar->hide();
    _impl->statusBar->setParent(nullptr);
  }

  _impl->statusBar = statusBar;
  if (statusBar) {
    _impl->layout->addWidget(statusBar);
    QlementineStyle::setBorderVisible(statusBar, _impl->statusBarBorderVisible);
    statusBar->show();
    _impl->overlay->raise();
  }
  _impl->updateLeading(*this);
  _impl->updateCorners();
}

bool UnifiedWindow::statusBarBorderVisible() const {
  return _impl->statusBarBorderVisible;
}

void UnifiedWindow::setStatusBarBorderVisible(bool visible) {
  if (visible != _impl->statusBarBorderVisible) {
    _impl->statusBarBorderVisible = visible;
    QlementineStyle::setBorderVisible(_impl->statusBar.data(), visible);
  }
}

int UnifiedWindow::contentCornerRadius() const {
  return _impl->contentCornerRadius;
}

void UnifiedWindow::setContentCornerRadius(int radius) {
  radius = std::max(0, radius);
  if (radius != _impl->contentCornerRadius) {
    _impl->contentCornerRadius = radius;
    _impl->updateCorners();
  }
}

Qt::Edges UnifiedWindow::contentInsetEdges() const {
  return _impl->contentInsetEdges;
}

void UnifiedWindow::setContentInsetEdges(Qt::Edges edges) {
  if (edges != _impl->contentInsetEdges) {
    _impl->contentInsetEdges = edges;
    _impl->updateCorners();
  }
}

QWidget* UnifiedWindow::leadingWidget() const {
  return _impl->leadingWidget.data();
}

void UnifiedWindow::setLeadingWidget(QWidget* widget) {
  if (widget == _impl->leadingWidget.data()) {
    return;
  }

  if (_impl->leadingWidget) {
    _impl->leadingWidget->hide();
    _impl->leadingWidget->setParent(nullptr);
  }

  _impl->leadingWidget = widget;
  if (widget) {
    widget->setParent(this);
    widget->stackUnder(_impl->titleBar);
    widget->show();
    _impl->overlay->raise();
  }
  _impl->updateLeading(*this);
}

bool UnifiedWindow::event(QEvent* evt) {
  const auto result = QWidget::event(evt);

  if (_impl && _impl->backend) {
    switch (evt->type()) {
      case QEvent::Resize:
      case QEvent::LayoutRequest:
        _impl->updateLeading(*this);
        _impl->updateCorners();
        break;
      case QEvent::PaletteChange:
      case QEvent::StyleChange:
        _impl->corners->update();
        break;
      case QEvent::WindowTitleChange:
        _impl->titleBar->setTitle(windowTitle());
        break;
      case QEvent::WinIdChange:
        _impl->backend->attachNativeWindow();
        break;
      case QEvent::Show:
        if (!_impl->shown) {
          _impl->shown = true;
          _impl->backend->attachNativeWindow();
        }
        break;
      default:
        break;
    }

    _impl->backend->handleEvent(evt);

    // Rounded corners: the overlay clips the content to the rounded shape when the backend asks for it.
    if (evt->type() == QEvent::Resize || evt->type() == QEvent::WindowStateChange || evt->type() == QEvent::Show) {
      _impl->overlay->setGeometry(rect());
      _impl->overlay->setVisible(_impl->backend->cornerRadius() > 0);
      _impl->overlay->raise();
    }
  }

  return result;
}

void UnifiedWindow::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  if (!_impl->backend->paintFrame(painter)) {
    painter.fillRect(rect(), palette().window());
  }
}
} // namespace oclero::qlementine
