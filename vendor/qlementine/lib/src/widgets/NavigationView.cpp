// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/widgets/NavigationView.hpp>

#include <QHBoxLayout>
#include <QPointer>

#include <algorithm>

namespace oclero::qlementine {
NavigationView::NavigationView(QWidget* parent)
  : QWidget(parent)
  , _sidebar(new NavigationSidebar(this))
  , _stackedWidget(new QStackedWidget(this)) {
  // Layout.
  auto* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(_sidebar);
  layout->addWidget(_stackedWidget, 1);

  // Show the page of the selected item. Items without a page keep the current one.
  QObject::connect(_sidebar, &AbstractItemListWidget::currentIndexChanged, this, [this]() {
    if (auto* currentWidget = page(_sidebar->currentIndex())) {
      _stackedWidget->setCurrentWidget(currentWidget);
    }
    Q_EMIT currentIndexChanged();
  });

  // The pages of removed items are owned by the view: delete them.
  QObject::connect(_sidebar, &NavigationSidebar::itemAboutToBeRemoved, this, [this](int index) {
    if (auto* removedWidget = page(index)) {
      removedWidget->deleteLater();
    }
  });
}

NavigationSidebar* NavigationView::sidebar() const {
  return _sidebar;
}

QStackedWidget* NavigationView::stackedWidget() const {
  return _stackedWidget;
}

int NavigationView::addPage(QWidget* page, const QString& text, const QIcon& icon, const QString& badge) {
  return _sidebar->addItem(text, icon, badge, makePageData(page));
}

int NavigationView::addBottomPage(QWidget* page, const QString& text, const QIcon& icon, const QString& badge) {
  return _sidebar->addBottomItem(text, icon, badge, makePageData(page));
}

int NavigationView::addChildPage(
  int parentIndex, QWidget* page, const QString& text, const QIcon& icon, const QString& badge) {
  return _sidebar->addChildItem(parentIndex, text, icon, badge, makePageData(page));
}

void NavigationView::removePage(int index) {
  _sidebar->removeItem(index);
}

QWidget* NavigationView::page(int index) const {
  return _sidebar->getItemData(index).value<QPointer<QWidget>>().data();
}

QWidget* NavigationView::currentPage() const {
  return page(_sidebar->currentIndex());
}

int NavigationView::currentIndex() const {
  return _sidebar->currentIndex();
}

void NavigationView::setCurrentIndex(int index) {
  _sidebar->setCurrentIndex(index);
}

int NavigationView::autoCompactWidth() const {
  return _autoCompactWidth;
}

void NavigationView::setAutoCompactWidth(int width) {
  width = std::max(0, width);
  if (width != _autoCompactWidth) {
    _autoCompactWidth = width;
    if (_autoCompactWidth > 0) {
      _sidebar->setCompact(availableWidth() < _autoCompactWidth);
    }
    Q_EMIT autoCompactWidthChanged();
  }
}

void NavigationView::resizeEvent(QResizeEvent* e) {
  QWidget::resizeEvent(e);

  if (_autoCompactWidth > 0) {
    _sidebar->setCompact(availableWidth() < _autoCompactWidth);
  }
}

int NavigationView::availableWidth() const {
  // Compare with the width of the view, or of the window when the sidebar was moved out of the view, and not with the
  // width of the sidebar, to avoid flickering while the sidebar animates.
  return _sidebar->parentWidget() == this ? width() : window()->width();
}

QVariant NavigationView::makePageData(QWidget* page) {
  if (!page) {
    return {};
  }

  // The page must be in the stack before the item is added, because adding an item can select it.
  if (_stackedWidget->indexOf(page) == -1) {
    _stackedWidget->addWidget(page);
  }
  return QVariant::fromValue(QPointer<QWidget>(page));
}
} // namespace oclero::qlementine
