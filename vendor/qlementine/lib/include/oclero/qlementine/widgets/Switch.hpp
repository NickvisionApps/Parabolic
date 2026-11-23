// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>
#include <oclero/qlementine/widgets/RoundedFocusFrame.hpp>

#include <QAbstractButton>
#include <QVariantAnimation>

namespace oclero::qlementine {
class QStyleOptionFocusRoundedRect;

/// A switch toggle button like on iOS and Android.
class QLEMENTINE_EXPORT Switch : public QAbstractButton {
  Q_OBJECT

  Q_PROPERTY(bool tristate READ isTristate WRITE setTristate NOTIFY tristateChanged)
  Q_PROPERTY(bool showAccessibilitySymbols READ showAccessibilitySymbols WRITE setShowAccessibilitySymbols NOTIFY
      showAccessibilitySymbolsChanged)

public:
  explicit Switch(QWidget* parent = nullptr);
  ~Switch() override = default;

  QSize sizeHint() const override;

  void initStyleOptionFocus(QStyleOptionFocusRoundedRect& opt) const;

  void setTristate(bool);
  bool isTristate() const;

  bool showAccessibilitySymbols() const;
  void setShowAccessibilitySymbols(bool showAccessibilitySymbols);

  Qt::CheckState checkState() const;
  void setCheckState(Qt::CheckState state);

Q_SIGNALS:
  void checkStateChanged(int);
  void tristateChanged(bool);
  void showAccessibilitySymbolsChanged(bool);

protected:
  void paintEvent(QPaintEvent* e) override;
  void enterEvent(QEnterEvent* e) override;
  void leaveEvent(QEvent* e) override;
  void changeEvent(QEvent* e) override;
  void focusInEvent(QFocusEvent* e) override;
  void focusOutEvent(QFocusEvent* e) override;
  void checkStateSet() override;
  void nextCheckState() override;

private:
  void setupAnimation();
  void startAnimation();
  void refreshThemeMetrics();
  QRect getSwitchRect() const;

  const QColor& getBgColor() const;
  const QColor& getBorderColor() const;
  const QColor& getFgColor() const;
  const QColor& getTextColor() const;

private:
  double _fullHandlePadding{ 2.0 };
  bool _isMouseOver{ false };
  bool _tristate{ false };
  bool _intermediate{ false };
  bool _blockRefresh{ false };
  bool _showAccessibilitySymbols{ false };
  Qt::CheckState _publishedState{ Qt::CheckState::Unchecked };
  QVariantAnimation _handleAnimation;
  QVariantAnimation _handlePaddingAnimation;
  QVariantAnimation _bgAnimation;
  QVariantAnimation _borderAnimation;
  QVariantAnimation _fgAnimation;
  QVariantAnimation _symbolAnimation;
  RoundedFocusFrame* _focusFrame{ nullptr };
};
} // namespace oclero::qlementine
