// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once


#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/Delegates.hpp>

#include <QStyledItemDelegate>
#include <QEvent>
#include <QObject>
#include <QComboBox>
#include <QTreeView>
#include <QAbstractItemView>
#include <QChildEvent>
#include <QScreen>

namespace oclero::qlementine {

/// Returns true if the delegate is a default Qt delegate (not a custom subclass).
inline bool isDefaultItemDelegate(const QAbstractItemDelegate* delegate) {
  if (!delegate)
    return true;
  const auto* meta = delegate->metaObject();
  return meta == &QStyledItemDelegate::staticMetaObject || meta == &QItemDelegate::staticMetaObject
         || QByteArray(meta->className()) == "QComboMenuDelegate";
}

// Event filter for the item view in the QComboBox's popup.
class ComboboxItemViewFilter : public QObject {
public:
  ~ComboboxItemViewFilter() override = default;

  ComboboxItemViewFilter(QComboBox* comboBox, QAbstractItemView* view)
    : QObject(view)
    , _comboBox(comboBox)
    , _view(view)
    , _initialMaxHeight(view->maximumHeight()) {
    _view->installEventFilter(this);

    if (auto* viewport = _view->viewport()) {
      viewport->installEventFilter(this);
    }

    auto* comboBoxPopup = _view->parentWidget();
    comboBoxPopup->installEventFilter(this);
    _comboBox->installEventFilter(this);

    if (const auto* treeView = qobject_cast<QTreeView*>(_view)) {
      QObject::connect(treeView, &QTreeView::expanded, this, &ComboboxItemViewFilter::fixViewGeometry);
    }
  }

protected:
  bool eventFilter(QObject* watchedObject, QEvent* evt) override {
    switch (evt->type()) {
      case QEvent::Type::ChildAdded: {
        if (watchedObject == _comboBox) {
          const auto* childEvent = static_cast<QChildEvent*>(evt);
          const auto* child = childEvent->child();
          if (child == _comboBox->view()) {
            if (auto* qlementine = qobject_cast<QlementineStyle*>(_comboBox->style())) {
              if (isDefaultItemDelegate(_comboBox->itemDelegate())) {
                _comboBox->setItemDelegate(new ComboBoxDelegate(_comboBox, *qlementine));
              }
            }
          }
        }
      } break;
      case QEvent::Type::Show:
        fixViewGeometry();
        break;
      case QEvent::Type::Resize:
        if (watchedObject == _comboBox) {
          fixViewGeometry();
        }
        break;
      // case QEvent::Type::MouseButtonPress:
      //   if (watchedObject == _view->viewport()) {
      //     // _clickPoint = _view->itemat
      //     _view->indexAt()
      //     return false;
      //   }
      //   break;
      // case QEvent::Type::MouseButtonRelease:
      //   if (watchedObject == _view->viewport()) {
      //     return true;
      //   }
      //   break;
      default:
        break;
    }
    return false;
  }

private:
  void fixViewGeometry() const {
    if (_comboBox) {
      if (auto* view = _comboBox->view()) {
        if (const auto* qlementineStyle = qobject_cast<QlementineStyle*>(_comboBox->style())) {
          const auto isTreeView = view->inherits("QTreeView");
          const auto hMargin = qlementineStyle->pixelMetric(QStyle::PM_MenuHMargin);
          const auto shadowWidth = qlementineStyle->theme().spacing;
          const auto borderWidth = qlementineStyle->theme().borderWidth;

          // Width.
          const auto absoluteMinWidth = qlementineStyle->theme().controlHeightLarge * (isTreeView ? 2 : 1);
          const auto absoluteMaxWidth = qlementineStyle->theme().controlHeightLarge * 24;
          const auto width = std::min(absoluteMaxWidth, std::max({
                                                          _comboBox->width(),
                                                          view->sizeHintForColumn(0),
                                                          absoluteMinWidth,
                                                        }))
                             + shadowWidth * 2 + hMargin * 2 + borderWidth * 2;

          // Height.
          const auto absoluteMinHeight = qlementineStyle->theme().controlHeightLarge * (isTreeView ? 5 : 1);
          const auto screen = view->screen();
          // Don't exceed the available screen space above or below the combobox.
          const auto comboGlobalY = _comboBox->mapToGlobal(QPoint(0, 0)).y();
          constexpr auto screenPadding = 128;
          int absoluteMaxHeight;
          if (screen != nullptr) {
            const auto screenGeom = screen->availableGeometry();
            const auto spaceBelow = screenGeom.bottom() - comboGlobalY - _comboBox->height() - screenPadding;
            const auto spaceAbove = comboGlobalY - screenGeom.top() - screenPadding;
            absoluteMaxHeight = std::max(spaceBelow, spaceAbove);
          } else {
            absoluteMaxHeight = qlementineStyle->theme().controlHeightLarge * 10;
          }
          const auto height = std::min(absoluteMaxHeight, std::max(absoluteMinHeight, viewMinimumSizeHint().height()));

          view->setFixedWidth(width);
          view->setFixedHeight(height);
          view->parentWidget()->adjustSize();
        }
      }
    }
  }

