// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/UnifiedWindow.hpp>

#include "utils/window/WindowBackend.hpp"

#include <QEvent>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
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
} // namespace

struct UnifiedWindow::Impl {
  TitleBar* titleBar{ nullptr };
  QVBoxLayout* layout{ nullptr };
  QHBoxLayout* body{ nullptr };
  QSpacerItem* leadingSpacer{ nullptr };
  QPointer<QWidget> centralWidget;
  QPointer<QStatusBar> statusBar;
  QPointer<QWidget> leadingWidget;
  int leadingWidth{ 0 };
  FrameOverlay* overlay{ nullptr };
  std::unique_ptr<WindowBackend> backend;
  bool shown{ false };

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
    statusBar->show();
    _impl->overlay->raise();
  }
  _impl->updateLeading(*this);
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
