// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/qlementine_export.h>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/utils/RadiusesF.hpp>

#include <QPainter>
#include <QRect>
#include <QWidget>
#include <QKeySequence>
#include <QAbstractSpinBox>
#include <QStyle>

namespace oclero::qlementine {
[[maybe_unused]] static constexpr auto QLEMENTINE_PI = 3.14159265358979323846;

/// Gets the device pixel ratio for the QWidget.
QLEMENTINE_EXPORT double getPixelRatio(QWidget const* w);

/// Parses the text to detect the MenuItem's label and shortcut, usually separated by a tab.
QLEMENTINE_EXPORT std::tuple<QString, QString> getMenuLabelAndShortcut(QString const& text);

/// Draws an antialiased pixel-perfect border for the ellipsis.
QLEMENTINE_EXPORT void drawEllipseBorder(QPainter* p, QRectF const& rect, QColor const& color, qreal const borderWidth);

/// Generates a QPainterPath that contains a rounded rectangle with different radiuses for each angle.
QLEMENTINE_EXPORT QPainterPath getMultipleRadiusesRectPath(QRectF const& rect, RadiusesF const& radiuses);

/// Draws an antialiased rect.
QLEMENTINE_EXPORT void drawRoundedRect(QPainter* p, QRectF const& rect, QBrush const& brush, qreal const radius = 0.);

/// Draws an antialiased rect with different radiuses.
QLEMENTINE_EXPORT void drawRoundedRect(QPainter* p, QRectF const& rect, QBrush const& brush, RadiusesF const& radiuses);

/// Draws an antialiased rect.
QLEMENTINE_EXPORT void drawRoundedRect(QPainter* p, QRect const& rect, QBrush const& brush, qreal const radius = 0.);

/// Draws an antialiased rect with different radiuses.
QLEMENTINE_EXPORT void drawRoundedRect(QPainter* p, QRect const& rect, QBrush const& brush, RadiusesF const& radiuses);

/// Draws an antialiased pixel-perfect border for the rounded rect.
QLEMENTINE_EXPORT void drawRoundedRectBorder(
  QPainter* p, QRectF const& rect, QColor const& color, qreal const borderWidth, qreal const radius = 0.);

/// Draws an antialiased pixel-perfect border for the rounded rect.
QLEMENTINE_EXPORT void drawRoundedRectBorder(
  QPainter* p, QRect const& rect, QColor const& color, qreal const borderWidth, qreal const radius = 0.);

/// Draws an antialiased pixel-perfect border for the rounded rect.
QLEMENTINE_EXPORT void drawRoundedRectBorder(
  QPainter* p, QRectF const& rect, QColor const& color, qreal const borderWidth, RadiusesF const& radiuses = {});

/// Draws an antialiased pixel-perfect border for the rounded rect.
QLEMENTINE_EXPORT void drawRoundedRectBorder(
  QPainter* p, QRect const& rect, QColor const& color, qreal const borderWidth, RadiusesF const& radiuses = {});

/// Draws a pixel-perfect border for the rect.
QLEMENTINE_EXPORT void drawRectBorder(QPainter* p, QRect const& rect, QColor const& color, qreal const borderWidth);

/// Draws a pixel-perfect border for the rect.
QLEMENTINE_EXPORT void drawRectBorder(QPainter* p, QRectF const& rect, QColor const& color, qreal const borderWidth);

/// Draws an antialiased triangle.
QLEMENTINE_EXPORT void drawRoundedTriangle(QPainter* p, QRectF const& rect, qreal const radius = 0.);

/// Draws a checkerboard texture.
QLEMENTINE_EXPORT void drawCheckerboard(
  QPainter* p, const QRectF& rect, const QColor& darkColor, const QColor& lightColor, const qreal cellWidth);

/// Draws the value of a progress bar. A clipping mask is used to ensure the rectangle radiuses are respected
/// even for values close to min or max.
QLEMENTINE_EXPORT void drawProgressBarValueRect(QPainter* p, QRect const& rect, QColor const& color, qreal min,
  qreal max, qreal value, qreal const radius = 0., bool inverted = false);

/// Draws a color mark. Will draw a border if the contrast between color and background is not high enough.
QLEMENTINE_EXPORT void drawColorMark(
  QPainter* p, QRect const& rect, const QColor& color, const QColor& borderColor, int borderWidth = 1);

/// Draws the border of a color mark.
QLEMENTINE_EXPORT void drawColorMarkBorder(QPainter* p, QRect const& rect, const QColor& borderColor, int borderWidth);

/// Draws a semi-transparent red rectangle.
QLEMENTINE_EXPORT void drawDebugRect(const QRect& rect, QPainter* p);

/// Function that draws and generates a QPixmap.
using PixmapMakerFunc = std::function<QPixmap(const QSize& s, const QColor& c)>;

/// Utility to add QPixmaps to all states of the QIcon. The callback in parameter will be called to draw each QPixmap.
QLEMENTINE_EXPORT void updateUncheckableButtonIconPixmap(
  QIcon& icon, const QSize& size, Theme const& theme, const PixmapMakerFunc& func);

/// Gets the path to draw the menu arrow in a Button.
QLEMENTINE_EXPORT QPainterPath getMenuIndicatorPath(const QRect& rect);

/// Draws the combobox double arrow.
QLEMENTINE_EXPORT void drawComboBoxIndicator(const QRect& rect, QPainter* p);

/// Draws the checkbox indicator (i.e. a check mark).
QLEMENTINE_EXPORT void drawCheckBoxIndicator(const QRect& rect, QPainter* p, qreal progress = 1.);

/// Draws the partially checked checkbox indicator (i.e. a dash).
QLEMENTINE_EXPORT void drawPartiallyCheckedCheckBoxIndicator(const QRect& rect, QPainter* p, qreal progress = 1.);

/// Draws the radiobutton indicator (i.e. a circle).
QLEMENTINE_EXPORT void drawRadioButtonIndicator(const QRect& rect, QPainter* p, qreal progress = 1.);

/// Draws a spinbox up/down indicator (i.e. +/- or up/down arrow).
QLEMENTINE_EXPORT void drawSpinBoxArrowIndicator(const QRect& rect, QPainter* p,
  QAbstractSpinBox::ButtonSymbols buttonSymbol, QStyle::SubControl subControl, QSize const& iconSize);

/// Draws an arrow that points to the right.
QLEMENTINE_EXPORT void drawArrowRight(const QRect& rect, QPainter* p);

/// Draws an arrow that points to the left.
QLEMENTINE_EXPORT void drawArrowLeft(const QRect& rect, QPainter* p);

/// Draws an arrow that points down.
QLEMENTINE_EXPORT void drawArrowDown(const QRect& rect, QPainter* p);

/// Draws an arrow that points up.
QLEMENTINE_EXPORT void drawArrowUp(const QRect& rect, QPainter* p);

/// Draws the sub-menu arrow.
QLEMENTINE_EXPORT void drawSubMenuIndicator(const QRect& rect, QPainter* p);

/// Draws a double right arrow.
QLEMENTINE_EXPORT void drawDoubleArrowRightIndicator(const QRect& rect, QPainter* p);

/// Draws a small double right arrow.
QLEMENTINE_EXPORT void drawToolBarExtensionIndicator(const QRect& rect, QPainter* p);

/// Draws a close indicator (i.e. the cross).
QLEMENTINE_EXPORT void drawCloseIndicator(const QRect& rect, QPainter* p);

/// Draws a treeview indicator.
QLEMENTINE_EXPORT void drawTreeViewIndicator(const QRect& rect, QPainter* p, bool open);

/// Draws a calendar indicator (e.g. for the widgets that display a calendar popup).
QLEMENTINE_EXPORT void drawCalendarIndicator(const QRect& rect, QPainter* p, const QColor& color);

/// Draws a grip indicator (for drag n' drop).
QLEMENTINE_EXPORT void drawGripIndicator(
  const QRect& rect, QPainter* p, const QColor& color, Qt::Orientation orientation);

/// Gets the tick interval according to the length of steps, range and available length.
QLEMENTINE_EXPORT int getTickInterval(
  int tickInterval, int singleStep, int pageStep, int min, int max, int sliderLength);

/// Draws the Slider tick marks.
QLEMENTINE_EXPORT void drawSliderTickMarks(QPainter* p, QRect const& tickmarksRect, QColor const& tickColor,
  const int min, const int max, const int interval, const int tickThickness, const int singleStep, const int pageStep);

/// Draws the Dial tick marks.
QLEMENTINE_EXPORT void drawDialTickMarks(QPainter* p, QRect const& tickmarksRect, QColor const& tickColor,
  const int min, const int max, const int tickThickness, const int tickLength, const int singleStep, const int pageStep,
  const int minArcLength);

/// Draws a Dial.
QLEMENTINE_EXPORT void drawDial(QPainter* p, QRect const& rect, int min, int max, double value, QColor const& bgColor,
  QColor const& handleColor, QColor const& grooveColor, QColor const& valueColor, QColor const& markColor,
  const int grooveThickness, const int markLength, const int markThickness);

/// Gets the path for a rounded tab. Specify negative radiuses if you want the tab to overlap its bounds.
QLEMENTINE_EXPORT QPainterPath getTabPath(QRect const& rect, const RadiusesF& radiuses);

/// Gets the path for a trapezoid tab, whose sides are slanted by `slant` pixels and whose top corners are rounded.
QLEMENTINE_EXPORT QPainterPath getTrapezoidTabPath(QRect const& rect, qreal slant, qreal radius);

/// Draws a rounded tab. Specify negative radiuses if you want the tab to overlap its bounds.
QLEMENTINE_EXPORT void drawTab(QPainter* p, QRect const& rect, const RadiusesF& radiuses, const QColor& bgColor,
  bool drawShadow = false, const QColor& shadowColor = Qt::black);

/// Draws a tab from an arbitrary path.
QLEMENTINE_EXPORT void drawTab(QPainter* p, const QPainterPath& path, const QColor& bgColor, bool drawShadow = false,
  const QColor& shadowColor = Qt::black);

/// Draws the shadow of a rounded tab.
QLEMENTINE_EXPORT void drawTabShadow(QPainter* p, QRect const& rect, const RadiusesF& radius, const QColor& color);

/// Draws the shadow of a tab from an arbitrary path.
QLEMENTINE_EXPORT void drawTabShadow(QPainter* p, const QPainterPath& path, const QColor& color);

/// Draws a RadioButton indicator according to its checked state.
QLEMENTINE_EXPORT void drawRadioButton(QPainter* p, const QRect& rect, QColor const& bgColor, const QColor& borderColor,
  QColor const& fgColor, const qreal borderWidth, qreal progress);

/// Draws a CheckButton indicator according to its checked state.
QLEMENTINE_EXPORT void drawCheckButton(QPainter* p, const QRect& rect, qreal radius, const QColor& bgColor,
  const QColor& borderColor, const QColor& fgColor, const qreal borderWidth, qreal progress, CheckState checkState);

/// Draws a menu separator.
QLEMENTINE_EXPORT void drawMenuSeparator(QPainter* p, const QRect& rect, QColor const& color, const int thickness);

/// Draws an elided text (with an ellipsis "…" at the end if necessary) inside a QRect.
/// The difference with Qt's method is the ellipsis (Qt doesn't draw one and just cuts the text).
QLEMENTINE_EXPORT void drawElidedMultiLineText(
  QPainter& p, const QRect& rect, const QString& text, const QPaintDevice* paintDevice);

/// Removes the trailing whitespaces at the end.
QLEMENTINE_EXPORT QString removeTrailingWhitespaces(const QString& str);

/// Gives the text to draw when displaying a shortcut.
QLEMENTINE_EXPORT QString displayedShortcutString(const QKeySequence& shortcut);

/// Draws a keyboard shortcut.
QLEMENTINE_EXPORT void drawShortcut(QPainter& p, const QKeySequence& shortcut, const QRect& rect, const Theme& theme,
  bool enabled, Qt::Alignment alignment = { Qt::AlignLeft | Qt::AlignVCenter });

/// Gets the necessary size to display the whole shortcut.
QLEMENTINE_EXPORT QSize shortcutSizeHint(const QKeySequence& shortcut, const Theme& theme);

/// Gets the QPixmap that corresponds to the state and matches the best the desired iconSize.
/// NB: the QPixmap may not be equal to iconSize: it can be smaller, but never larger.
QLEMENTINE_EXPORT QPixmap getPixmap(
  const QIcon& icon, const QSize& iconSize, const MouseState mouse, const CheckState checked, const QWidget* widget);

/// Draws the icon to fill the rect. Returns the actual rect occupied by the pixmap (it can be smaller).
QLEMENTINE_EXPORT QRect drawIcon(const QRect& rect, QPainter* p, const QIcon& icon, const MouseState mouse,
  const CheckState checked, const QWidget* widget, bool colorize = false, const QColor& color = {});

/// Updates the QIcon with the QPixmap given by the function at the right size and for all states.
QLEMENTINE_EXPORT void updateUncheckableButtonIconPixmap(
  QIcon& icon, const QSize& size, QlementineStyle const& style, const PixmapMakerFunc& func);

/// Updates the QIcon that contains the check mark.
/// NB: The unchecked version of the QIcon is empty on purpose (i.e. no check).
QLEMENTINE_EXPORT void updateCheckIcon(QIcon& icon, QSize const& size, QlementineStyle const& style);

/// Generates a pixmap for a specific state of QLineEdit's clear button.
QLEMENTINE_EXPORT QPixmap makeClearButtonPixmap(QSize const& size, QColor const& color);

/// Generates a pixmap that contains a check mark.
QLEMENTINE_EXPORT QPixmap makeCheckPixmap(QSize const& size, QColor const& color);

/// Generates a pixmap that contains a calendar.
QLEMENTINE_EXPORT QPixmap makeCalendarPixmap(QSize const& size, QColor const& color);

/// Generates a pixmap that contains a double right arrow.
QLEMENTINE_EXPORT QPixmap makeDoubleArrowRightPixmap(QSize const& size, QColor const& color);

/// Generates a pixmap that contains a small double right arrow.
QLEMENTINE_EXPORT QPixmap makeToolBarExtensionPixmap(QSize const& size, QColor const& color);

/// Generates a pixmap that contains a left arrow.
QLEMENTINE_EXPORT QPixmap makeArrowLeftPixmap(QSize const& size, QColor const& color);

/// Generates a pixmap that contains a right arrow.
QLEMENTINE_EXPORT QPixmap makeArrowRightPixmap(QSize const& size, QColor const& color);

QLEMENTINE_EXPORT QPixmap makeMessageBoxWarningPixmap(QSize const& size, QColor const& bgColor, QColor const& fgColor);
QLEMENTINE_EXPORT QPixmap makeMessageBoxCriticalPixmap(QSize const& size, QColor const& bgColor, QColor const& fgColor);
QLEMENTINE_EXPORT QPixmap makeMessageBoxQuestionPixmap(QSize const& size, QColor const& bgColor, QColor const& fgColor);
QLEMENTINE_EXPORT QPixmap makeMessageBoxInformationPixmap(
  QSize const& size, QColor const& bgColor, QColor const& fgColor);

QLEMENTINE_EXPORT void updateMessageBoxWarningIcon(QIcon& icon, QSize const& size, Theme const& theme);
QLEMENTINE_EXPORT void updateMessageBoxCriticalIcon(QIcon& icon, QSize const& size, Theme const& theme);
QLEMENTINE_EXPORT void updateMessageBoxQuestionIcon(QIcon& icon, QSize const& size, Theme const& theme);
QLEMENTINE_EXPORT void updateMessageBoxInformationIcon(QIcon& icon, QSize const& size, Theme const& theme);
} // namespace oclero::qlementine