  QSize viewMinimumSizeHint() const {
    auto height = 0;
    if (const auto* treeView = qobject_cast<QTreeView*>(_view)) {
      // For a QTreeView, look at expanded rows.
      auto currentIndex = treeView->indexAt(QPoint(0, 0));
      while (currentIndex.isValid()) {
        const auto rowSizeHint = _view->sizeHintForIndex(currentIndex).height();
        height = std::min(_initialMaxHeight, height + std::max(0, rowSizeHint));
        currentIndex = treeView->indexBelow(currentIndex);
      }
    } else {
      // QListView::minimumSizeHint() doesn't give the correct minimumHeight,
      // so we have to compute it.
      const auto rowCount = _view->model()->rowCount();
      for (auto i = 0; i < rowCount && height <= _initialMaxHeight; ++i) {
        const auto rowSizeHint = _view->sizeHintForRow(i);
        height = std::min(_initialMaxHeight, height + rowSizeHint);
      }
    }
    // It looks like it is OK for the width, though.
    const auto width = _view->sizeHintForColumn(0);
    return { width, height };
  }

  QComboBox* _comboBox{ nullptr };
  QAbstractItemView* _view{ nullptr };
  int _initialMaxHeight{ 0 };
  QModelIndex _clickedIndex{};
};

class ComboboxFilter : public QObject {
public:
  ~ComboboxFilter() override = default;

  explicit ComboboxFilter(QComboBox* comboBox)
    : QObject(comboBox)
    , _comboBox(comboBox) {
    if (const auto* view = comboBox->view()) {
      if (auto* popup = view->parentWidget()) {
        popup->installEventFilter(this);
      }
    }
  }

  bool eventFilter(QObject* watchedObject, QEvent* evt) override {
    switch (evt->type()) {
      // This is the only way we found to know when the QComboBox's view has changed.
      case QEvent::ChildAdded: {
        const auto* childEvent = static_cast<QChildEvent*>(evt);
        const auto* child = childEvent->child();
        if (child == _comboBox->view()) {
          if (auto* qlementine = qobject_cast<QlementineStyle*>(_comboBox->style())) {
            if (isDefaultItemDelegate(_comboBox->itemDelegate())) {
              _comboBox->setItemDelegate(new ComboBoxDelegate(_comboBox, *qlementine));
            }
          }

          // if (const auto* treeView = qobject_cast<QTreeView*>(child)) {
          //   QObject::connect(treeView, &QTreeView::expanded, this, [this]() {
          //     // &ComboboxItemViewFilter::fixViewGeometry
          //     qDebug() << "expanded";
          //     fixViewGeometry();

          //   });
          // }
        }
      } break;
      default:
        break;
    }

    return QObject::eventFilter(watchedObject, evt);
  }

private:
  QComboBox* _comboBox{ nullptr };
};
} // namespace oclero::qlementine
