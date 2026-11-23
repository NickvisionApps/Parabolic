// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>
#include <oclero/qlementine/style/Theme.hpp>
#include <oclero/qlementine/style/QlementineStyleOption.hpp>

#include <QWidget>
#include <QIcon>
#include <QVariant>
#include <QString>
#include <QVariantAnimation>

#include <vector>

namespace oclero::qlementine {
class RoundedFocusFrame;
class TitleBar;

/// A class to factorize common behavior between widgets such as tab bars.
class QLEMENTINE_EXPORT AbstractItemListWidget : public QWidget {
  Q_OBJECT

  // Lets the title bar know if a point is over an item or in the empty space.
  friend class TitleBar;

  Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
  Q_PROPERTY(QVariant currentData READ currentData WRITE setCurrentData NOTIFY currentDataChanged)
  Q_PROPERTY(int itemCount READ itemCount NOTIFY itemCountChanged)
  Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize NOTIFY iconSizeChanged)
  Q_PROPERTY(bool itemsShouldExpand READ itemsShouldExpand WRITE setItemsShouldExpand NOTIFY itemsShouldExpandChanged)

public:
  explicit AbstractItemListWidget(QWidget* parent = nullptr);
  ~AbstractItemListWidget() override;

  QSize sizeHint() const override;

  int itemCount() const;

  int currentIndex() const;
  void setCurrentIndex(int index);

  QVariant currentData() const;
  void setCurrentData(const QVariant& currentData);

  const QSize& iconSize() const;
  void setIconSize(const QSize& size);

  bool itemsShouldExpand() const;
  void setItemsShouldExpand(bool expand);

  int addItem(const QString& text, const QIcon& icon = {}, const QString& badge = {}, const QVariant& itemData = {});
  virtual void removeItem(int index);
  int findItemIndex(const QVariant& itemData) const;

  void setItemData(int index, const QVariant& text);
  QVariant getItemData(int index) const;

  void setItemText(int index, const QString& text);
  QString getItemText(int index) const;

  void setItemIcon(int index, const QIcon& icon);
  QIcon getItemIcon(int index) const;

  void setItemBadge(int index, const QString& badge);
  QString getItemBadge(int index) const;

  void setItemEnabled(int index, bool enabled);
  bool isItemEnabled(int index) const;

  Q_SLOT void moveToNextItem();
  Q_SLOT void moveToPreviousItem();

  virtual void initStyleOptionFocus(QStyleOptionFocusRoundedRect& opt) const;

Q_SIGNALS:
  void itemCountChanged();
  void currentIndexChanged();
  void currentDataChanged();
  void iconSizeChanged();
  void itemsShouldExpandChanged();

protected: // QWidget override.
  void keyPressEvent(QKeyEvent* e) override;
  void keyReleaseEvent(QKeyEvent* e) override;
  void mousePressEvent(QMouseEvent* e) override;
  void enterEvent(QEnterEvent* e) override;
  void leaveEvent(QEvent* e) override;
  void mouseReleaseEvent(QMouseEvent* e) override;
  void mouseMoveEvent(QMouseEvent* e) override;
  void paintEvent(QPaintEvent* e) override;
  void resizeEvent(QResizeEvent* e) override;
  void focusInEvent(QFocusEvent* e) override;
  void showEvent(QShowEvent* e) override;
  bool event(QEvent* e) override;
  void changeEvent(QEvent* e) override;

protected: // Available to child classes.
  enum class ItemKind {
    Entry,
    Section,
    Separator,
  };

  struct Item {
    bool enabled{ true };
    QString text;
    QIcon icon;
    QString badge;
    QVariant data;
    QRect rect;
    QSize sizeHint;
    QVariantAnimation* bgColorAnimation{ nullptr };
    QVariantAnimation* fgColorAnimation{ nullptr };
    QVariantAnimation* badgeBgAnimation{ nullptr };
    QVariantAnimation* badgeFgAnimation{ nullptr };
    ItemKind kind{ ItemKind::Entry };
    int parent{ -1 };
    bool expanded{ false };
    double expansion{ 0. };
    bool bottom{ false };
  };

  int insertItem(Item item);
  std::vector<Item>& items();
  const std::vector<Item>& items() const;
  int focusedIndex() const;
  int pressedIndex() const;
  void updateFocusFrame();
  virtual bool canSelectItem(int index) const;

  virtual QMargins getItemPadding() const;
  virtual int getItemMinimumHeight() const;
  virtual int getItemSpacing() const;
  virtual double getItemRadius() const;
  virtual const QColor& getItemFgColor(MouseState mouse, bool selected, const Theme& theme) const = 0;
  virtual const QColor& getItemBgColor(MouseState mouse, const Theme& theme) const = 0;
  virtual const QColor& getItemFgColor(MouseState mouse, bool selected, const QPalette& palette) const;
  virtual const QColor& getItemBgColor(MouseState mouse, const QPalette& palette) const;

  virtual QMargins getPadding() const;
  virtual int getSpacing() const;
  virtual double getRadius() const;
  virtual const QColor& getBgColor(const Theme& theme) const = 0;
  virtual const QColor& getBgColor(const QPalette& palette) const;

  virtual QMargins getBadgePadding() const;
  virtual const QColor& getItemBadgeBgColor(MouseState mouse, bool selected, const Theme& theme) const = 0;
  virtual const QColor& getItemBadgeFgColor(MouseState mouse, bool selected, const Theme& theme) const = 0;
  virtual const QColor& getItemBadgeBgColor(MouseState mouse, bool selected, const QPalette& palette) const;
  virtual const QColor& getItemBadgeFgColor(MouseState mouse, bool selected, const QPalette& palette) const;

  virtual void drawBackground(QPainter& p) const;
  virtual void drawItemBackground(QPainter& p, const Item& item) const;
  virtual void drawItemForeground(QPainter& p, const Item& item) const;
  virtual void drawCurrentItemIndicator(QPainter& p) const;
  virtual bool shouldDrawItemBgForCurrent() const;

  virtual QFont labelFont() const;

protected:
  virtual int itemAtPos(const QPoint& pos) const;
  bool hitTestItemRect(const QPoint& pos, const QRect& itemRect, int leftPadding, int rightPadding) const;
  virtual void updateItemsSizeHints();
  virtual void updateItemRects();
  MouseState getItemMouseState(int index, const Item& item) const;
  std::tuple<QColor, QColor, QColor, QColor> getItemBgAndFgColor(int index, MouseState mouse) const;
  const QColor& getCurrentItemIndicatorColor() const;
  virtual const QColor& getBgColor() const;
  virtual QRect getCurrentItemRect() const;
  QRect getFocusedItemRect() const;
  QRect getAnimatedCurrentItemRect() const;
  void updateCurrentIndexAnimation(bool immediate = false);
  void updateItemsAnimations();
  void setFocusedIndex(int index);
  void setHoveredIndex(int index, bool updateAnims = true);
  void setPressedIndex(int index, bool updateAnims = true);
  const QFont& badgeFont() const;
  const QRect& actualRect() const;

private:
  QRect _actualRect;
  bool _itemsShouldExpand{ false };
  int _currentIndex{ -1 };
  int _focusedIndex{ -1 };
  int _hoveredIndex{ -1 };
  int _pressedIndex{ -1 };
  QSize _iconSize;
  std::vector<Item> _items;
  QVariantAnimation _currentIndexAnimation;
  RoundedFocusFrame* _focusFrame{ nullptr };
  QFont _badgeFont;
  bool _firstShow{ false };
};
} // namespace oclero::qlementine
