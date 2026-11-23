// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/style/QlementineStyle.hpp>

#include <oclero/qlementine/resources/ResourceInitialization.hpp>
#include <oclero/qlementine/animation/WidgetAnimator.hpp>
#include <oclero/qlementine/animation/WidgetAnimationManager.hpp>
#include <oclero/qlementine/utils/PrimitiveUtils.hpp>
#include <oclero/qlementine/utils/FontUtils.hpp>
#include <oclero/qlementine/utils/ImageUtils.hpp>
#include <oclero/qlementine/utils/RadiusesF.hpp>
#include <oclero/qlementine/utils/StateUtils.hpp>
#include <oclero/qlementine/utils/StyleUtils.hpp>
#include <oclero/qlementine/utils/WidgetUtils.hpp>
#include <oclero/qlementine/utils/ColorUtils.hpp>
#include <oclero/qlementine/utils/IconUtils.hpp>
#include <oclero/qlementine/style/Delegates.hpp>
#include <oclero/qlementine/style/QlementineStyleOption.hpp>
#include <oclero/qlementine/widgets/RoundedFocusFrame.hpp>
#include <oclero/qlementine/widgets/AbstractItemListWidget.hpp>
#include <oclero/qlementine/widgets/Switch.hpp>
#include <oclero/qlementine/widgets/TitleBar.hpp>
#include <oclero/qlementine/widgets/LineEdit.hpp>
#include <oclero/qlementine/widgets/ColorButton.hpp>
#include <oclero/qlementine/widgets/PlainTextEdit.hpp>

#include "PolishedWidgetRegistry.hpp"
#include "PolishUtils.hpp"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QDial>
#include <QFontComboBox>
#include <QFontDatabase>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLineEdit>
#include <QListView>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QPainterStateGuard>
#include <QPainterPath>
#include <QPixmapCache>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpinBox>
#include <QTableView>
#include <QTextEdit>
#include <QTextEdit>
#include <QTextEdit>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QToolTip>
#include <QTreeView>
#include <QWindow>

#include <cmath>
#include <mutex>
#include <optional>

namespace oclero::qlementine {

static bool isInTitleBar(const QWidget* widget) {
  for (const auto* ancestor = widget; ancestor; ancestor = ancestor->parentWidget()) {
    if (qobject_cast<const TitleBar*>(ancestor)) {
      return true;
    }
  }
  return false;
}

static const QStyleOptionButton* buttonStyleOption(const QStyleOption* opt) {
  if (const auto* roundedButton = qstyleoption_cast<const QStyleOptionRoundedButton*>(opt)) {
    return roundedButton;
  }
  if (const auto* commandButton = qstyleoption_cast<const QStyleOptionCommandLinkButton*>(opt)) {
    return commandButton;
  }
  return qstyleoption_cast<const QStyleOptionButton*>(opt);
}

QlementineStyle* appStyle() {
  return qobject_cast<QlementineStyle*>(qApp->style());
}

/// Used to initializeResources from .qrc only once.
static std::once_flag qlementineOnceFlag;

constexpr auto hardcodedButtonSpacing = 4; // qpushbutton.cpp line 410, qcombobox.cpp line 418/437
//constexpr auto hardcodedLineEditVMargin = 1; // qlinedit_p.cpp line 68
constexpr auto hardcodedLineEditHMargin = 2; // qlinedit_p.cpp line 69
//constexpr auto hardcodedTabSpacing = 4; // qtabbar.cpp line 1596

// A pen width of 1 causes visual bugs.
constexpr auto iconPenWidth = 1.01;

// Used to determine if the icon must be colorized according to the Theme's colors or not.
constexpr auto Property_AutoIconColor = "autoIconColor";

// Used to determine the status of a widget.
constexpr auto Property_Status = "status";


struct QlementineStyleImpl {
  QlementineStyle& owner;
  Theme theme{};
  std::unique_ptr<QFontMetrics> fontMetricsBold{ nullptr };
  WidgetAnimationManager animations;
  std::unordered_map<QStyle::StandardPixmap, QIcon> standardIconCache;
  std::unordered_map<QlementineStyle::StandardPixmapExt, QIcon> standardIconExtCache;
  PolishedWidgetRegistry polishedWidgets;
  std::optional<QFont> previousAppFont;
  AutoIconColor autoIconColor{ AutoIconColor::None };
  bool toolTipMatchesTheme{ false };
  std::function<QString(QString)> iconPathFunc;

  explicit QlementineStyleImpl(QlementineStyle& o)
    : owner(o) {
    std::call_once(qlementineOnceFlag, qlementine::resources::initializeResources);
    installFonts();
  }

  /// Registers all the theme fonts to Qt's font database.
  void installFonts() {
    const auto regularFontPath = QString(":/qlementine/resources/fonts/inter/%1.ttf");
    QFontDatabase::addApplicationFont(regularFontPath.arg(QStringLiteral("Inter-Regular")));
    QFontDatabase::addApplicationFont(regularFontPath.arg(QStringLiteral("Inter-Italic")));
    QFontDatabase::addApplicationFont(regularFontPath.arg(QStringLiteral("Inter-Bold")));
    QFontDatabase::addApplicationFont(regularFontPath.arg(QStringLiteral("Inter-BoldItalic")));

    const auto fixedFontPath = QString(":/qlementine/resources/fonts/roboto-mono/%1.ttf");
    QFontDatabase::addApplicationFont(fixedFontPath.arg(QStringLiteral("RobotoMono-Regular")));
    QFontDatabase::addApplicationFont(fixedFontPath.arg(QStringLiteral("RobotoMono-Italic")));
    QFontDatabase::addApplicationFont(fixedFontPath.arg(QStringLiteral("RobotoMono-Bold")));
    QFontDatabase::addApplicationFont(fixedFontPath.arg(QStringLiteral("RobotoMono-BoldItalic")));

    const auto titleFontPath = QString(":/qlementine/resources/fonts/inter/%1.ttf");
    QFontDatabase::addApplicationFont(titleFontPath.arg(QStringLiteral("InterDisplay-Regular")));
    QFontDatabase::addApplicationFont(titleFontPath.arg(QStringLiteral("InterDisplay-Italic")));
    QFontDatabase::addApplicationFont(titleFontPath.arg(QStringLiteral("InterDisplay-Bold")));
    QFontDatabase::addApplicationFont(titleFontPath.arg(QStringLiteral("InterDisplay-BoldItalic")));
  }

  /// Some widgets need to have a QPalette explicitely set.
  void updatePalette() const {
    auto palette = theme.palette;
    palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::ToolTipBase, owner.toolTipBackgroundColor());
    palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::ToolTipText, owner.toolTipForegroundColor());
    QToolTip::setPalette(palette);
  }

  /// Updates the font cache.
  void updateFonts() {
    fontMetricsBold = std::make_unique<QFontMetrics>(theme.fontBold);
  }

  /// Gets (or create if not existing yet) an icon from the cache.
  QIcon& getStandardIconExt(const QlementineStyle::StandardPixmapExt sp, QSize const& size) {
    auto& icon = standardIconExtCache[sp];
    const auto availableSizes = icon.availableSizes();
    if (!availableSizes.contains(size)) {
      switch (sp) {
        case QlementineStyle::StandardPixmapExt::SP_Check:
          updateCheckIcon(icon, size, owner);
          break;
        case QlementineStyle::StandardPixmapExt::SP_Calendar:
          updateUncheckableButtonIconPixmap(icon, size, owner, makeCalendarPixmap);
          break;
        default:
          break;
      }
    }
    return icon;
  }

  /// Gets (or create if not existing yet) an icon from the cache.
  QIcon& getStandardIcon(const QStyle::StandardPixmap standardPixmap, QSize const& size) {
    auto& icon = standardIconCache[standardPixmap];
    const auto availableSizes = icon.availableSizes();
    if (!availableSizes.contains(size)) {
      // TODO Other icons : use QPainter or load SVG file.
      switch (standardPixmap) {
        case QlementineStyle::SP_LineEditClearButton:
          updateUncheckableButtonIconPixmap(icon, size, owner, makeClearButtonPixmap);
          break;
        case QlementineStyle::SP_ToolBarVerticalExtensionButton:
        case QlementineStyle::SP_ToolBarHorizontalExtensionButton:
          updateUncheckableButtonIconPixmap(icon, size, owner, makeToolBarExtensionPixmap);
          break;
        case QlementineStyle::SP_ArrowLeft:
          updateUncheckableButtonIconPixmap(icon, size, owner, makeArrowLeftPixmap);
          break;
        case QlementineStyle::SP_ArrowRight:
          updateUncheckableButtonIconPixmap(icon, size, owner, makeArrowRightPixmap);
          break;
        case QlementineStyle::SP_MessageBoxWarning:
          updateMessageBoxWarningIcon(icon, size, theme);
          break;
        case QlementineStyle::SP_MessageBoxCritical:
          updateMessageBoxCriticalIcon(icon, size, theme);
          break;
        case QlementineStyle::SP_MessageBoxInformation:
          updateMessageBoxInformationIcon(icon, size, theme);
          break;
        case QlementineStyle::SP_MessageBoxQuestion:
          updateMessageBoxQuestionIcon(icon, size, theme);
          break;
        default:
          break;
      }
    }
    return icon;
  }

  /// Returns true if the QTabBar will show its scroll buttons.
  static bool areTabBarScrollButtonsVisible(const QTabBar* tabBar) {
    if (!tabBar->usesScrollButtons())
      return false;

    // Ignore right button They go in pair: if one is visible, the other is too.
    const auto toolButtons = tabBar->findChildren<QToolButton*>();
    auto leftButtonVisible = false;
    for (const auto* toolButton : toolButtons) {
      if (toolButton->arrowType() == Qt::ArrowType::LeftArrow) {
        leftButtonVisible = toolButton->isVisible();
        break;
      }
    }
    return leftButtonVisible;
  }

  /// Parts of a QTabBar/QTabWidget that have rounded corners.
  enum class TabBarPart { Bar, Pane, Start, End };

  static bool isTabShapeVertical(QTabBar::Shape shape) {
    return shape == QTabBar::RoundedEast || shape == QTabBar::RoundedWest || shape == QTabBar::TriangularEast
           || shape == QTabBar::TriangularWest;
  }

  static bool isTabShapeTriangular(QTabBar::Shape shape) {
    return shape == QTabBar::TriangularNorth || shape == QTabBar::TriangularSouth || shape == QTabBar::TriangularWest
           || shape == QTabBar::TriangularEast;
  }

  static QTabBar::Shape tabBarShape(const QWidget* w) {
    const auto* tabBar = qobject_cast<const QTabBar*>(w);
    return tabBar ? tabBar->shape() : QTabBar::RoundedNorth;
  }

  // Returns the rect of the tab in its local frame: the frame in which the tab is horizontal, with its
  // outer edge on top and its text running from left to right.
  static QRect tabLocalRect(QTabBar::Shape shape, const QRect& rect) {
    return isTabShapeVertical(shape) ? QRect(0, 0, rect.height(), rect.width())
                                     : QRect(0, 0, rect.width(), rect.height());
  }

  // Returns the transform that maps the local frame of a tab to the widget's coordinates.
  static QTransform tabLocalTransform(QTabBar::Shape shape, const QRect& rect) {
    switch (shape) {
      case QTabBar::RoundedSouth:
      case QTabBar::TriangularSouth:
        return QTransform(1., 0., 0., -1., rect.x(), rect.y() + rect.height());
      case QTabBar::RoundedWest:
      case QTabBar::TriangularWest:
        return QTransform(0., -1., 1., 0., rect.x(), rect.y() + rect.height());
      case QTabBar::RoundedEast:
      case QTabBar::TriangularEast:
        return QTransform(0., 1., -1., 0., rect.x() + rect.width(), rect.y());
      default:
        return QTransform::fromTranslate(rect.x(), rect.y());
    }
  }

  // Returns the radiuses of a part of the tab bar, according to the side where the tab bar is.
  static RadiusesF tabBarRadiuses(QTabBar::Shape shape, double radius, TabBarPart part) {
    switch (shape) {
      case QTabBar::RoundedSouth:
      case QTabBar::TriangularSouth:
        switch (part) {
          case TabBarPart::Bar:
            return RadiusesF(0., 0., radius, radius);
          case TabBarPart::Pane:
            return RadiusesF(radius, radius, 0., 0.);
          case TabBarPart::Start:
            return RadiusesF(0., 0., 0., radius);
          case TabBarPart::End:
            return RadiusesF(0., 0., radius, 0.);
        }
        break;
      case QTabBar::RoundedWest:
      case QTabBar::TriangularWest:
        switch (part) {
          case TabBarPart::Bar:
            return RadiusesF(radius, 0., 0., radius);
          case TabBarPart::Pane:
            return RadiusesF(0., radius, radius, 0.);
          case TabBarPart::Start:
            return RadiusesF(radius, 0., 0., 0.);
          case TabBarPart::End:
            return RadiusesF(0., 0., 0., radius);
        }
        break;
      case QTabBar::RoundedEast:
      case QTabBar::TriangularEast:
        switch (part) {
          case TabBarPart::Bar:
            return RadiusesF(0., radius, radius, 0.);
          case TabBarPart::Pane:
            return RadiusesF(radius, 0., 0., radius);
          case TabBarPart::Start:
            return RadiusesF(0., radius, 0., 0.);
          case TabBarPart::End:
            return RadiusesF(0., 0., radius, 0.);
        }
        break;
      default:
        switch (part) {
          case TabBarPart::Bar:
            return RadiusesF(radius, radius, 0., 0.);
          case TabBarPart::Pane:
            return RadiusesF(0., 0., radius, radius);
          case TabBarPart::Start:
            return RadiusesF(radius, 0., 0., 0.);
          case TabBarPart::End:
            return RadiusesF(0., radius, 0., 0.);
        }
        break;
    }
    return RadiusesF();
  }

  // Returns how much the sides of a triangular tab are slanted.
  int tabSlant() const {
    return theme.spacing * 2;
  }

  // Returns the space taken by the slanted sides of a triangular tab, on each end, that the content must avoid.
  int tabContentInset(QTabBar::Shape shape) const {
    return isTabShapeTriangular(shape) ? tabSlant() / 2 : 0;
  }

  // Returns the extra padding around the tab, in the local frame of the tab.
  // We add extra padding so we have some space to draw nice curve ends.
  QMargins tabExtraPadding(const QStyleOptionTab* optTab, const QWidget*) const {
    const auto spacing = theme.spacing;
    const auto paddingTop = spacing / 2;

    const auto isMoving = optTab->position == QStyleOptionTab::TabPosition::Moving;
    const auto isFirst = isMoving || optTab->position == QStyleOptionTab::TabPosition::OnlyOneTab
                         || optTab->position == QStyleOptionTab::TabPosition::Beginning;
    const auto isLast = isMoving || optTab->position == QStyleOptionTab::TabPosition::OnlyOneTab
                        || optTab->position == QStyleOptionTab::TabPosition::End;

    // The text of a tab on the West side runs from the bottom to the top.
    const auto isReversed = optTab->shape == QTabBar::RoundedWest || optTab->shape == QTabBar::TriangularWest;
    const auto paddingStart = isFirst ? spacing : 0;
    const auto paddingEnd = isLast ? spacing : 0;
    const auto paddingLeft = isReversed ? paddingEnd : paddingStart;
    const auto paddingRight = isReversed ? paddingStart : paddingEnd;

    const auto paddingBottom = 0;
    return QMargins(paddingLeft, paddingTop, paddingRight, paddingBottom);
  }

  /// Makes an IconTheme from the Theme.
  IconTheme iconThemeFromTheme(ColorRole role = ColorRole::Secondary) const {
    switch (role) {
      case ColorRole::Primary:
        return {
          owner.iconForegroundColor(MouseState::Normal, ColorRole::Primary),
          owner.iconForegroundColor(MouseState::Hovered, ColorRole::Primary),
          owner.iconForegroundColor(MouseState::Pressed, ColorRole::Primary),
          owner.iconForegroundColor(MouseState::Disabled, ColorRole::Primary),
        };
      case ColorRole::Secondary:
      default:
        return {
          owner.iconForegroundColor(MouseState::Normal, ColorRole::Secondary),
          owner.iconForegroundColor(MouseState::Hovered, ColorRole::Secondary),
          owner.iconForegroundColor(MouseState::Pressed, ColorRole::Secondary),
          owner.iconForegroundColor(MouseState::Disabled, ColorRole::Secondary),
        };
    }
  }

  static QRect sliderHandleRect(
    const QlementineStyle& style, const QStyleOptionSlider& opt, const qreal position, const QWidget* widget) {
    const auto handleW = style.pixelMetric(QStyle::PM_SliderLength, &opt, widget);
    const auto handleH = style.pixelMetric(QStyle::PM_SliderThickness, &opt, widget);
    const auto handleY = opt.rect.y() + (opt.rect.height() - handleH) / 2;
    const auto range = opt.maximum - opt.minimum;
    const auto ratio = range == 0 ? 0. : (position - opt.minimum) / range;
    const auto handleX = opt.rect.x() + static_cast<int>(ratio * (opt.rect.width() - handleW));
    return QRect{ handleX, handleY, handleW, handleH };
  }

  static QRect sliderFocusRect(
    const QlementineStyle& style, const QStyleOptionSlider& opt, const QRect& handleRect, const QWidget* widget) {
    const auto deltaX = style.pixelMetric(QStyle::PM_FocusFrameHMargin, &opt, widget);
    const auto deltaY = style.pixelMetric(QStyle::PM_FocusFrameVMargin, &opt, widget);
    const auto vMargin = deltaY / 2;
    const auto hMargin = deltaX / 2;
    return handleRect.translated(deltaX, deltaY).marginsAdded(QMargins(hMargin, vMargin, hMargin, vMargin));
  }
};

QlementineStyle::QlementineStyle(QObject* parent)
  : _impl(new QlementineStyleImpl{ *this }) {
  _impl->updatePalette();
  setParent(parent);
  setObjectName(QStringLiteral("QlementineStyle"));

  // This method is virtual so it should not be called in the base class constructor.
  QTimer::singleShot(0, this, [this]() {
    triggerCompleteRepaint();
  });
}

QlementineStyle::~QlementineStyle() = default;

Theme const& QlementineStyle::theme() const {
  return _impl->theme;
}

void QlementineStyle::setTheme(Theme const& theme) {
  if (_impl->theme != theme) {
    _impl->theme = theme;
    Q_EMIT themeChanged();

    triggerCompleteRepaint();
  }
}

void QlementineStyle::setThemeJsonPath(QString const& jsonPath) {
  const auto themeOpt = Theme::fromJsonPath(jsonPath);
  if (themeOpt.has_value()) {
    setTheme(themeOpt.value());
  }
}

void QlementineStyle::setThemeJsonString(QString const& jsonString) {
  const auto themeOpt = Theme::fromJsonString(jsonString);
  if (themeOpt.has_value()) {
    setTheme(themeOpt.value());
  }
}

bool QlementineStyle::animationsEnabled() const {
  return _impl->animations.enabled();
}

void QlementineStyle::setAnimationsEnabled(bool enabled) {
  if (enabled != _impl->animations.enabled()) {
    _impl->animations.setEnabled(enabled);
    Q_EMIT animationsEnabledChanged();
    triggerCompleteRepaint();
  }
}

void QlementineStyle::triggerCompleteRepaint() {
  _impl->updateFonts();
  _impl->updatePalette();

  // Clear generated icons because they depend on colors.
  _impl->standardIconCache.clear();
  QPixmapCache::clear();

  // Update the palette.
  const auto palette = standardPalette();
  QApplication::setPalette(palette);

  // Update the application font.
  QApplication::setFont(_impl->theme.fontRegular);

  // Repaint all top-level widgets.
  const auto topLevelWidgets = QApplication::topLevelWidgets();
  for (auto* widget : topLevelWidgets) {
    widget->update();
  }
}

// Sets automatic icon colorization for the style.
void QlementineStyle::setAutoIconColor(AutoIconColor autoIconColor) {
  _impl->autoIconColor = autoIconColor;
  triggerCompleteRepaint();
}

AutoIconColor QlementineStyle::autoIconColor() const {
  return _impl->autoIconColor;
}

void QlementineStyle::setToolTipMatchesTheme(bool enabled) {
  _impl->toolTipMatchesTheme = enabled;
  triggerCompleteRepaint();
}

bool QlementineStyle::toolTipMatchesTheme() const {
  return _impl->toolTipMatchesTheme;
}

// Sets automatic icon colorization for a specific widget.
// This overrides the default value from ist parent or from autoIconColorEnabled().
void QlementineStyle::setAutoIconColor(QWidget* widget, AutoIconColor autoIconColor) {
  if (widget) {
    widget->setProperty(Property_AutoIconColor, QVariant::fromValue(autoIconColor));
  }
}

AutoIconColor QlementineStyle::autoIconColor(const QWidget* widget) {
  //. Fallback to the app's style.
  if (!widget) {
    const auto* qlementine_style = appStyle();
    return qlementine_style ? qlementine_style->_impl->autoIconColor : AutoIconColor::None;
  }

  // Look for the property on the widget.
  const auto property = widget->property(Property_AutoIconColor);
  if (!property.isValid()) {
    return autoIconColor(widget->parentWidget());
  }
  return property.value<AutoIconColor>();
}

QPixmap QlementineStyle::getColorizedPixmap(
  const QPixmap& input, AutoIconColor autoIconColor, const QColor& fgColor, const QColor& textColor) const {
  switch (autoIconColor) {
    case AutoIconColor::None:
      return input;
    case AutoIconColor::ForegroundColor:
      return qlementine::getColorizedPixmap(input, fgColor);
    case AutoIconColor::TextColor:
      return qlementine::getColorizedPixmap(input, textColor);
  }
  return input;
}

QPixmap QlementineStyle::getColorizedPixmap(
  const QPixmap& input, const QWidget* widget, const QColor& fgColor, const QColor& textColor, MouseState mouse) const {
  const auto aic = autoIconColor(widget);
  if (aic == AutoIconColor::None) {
    return input;
  }
  if (const auto status = widgetStatus(widget); status != Status::Default) {
    const auto& color = statusColor(status, mouse);
    return qlementine::getColorizedPixmap(input, color);
  }
  return getColorizedPixmap(input, aic, fgColor, textColor);
}

QIcon QlementineStyle::makeThemedIcon(const QString& svgPath, const QSize& size, ColorRole role) const {
  const auto iconTheme = _impl->iconThemeFromTheme(role);
  return makeIconFromSvg(svgPath, iconTheme, size);
}

QIcon QlementineStyle::makeThemedIconFromName(const QString& name, const QSize& size, ColorRole role) const {
  if (_impl->iconPathFunc) {
    const auto iconPath = _impl->iconPathFunc(name);
    return makeThemedIcon(iconPath, size, role);
  } else {
    return QIcon::fromTheme(name);
  }
}

QIcon QlementineStyle::makeThemedIconFromData(const QByteArray& svgData, const QSize& size, ColorRole role) const {
  const auto iconTheme = _impl->iconThemeFromTheme(role);
  return makeIconFromSvgData(svgData, iconTheme, size);
}

void QlementineStyle::setIconPathGetter(const std::function<QString(QString)>& func) {
  _impl->iconPathFunc = func;
}

/* QStyle overrides. */

void QlementineStyle::drawPrimitive(PrimitiveElement pe, const QStyleOption* opt, QPainter* p, const QWidget* w) const {
  QPainterStateGuard stateGuard(p);
  p->setRenderHint(QPainter::SmoothPixmapTransform, true);
  switch (pe) {
    case PE_Frame:
      //qDebug() << pe;
      return;
    case PE_FrameDefaultButton:
      break;
    case PE_FrameDockWidget:
      break;
    case PE_FrameFocusRect: {
      const auto* optRoundedFocus = qstyleoption_cast<const QStyleOptionFocusRoundedRect*>(opt);
      const QStyleOptionFocusRect* optFocus =
        optRoundedFocus ? optRoundedFocus : qstyleoption_cast<const QStyleOptionFocusRect*>(opt);
      if (optFocus) {
        if (optFocus->rect.isEmpty())
          return;

        RadiusesF borderRadiuses;
        if (optRoundedFocus) {
          borderRadiuses = optRoundedFocus->radiuses;
        }

        const auto status = widgetStatus(w);
        const auto& borderColor = focusBorderColor(status);
        const auto focused = optFocus->state.testFlag(State_HasFocus);
        const auto progress = focused ? 1. : 0.;
        const auto currentProgress =
          _impl->animations.animateFocusBorderProgress(w, progress, _impl->theme.focusAnimationDuration);
        const auto currentBorderW = currentProgress * _impl->theme.focusBorderWidth;
        const auto margin = (1. - currentProgress) * _impl->theme.focusBorderWidth;
        const auto currentFocusRect = QRectF(optFocus->rect).marginsRemoved(QMarginsF(margin, margin, margin, margin));
        const auto currentRadius = borderRadiuses + currentBorderW;

        if (currentBorderW >= 0.1) {
          drawRoundedRectBorder(p, currentFocusRect, borderColor, currentBorderW, currentRadius);
        }
      }
    }
      return;
    case PE_FrameGroupBox:
      if (const auto* frameOpt = qstyleoption_cast<const QStyleOptionFrame*>(opt)) {
        const auto mouse = qlementine::getMouseState(frameOpt->state);
        const auto& bgColor = groupBoxBackgroundColor(mouse);
        const auto& borderColor = groupBoxBorderColor(mouse);
        const auto borderW = _impl->theme.borderWidth;
        drawRoundedRect(p, frameOpt->rect, bgColor, _impl->theme.borderRadius);
        drawRoundedRectBorder(p, frameOpt->rect, borderColor, borderW, _impl->theme.borderRadius);
      }
      return;
    case PE_FrameLineEdit:
      break;
    case PE_FrameMenu:
      return; // Let PE_PanelMenu do the drawing.
    case PE_FrameStatusBarItem: {
      const auto rect = opt->rect;
      const auto penColor = _impl->theme.borderColor;
      const auto penWidth = _impl->theme.borderWidth;
      const auto p1 = QPoint{ rect.x() + 1 + penWidth, rect.y() + rect.x() };
      const auto p2 = QPoint{ rect.x() + 1 + penWidth, rect.y() + rect.height() };
      p->setPen(QPen(penColor, penWidth, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
      p->setBrush(Qt::NoBrush);
      p->drawLine(p1, p2);
    }
      return;
    case PE_FrameTabWidget: {
      // QTabWidget.cpp, line 1296, in QTabWidget::paintEvent():
      // The widget does not draw the Tab bar background unless it's in
      // document mode. We always want that background, whatever the mode,
      // so this hack does the trick.
      const auto* tabWidget = qobject_cast<const QTabWidget*>(w);
      const auto documentMode = tabWidget && tabWidget->documentMode();
      const auto* tabBar = tabWidget ? tabWidget->tabBar() : nullptr;
      if (!documentMode && tabBar) {
        // Draw a border around the content.
        const auto shape = tabBar->shape();
        const auto mouse = getMouseState(opt->state);
        const auto radius = _impl->theme.borderRadius * 1.5;
        const auto borderColor = tabBarBackgroundColor(mouse);
        const auto borderW = _impl->theme.borderWidth;
        auto frameRect = opt->rect;
        switch (shape) {
          case QTabBar::RoundedSouth:
          case QTabBar::TriangularSouth:
            frameRect.adjust(0, 0, 0, borderW);
            break;
          case QTabBar::RoundedWest:
          case QTabBar::TriangularWest:
            frameRect.adjust(-borderW, 0, 0, 0);
            break;
          case QTabBar::RoundedEast:
          case QTabBar::TriangularEast:
            frameRect.adjust(0, 0, borderW, 0);
            break;
          default:
            frameRect.adjust(0, -borderW, 0, 0);
            break;
        }
        drawRoundedRectBorder(p, frameRect, borderColor, borderW,
          QlementineStyleImpl::tabBarRadiuses(shape, radius, QlementineStyleImpl::TabBarPart::Pane));

        // Draw the background of the tab bar.
        QStyleOptionTabBarBase tabBarOpt;
        tabBarOpt.initFrom(tabBar);
        tabBarOpt.rect = QlementineStyleImpl::isTabShapeVertical(shape)
                           ? QRect(tabBar->x(), 0, tabBar->width(), tabWidget->height())
                           : QRect(0, tabBar->y(), tabWidget->width(), tabBar->height());
        tabBarOpt.shape = shape;
        tabBarOpt.documentMode = documentMode;
        drawPrimitive(PE_FrameTabBarBase, &tabBarOpt, p, tabBar);
      }
    }
      return;
    case PE_FrameWindow:
      break;
    case PE_FrameButtonBevel: {
      // Try to get information about rounded corners. By default, all corners are rounded.
      const auto* optButton = buttonStyleOption(opt);
      const auto* optRoundedButton = qstyleoption_cast<const QStyleOptionRoundedButton*>(opt);
      if (optRoundedButton) {
        optButton = optRoundedButton;
      }
      if (optButton) {
        const auto isDefault = optButton->features.testFlag(QStyleOptionButton::DefaultButton);
        const auto isFlat = optButton->features.testFlag(QStyleOptionButton::Flat);
        const auto mouse = isFlat ? getToolButtonMouseState(opt->state) : getMouseState(opt->state);
        const auto role = getColorRole(opt->state, isDefault);
        const auto hasFocus = optButton->state.testFlag(QStyle::State_HasFocus);
        const auto effectiveMouse =
          isFlat && hasFocus && mouse == MouseState::Transparent ? MouseState::Hovered : mouse;
        const auto& bgColor =
          isFlat ? toolButtonBackgroundColor(effectiveMouse, role) : buttonBackgroundColor(effectiveMouse, role, w);
        const auto& currentBgColor =
          _impl->animations.animateBackgroundColor(w, bgColor, _impl->theme.animationDuration);
        const auto radiuses = optRoundedButton ? optRoundedButton->radiuses : RadiusesF{ _impl->theme.borderRadius };
        drawRoundedRect(p, optButton->rect, currentBgColor, radiuses);
      }
      return;
    }
    case PE_FrameTabBarBase:
      if (const auto* optTabBar = qstyleoption_cast<const QStyleOptionTabBarBase*>(opt)) {
        const auto mouse = getMouseState(opt->state);
        const auto& bgColor = tabBarBackgroundColor(mouse);
        if (optTabBar->documentMode) {
          p->fillRect(opt->rect, bgColor);
        } else {
          const auto radius = _impl->theme.borderRadius * 1.5;
          drawRoundedRect(p, opt->rect, bgColor,
            QlementineStyleImpl::tabBarRadiuses(optTabBar->shape, radius, QlementineStyleImpl::TabBarPart::Bar));
        }
      }
      return;
    case PE_PanelButtonCommand:
      break;
    case PE_PanelButtonBevel:
      break;
    case PE_FrameButtonTool:
      return;
    case PE_PanelButtonTool:
      if (const auto* optToolButton = qstyleoption_cast<const QStyleOptionToolButton*>(opt)) {
        const auto& rect = optToolButton->rect;

        // Special case/hack for buttons in TabBar.
        const auto isTabBarScrollButton =
          qobject_cast<const QTabBar*>(w->parentWidget()) != nullptr && optToolButton->arrowType != Qt::NoArrow;
        const auto hasMenu = optToolButton->features.testFlag(QStyleOptionToolButton::HasMenu);
        const auto menuIsOnSeparateButton =
          hasMenu && optToolButton->features.testFlag(QStyleOptionToolButton::ToolButtonFeature::MenuButtonPopup);

        const auto isMenuBarExtensionButton = qobject_cast<const QMenuBar*>(w->parentWidget()) != nullptr;
        const auto radius = isMenuBarExtensionButton ? _impl->theme.menuBarItemBorderRadius : _impl->theme.borderRadius;

        // Radiuses depend on the type of ToolButton.
        const auto& buttonRadiuses =
          isTabBarScrollButton ? RadiusesF{ rect.height() }
                               : (menuIsOnSeparateButton ? RadiusesF{ radius, 0., 0., radius } : RadiusesF{ radius });

        // Little hack to avoid having a checked extension button.
        auto buttonState = optToolButton->state;
        const auto isExtensionButton = w->objectName() == QStringLiteral("qt_toolbar_ext_button");
        if (isExtensionButton) {
          buttonState.setFlag(State_On, false);
        }
        const auto mouse = getMouseState(buttonState);
        const auto role = getColorRole(buttonState, false);

        // Draw background.
        const auto& bgColor =
          isTabBarScrollButton ? tabBarScrollButtonBackgroundColor(mouse) : toolButtonBackgroundColor(mouse, role);
        const auto& currentColor = _impl->animations.animateBackgroundColor(w, bgColor, _impl->theme.animationDuration);
        drawRoundedRect(p, rect, currentColor, buttonRadiuses);
      }
      return;
    case PE_PanelMenuBar: {
      const auto& bgColor = menuBarBackgroundColor();
      const auto& borderColor = menuBarBorderColor();
      const auto lineWidth = _impl->theme.borderWidth;
      const auto x1 = static_cast<double>(opt->rect.x());
      const auto x2 = static_cast<double>(x1 + opt->rect.width());
      const auto y = opt->rect.y() + opt->rect.height() - lineWidth / 2.;

      p->fillRect(opt->rect, bgColor);
      p->setBrush(Qt::NoBrush);
      p->setPen(QPen(borderColor, lineWidth, Qt::SolidLine, Qt::FlatCap));
      p->drawLine(QPointF(x1, lineWidth / 2.), QPointF(x2, lineWidth / 2.));
      p->drawLine(QPointF(x1, y), QPointF(x2, y));
    }
      return;
    case PE_PanelToolBar:
      if (const auto* optToolBar = qstyleoption_cast<const QStyleOptionToolBar*>(opt)) {
        const auto& bgColor = toolBarBackgroundColor();
        const auto& rect = optToolBar->rect;
        p->fillRect(rect, bgColor);

        const auto lineW = _impl->theme.borderWidth;
        const auto& lineColor = toolBarBorderColor();
        p->setPen(QPen(lineColor, lineW, Qt::SolidLine, Qt::FlatCap));
        p->setBrush(Qt::NoBrush);

        auto* toolBar = qobject_cast<const QToolBar*>(w);
        const auto orientation = toolBar->orientation();
        const auto allowedAreas = toolBar->allowedAreas();

        if (orientation == Qt::Horizontal) {
          if (allowedAreas.testFlag(Qt::TopToolBarArea)) {
            // Draw bottom border
            const auto x1 = rect.x();
            const auto y1 = rect.y() + rect.height() - lineW / 2.;
            const auto x2 = rect.x() + rect.width();
            const auto y2 = y1;
            p->drawLine(QPointF(x1, y1), QPointF(x2, y2));
          }
          if (allowedAreas.testFlag(Qt::BottomToolBarArea)) {
            // Draw top border
            const auto x1 = rect.x();
            const auto y1 = rect.y() + lineW / 2.;
            const auto x2 = rect.x() + rect.width();
            const auto y2 = y1;
            p->drawLine(QPointF(x1, y1), QPointF(x2, y2));
          }
        } else if (orientation == Qt::Vertical) {
          if (allowedAreas.testFlag(Qt::LeftToolBarArea)) {
            // Draw right border
            const auto x1 = rect.x() + rect.width() - lineW / 2.;
            const auto y1 = rect.y();
            const auto x2 = x1;
            const auto y2 = rect.y() + rect.height();
            p->drawLine(QPointF(x1, y1), QPointF(x2, y2));
          }
          if (allowedAreas.testFlag(Qt::RightToolBarArea)) {
            // Draw left border
            const auto x1 = rect.x() + lineW / 2.;
            const auto y1 = rect.y();
            const auto x2 = x1;
            const auto y2 = rect.y() + rect.height();
            p->drawLine(QPointF(x1, y1), QPointF(x2, y2));
          }
        }
      }
      return;
    case PE_PanelLineEdit:
      if (const auto* optPanelLineEdit = qstyleoption_cast<const QStyleOptionFrame*>(opt)) {
        const auto* parentWidget = w ? w->parentWidget() : nullptr;
        const auto* parentParentWidget = parentWidget ? parentWidget->parentWidget() : nullptr;
        const auto isTabCellEditor =
          parentParentWidget && qobject_cast<const QAbstractItemView*>(parentParentWidget->parentWidget());

        const auto isComboBoxLineEdit = qobject_cast<const QComboBox*>(w->parentWidget());
        const auto qPlainTextEdit = qobject_cast<const QPlainTextEdit*>(w);
        const auto isPlainQPlainTextEdit = qPlainTextEdit && qPlainTextEdit->frameShadow() == QFrame::Shadow::Plain;
        const auto isPlainLineEdit = !isComboBoxLineEdit && !qPlainTextEdit && optPanelLineEdit->lineWidth == 0;
        const auto isPlain = isPlainQPlainTextEdit || isPlainLineEdit;

        const auto radiusF = static_cast<double>(_impl->theme.borderRadius);
        auto radiuses = RadiusesF{ radiusF };
        if (isPlain || isTabCellEditor || (w && w->metaObject()->className() == QStringLiteral("QExpandingLineEdit"))) {
          // The QExpandingLineEdit class is used by QStyleItemDelegate when the cell context type is text.
          radiuses.topRight = 0.;
          radiuses.bottomRight = 0.;
          radiuses.topLeft = 0.;
          radiuses.bottomLeft = 0.;
        } else if (qobject_cast<const QAbstractSpinBox*>(parentWidget) != nullptr
                   || qobject_cast<const QComboBox*>(parentWidget) != nullptr) {
          radiuses.topRight = 0.;
          radiuses.bottomRight = 0.;
        }

        // Fix qlinedit.cpp:118, State_Sunken is always true.
        auto fixedState = optPanelLineEdit->state;
        fixedState.setFlag(QStyle::State_Sunken, false);

        const auto& rect = optPanelLineEdit->rect;
        const auto status = widgetStatus(w);
        const auto mouse = getMouseState(fixedState);
        const auto focus = getFocusState(optPanelLineEdit->state);
        const auto& bgColor = textFieldBackgroundColor(mouse, status);
        const auto& borderColor = textFieldBorderColor(mouse, focus, status);
        const auto borderW = _impl->theme.borderWidth;
        const auto& currentBorderColor =
          _impl->animations.animateBorderColor(w, borderColor, _impl->theme.animationDuration);

        // Background.
        drawRoundedRect(p, rect, bgColor, radiuses);
        if (!isPlainLineEdit) {
          drawRoundedRectBorder(p, rect, currentBorderColor, borderW, radiuses);
        }
        // Drawing the caret should be non-antialiased
        p->setRenderHint(QPainter::Antialiasing, false);
      }
      return;
    case PE_IndicatorArrowDown:
      break;
    case PE_IndicatorArrowLeft:
      break;
    case PE_IndicatorArrowRight:
      break;
    case PE_IndicatorArrowUp:
      break;
    case PE_IndicatorBranch:
      if (const auto* optItem = qstyleoption_cast<const QStyleOptionViewItem*>(opt)) {
        // Arrow.
        if (opt->state.testFlag(State_Children)) {
          const auto open = opt->state & State_Open;
          const auto indicatorSize = _impl->theme.iconSize;
          const auto hShift = _impl->theme.spacing / 4;
          const auto indicatorRect =
            QRect(QPoint{ hShift + optItem->rect.x() + (optItem->rect.width() - indicatorSize.width()) / 2,
                    optItem->rect.y() + (optItem->rect.height() - indicatorSize.height()) / 2 },
              indicatorSize);

          const auto mouse = getMouseState(opt->state);
          const auto selection = getSelectionState(opt->state);
          const auto active = getActiveState(opt->state);
          const auto widgetHasFocus = w->hasFocus();
          const auto focus =
            widgetHasFocus && selection == SelectionState::Selected ? FocusState::Focused : FocusState::NotFocused;
          const auto& fgColor = listItemForegroundColor(mouse, selection, focus, active);
          p->setRenderHint(QPainter::Antialiasing, true);
          p->setBrush(Qt::NoBrush);
          p->setPen(QPen(fgColor, iconPenWidth, Qt::SolidLine, Qt::RoundCap));
          drawTreeViewIndicator(indicatorRect, p, open);
        }
      }
      return;
    case PE_IndicatorButtonDropDown:
      break;
    case PE_IndicatorItemViewItemCheck:
      if (const auto* optItem = qstyleoption_cast<const QStyleOptionViewItem*>(opt)) {
        const auto checkState = getCheckState(optItem->checkState);
        const auto checkBoxProgress = checkState == CheckState::NotChecked ? 0. : 1.;
        const auto mouse = getMouseState(optItem->state);
        const auto selected = getSelectionState(optItem->state);
        const auto active = getActiveState(optItem->state);
        const auto& checkBoxFgColor = listItemCheckButtonForegroundColor(mouse, checkState, selected, active);
        const auto& checkBoxBgColor = listItemCheckButtonBackgroundColor(mouse, checkState, selected, active);
        const auto& checkBoxBorderColor = listItemCheckButtonBorderColor(mouse, checkState, selected, active);
        const auto radius = _impl->theme.checkBoxBorderRadius;
        const auto borderWidth = _impl->theme.borderWidth;
        // Ensure the rect is a perfect square, centered in optButton->rect;.
        const auto indicatorSize = std::max(optItem->rect.width(), optItem->rect.height());
        const auto indicatorX = optItem->rect.x() + (optItem->rect.width() - indicatorSize);
        const auto indicatorY = optItem->rect.y() + (optItem->rect.height() - indicatorSize);
        const auto indicatorRect = QRect{ indicatorX, indicatorY, indicatorSize, indicatorSize };
        drawCheckButton(p, indicatorRect, radius, checkBoxBgColor, checkBoxBorderColor, checkBoxFgColor, borderWidth,
          checkBoxProgress, checkState);
      }
      return;
    case PE_IndicatorCheckBox:
    case PE_IndicatorRadioButton:
      if (const auto* optButton = buttonStyleOption(opt)) {
        const auto checkState = getCheckState(optButton->state);
        const auto mouse = getMouseState(optButton->state);
        const auto focus = getFocusState(optButton->state);
        const auto& bgColor = checkButtonBackgroundColor(mouse, checkState);
        const auto& fgColor = checkButtonForegroundColor(mouse, checkState);
        const auto& borderColor = checkButtonBorderColor(mouse, focus, checkState);
        const auto borderW = _impl->theme.borderWidth;

        // Ensure the rect is a perfect square, centered in optButton->rect.
        const auto indicatorSize = std::max(optButton->rect.width(), optButton->rect.height());
        const auto indicatorX = optButton->rect.x() + (optButton->rect.width() - indicatorSize);
        const auto indicatorY = optButton->rect.y() + (optButton->rect.height() - indicatorSize);
        const auto indicatorRect = QRect{ indicatorX, indicatorY, indicatorSize, indicatorSize };

        // Animations.
        const auto progress = checkState == CheckState::NotChecked ? 0. : 1.;
        const auto& currentBgColor =
          _impl->animations.animateBackgroundColor(w, bgColor, _impl->theme.animationDuration);
        const auto& currentBorderColor =
          _impl->animations.animateBorderColor(w, borderColor, _impl->theme.animationDuration);
        const auto currentProgress = _impl->animations.animateProgress(w, progress, _impl->theme.animationDuration);

        const auto isRadio = pe == PE_IndicatorRadioButton;
        if (isRadio) {
          drawRadioButton(p, indicatorRect, currentBgColor, currentBorderColor, fgColor, borderW, currentProgress);
        } else {
          const auto radius = _impl->theme.checkBoxBorderRadius;
          drawCheckButton(p, indicatorRect, radius, currentBgColor, currentBorderColor, fgColor, borderW,
            currentProgress, checkState);
        }
      }
      return;
    case PE_IndicatorDockWidgetResizeHandle:
      break;
    case PE_IndicatorHeaderArrow:
      if (const auto* optHeader = qstyleoption_cast<const QStyleOptionHeader*>(opt)) {
        const auto indicatorType = optHeader->sortIndicator;
        const auto mouse = getMouseState(optHeader->state);
        const auto checked = getCheckState(optHeader->state);
        const auto& fgColor = tableHeaderFgColor(mouse, checked);
        p->setRenderHint(QPainter::Antialiasing, true);
        p->setBrush(Qt::NoBrush);
        p->setPen(QPen(fgColor, 1.001, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        switch (indicatorType) {
          case QStyleOptionHeader::SortIndicator::SortDown:
            drawArrowUp(optHeader->rect, p);
            break;
          case QStyleOptionHeader::SortIndicator::SortUp:
            drawArrowDown(optHeader->rect, p);
            break;
          default:
            break;
        }
      }
      return;
    case PE_IndicatorMenuCheckMark:
      break;
    case PE_IndicatorProgressChunk:
      break;
    case PE_IndicatorSpinDown:
      break;
    case PE_IndicatorSpinMinus:
      break;
    case PE_IndicatorSpinPlus:
      break;
    case PE_IndicatorSpinUp:
      break;
    case PE_IndicatorToolBarHandle:
      // TODO
      break;
    case PE_IndicatorToolBarSeparator: {
      const auto& rect = opt->rect;
      const auto& color = toolBarSeparatorColor();
      const auto horizontal = opt->state.testFlag(State_Horizontal);
      const auto lineW = _impl->theme.borderWidth;
      constexpr auto padding = 0; //_impl->theme.spacing / 2;
      p->setBrush(Qt::NoBrush);
      p->setPen(QPen(color, lineW, Qt::SolidLine, Qt::FlatCap));
      if (horizontal) {
        const auto x = rect.x() + (rect.width() - lineW) / 2.;
        const auto y1 = static_cast<double>(rect.y() + padding);
        const auto y2 = static_cast<double>(rect.y() + rect.height() - padding);
        p->drawLine(QPointF{ x, y1 }, QPointF{ x, y2 });
      } else {
        const auto y = rect.y() + (rect.height() - lineW) / 2.;
        const auto x1 = static_cast<double>(rect.x() + padding);
        const auto x2 = static_cast<double>(rect.x() + rect.width() - padding);
        p->drawLine(QPointF{ x1, y }, QPointF{ x2, y });
      }
    }
      return;
    case PE_PanelTipLabel: {
      const auto& bgColor = toolTipBackgroundColor();
      const auto& borderColor = toolTipBorderColor();
      constexpr auto radius = 0.;
      const auto borderW = _impl->theme.borderWidth;
      p->setRenderHint(QPainter::Antialiasing, true);
      p->setPen(Qt::NoPen);
      p->setBrush(bgColor);
      p->drawRoundedRect(opt->rect, radius, radius);
      drawRoundedRectBorder(p, opt->rect, borderColor, borderW, radius);
    }
      return;
    case PE_IndicatorTabTear: {
      const auto* tabBar = qobject_cast<const QTabBar*>(w);
      const auto documentMode = tabBar && tabBar->documentMode();
      const auto shape = QlementineStyleImpl::tabBarShape(w);
      const auto& rect = opt->rect;
      const auto startPos = QPointF(rect.topLeft());
      const auto shadowW = _impl->theme.spacing * 3;
      const auto endPos =
        QPointF(rect.topLeft())
        + (QlementineStyleImpl::isTabShapeVertical(shape) ? QPointF{ 0., static_cast<double>(shadowW) }
                                                          : QPointF{ static_cast<double>(shadowW), 0. });
      auto gradient = QLinearGradient(startPos, endPos);
      const auto& startColor = tabBarShadowColor();
      const auto& endColor = _impl->theme.shadowColorTransparent;
      gradient.setColorAt(0., startColor);
      gradient.setColorAt(1., endColor);
      const auto radius = _impl->theme.borderRadius * 1.5;
      const auto compModeBackup = p->compositionMode();
      p->setCompositionMode(QPainter::CompositionMode_Multiply);

      drawRoundedRect(p, rect, gradient,
        documentMode ? RadiusesF(0.)
                     : QlementineStyleImpl::tabBarRadiuses(shape, radius, QlementineStyleImpl::TabBarPart::Start));
      p->setCompositionMode(compModeBackup);
    }
      return;
    case PE_IndicatorTabTearRight: {
      const auto* tabBar = qobject_cast<const QTabBar*>(w);
      const auto documentMode = tabBar && tabBar->documentMode();
      const auto shape = QlementineStyleImpl::tabBarShape(w);
      const auto vertical = QlementineStyleImpl::isTabShapeVertical(shape);
      const auto& rect = opt->rect;

      const auto scrollButtonsW = _impl->theme.controlHeightMedium * 2 + _impl->theme.spacing * 3;
      const auto shadowW = _impl->theme.spacing * 3;

      // Shadow gradient.
      const auto startPos = QPointF(rect.topLeft());
      const auto endPos =
        QPointF(rect.topLeft())
        + (vertical ? QPointF{ 0., static_cast<double>(shadowW) } : QPointF{ static_cast<double>(shadowW), 0. });
      auto gradient = QLinearGradient(startPos, endPos);
      const auto& startColor = _impl->theme.shadowColorTransparent;
      const auto& endColor = tabBarShadowColor();
      gradient.setColorAt(0., startColor);
      gradient.setColorAt(1., endColor);
      const auto compModeBackup = p->compositionMode();
      p->setCompositionMode(QPainter::CompositionMode_Multiply);
      const auto radius = _impl->theme.borderRadius * 1.5;
      const auto endRadiuses =
        documentMode ? RadiusesF(0.)
                     : QlementineStyleImpl::tabBarRadiuses(shape, radius, QlementineStyleImpl::TabBarPart::End);
      drawRoundedRect(p, rect, gradient, endRadiuses);
      p->setCompositionMode(compModeBackup);

      // Filled rectangle below scroll buttons.
      // We need to fill the whole surface to ensure tabs are not visible below.
      const auto mouse = getMouseState(opt->state);
      const auto& tabBarBgColor = tabBarBackgroundColor(mouse);
      const auto filledRect =
        vertical ? QRect(rect.x(), rect.y() + rect.height() - scrollButtonsW, rect.width(), scrollButtonsW)
                 : QRect(rect.x() + rect.width() - scrollButtonsW, rect.y(), scrollButtonsW, rect.height());
      drawRoundedRect(p, filledRect, tabBarBgColor, endRadiuses);
    }
      return;
    case PE_PanelScrollAreaCorner:
      break;
    case PE_Widget:
      break;
    case PE_IndicatorColumnViewArrow:
      break;
    case PE_IndicatorItemViewItemDrop:
      break;
    case PE_PanelItemViewItem:
      if (const auto* optItem = qstyleoption_cast<const QStyleOptionViewItem*>(opt)) {
        const auto& rect = optItem->rect;
        const auto row = optItem->index.row();
        const auto column = optItem->index.column();

        // Draw cell background color.
        // Make it consistent with the text color in CE_ItemViewItem.
        const auto itemState = optItem->state;
        const auto mouse = getMouseState(itemState);
        const auto selection = getSelectionState(itemState);
        const auto widgetHasFocus = (w && w->hasFocus());
        const auto focus =
          widgetHasFocus && selection == SelectionState::Selected ? FocusState::Focused : FocusState::NotFocused;
        const auto active = getActiveState(itemState);
        const auto& color = listItemBackgroundColor(mouse, selection, focus, active, optItem->index, w);
        const auto listView = qobject_cast<const QListView*>(w);
        const auto viewMode = listView ? listView->viewMode() : QListView::ListMode;

        if (viewMode == QListView::IconMode) {
          p->setRenderHint(QPainter::Antialiasing, true);
          p->setBrush(color);
          p->setPen(Qt::NoPen);
          p->drawRoundedRect(rect, _impl->theme.borderRadius, _impl->theme.borderRadius);
        } else {
          p->fillRect(rect, color);

          // Border on the left if necessary.
          if (column == 0) {
            if (const auto* tableView = qobject_cast<const QTableView*>(w)) {
              if (tableView->showGrid() && tableView->verticalHeader()->isHidden()) {
                const auto lineW = _impl->theme.borderWidth;
                const auto p1 = QPointF(rect.x() + lineW * .5, rect.y());
                const auto p2 = QPointF(rect.x() + lineW * .5, rect.y() + rect.height());
                const auto& lineColor = tableLineColor();
                p->setRenderHint(QPainter::Antialiasing, false);
                p->setPen(QPen(lineColor, lineW));
                p->drawLine(p1, p2);
              }
            }
          }
          // Border on the top if necessary.
          if (row == 0) {
            if (const auto* tableView = qobject_cast<const QTableView*>(w)) {
              if (tableView->showGrid() && tableView->horizontalHeader()->isHidden()) {
                const auto lineW = _impl->theme.borderWidth;
                const auto p1 = QPointF(rect.x(), rect.y() + lineW * .5);
                const auto p2 = QPointF(rect.x() + rect.width(), rect.y() + lineW * .5);
                const auto& lineColor = tableLineColor();
                p->setRenderHint(QPainter::Antialiasing, false);
                p->setPen(QPen(lineColor, lineW));
                p->drawLine(p1, p2);
              }
            }
          }

          // Border that indicates which cell has focus.
          // We don't show this border in the first column of a table/tree (the column with the arrow).
          const auto isTable = qobject_cast<const QTableView*>(w) != nullptr;
          if (isTable && row < 0)
            return;

#if 0
          //const auto* itemView = qobject_cast<const QAbstractItemView*>(optItem->widget);
          //const auto* model = itemView ? itemView->model() : nullptr;
          //const auto columnCount = model ? model->columnCount() : 1;
          //const auto multiColumn = columnCount > 1;
          //const auto isCurrentCell = active == ActiveState::Active && focus == FocusState::Focused;
          //const auto multiSelection = itemView ? itemView->selectionMode() != QAbstractItemView::SelectionMode::SingleSelection : false;
          const auto showCellFocus = true; //multiColumn ? isCurrentCell : multiSelection;
          const auto cellFocus = showCellFocus ? focus : FocusState::NotFocused;
          const auto& borderColor = cellItemFocusBorderColor(cellFocus, selection, active);
          const auto borderW = _impl->theme.borderWidth * 2;
          auto borderRect = optItem->rect;
          borderRect.setLeft(0);
          drawRectBorder(p, borderRect, borderColor, borderW);
#endif
        }
      }
      return;
    case PE_PanelItemViewRow:
      if (const auto* optItem = qstyleoption_cast<const QStyleOptionViewItem*>(opt)) {
        // Draw alternate row color.
        const auto alternate = getAlternateState(optItem->features);
        const auto mouse = optItem->state.testFlag(State_Enabled) ? MouseState::Normal : MouseState::Disabled;
        const auto& color = listItemRowBackgroundColor(mouse, alternate);
        p->fillRect(optItem->rect, color);

        // Draw selection color in the arrow area,
        // except in comboboxes as selection drawing is handled by the delegate already.
        const auto* popup = w->parentWidget();
        const auto isComboBoxPopupContainer = popup != nullptr && popup->inherits("QComboBoxPrivateContainer");
        if (!isComboBoxPopupContainer) {
          drawPrimitive(PE_PanelItemViewItem, opt, p, w);
        }
      }
      return;
    case PE_PanelStatusBar: {
      const auto& bgColor = statusBarBackgroundColor();
      const auto& borderColor = statusBarBorderColor();
      const auto borderW = _impl->theme.borderWidth;
      p->fillRect(opt->rect, bgColor);

      const auto lineRect = QRect(opt->rect.x(), opt->rect.y(), opt->rect.width(), borderW);
      p->fillRect(lineRect, borderColor);
    }
      return;
    case PE_IndicatorTabClose:
      if (const auto* button = qobject_cast<const QAbstractButton*>(w)) {
        if (const auto* tabBar = qobject_cast<const QTabBar*>(w->parentWidget())) {
          // Check if button should be visible.
          const auto& rect = opt->rect;
          const auto tabIndex = tabBar->tabAt(w->mapToParent(rect.center()));
          const auto tabSelected = opt->state.testFlag(State_Selected);

          auto tabHovered = false;
          if (tabBar->underMouse()) {
            const auto mousePos = tabBar->mapFromGlobal(QCursor::pos());
            const auto mouseTab = tabBar->tabAt(mousePos);
            tabHovered = tabIndex == mouseTab /*&& opt->state.testFlag(State_Raised)*/;
          }

          const auto pressedButtons = QGuiApplication::mouseButtons();
          const auto tabBarPressed = pressedButtons != Qt::NoButton && !button->isDown();
          const auto visible = (!tabBarPressed && tabHovered) || tabSelected;

          // Avoid animation (t=0) if mouse not over.
          const auto duration = visible ? _impl->theme.animationDuration : 0;

          // Background.
          const auto radius = static_cast<double>(rect.height()) / 2.;
          const auto mouse = getTabItemMouseState(opt->state, tabHovered);
          const auto selected = getSelectionState(opt->state);
          const auto& bgColor = tabCloseButtonBackgroundColor(mouse, selected);
          const auto currentBgColor = _impl->animations.animateBackgroundColor(button, bgColor, duration);
          p->setRenderHint(QPainter::Antialiasing, true);
          p->setPen(Qt::NoPen);
          p->setBrush(currentBgColor);
          p->drawRoundedRect(rect, radius, radius);

          // Foreground.
          const auto& fgColor = tabCloseButtonForegroundColor(mouse, selected);
          const auto currentFgColor = _impl->animations.animateForegroundColor(button, fgColor, duration);
          p->setPen(QPen(currentFgColor, iconPenWidth, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
          p->setBrush(Qt::NoBrush);
          const auto& iconSize = _impl->theme.iconSize;
          const auto closeRect = QRect(rect.x() + (rect.width() - iconSize.width()) / 2,
            rect.y() + (rect.height() - iconSize.height()) / 2, iconSize.width(), iconSize.height());
          drawCloseIndicator(closeRect, p);
        }
      }
      return;
    case PE_PanelMenu: {
      const auto radius = _impl->theme.borderRadius;
      const auto& bgColor = menuBackgroundColor();
      const auto& borderColor = menuBorderColor();
      const auto borderW = _impl->theme.borderWidth;
      p->setRenderHint(QPainter::Antialiasing, true);
      const auto totalRect = opt->rect;
      const auto shadowPadding = pixelMetric(PM_MenuPanelWidth);
      const auto frameRect = totalRect.marginsRemoved({ shadowPadding, shadowPadding, shadowPadding, shadowPadding });
      const auto dropShadowRadius = _impl->theme.spacing;
      const auto dropShadowOffsetY = shadowPadding / 3;
      const auto dropShadowPixmap =
        getDropShadowPixmap(frameRect.size(), radius, dropShadowRadius, _impl->theme.shadowColor1);
      const auto dropShadowX = frameRect.x() + (frameRect.width() - dropShadowPixmap.width()) / 2;
      const auto dropShadowY = frameRect.y() + (frameRect.height() - dropShadowPixmap.height()) / 2 + dropShadowOffsetY;

      const auto compMode = p->compositionMode();
      p->setCompositionMode(QPainter::CompositionMode::CompositionMode_Multiply);
      p->drawPixmap(dropShadowX, dropShadowY, dropShadowPixmap);
      p->setCompositionMode(compMode);
      // Avoid ugly antialiasing artefacts in the corners.
      const auto halfBorderW = borderW / 2.;
      const auto bgFrameRect =
        QRectF(frameRect).marginsRemoved(QMarginsF(halfBorderW, halfBorderW, halfBorderW, halfBorderW));
      drawRoundedRect(p, bgFrameRect, bgColor, radius);
      drawRoundedRectBorder(p, frameRect, borderColor, borderW, radius);
    }
      return;
    default:
      break;
  }
  QCommonStyle::drawPrimitive(pe, opt, p, w);
}

void QlementineStyle::drawControl(ControlElement ce, const QStyleOption* opt, QPainter* p, const QWidget* w) const {
  QPainterStateGuard stateGuard(p);
  p->setRenderHint(QPainter::SmoothPixmapTransform, true);
  switch (ce) {
    case CE_PushButton:
      if (const auto* optButton = buttonStyleOption(opt)) {
        // Button background and border.
        drawControl(CE_PushButtonBevel, optButton, p, w);

        // Button foreground (text and icon).
        auto optButtonFg = QStyleOptionButton(*optButton);
        optButtonFg.rect = subElementRect(SE_PushButtonContents, opt, w);
        drawControl(CE_PushButtonLabel, &optButtonFg, p, w);
      }
      return;
    case CE_PushButtonBevel:
      if (const auto* optRoundedButton = qstyleoption_cast<const QStyleOptionRoundedButton*>(opt)) {
        // Draw background rect.
        auto optButtonBg = QStyleOptionRoundedButton(*optRoundedButton);
        optButtonBg.rect = subElementRect(SE_PushButtonBevel, opt, w);
        drawPrimitive(PE_FrameButtonBevel, &optButtonBg, p, w);
      } else if (const auto* optButton = buttonStyleOption(opt)) {
        // Draw background rect.
        auto optButtonBg = QStyleOptionButton(*optButton);
        optButtonBg.rect = subElementRect(SE_PushButtonBevel, opt, w);
        drawPrimitive(PE_FrameButtonBevel, &optButtonBg, p, w);
      }
      return;
    case CE_PushButtonLabel:
      if (const auto* optButton = buttonStyleOption(opt)) {
        // Content.
        const auto mouse = getMouseState(optButton->state);
        const auto isDefault = optButton->features.testFlag(QStyleOptionButton::DefaultButton);
        const auto role = getColorRole(optButton->state, isDefault);
        const auto& fgColor = buttonForegroundColor(mouse, role);
        const auto& currentFgColor =
          _impl->animations.animateForegroundColor(w, fgColor, _impl->theme.animationDuration);
        const auto indicatorSize = pixelMetric(PM_MenuButtonIndicator, opt, w);
        const auto spacing = _impl->theme.spacing;
        const auto hasMenu = optButton->features.testFlag(QStyleOptionButton::HasMenu);
        const auto centered = !hasMenu;
        const auto checked = getCheckState(optButton->state);
        const auto pixmap = getPixmap(optButton->icon, optButton->iconSize, mouse, checked, w);
        const auto& colorizedPixmap = getColorizedPixmap(pixmap, w, currentFgColor, currentFgColor, mouse);
        const auto pixmapPixelRatio = colorizedPixmap.devicePixelRatio();
        const auto iconW = colorizedPixmap.isNull() ? 0 : static_cast<int>(colorizedPixmap.width() / pixmapPixelRatio);
        const auto textW = qlementine::textWidth(optButton->fontMetrics, optButton->text);
        const auto iconSpacing = iconW > 0 && !optButton->text.isEmpty() && textW > 0 ? spacing : 0;
        const auto& fgRect =
          hasMenu ? optButton->rect.marginsRemoved(QMargins{ 0, 0, indicatorSize + spacing, 0 }) : optButton->rect;
        const auto contentW = centered ? std::min(fgRect.width(), iconW + iconSpacing + textW) : fgRect.width();
        const auto contentX = centered ? fgRect.x() + (fgRect.width() - contentW) / 2 : fgRect.x();
        const auto contentRect = QRect(contentX, fgRect.y(), contentW, fgRect.height());
        auto availableW = contentW;
        auto availableX = contentX;
        p->setRenderHint(QPainter::RenderHint::Antialiasing);

        // Icon.
        if (iconW > 0) {
          const auto pixmapW = pixmapPixelRatio != 0 ? (int) ((qreal) colorizedPixmap.width() / pixmapPixelRatio) : 0;
          const auto pixmapH = pixmapPixelRatio != 0 ? (int) ((qreal) colorizedPixmap.height() / pixmapPixelRatio) : 0;
          const auto pixmapX =
            textW == 0 && !hasMenu ? contentRect.x() + (contentRect.width() - pixmapW) / 2 : contentRect.x();
          const auto pixmapY = contentRect.y() + (contentRect.height() - pixmapH) / 2;
          const auto pixmapRect = QRect{ pixmapX, pixmapY, pixmapW, pixmapH };
          availableW -= pixmapW + iconSpacing;
          availableX += pixmapW + iconSpacing;
          p->drawPixmap(pixmapRect, colorizedPixmap);
        }

        // Text.
        if (availableW > 0 && textW > 0) {
          const auto elidedText =
            optButton->fontMetrics.elidedText(optButton->text, Qt::ElideRight, availableW, Qt::TextSingleLine);
          const auto elidedTextW = qlementine::textWidth(optButton->fontMetrics, elidedText);
          const auto textRect = QRect{ availableX, contentRect.y(), elidedTextW, contentRect.height() };
          int textFlags = Qt::AlignVCenter | Qt::AlignBaseline | Qt::TextSingleLine | Qt::TextHideMnemonic;
          if (iconW == 0) {
            textFlags |= Qt::AlignHCenter;
          } else {
            textFlags |= Qt::AlignLeft;
          }
          p->setBrush(Qt::NoBrush);
          p->setPen(currentFgColor);
          p->drawText(textRect, textFlags, elidedText, nullptr);
        }

        // Arrow (if menu).
        if (hasMenu) {
          const auto indicatorW = indicatorSize;
          const auto indicatorH = indicatorSize;
          const auto indicatorX = optButton->rect.x() + optButton->rect.width() - indicatorW;
          const auto indicatorY = optButton->rect.y() + (optButton->rect.height() - indicatorH) / 2;
          const auto indicatorRect = QRect{ indicatorX, indicatorY, indicatorW, indicatorH };
          const auto path = getMenuIndicatorPath(indicatorRect);
          p->setBrush(Qt::NoBrush);
          p->setPen(QPen(currentFgColor, iconPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
          p->drawPath(path);
        }
      }
      return;
    case CE_RadioButton:
    case CE_CheckBox:
      if (const auto* optButton = buttonStyleOption(opt)) {
        const auto isRadio = ce == CE_RadioButton;

        // Draw rect and indicator.
        auto optIndicator = QStyleOptionButton(*optButton);
        optIndicator.rect = subElementRect(isRadio ? SE_RadioButtonIndicator : SE_CheckBoxIndicator, opt, w);
        drawPrimitive(isRadio ? PE_IndicatorRadioButton : PE_IndicatorCheckBox, &optIndicator, p, w);

        // Draw label.
        auto optLabel = QStyleOptionButton{ *optButton };
        optLabel.rect = subElementRect(isRadio ? SE_RadioButtonContents : SE_CheckBoxContents, opt, w);
        drawControl(isRadio ? CE_RadioButtonLabel : CE_CheckBoxLabel, &optLabel, p, w);
      }
      return;
    case CE_CheckBoxLabel:
    case CE_RadioButtonLabel:
      if (const auto* optButton = buttonStyleOption(opt)) {
        // Draw text and icon.
        const auto mouse = getMouseState(optButton->state);
        const auto& fgColor = labelForegroundColor(mouse, w);
        const auto spacing = _impl->theme.spacing;
        const auto checked = getCheckState(optButton->state);
        const auto pixmap = getPixmap(optButton->icon, optButton->iconSize, mouse, checked, w);
        const auto& colorizedPixmap = getColorizedPixmap(pixmap, w, fgColor, fgColor, mouse);
        const auto pixmapPixelRatio = colorizedPixmap.devicePixelRatio();
        const auto iconW =
          colorizedPixmap.isNull() ? 0 : static_cast<int>((qreal) colorizedPixmap.width() / (pixmapPixelRatio));
        const auto iconSpacing = iconW > 0 ? spacing : 0;
        auto availableW = optButton->rect.width();
        auto availableX = optButton->rect.x();

        p->setRenderHint(QPainter::RenderHint::Antialiasing);

        // Icon.
        if (iconW > 0) {
          const auto pixmapW = pixmapPixelRatio != 0 ? (int) ((qreal) colorizedPixmap.width() / pixmapPixelRatio) : 0;
          const auto pixmapH = pixmapPixelRatio != 0 ? (int) ((qreal) colorizedPixmap.height() / pixmapPixelRatio) : 0;
          const auto pixmapX = optButton->rect.x();
          const auto pixmapY = optButton->rect.y() + (optButton->rect.height() - pixmapH) / 2;
          const auto pixmapRect = QRect{ pixmapX, pixmapY, pixmapW, pixmapH };
          availableW -= pixmapW + iconSpacing;
          availableX += pixmapW + iconSpacing;
          p->drawPixmap(pixmapRect, colorizedPixmap);
        }

        // Text.
        if (availableW > 0 && !optButton->text.isEmpty()) {
          const auto elidedText =
            optButton->fontMetrics.elidedText(optButton->text, Qt::ElideRight, availableW, Qt::TextSingleLine);
          const auto textRect = QRect{ availableX, optButton->rect.y(), availableW, optButton->rect.height() };
          constexpr auto textFlags =
            Qt::AlignVCenter | Qt::AlignBaseline | Qt::TextSingleLine | Qt::AlignLeft | Qt::TextHideMnemonic;
          p->setBrush(Qt::NoBrush);
          p->setPen(fgColor);
          p->drawText(textRect, textFlags, elidedText, nullptr);
        }
      }
      return;
    case CE_TabBarTab:
      if (const auto* optTab = qstyleoption_cast<const QStyleOptionTab*>(opt)) {
        const auto padding = _impl->tabExtraPadding(optTab, w);

        // Background.
        auto tabBgOpt = QStyleOptionTab(*optTab);
        const auto localRect = QlementineStyleImpl::tabLocalRect(optTab->shape, optTab->rect);
        tabBgOpt.rect = QlementineStyleImpl::tabLocalTransform(optTab->shape, optTab->rect)
                          .mapRect(localRect.marginsRemoved(padding));
        drawControl(CE_TabBarTabShape, &tabBgOpt, p, w);

        // Foreground.
        auto tabFgOpt = QStyleOptionTab(*optTab);
        const auto labelRect = subElementRect(SE_TabBarTabText, optTab, w);
        tabFgOpt.rect = labelRect;
        drawControl(CE_TabBarTabLabel, &tabFgOpt, p, w);
      }
      return;
    case CE_TabBarTabShape:
      if (const auto* optTab = qstyleoption_cast<const QStyleOptionTab*>(opt)) {
        const auto mouse = getMouseState(optTab->state);
        const auto isMoving = optTab->position == QStyleOptionTab::TabPosition::Moving;
        const auto selection = isMoving ? SelectionState::Selected : getSelectionState(optTab->state);
        const auto mouseOverTab = mouse == MouseState::Hovered || mouse == MouseState::Pressed;
        const auto mousePressed = mouse == MouseState::Pressed;
        const auto tabIsSelected = selection == SelectionState::Selected;

        // Avoid drawing the tab if the mouse is over scroll buttons.
        const auto* tabBar = qobject_cast<const QTabBar*>(w);
        const auto cursorPos = tabBar->mapFromGlobal(QCursor::pos());
        const auto spacing = _impl->theme.spacing;
        const auto buttonsVisible = QlementineStyleImpl::areTabBarScrollButtonsVisible(tabBar);
        const auto buttonsW = buttonsVisible ? _impl->theme.controlHeightMedium * 2 + spacing * 3 : 0;
        const auto isVertical = QlementineStyleImpl::isTabShapeVertical(optTab->shape);
        const auto mouseOverButtons =
          isVertical ? cursorPos.y() > tabBar->height() - buttonsW : cursorPos.x() > tabBar->width() - buttonsW;

        // The tab shape must be drawn in these cases:
        // - Always when the tab is selected.
        // - When the tab is actually hovered (i.e. the mouse isn't over the left/right buttons).
        const auto drawShape = tabIsSelected || (!mouseOverButtons && mouseOverTab);
        const auto drawShadow = isMoving || (tabIsSelected && !mousePressed);
        if (drawShape) {
          const auto radius = _impl->theme.borderRadius;
          const auto& bgColor = tabBackgroundColor(mouse, selection);
          const auto localRect = QlementineStyleImpl::tabLocalRect(optTab->shape, optTab->rect);
          QPainterStateGuard tabShapeStateGuard(p);
          p->setTransform(QlementineStyleImpl::tabLocalTransform(optTab->shape, optTab->rect), true);
          if (QlementineStyleImpl::isTabShapeTriangular(optTab->shape)) {
            drawTab(p, getTrapezoidTabPath(localRect, _impl->tabSlant(), radius), bgColor, drawShadow,
              _impl->theme.shadowColor2);
          } else {
            const auto& radiuses =
              tabIsSelected ? RadiusesF(radius, radius, radius, radius) : RadiusesF(radius, radius, 0., 0.);
            drawTab(p, localRect, radiuses, bgColor, drawShadow, _impl->theme.shadowColor2);
          }
        }
      }
      return;
    case CE_TabBarTabLabel:
      if (const auto* optTab = qstyleoption_cast<const QStyleOptionTab*>(opt)) {
        // Draw in the local frame of the tab, so the label is rotated on vertical tabs.
        const auto rect = QlementineStyleImpl::tabLocalRect(optTab->shape, optTab->rect);
        QPainterStateGuard tabLabelStateGuard(p);
        p->setTransform(QlementineStyleImpl::tabLocalTransform(optTab->shape, optTab->rect), true);

        const auto mouse = getMouseState(optTab->state);
        const auto isMoving = optTab->position == QStyleOptionTab::TabPosition::Moving;
        const auto selection = isMoving ? SelectionState::Selected : getSelectionState(optTab->state);
        const auto& fgColor = tabForegroundColor(mouse, selection);

        const auto spacing = _impl->theme.spacing;
        const auto& icon = optTab->icon;
        const auto& iconSize = icon.isNull() ? QSize{ 0, 0 } : optTab->iconSize;
        const auto& fm = optTab->fontMetrics;
        const auto textAvailableWidth = rect.width() - (iconSize.isEmpty() ? 0 : iconSize.width() + spacing);
        const auto elidedText = fm.elidedText(optTab->text, Qt::ElideMiddle, textAvailableWidth, Qt::TextSingleLine);
        const auto hasText = elidedText != QStringLiteral("…");
        const auto textColor = tabTextColor(mouse, selection, optTab, w);

        // TODO handle expanding QTabBar.
        //const auto textW = hasText ? fm.boundingRect(rect, Qt::AlignLeft, elidedText).width() : 0;
        // const auto contentDesiredW = textW + spacing + iconSize.width();
        // const auto parentTabBar = qobject_cast<const QTabBar*>(w);
        // const auto expandingTabBar = parentTabBar && parentTabBar->expanding();
        // auto availableW = expandingTabBar ? contentDesiredW : rect.width();
        // auto availableX = expandingTabBar ? rect.x() + (rect.width() - contentDesiredW) / 2 : rect.x();
        auto availableW = rect.width();
        auto availableX = rect.x();

        // Icon.
        if (!iconSize.isEmpty()) {
          const auto checked = selection == SelectionState::Selected ? CheckState::Checked : CheckState::NotChecked;
          const auto pixmap = getPixmap(icon, iconSize, mouse, checked, w);
          const auto& colorizedPixmap = getColorizedPixmap(pixmap, w, fgColor, textColor, mouse);
          const auto pixmapPixelRatio = colorizedPixmap.devicePixelRatio();
          const auto pixmapW = pixmapPixelRatio != 0 ? (int) ((qreal) colorizedPixmap.width() / pixmapPixelRatio) : 0;
          const auto pixmapH = pixmapPixelRatio != 0 ? (int) ((qreal) colorizedPixmap.height() / pixmapPixelRatio) : 0;
          const auto pixmapX = hasText ? availableX : rect.x() + (rect.width() - pixmapW) / 2;
          const auto pixmapY = rect.y() + (rect.height() - pixmapH) / 2;
          const auto pixmapRect = QRect{ pixmapX, pixmapY, pixmapW, pixmapH };
          availableW -= pixmapW + spacing;
          availableX += pixmapW + spacing;
          p->drawPixmap(pixmapRect, colorizedPixmap);
        }

        // Text.
        if (availableW > 0 && hasText) {
          const auto textRect = QRect{ availableX, rect.y(), availableW, rect.height() };
          const auto textFlags =
            Qt::AlignVCenter | Qt::AlignBaseline | Qt::TextSingleLine | Qt::TextHideMnemonic | Qt::AlignLeft;
          // TODO handle expanding QTabBar.
          // if (expandingTabBar) {
          //   textFlags |= Qt::AlignHCenter;
          // } else {
          //   textFlags |= Qt::AlignLeft;
          // }
          p->setBrush(Qt::NoBrush);
          p->setPen(textColor);
          p->drawText(textRect, textFlags, elidedText);
        }
      }
      return;
    case CE_ProgressBar:
      if (const auto* optProgressBar = qstyleoption_cast<const QStyleOptionProgressBar*>(opt)) {
        // Groove.
        auto optGroove = QStyleOptionProgressBar(*optProgressBar);
        optGroove.rect = subElementRect(SE_ProgressBarGroove, optProgressBar, w);
        drawControl(CE_ProgressBarGroove, &optGroove, p, w);

        // Value.
        auto optContent = QStyleOptionProgressBar(*optProgressBar);
        optContent.rect = subElementRect(SE_ProgressBarContents, optProgressBar, w);
        drawControl(CE_ProgressBarContents, &optContent, p, w);

        if (optProgressBar->textVisible) {
          auto optText = QStyleOptionProgressBar(*optProgressBar);
          optText.rect = subElementRect(SE_ProgressBarLabel, optProgressBar, w);
          drawControl(CE_ProgressBarLabel, &optText, p, w);
        }
      }
      return;
    case CE_ProgressBarGroove:
      if (const auto* optProgressBar = qstyleoption_cast<const QStyleOptionProgressBar*>(opt)) {
        // Background.
        const auto radius = optProgressBar->rect.height() / 2.;
        const auto mouse = getMouseState(optProgressBar->state);
        const auto& color = progressBarGrooveColor(mouse);
        drawRoundedRect(p, optProgressBar->rect, color, radius);
      }
      return;
    case CE_ProgressBarContents:
      if (const auto* optProgressBar = qstyleoption_cast<const QStyleOptionProgressBar*>(opt)) {
        // Draw foreground rect.
        const auto radius = optProgressBar->rect.height() / 2.;
        const auto mouse = getMouseState(optProgressBar->state);
        const auto& color = progressBarValueColor(mouse);
        const auto indeterminate = optProgressBar->maximum == 0 && optProgressBar->minimum == 0;

        if (indeterminate) {
          // Goes from 0 to 1.
          const auto currentProgress =
            _impl->animations.animateProgress3(w, 1., _impl->theme.animationDuration * 8, true);
          // Bell that goes from 0 to 1 then 1 to 0, centered on 0.5.
          const auto currentRatio = std::pow(std::sin(QLEMENTINE_PI * currentProgress), 2);

          const auto valueRectW = static_cast<int>(optProgressBar->rect.width() * 0.25);
          const auto valueRectX = optProgressBar->rect.x() + (optProgressBar->rect.width() - valueRectW) * currentRatio;
          const auto valueRect =
            QRectF(valueRectX, optProgressBar->rect.y(), valueRectW, optProgressBar->rect.height());

          p->setPen(Qt::NoPen);
          p->setBrush(color);
          p->drawRoundedRect(valueRect, 3, 3);
        } else {
          _impl->animations.animateProgress3(w, 0., 0, false); // Stop loop, just in case.
          const auto progress = optProgressBar->progress;
          const auto currentProgress = _impl->animations.animateProgress(w, progress, _impl->theme.animationDuration);
          drawProgressBarValueRect(p, optProgressBar->rect, color, optProgressBar->minimum, optProgressBar->maximum,
            currentProgress, radius, optProgressBar->invertedAppearance);
        }
      }
      return;
    case CE_ProgressBarLabel:
      if (const auto* optProgressBar = qstyleoption_cast<const QStyleOptionProgressBar*>(opt)) {
        const auto mouse = getMouseState(optProgressBar->state);
        const auto& color = labelForegroundColor(mouse, w);
        const auto textFlags =
          Qt::AlignVCenter | Qt::AlignBaseline | Qt::TextSingleLine | Qt::AlignRight | Qt::TextHideMnemonic;
        p->setBrush(Qt::NoBrush);
        p->setPen(color);
        p->drawText(optProgressBar->rect, textFlags, optProgressBar->text);
      }
      return;
    case CE_MenuItem:
      if (const auto* optMenuItem = qstyleoption_cast<const QStyleOptionMenuItem*>(opt)) {
        if (optMenuItem->menuItemType == QStyleOptionMenuItem::Separator) {
          const auto& color = menuSeparatorColor();
          const auto hMargin = 0;
          const auto rect = optMenuItem->rect.marginsRemoved(QMargins(hMargin, 0, hMargin, 0));
          const auto separatorThickness = _impl->theme.borderWidth;
          drawMenuSeparator(p, rect, color, separatorThickness);

        } else if (optMenuItem->menuItemType == QStyleOptionMenuItem::Normal
                   || optMenuItem->menuItemType == QStyleOptionMenuItem::SubMenu) {
          const auto mouse = getMenuItemMouseState(optMenuItem->state);

          // Background.
          const auto& bgRect = optMenuItem->rect;
          const auto& bgColor = menuItemBackgroundColor(mouse);
          const auto menuItemRadius = _impl->theme.menuItemBorderRadius;
          p->setRenderHint(QPainter::Antialiasing, true);
          p->setPen(Qt::NoPen);
          p->setBrush(bgColor);
          p->drawRoundedRect(bgRect, menuItemRadius, menuItemRadius);

          // Foreground.
          const auto spacing = _impl->theme.spacing;
          const auto& fgColor = menuItemForegroundColor(mouse);
          const auto menuHasCheckable = optMenuItem->menuHasCheckableItems;
          const auto checkable = optMenuItem->checkType != QStyleOptionMenuItem::NotCheckable;
          const auto checkState = optMenuItem->checked ? CheckState::Checked : CheckState::NotChecked;
          const auto arrowW = _impl->theme.iconSize.width();
          const auto hPadding = _impl->theme.spacing;
          const auto fgRect = bgRect.marginsRemoved(QMargins{ hPadding, 0, hPadding, 0 });
          const auto [label, shortcut] = getMenuLabelAndShortcut(optMenuItem->text);
          const auto useMnemonic = styleHint(SH_UnderlineShortcut, opt, w);
          const auto* parent = w ? w->parentWidget() : nullptr;
          const auto hasFocus = (w && w->hasFocus()) || (parent && parent->hasFocus());
          const auto hasSubMenu = optMenuItem->menuItemType == QStyleOptionMenuItem::SubMenu;
          const auto showMnemonic = hasFocus;
          auto availableW = fgRect.width() - (hasSubMenu ? arrowW + spacing : 0);
          auto availableX = fgRect.x();

          // Check.
          if (menuHasCheckable || checkable) {
            const auto checkBoxSize = _impl->theme.iconSize;

            if (checkable) {
              const auto checkBoxX = availableX;
              const auto checkBoxY = fgRect.y() + (fgRect.height() - checkBoxSize.height()) / 2;
              const auto checkboxRect = QRect{ QPoint{ checkBoxX, checkBoxY }, checkBoxSize };
              const auto isRadio = optMenuItem->checkType == QStyleOptionMenuItem::Exclusive;
              const auto progress = checkState == CheckState::Checked ? 1. : 0.;
              const auto borderW = _impl->theme.borderWidth;
              const auto selected = getSelectionState(optMenuItem->state);
              const auto active = getActiveState(optMenuItem->state);
              const auto& boxFgColor = listItemCheckButtonForegroundColor(mouse, checkState, selected, active);
              const auto& boxBgColor = listItemCheckButtonBackgroundColor(mouse, checkState, selected, active);
              const auto& boxBorderColor = listItemCheckButtonBorderColor(mouse, checkState, selected, active);

              // TODO draw smaller checks.
              if (isRadio) {
                drawRadioButton(p, checkboxRect, boxBgColor, boxBorderColor, boxFgColor, borderW, progress);
              } else {
                const auto checkBoxRadius = _impl->theme.checkBoxBorderRadius;
                drawCheckButton(p, checkboxRect, checkBoxRadius, boxBgColor, boxBorderColor, boxFgColor, borderW,
                  progress, checkState);
              }
            }

            const auto taken = checkBoxSize.width() + spacing;
            availableW -= taken;
            availableX += taken;
          }

          // Icon.
          const auto iconSpace =
            !QCoreApplication::testAttribute(Qt::AA_DontShowIconsInMenus) && optMenuItem->maxIconWidth > 0
              ? optMenuItem->maxIconWidth + spacing
              : 0;
          const auto pixmap = getPixmap(optMenuItem->icon, _impl->theme.iconSize, mouse, checkState, w);
          if (!pixmap.isNull()) {
            const auto& colorizedPixmap = getColorizedPixmap(pixmap, w, fgColor, fgColor, mouse);
            const auto targetPxRatio = colorizedPixmap.devicePixelRatio();
            const auto pixmapW = targetPxRatio != 0 ? (int) ((qreal) colorizedPixmap.width() / targetPxRatio) : 0;
            const auto pixmapH = targetPxRatio != 0 ? (int) ((qreal) colorizedPixmap.height() / targetPxRatio) : 0;
            const auto pixmapX = availableX;
            const auto pixmapY = fgRect.y() + (fgRect.height() - pixmapH) / 2;
            const auto pixmapRect = QRect{ pixmapX, pixmapY, pixmapW, pixmapH };
            p->drawPixmap(pixmapRect, colorizedPixmap);
          }
          availableW -= iconSpace;
          availableX += iconSpace;

          // Shortcut text.
          if (!shortcut.isEmpty()) {
            const auto& fm = optMenuItem->fontMetrics;
            const auto shortcutW = fm.boundingRect(optMenuItem->rect, Qt::AlignRight, shortcut).width();
            if (availableW > shortcutW) {
              const auto shortcutX = fgRect.x() + fgRect.width() - shortcutW;
              const auto shortcutRect = QRect{ shortcutX, fgRect.y(), shortcutW, fgRect.height() };
              constexpr auto shortcutFlags =
                Qt::AlignVCenter | Qt::AlignBaseline | Qt::TextSingleLine | Qt::AlignRight | Qt::TextHideMnemonic;
              const auto& shortcutColor = menuItemSecondaryForegroundColor(mouse);
              p->setPen(shortcutColor);
              p->drawText(shortcutRect, shortcutFlags, shortcut);

              const auto taken = shortcutW + spacing * 2;
              availableW -= taken;
            }
          }

          // Text.
          if (!label.isEmpty()) {
            const auto textW = availableW;
            const auto& fm = optMenuItem->fontMetrics;
            const auto elidedText = fm.elidedText(label, Qt::ElideRight, textW, Qt::TextSingleLine);
            const auto textX = availableX;
            const auto textRect = QRect{ textX, fgRect.y(), availableW, fgRect.height() };
            int textFlags =
              Qt::AlignVCenter | Qt::AlignBaseline | Qt::TextSingleLine | Qt::TextShowMnemonic | Qt::AlignLeft;
            if (useMnemonic) {
              textFlags |= Qt::TextShowMnemonic;
            }
            if (!showMnemonic) {
              textFlags |= Qt::TextHideMnemonic;
            }

            p->setPen(fgColor);
            p->drawText(textRect, textFlags, elidedText, nullptr);
          }

          // Menu indicator.
          if (hasSubMenu) {
            const auto arrowRightMargin = spacing;
            const auto arrowSize = _impl->theme.iconSize;
            const auto arrowX = bgRect.x() + bgRect.width() - arrowSize.width() - arrowRightMargin;
            const auto arrowY = bgRect.y() + (bgRect.height() - arrowSize.height()) / 2;
            const auto arrowRect = QRect(arrowX, arrowY, arrowSize.width(), arrowSize.height());
            p->setBrush(Qt::NoBrush);
            p->setPen(QPen(fgColor, iconPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            drawSubMenuIndicator(arrowRect, p);
          }
        }
      }
      return;
    case CE_MenuScroller:
      if (const auto* optMenuItem = qstyleoption_cast<const QStyleOptionMenuItem*>(opt)) {
        // Background .
        const auto mouse = getMenuItemMouseState(optMenuItem->state);
        const auto& bgColor = menuItemBackgroundColor(mouse);
        const auto radius = _impl->theme.menuItemBorderRadius;
        drawRoundedRect(p, opt->rect, bgColor, radius);

        // Foreground.
        const auto isDownArrow = optMenuItem->state.testFlag(State_DownArrow);
        const auto& fgColor = menuItemForegroundColor(mouse);
        const auto iconSize = _impl->theme.iconSize;
        const auto iconX = opt->rect.x() + (opt->rect.width() - iconSize.width()) / 2;
        const auto iconY = opt->rect.y() + (opt->rect.height() - iconSize.height()) / 2;
        const auto iconRect = QRect{ QPoint(iconX, iconY), iconSize };
        p->setBrush(Qt::NoBrush);
        p->setPen(fgColor);
        // NB: we cheat a bit by translating the arrow so it appears vertically centered
        // in the Scroller + MenuVMargin area.
        const auto yTranslate = isDownArrow ? QPoint(0, iconSize.height() / 4) : QPoint(0, -iconSize.height() / 4);
        if (isDownArrow) {
          drawArrowDown(iconRect.translated(yTranslate), p);
        } else {
          drawArrowUp(iconRect.translated(yTranslate), p);
        }
      }
      return;
    case CE_MenuVMargin:
      // Nothing to draw.
      return;
    case CE_MenuHMargin:
      // Nothing to draw.
      return;
    case CE_MenuTearoff:
      if (const auto* optMenuItem = qstyleoption_cast<const QStyleOptionMenuItem*>(opt)) {
        // Background .
        const auto mouse = getMenuItemMouseState(optMenuItem->state);
        const auto& bgColor = menuItemBackgroundColor(mouse);
        const auto radius = _impl->theme.menuItemBorderRadius;
        drawRoundedRect(p, opt->rect, bgColor, radius);

        // Foreground.
        const auto& fgColor = menuItemForegroundColor(mouse);
        const auto iconSize = _impl->theme.iconSize;
        const auto iconX = opt->rect.x() + (opt->rect.width() - iconSize.width()) / 2;
        const auto iconY = opt->rect.y() + (opt->rect.height() - iconSize.height()) / 2;
        const auto iconRect = QRect{ QPoint(iconX, iconY), iconSize };
        drawGripIndicator(iconRect, p, fgColor, Qt::Horizontal);
      }
      return;
    case CE_MenuEmptyArea:
      // Nothing to draw.
      return;
    case CE_MenuBarItem:
      if (const auto* optMenuItem = qstyleoption_cast<const QStyleOptionMenuItem*>(opt)) {
        // MenuBar background.
        const auto& barBgColor = menuBarBackgroundColor();
        p->fillRect(optMenuItem->rect, barBgColor);

        // Item.
        const auto mouse = getMenuItemMouseState(optMenuItem->state);
        const auto selected = getSelectionState(optMenuItem->state);
        const auto& bgColor = menuBarItemBackgroundColor(mouse, selected);
        const auto& fgColor = menuBarItemForegroundColor(mouse, selected);
        int textFlags = Qt::AlignVCenter | Qt::AlignBaseline | Qt::TextSingleLine | Qt::AlignHCenter;
        if (styleHint(SH_UnderlineShortcut, opt, w)) {
          textFlags |= Qt::TextShowMnemonic;
        }
        if (w && !w->hasFocus()) {
          textFlags |= Qt::TextHideMnemonic;
        }
        const auto radius = _impl->theme.menuBarItemBorderRadius;
        p->setPen(Qt::NoPen);
        p->setBrush(bgColor);
        p->setRenderHint(QPainter::Antialiasing, true);
        p->drawRoundedRect(opt->rect, radius, radius);
        p->setBrush(Qt::NoBrush);
        p->setPen(fgColor);
        p->drawText(optMenuItem->rect, textFlags, optMenuItem->text);
      }
      return;
    case CE_MenuBarEmptyArea: {
      const auto& bgColor = menuBarBackgroundColor();
      p->fillRect(opt->rect, bgColor);
    }
      return;
    case CE_ToolButtonLabel:
      if (const auto* optToolButton = qstyleoption_cast<const QStyleOptionToolButton*>(opt)) {
        // Foreground.
        p->setRenderHint(QPainter::RenderHint::Antialiasing);
        const auto& rect = optToolButton->rect;
        const auto& icon = optToolButton->icon;

        // Little hack to avoid having a checked extension button.
        auto buttonState = optToolButton->state;
        const auto isExtensionButton = (w && w->objectName() == QStringLiteral("qt_toolbar_ext_button"));
        if (isExtensionButton) {
          buttonState.setFlag(State_On, false);
        }

        const auto& iconSize = icon.isNull() ? QSize{ 0, 0 } : optToolButton->iconSize;
        const auto& fm = optToolButton->fontMetrics;
        const auto buttonStyle = optToolButton->toolButtonStyle;
        const auto showText = buttonStyle != Qt::ToolButtonIconOnly;
        const auto showIcon = buttonStyle != Qt::ToolButtonTextOnly;
        const auto mouse = getToolButtonMouseState(buttonState);
        const auto role = getColorRole(buttonState, false);
        const auto checked = getCheckState(buttonState);
        const auto& fgColor = toolButtonForegroundColor(mouse, role);
        const auto spacing = _impl->theme.spacing;
        const auto hasMenu = optToolButton->features.testFlag(QStyleOptionToolButton::HasMenu);
        const auto leftPadding = buttonStyle == Qt::ToolButtonTextOnly ? spacing * 2 : spacing;
        const auto hasIcon = showIcon && !iconSize.isEmpty();
        const auto hasText = showText && !optToolButton->text.isEmpty();
        const auto rightPadding =
          !hasMenu && (buttonStyle == Qt::ToolButtonTextOnly || buttonStyle == Qt::ToolButtonTextBesideIcon)
            ? spacing * 2
            : spacing;
        const auto topPadding = buttonStyle == Qt::ToolButtonTextUnderIcon ? spacing : 0;
        const auto bottomPadding = buttonStyle == Qt::ToolButtonTextUnderIcon ? spacing : 0;
        const auto fgRect = rect.adjusted(leftPadding, topPadding, -rightPadding, -bottomPadding);
        const auto centered = !hasMenu;
        const auto textW = fm.boundingRect(optToolButton->rect, Qt::AlignCenter, optToolButton->text).width();
        const auto textH = fm.boundingRect(optToolButton->rect, Qt::AlignCenter, optToolButton->text).height();

        auto availableW = 0;
        auto availableX = 0;
        if (buttonStyle != Qt::ToolButtonTextUnderIcon) {
          availableW = centered ? std::min(fgRect.width(), iconSize.width() + spacing + textW) : fgRect.width();
          availableX = centered ? fgRect.x() + (fgRect.width() - availableW) / 2 : fgRect.x();
        } else {
          const auto contentMaxW = std::max(iconSize.width(), textW);
          availableW = centered ? std::min(fgRect.width(), contentMaxW) : fgRect.width();
          availableX = centered ? fgRect.x() + (fgRect.width() - availableW) / 2 : fgRect.x();
        }
        auto availableH = std::min(fgRect.height(), iconSize.height() + spacing + textH);
        auto availableY = fgRect.y();

        // Icon.
        if (hasIcon) {
          const auto pixmap = getPixmap(icon, iconSize, mouse, checked, w);
          const auto& colorizedPixmap = getColorizedPixmap(pixmap, w, fgColor, fgColor, mouse);
          const auto pixmapPixelRatio = colorizedPixmap.devicePixelRatio();
          const auto pixmapW = pixmapPixelRatio != 0 ? (int) ((qreal) colorizedPixmap.width() / pixmapPixelRatio) : 0;
          const auto pixmapH = pixmapPixelRatio != 0 ? (int) ((qreal) colorizedPixmap.height() / pixmapPixelRatio) : 0;
          if (buttonStyle == Qt::ToolButtonTextUnderIcon) {
            const auto pixmapX = availableX + (availableW - pixmapW) / 2;
            const auto pixmapY = fgRect.y() + (fgRect.height() - availableH) / 2;
            const auto pixmapRect = QRect{ pixmapX, pixmapY, pixmapW, pixmapH };
            availableH -= pixmapH + spacing;
            availableY += pixmapH + spacing;
            p->drawPixmap(pixmapRect, colorizedPixmap);
          } else {
            const auto iconOnly = buttonStyle == Qt::ToolButtonIconOnly;
            const auto pixmapX = iconOnly ? availableX + (availableW - pixmapW) / 2 : availableX;
            const auto pixmapY = rect.y() + (rect.height() - pixmapH) / 2;
            const auto pixmapRect = QRect{ pixmapX, pixmapY, pixmapW, pixmapH };
            availableW -= pixmapW + spacing;
            availableX += pixmapW + spacing;
            p->drawPixmap(pixmapRect, colorizedPixmap);
          }
        }

        // Text.
        if (hasText) {
          if (buttonStyle == Qt::ToolButtonTextUnderIcon && availableH > 0) {
            const auto elidedText = fm.elidedText(optToolButton->text, Qt::ElideRight, availableW, Qt::TextSingleLine);
            const auto textRect = QRect{ availableX, availableY, availableW, availableH };
            const int textFlags = Qt::AlignCenter | Qt::AlignBaseline | Qt::TextSingleLine | Qt::TextHideMnemonic;
            p->setBrush(Qt::NoBrush);
            p->setPen(fgColor);
            p->drawText(textRect, textFlags, elidedText, nullptr);
          } else if (availableW > 0) {
            const auto elidedText = fm.elidedText(optToolButton->text, Qt::ElideRight, availableW, Qt::TextSingleLine);
            const auto elidedTextW = fm.boundingRect(optToolButton->rect, Qt::AlignCenter, elidedText).width();
            const auto textRect = QRect{ availableX, availableY, elidedTextW, availableH };
            int textFlags = Qt::AlignVCenter | Qt::AlignBaseline | Qt::TextSingleLine | Qt::TextHideMnemonic;
            if (iconSize.isEmpty() || !showIcon) {
              textFlags |= Qt::AlignHCenter;
            } else {
              textFlags |= Qt::AlignLeft;
            }
            p->setBrush(Qt::NoBrush);
            p->setPen(fgColor);
            p->drawText(textRect, textFlags, elidedText, nullptr);
          }
        }
      }
      return;
    case CE_Header:
      if (const auto* optHeader = qstyleoption_cast<const QStyleOptionHeader*>(opt)) {
        // Draw background.
        drawControl(CE_HeaderSection, optHeader, p, w);

        // Draw label.
        auto optHeaderLabel = QStyleOptionHeader(*optHeader);
        optHeaderLabel.rect = subElementRect(SE_HeaderLabel, optHeader, w);
        if (optHeaderLabel.rect.isValid()) {
          drawControl(CE_HeaderLabel, &optHeaderLabel, p, w);
        }

        // Draw arrow indicator.
        if (optHeader->sortIndicator != QStyleOptionHeader::None) {
          auto optHeaderIndicator = QStyleOptionHeader(*optHeader);
          optHeaderIndicator.rect = subElementRect(SE_HeaderArrow, optHeader, w);
          drawPrimitive(PE_IndicatorHeaderArrow, &optHeaderIndicator, p, w);
        }
      }
      return;
    case CE_HeaderSection:
      if (const auto* optHeader = qstyleoption_cast<const QStyleOptionHeader*>(opt)) {
        const auto* wParent = w ? w->parentWidget() : nullptr;
        const auto* tableView = qobject_cast<const QTableView*>(wParent);
        const auto* treeView = qobject_cast<const QTreeView*>(wParent);
        const auto* horizontalHeader = tableView  ? tableView->horizontalHeader()
                                       : treeView ? treeView->header()
                                                  : nullptr;
        const auto* verticalHeader = tableView ? tableView->verticalHeader() : nullptr;
        const auto isVertical = optHeader->orientation == Qt::Vertical;
        const auto isHorizontal = optHeader->orientation == Qt::Horizontal;
        const auto& rect = opt->rect;

        // Sometimes, we don't want external borders, for aesthetics purposes.
        // Example: a QTreeView beside a QSplitter. We want to avoid the splitter's separator and the
        // header's borders being side to side, because it'll look like a larger ugly border.
        const auto shadow = tableView  ? tableView->frameShadow()
                            : treeView ? treeView->frameShadow()
                                       : QFrame::Shadow::Sunken;
        const auto drawTableExternalBorders = shadow != QFrame::Plain;

        // Background.
        const auto mouse = getMouseState(opt->state);
        const auto checked = getCheckState(opt->state);
        const auto& bgColor = tableHeaderBgColor(mouse, checked);
        p->fillRect(rect, bgColor);

        // Lines.
        const auto& lineColor = tableLineColor();
        const auto lineW = _impl->theme.borderWidth;
        p->setRenderHint(QPainter::Antialiasing, false);
        p->setBrush(Qt::NoBrush);
        p->setPen(QPen(lineColor, lineW));

        // Line at the top.
        const auto drawTopBorder =
          drawTableExternalBorders
          || (isVertical && optHeader->position == QStyleOptionHeader::SectionPosition::Beginning)
          || (isHorizontal && optHeader->position != QStyleOptionHeader::SectionPosition::Beginning);
        if (drawTopBorder) {
          const auto horizontalHeaderHidden = horizontalHeader ? horizontalHeader->isHidden() : true;
          if (optHeader->orientation == Qt::Horizontal
              || (horizontalHeaderHidden && optHeader->position == QStyleOptionHeader::Beginning)) {
            const auto p1 = QPointF(rect.x(), rect.y() + lineW * .5);
            const auto p2 = QPointF(rect.x() + rect.width(), rect.y() + lineW * .5);
            p->drawLine(p1, p2);
          }
        }

        // Line on the right.
        const auto drawRightBorder =
          drawTableExternalBorders || isVertical
          || (isHorizontal && optHeader->position == QStyleOptionHeader::SectionPosition::End);
        if (drawRightBorder) {
          const auto p1 = QPointF(rect.x() + rect.width() - lineW * .5, rect.y());
          const auto p2 = QPointF(rect.x() + rect.width() - lineW * .5, rect.y() + rect.height());
          p->drawLine(p1, p2);
        }

        // Line at the bottom.
        const auto drawBottomBorder = drawTableExternalBorders
                                      || (isVertical && optHeader->position != QStyleOptionHeader::SectionPosition::End)
                                      || isHorizontal;
        if (drawBottomBorder) {
          const auto p1 = QPointF(rect.x(), rect.y() + rect.height() - lineW * .5);
          const auto p2 = QPointF(rect.x() + rect.width(), rect.y() + rect.height() - lineW * .5);
          p->drawLine(p1, p2);
        }

        // Line at the left.
        const auto drawLeftBorder =
          drawTableExternalBorders
          || (isHorizontal && optHeader->position == QStyleOptionHeader::SectionPosition::Beginning);
        if (drawLeftBorder) {
          const auto verticalHeaderHidden = verticalHeader ? verticalHeader->isHidden() : true;
          if (optHeader->orientation == Qt::Vertical
              || (optHeader->orientation == Qt::Horizontal && optHeader->position == QStyleOptionHeader::OnlyOneSection)
              || (verticalHeaderHidden && optHeader->position == QStyleOptionHeader::Beginning)) {
            const auto p1 = QPointF(rect.x() + lineW * .5, rect.y());
            const auto p2 = QPointF(rect.x() + lineW * .5, rect.y() + rect.height());
            p->drawLine(p1, p2);
          }
        }
      }
      return;
    case CE_HeaderLabel:
      if (const auto* optHeader = qstyleoption_cast<const QStyleOptionHeader*>(opt)) {
        // We don't care about iconAlignment to make things simpler.
        // Label = { icon + text }
        // We try to respect the label alignment as much as possible, given the available width.
        const auto& rect = optHeader->rect;

        const auto iconExtent = pixelMetric(PM_SmallIconSize, opt);
        const auto spacing = _impl->theme.spacing;

        const auto hasArrow = optHeader->sortIndicator != QStyleOptionHeader::SortIndicator::None;
        const auto arrowSpace = spacing / 2 + iconExtent;
        const auto maxLabelX = hasArrow ? rect.x() + rect.width() - arrowSpace : rect.x() + rect.width();
        const auto headerAlignment = optHeader->textAlignment;

        const auto& text = optHeader->text;
        const auto headerIsSelected = optHeader->state.testFlag(QStyle::State_On);
        auto font = QFont(p->font());
        if (headerIsSelected) {
          font.setBold(true);
          p->setFont(font);
        }
        const auto fm = QFontMetrics(font);
        const auto availableW = rect.width();
        const auto& icon = optHeader->icon;
        const auto hasIcon = !icon.isNull();
        const auto iconSpace = hasIcon ? spacing + iconExtent : 0;
        const auto textAvailableW =
          availableW - iconSpace - (hasArrow && headerAlignment.testFlag(Qt::AlignRight) ? arrowSpace : 0);
        const auto textTheoricalW = fm.size(Qt::TextSingleLine, text).width();
        const auto textW = std::min(textTheoricalW, textAvailableW);
        const auto labelW = textW + (hasIcon ? iconSpace : 0);
        const auto labelY = rect.y();
        const auto labelH = rect.height();
        auto labelX = rect.x();
        if (optHeader->textAlignment.testFlag(Qt::AlignmentFlag::AlignRight)) {
          labelX =
            rect.x() + rect.width() - labelW - (hasArrow && headerAlignment.testFlag(Qt::AlignRight) ? arrowSpace : 0);
        } else if (optHeader->textAlignment.testFlag(Qt::AlignmentFlag::AlignHCenter)) {
          labelX = rect.x() + (rect.width() - labelW) / 2;
        }
        const auto textX = labelX + iconSpace;
        auto textRect = QRect(textX, labelY, textW, labelH);

        const auto mouse = getMouseState(optHeader->state);
        const auto checked = getCheckState(optHeader->state);
        const auto& fgColor = tableHeaderFgColor(mouse, checked);

        // Icon.
        if (hasIcon && availableW > iconExtent) {
          const auto iconX = labelX;
          const auto iconY = labelY + (labelH - iconExtent) / 2;
          const auto iconRect = QRect(iconX, iconY, iconExtent, iconExtent);

          if (!hasArrow || iconRect.right() <= maxLabelX) {
            const auto colorize = this->autoIconColor(w) != AutoIconColor::None;
            const auto iconMode = (optHeader->state & State_Enabled || colorize) ? QIcon::Normal : QIcon::Disabled;
            const auto iconPixmap =
              icon.pixmap({ iconExtent, iconExtent }, qlementine::getWindow(w)->devicePixelRatio(), iconMode);
            const auto& colorizedPixmap = getColorizedPixmap(iconPixmap, w, fgColor, fgColor, mouse);
            p->drawPixmap(iconRect, colorizedPixmap);
          }
        }

        // Text.
        if (textW > 0) {
          if (hasArrow && textRect.right() > maxLabelX) {
            textRect.setRight(std::min(maxLabelX, textRect.right()));
          }
          const auto elidedText =
            fm.elidedText(text, Qt::TextElideMode::ElideRight, textRect.width(), Qt::TextSingleLine);
          p->setBrush(Qt::NoBrush);
          p->setPen(fgColor);
          const auto textHAlignment =
            headerAlignment.testFlag(Qt::AlignmentFlag::AlignRight) && textTheoricalW < textAvailableW ? Qt::AlignRight
                                                                                                       : Qt::AlignLeft;
          auto textFlags = Qt::Alignment{ int(Qt::AlignVCenter | Qt::TextSingleLine) };
          textFlags.setFlag(textHAlignment, true);
          p->drawText(textRect, int(textFlags), elidedText);
        }
      }
      return;
    case CE_HeaderEmptyArea: {
      const auto& bgColor = tableHeaderBgColor(MouseState::Normal, CheckState::NotChecked);
      p->fillRect(opt->rect, bgColor);
    }
      return;
    case CE_ToolBoxTab:
      break;
    case CE_SizeGrip:
      break;
    case CE_Splitter: {
      constexpr auto maxSplitterThickness = 2;
      constexpr auto minSplitterThickness = 1;
      const auto& rect = opt->rect;
      const auto mouse = getMouseState(opt->state);
      const auto& lineColor = splitterColor(mouse);
      const auto isHorizontal = opt->state.testFlag(QStyle::State_Horizontal);
      const auto lineThickness =
        std::clamp(isHorizontal ? rect.width() : rect.height(), minSplitterThickness, maxSplitterThickness);
      const auto lineW = isHorizontal ? lineThickness : rect.width();
      const auto lineH = isHorizontal ? rect.height() : lineThickness;
      const auto lineX = isHorizontal ? rect.x() + (rect.width() - lineThickness) / 2 : rect.x();
      const auto lineY = isHorizontal ? rect.y() : rect.y() + (rect.height() - lineThickness) / 2;
      const auto lineRect = QRect(lineX, lineY, lineW, lineH);
      p->fillRect(lineRect, lineColor);
    }
      return;
    // case CE_RubberBand:
    //   break;
    // case CE_DockWidgetTitle:
    //   break;
    // case CE_ScrollBarAddLine:
    //   // TODO
    //   break;
    // case CE_ScrollBarSubLine:
    //   // TODO
    //   break;
    // case CE_ScrollBarAddPage:
    //   // TODO
    //   break;
    // case CE_ScrollBarSubPage:
    //   // TODO
    //   break;
    // case CE_ScrollBarSlider:
    //   // TODO
    //   break;
    // case CE_ScrollBarFirst:
    //   // TODO
    //   break;
    // case CE_ScrollBarLast:
    //   // TODO
    //   break;
    case CE_FocusFrame:
      if (const auto* focusFrame = qobject_cast<const QFocusFrame*>(w)) {
        const auto* monitoredWidget = focusFrame->widget();
        const auto hasFocus = monitoredWidget ? monitoredWidget->hasFocus() : false;
        const auto borderW = _impl->theme.focusBorderWidth;

        QStyleOptionFocusRoundedRect optFocus;
        optFocus.QStyleOption::operator=(*opt);
        optFocus.state.setFlag(State_HasFocus, hasFocus);

        // The focus frame is placed differently according to the widget.
        if (const auto* button = qobject_cast<const QPushButton*>(monitoredWidget)) {
          // Prepare monitored widget QStyleOption.
          QStyleOptionButton optButton;
          optButton.QStyleOption::operator=(*opt);
          optButton.initFrom(button);

          // PushButton: placed around the button itself.
          optFocus.rect = subElementRect(SE_PushButtonFocusRect, &optButton, button);
          optFocus.radiuses = _impl->theme.borderRadius;
        } else if (const auto* toolButton = qobject_cast<const QToolButton*>(monitoredWidget)) {
          // Prepare monitored widget QStyleOption.
          QStyleOptionToolButton optToolButton;
          optToolButton.QStyleOption::operator=(*opt);
          optToolButton.initFrom(toolButton);

          // ToolButton: placed around the button itself.
          optFocus.rect = subElementRect(SE_PushButtonFocusRect, &optToolButton, toolButton);
          optFocus.radiuses = _impl->theme.borderRadius;
        } else if (const auto* checkBox = qobject_cast<const QCheckBox*>(monitoredWidget)) {
          // Prepare monitored widget QStyleOption.
          QStyleOptionButton optCheckBox;
          optCheckBox.QStyleOption::operator=(*opt);
          optCheckBox.initFrom(checkBox);

          // Checkbox: placed around the check button.
          optFocus.rect = subElementRect(SE_CheckBoxFocusRect, &optCheckBox, checkBox);
          optFocus.radiuses = _impl->theme.checkBoxBorderRadius;
        } else if (const auto* radioButton = qobject_cast<const QRadioButton*>(monitoredWidget)) {
          // Prepare monitored widget QStyleOption.
          QStyleOptionButton optRadioButton;
          optRadioButton.QStyleOption::operator=(*opt);
          optRadioButton.initFrom(radioButton);

          // Checkbox: placed around the check button.
          optFocus.rect = subElementRect(SE_CheckBoxFocusRect, &optRadioButton, radioButton);
          optFocus.radiuses = std::min(optFocus.rect.width(), optFocus.rect.height()) / 2.;
        } else if (const auto* colorButton = qobject_cast<const ColorButton*>(monitoredWidget)) {
          // Prepare monitored widget QStyleOption.
          QStyleOptionButton optColorButton;
          optColorButton.QStyleOption::operator=(*opt);
          optColorButton.initFrom(colorButton);

          // ColorButton: circle around the button.
          optFocus.rect = subElementRect(SE_PushButtonFocusRect, &optColorButton, colorButton);
          optFocus.radiuses = optFocus.rect.height() / 2.;
        } else if (const auto* switchWidget = qobject_cast<const Switch*>(monitoredWidget)) {
          switchWidget->initStyleOptionFocus(optFocus);
        } else if (const auto* abstractButton = qobject_cast<const QAbstractButton*>(monitoredWidget)) {
          // Prepare monitored widget QStyleOption.
          QStyleOptionButton optAbstractButton;
          optAbstractButton.QStyleOption::operator=(*opt);
          optAbstractButton.initFrom(abstractButton);

          // AbstractButton (fallback): placed around the button.
          optFocus.rect = subElementRect(SE_PushButtonFocusRect, &optAbstractButton, abstractButton);
          optFocus.radiuses = _impl->theme.borderRadius;
        } else if (const auto* slider = qobject_cast<const QSlider*>(monitoredWidget)) {
          // Prepare monitored widget QStyleOption.
          const auto currentPos = _impl->animations.getAnimatedProgress(slider);
          QStyleOptionSlider optSlider;
          optSlider.QStyleOption::operator=(*opt);
          optSlider.initFrom(slider);
          optSlider.minimum = slider->minimum();
          optSlider.maximum = slider->maximum();
          optSlider.sliderPosition = slider->sliderPosition();
          const auto sliderPosition = currentPos ? currentPos.value() : static_cast<qreal>(optSlider.sliderPosition);

          // Slider: placed around the handle.
          const auto sliderHandleRect = QlementineStyleImpl::sliderHandleRect(*this, optSlider, sliderPosition, slider);
          optFocus.rect = QlementineStyleImpl::sliderFocusRect(*this, optSlider, sliderHandleRect, slider);
          optFocus.radiuses = optFocus.rect.height() / 2.;
        } else if (const auto* dial = qobject_cast<const QDial*>(monitoredWidget)) {
          // Prepare monitored widget QStyleOption.
          QStyleOptionSlider optDial;
          optDial.initFrom(dial);
          optDial.minimum = dial->minimum();
          optDial.maximum = dial->maximum();
          optDial.sliderPosition = dial->sliderPosition();
          optDial.subControls.setFlag(SC_DialTickmarks, dial->notchesVisible());

          // Dial: placed around the handle.
          optFocus.rect = subElementRect(SE_SliderFocusRect, &optDial, dial);
          optFocus.radiuses = optFocus.rect.height() / 2.;
        } else if (const auto* lineEdit = qobject_cast<const QLineEdit*>(monitoredWidget)) {
          // LineEdit: placed around the whole text field.
          const auto* parentWidget = monitoredWidget ? monitoredWidget->parentWidget() : nullptr;
          const auto* parentParentWidget = parentWidget ? parentWidget->parentWidget() : nullptr;

          // Check if the QLineEdit should have radiuses.
          const auto isComboBoxLineEdit = qobject_cast<const QComboBox*>(lineEdit->parentWidget()) != nullptr;
          const auto isPlainLineEdit = !isComboBoxLineEdit && !lineEdit->hasFrame();

          // Check if the QLineEdit is a cell editor of a QTableView or equivalent.
          const auto isTabCellEditor =
            (parentWidget && qobject_cast<const QAbstractItemView*>(parentWidget->parentWidget()))
            || (parentParentWidget && qobject_cast<const QAbstractItemView*>(parentParentWidget->parentWidget()));

          // Check if the QLineEdit is within a QSpinBox or a QComboBox.
          const auto* parentSpinbox = qobject_cast<const QAbstractSpinBox*>(parentWidget);
          const auto* parentCombobox = qobject_cast<const QComboBox*>(parentWidget);

          const auto margin = isPlainLineEdit ? borderW * 2 : borderW;
          optFocus.rect = optFocus.rect.marginsRemoved(QMargins(margin, margin, margin, margin));
          optFocus.radiuses = _impl->theme.borderRadius;

          // Check if the QLineEdit is inside a QSpinBox and +/- buttons are visible,
          // or inside an editable QComboBox.
          if (isPlainLineEdit || isTabCellEditor) {
            optFocus.radiuses.topRight = 0.;
            optFocus.radiuses.topLeft = 0.;
            optFocus.radiuses.bottomRight = 0.;
            optFocus.radiuses.bottomLeft = 0.;
          } else if ((parentSpinbox && parentSpinbox->buttonSymbols() != QAbstractSpinBox::NoButtons)
                     || (parentCombobox && parentCombobox->isEditable())) {
            optFocus.radiuses.topRight = 0.;
            optFocus.radiuses.bottomRight = 0.;
          }
        } else if (const auto* groupBox = qobject_cast<const QGroupBox*>(monitoredWidget)) {
          if (groupBox->isCheckable()) {
            // Prepare monitored widget QStyleOption.
            QStyleOptionGroupBox optGroupBox;
            optGroupBox.QStyleOption::operator=(*opt);
            optGroupBox.initFrom(groupBox);
            optGroupBox.subControls.setFlag(SC_GroupBoxCheckBox, true);

            // GroupBox: placed around the CheckBox.
            const auto checkRect = subControlRect(CC_GroupBox, &optGroupBox, SC_GroupBoxCheckBox, groupBox)
                                     .marginsAdded(QMargins(borderW, borderW, borderW, borderW));
            const auto deltaX = pixelMetric(PM_FocusFrameHMargin, opt, w);
            const auto deltaY = pixelMetric(PM_FocusFrameVMargin, opt, w);
            optFocus.rect = checkRect.translated(deltaX, deltaY);
            optFocus.radiuses = _impl->theme.checkBoxBorderRadius;
          }
        } else if (const auto* comboBox = qobject_cast<const QComboBox*>(monitoredWidget)) {
          // Check if the QLineEdit is within a QComboBox.
          if (comboBox->isEditable()) {
            // Don't draw the focus border because the QComboBox already has it.
            optFocus.rect = {};
            return;
          }

          const auto* parentWidget = comboBox->parentWidget();
          const auto isTabCellEditor =
            parentWidget && qobject_cast<const QAbstractItemView*>(parentWidget->parentWidget());

          // Prepare monitored widget QStyleOption.
          QStyleOptionComboBox optComboBox;
          optComboBox.QStyleOption::operator=(*opt);
          optComboBox.initFrom(comboBox);

          // ComboBox: placed around the button itself.
          optFocus.rect = subElementRect(SE_ComboBoxFocusRect, &optComboBox, comboBox);
          optFocus.radiuses = isTabCellEditor ? 0. : _impl->theme.borderRadius;
        } else if (const auto* abstractItemListWidget = qobject_cast<const AbstractItemListWidget*>(monitoredWidget)) {
          abstractItemListWidget->initStyleOptionFocus(optFocus);
        } else {
          auto customRadius = RadiusesF{ -1 };
          if (const auto* roundedFocusFrame = qobject_cast<const RoundedFocusFrame*>(focusFrame)) {
            customRadius = roundedFocusFrame->radiuses();
          }

          optFocus.rect = monitoredWidget ? monitoredWidget->rect()
                                              .translated(borderW * 2, borderW * 2)
                                              .marginsAdded(QMargins(borderW, borderW, borderW, borderW))
                                          : QRect();

          optFocus.radiuses = customRadius >= 0 ? customRadius : RadiusesF(_impl->theme.borderRadius);
        }

        // Draw the focus border.
        drawPrimitive(PE_FrameFocusRect, &optFocus, p, w);
      }
      return;
    case CE_ComboBoxLabel:
      if (const auto* optComboBox = qstyleoption_cast<const QStyleOptionComboBox*>(opt)) {
        if (optComboBox->editable) {
          return;
        }
        const auto& totalRect = optComboBox->rect;
        // Draw text and icon.
        const auto mouse = getMouseState(optComboBox->state);
        const auto& fgColor = comboBoxForegroundColor(mouse);
        const auto& currentFgColor =
          _impl->animations.animateForegroundColor(w, fgColor, _impl->theme.animationDuration);
        const auto indicatorSize = _impl->theme.iconSize;
        const auto spacing = _impl->theme.spacing;
        const auto contentLeftPadding = spacing;
        const auto contentRightPadding = 2 * spacing + indicatorSize.width();
        const auto contentRect = totalRect.marginsRemoved({ contentLeftPadding, 0, contentRightPadding, 0 });
        const auto pixmap =
          getPixmap(optComboBox->currentIcon, optComboBox->iconSize, mouse, CheckState::NotChecked, w);
        const auto& colorizedPixmap = getColorizedPixmap(pixmap, w, fgColor, fgColor, mouse); // No animation for icon?
        const auto iconW = colorizedPixmap.isNull() ? 0 : colorizedPixmap.width() / colorizedPixmap.devicePixelRatio();
        const auto iconSpacing = iconW > 0 ? spacing : 0;
        auto availableW = contentRect.width();
        auto availableX = contentRect.x();
        p->setRenderHint(QPainter::RenderHint::Antialiasing);

        // Icon.
        if (iconW > 0) {
          const auto pixmapPixelRatio = colorizedPixmap.devicePixelRatio();
          const auto pixmapW = pixmapPixelRatio != 0 ? (int) ((qreal) colorizedPixmap.width() / pixmapPixelRatio) : 0;
          const auto pixmapH = pixmapPixelRatio != 0 ? (int) ((qreal) colorizedPixmap.height() / pixmapPixelRatio) : 0;
          const auto pixmapX = contentRect.x();
          const auto pixmapY = contentRect.y() + (contentRect.height() - pixmapH) / 2;
          const auto pixmapRect = QRect{ pixmapX, pixmapY, pixmapW, pixmapH };
          availableW -= pixmapW + iconSpacing;
          availableX += pixmapW + iconSpacing;
          p->drawPixmap(pixmapRect, colorizedPixmap);
        }

        // Text.
        if (availableW > 0 && !optComboBox->currentText.isEmpty()) {
          const auto elidedText = optComboBox->fontMetrics.elidedText(
            optComboBox->currentText, Qt::ElideRight, availableW, Qt::TextSingleLine);
          const auto textRect = QRect{ availableX, contentRect.y(), availableW, contentRect.height() };
          constexpr auto textFlags =
            Qt::AlignVCenter | Qt::AlignBaseline | Qt::TextSingleLine | Qt::AlignLeft | Qt::TextHideMnemonic;
          p->setBrush(Qt::NoBrush);

          const auto status = widgetStatus(w);
          const auto& textColor = comboBoxTextColor(mouse, status, w);
          p->setPen(textColor);
          p->drawText(textRect, textFlags, elidedText, nullptr);
        }

        // Draw indicator on the right.
        const auto indicatorX = contentRect.x() + contentRect.width() + spacing;
        const auto indicatorY = totalRect.y() + (totalRect.height() - indicatorSize.height()) / 2;
        const auto indicatorRect = QRect{ indicatorX, indicatorY, indicatorSize.width(), indicatorSize.height() };
        p->setBrush(Qt::NoBrush);
        p->setPen(QPen(currentFgColor, iconPenWidth, Qt::SolidLine, Qt::FlatCap));
        drawComboBoxIndicator(indicatorRect, p);
      }
      return;
    case CE_ToolBar:
      if (const auto* optToolBar = qstyleoption_cast<const QStyleOptionToolBar*>(opt)) {
        //// Reserve the beveled appearance only for mainwindow toolbars.
        //if (w && !(qobject_cast<const QMainWindow*>(w->parentWidget())))
        //  break;
        // Background.
        drawPrimitive(PE_PanelToolBar, optToolBar, p, w);
      }
      return;
    case CE_ToolBoxTabShape:
      break;
    case CE_ToolBoxTabLabel:
      break;
    case CE_ColumnViewGrip:
      break;
    case CE_ItemViewItem:
      if (const auto* optItem = qstyleoption_cast<const QStyleOptionViewItem*>(opt)) {
        // Background.
        drawPrimitive(PE_PanelItemViewItem, optItem, p, w);

        // Foreground.
        const auto& features = optItem->features;
        const auto hasIcon = features.testFlag(QStyleOptionViewItem::HasDecoration) && !optItem->icon.isNull();
        const auto& iconSize = hasIcon ? optItem->decorationSize : QSize{ 0, 0 };
        const auto selected = getSelectionState(optItem->state);
        const auto hasCheck = features.testFlag(QStyleOptionViewItem::HasCheckIndicator);
        const auto isChecked = hasCheck && optItem->checkState == Qt::Checked;
        const auto checked = isChecked ? CheckState::Checked : CheckState::NotChecked;
        const auto active = getActiveState(optItem->state);

        const auto* styleProxy = this->proxy();
        const auto checkboxRect = styleProxy->subElementRect(SE_ItemViewItemCheckIndicator, optItem, w);
        const auto iconRect = styleProxy->subElementRect(SE_ItemViewItemDecoration, optItem, w);
        const auto textRect = styleProxy->subElementRect(SE_ItemViewItemText, optItem, w);

        // We show the selected color on the whole row, not only the cell.
        // Make it consistent with the background color in PE_PanelItemViewItem.
        const auto widgetHasFocus = (w && w->hasFocus());
        const auto focus =
          widgetHasFocus && selected == SelectionState::Selected ? FocusState::Focused : FocusState::NotFocused;

        // Checkbox, if any.
        if (hasCheck) {
          auto checkBoxState = optItem->state;
          //// TODO: How to know if checkbox hovered/pressed?
          //auto checkHovered = false;
          //if (w->underMouse()) {
          //  const auto mousePos = w->mapFromGlobal(QCursor::pos());
          //  checkHovered = checkboxRect.contains(mousePos);
          //}
          //auto checkBoxState = optItem->state;
          //checkBoxState.setFlag(State_MouseOver, checkHovered);
          //checkBoxState.setFlag(State_Sunken, checkHovered && checkBoxState.testFlag(State_Sunken));
          checkBoxState.setFlag(State_Selected, selected == SelectionState::Selected && focus == FocusState::Focused);
          checkBoxState.setFlag(State_MouseOver, false);
          checkBoxState.setFlag(State_Sunken, false);
          checkBoxState.setFlag(State_HasFocus, focus == FocusState::Focused);
          auto checkOpt = QStyleOptionViewItem(*optItem);
          checkOpt.rect = checkboxRect;
          checkOpt.state = checkBoxState;
          drawPrimitive(PE_IndicatorItemViewItemCheck, &checkOpt, p, w);
        }

        // Actual content.
        const auto itemMouse = getMouseState(optItem->state);
        const auto& fgColor = listItemForegroundColor(itemMouse, selected, focus, active);
        constexpr auto paletteColorRole = QPalette::ColorRole::Text;
        const auto paletteColorGroup = getPaletteColorGroup(optItem->state);
        const auto& textColor =
          focus == FocusState::Focused ? fgColor : optItem->palette.color(paletteColorGroup, paletteColorRole);

        // Icon.
        if (iconRect.width() > 0 && hasIcon) {
          const auto pixmap = getPixmap(optItem->icon, iconSize, itemMouse, checked, w);
          const auto autoIconColor = listItemAutoIconColor(itemMouse, selected, focus, active, optItem->index, w);

          if (itemMouse == MouseState::Disabled && autoIconColor == AutoIconColor::None) {
            const auto& bgColor =
              listItemBackgroundColor(MouseState::Normal, selected, focus, active, optItem->index, w);
            const auto premultipiedColor = getColorSourceOver(bgColor, fgColor);
            const auto& tintedPixmap = getTintedPixmap(pixmap, premultipiedColor);
            const auto opacity = selected == SelectionState::Selected ? 0.3 : 0.25;
            const auto backupOpacity = p->opacity();
            p->setOpacity(opacity * backupOpacity);
            p->drawPixmap(iconRect, tintedPixmap);
            p->setOpacity(backupOpacity);
          } else {
            const auto status = widgetStatus(w);
            const auto& colorizedPixmap = (status != Status::Default && autoIconColor != AutoIconColor::None)
                                            ? qlementine::getColorizedPixmap(pixmap, statusColor(status, itemMouse))
                                            : getColorizedPixmap(pixmap, autoIconColor, fgColor, textColor);
            p->drawPixmap(iconRect, colorizedPixmap);
          }
        }

        // Text.
        if (textRect.width() > 0 && !optItem->text.isEmpty()) {
          const auto& fm = optItem->fontMetrics;
          if (textRect.width() > 0) {
            const auto elidedText = fm.elidedText(optItem->text, Qt::ElideRight, textRect.width(), Qt::TextSingleLine);
            const auto textAlignment = optItem->displayAlignment;
            auto textFlags = Qt::AlignVCenter | Qt::AlignBaseline | Qt::TextSingleLine;
            textFlags = textAlignment.testFlag(Qt::AlignHCenter) ? textFlags | Qt::AlignHCenter
                        : textAlignment.testFlag(Qt::AlignRight) ? textFlags | Qt::AlignRight
                                                                 : textFlags | Qt::AlignLeft;
            p->setFont(optItem->font);
            p->setBrush(Qt::NoBrush);
            p->setPen(textColor);
            p->drawText(textRect, static_cast<int>(textFlags), elidedText, nullptr);
          }
        }
      }
      return;
    case CE_ShapedFrame:
      if (const auto* frameOpt = qstyleoption_cast<const QStyleOptionFrame*>(opt)) {
        const auto frameShape = frameOpt->frameShape;
        const auto lineW = _impl->theme.borderWidth;

        switch (frameShape) {
          case QFrame::HLine:
          case QFrame::VLine: {
            const auto& lineColor = separatorColor();
            const auto& pen = QPen(lineColor, lineW, Qt::PenStyle::SolidLine, Qt::PenCapStyle::FlatCap);
            p->setBrush(Qt::NoBrush);
            p->setPen(pen);
            if (frameShape == QFrame::HLine) {
              const auto p1 = QPoint(opt->rect.x(), opt->rect.y() + opt->rect.height() / 2);
              const auto p2 = QPoint(opt->rect.x() + opt->rect.width(), p1.y());
              p->drawLine(p1, p2);
            } else {
              const auto p1 = QPoint(opt->rect.x() + opt->rect.width() / 2, opt->rect.y());
              const auto p2 = QPoint(p1.x(), p1.y() + opt->rect.height());
              p->drawLine(p1, p2);
            }
          } break;
          default: {
            if (w != nullptr) {
              const auto& borderColor = frameBorderColor();
              const auto& pen = QPen(borderColor, lineW, Qt::PenStyle::SolidLine, Qt::PenCapStyle::SquareCap);
              const auto bgRole = w->backgroundRole();
              if (bgRole != QPalette::NoRole && w->autoFillBackground()) {
                const auto& palette = _impl->theme.palette;
                const auto& bgColor = palette.color(QPalette::ColorGroup::Normal, bgRole);
                p->setPen(pen);
                p->setBrush(bgColor);
                p->setRenderHint(QPainter::Antialiasing, true);
                p->drawRect(frameOpt->rect);
              }
            }
          } break;
        }
      }
      return;
    default:
      break;
  }
  QCommonStyle::drawControl(ce, opt, p, w);
}

QRect QlementineStyle::subElementRect(SubElement se, const QStyleOption* opt, const QWidget* w) const {
  switch (se) {
    case SE_PushButtonContents:
      if (const auto* optButton = buttonStyleOption(opt)) {
        const auto hasIcon = !optButton->icon.isNull();
        const auto hasText = !optButton->text.isEmpty();
        const auto hasMenu = optButton->features.testFlag(QStyleOptionButton::HasMenu);
        const auto padding = pixelMetric(PM_ButtonMargin);
        auto [paddingLeft, paddingRight] = getHPaddings(hasIcon, hasText, hasMenu, padding);

        // Keep visual balance for text+icon QPushButton without menu indicator.
        if (hasText && hasIcon && !hasMenu) {
          paddingLeft = padding * 2;
          paddingRight = padding * 2;
        }

        if (paddingLeft + paddingRight >= opt->rect.width()) {
          return opt->rect;
        }
        return opt->rect.marginsRemoved({ paddingLeft, 0, paddingRight, 0 });
      }
      return opt->rect;
    case SE_PushButtonBevel:
      return opt->rect;
    case SE_PushButtonFocusRect: {
      const auto borderW = _impl->theme.focusBorderWidth;
      return opt->rect.translated(borderW * 2, borderW * 2).marginsAdded(QMargins(borderW, borderW, borderW, borderW));
    }
    case SE_RadioButtonIndicator:
    case SE_CheckBoxIndicator: {
      const auto indicatorSize = pixelMetric(PM_IndicatorWidth);
      const auto indicatorY = opt->rect.y() + (opt->rect.height() - indicatorSize) / 2;
      return QRect{ opt->rect.x(), indicatorY, indicatorSize, indicatorSize };
    }
    case SE_RadioButtonFocusRect:
    case SE_CheckBoxFocusRect: {
      const auto borderW = _impl->theme.focusBorderWidth;
      const auto checkRect =
        subElementRect(SE_CheckBoxIndicator, opt, w).marginsAdded(QMargins(borderW, borderW, borderW, borderW));
      const auto deltaX = pixelMetric(PM_FocusFrameHMargin, opt, w);
      const auto deltaY = pixelMetric(PM_FocusFrameVMargin, opt, w);
      return checkRect.translated(deltaX, deltaY);
    }
    case SE_RadioButtonContents:
    case SE_CheckBoxContents: {
      const auto indicatorSize = pixelMetric(PM_IndicatorWidth);
      const auto spacing = pixelMetric(PM_CheckBoxLabelSpacing);
      return opt->rect.marginsRemoved({ indicatorSize + spacing, 0, 0, 0 });
    }
    case SE_CheckBoxClickRect:
    case SE_RadioButtonClickRect:
      // Make the whole widget clickable, not only the check/radio indicator.
      return opt->rect;
    case SE_ComboBoxFocusRect: {
      const auto borderW = _impl->theme.focusBorderWidth;
      return opt->rect.translated(borderW * 2, borderW * 2).marginsAdded(QMargins(borderW, borderW, borderW, borderW));
    }
    case SE_SliderFocusRect:
      // Also used for Dial.
      if (const auto* optSlider = qstyleoption_cast<const QStyleOptionSlider*>(opt)) {
        const auto* isDial = qobject_cast<const QDial*>(w);
        const auto complexControl = isDial ? CC_Dial : CC_Slider;
        const auto subControl = isDial ? SC_DialHandle : SC_SliderHandle;
        const auto handleRect = subControlRect(complexControl, optSlider, subControl, w);
        return QlementineStyleImpl::sliderFocusRect(*this, *optSlider, handleRect, w);
      }
      return opt->rect;
    case SE_ProgressBarContents:
    case SE_ProgressBarGroove:
      if (const auto* optProgressBar = qstyleoption_cast<const QStyleOptionProgressBar*>(opt)) {
        const auto showText = optProgressBar->textVisible;
        const auto labelW = showText ? optProgressBar->fontMetrics
                                         .boundingRect(optProgressBar->rect, Qt::AlignRight, QStringLiteral("100%"))
                                         .width()
                                     : 0;
        const auto spacing = showText ? _impl->theme.spacing : 0;
        const auto barW = opt->rect.width() - labelW - spacing;
        const auto barH = _impl->theme.progressBarGrooveHeight;
        const auto barY = opt->rect.y() + (opt->rect.height() - barH) / 2;
        return QRect{ opt->rect.x(), barY, barW, barH };
      }
      return {};
    case SE_ProgressBarLabel:
      if (const auto* optProgressBar = qstyleoption_cast<const QStyleOptionProgressBar*>(opt)) {
        const auto showText = optProgressBar->textVisible;
        const auto labelW = showText ? optProgressBar->fontMetrics
                                         .boundingRect(optProgressBar->rect, Qt::AlignRight, QStringLiteral("100%"))
                                         .width()
                                     : 0;
        const auto labelH = showText ? optProgressBar->fontMetrics.height() : 0;
        const auto labelX = opt->rect.right() - labelW;
        const auto labelY = opt->rect.y() + (opt->rect.height() - labelH) / 2;
        return QRect{ labelX, labelY, labelW, labelH };
      }
      return {};
    case SE_ToolBoxTabContents:
      break;
    case SE_HeaderLabel:
      if (const auto* optHeader = qstyleoption_cast<const QStyleOptionHeader*>(opt)) {
        const auto& rect = optHeader->rect;
        const auto paddingH = pixelMetric(PM_HeaderMargin);
        const auto labelW = rect.width() - paddingH * 2;
        const auto labelH = rect.height();
        const auto labelX = rect.x() + paddingH;
        const auto labelY = rect.y();
        return QRect{ labelX, labelY, labelW, labelH };
      }
      return {};
    case SE_HeaderArrow:
      if (const auto* optHeader = qstyleoption_cast<const QStyleOptionHeader*>(opt)) {
        const auto hasArrow = optHeader->sortIndicator != QStyleOptionHeader::SortIndicator::None;
        if (hasArrow) {
          const auto& rect = optHeader->rect;
          const auto paddingH = pixelMetric(PM_HeaderMargin);
          const auto iconExtent = pixelMetric(PM_SmallIconSize);
          const auto arrowW = iconExtent;
          const auto arrowH = iconExtent;
          const auto arrowX = rect.x() + rect.width() - paddingH - arrowW;
          const auto arrowY = rect.y() + (rect.height() - arrowH) / 2;
          return QRect{ arrowX, arrowY, arrowW, arrowH };
        }
      }
      break;
    case SE_ItemViewItemCheckIndicator:
    case SE_ItemViewItemDecoration:
    case SE_ItemViewItemText:
      if (const auto* optItem = qstyleoption_cast<const QStyleOptionViewItem*>(opt)) {
        const auto* listView = qobject_cast<const QListView*>(w);
        const auto iconMode = listView && listView->viewMode() == QListView::IconMode;
        const auto& features = optItem->features;
        const auto spacing = _impl->theme.spacing;
        const auto hPadding = spacing;
        const auto hasIcon = features.testFlag(QStyleOptionViewItem::HasDecoration) && !optItem->icon.isNull();
        const auto iconSize = hasIcon ? optItem->decorationSize : QSize{ 0, 0 };
        const auto hasText = features.testFlag(QStyleOptionViewItem::HasDisplay) && !optItem->text.isEmpty();
        const auto textH = hasText ? optItem->fontMetrics.height() : 0;
        const auto hasCheck = features.testFlag(QStyleOptionViewItem::HasCheckIndicator);
        const auto checkSize = hasCheck ? _impl->theme.iconSize : QSize{ 0, 0 };

        if (iconMode) {
          const auto contentH = iconSize.height() + (hasIcon && hasText ? spacing : 0) + textH
                                + (hasCheck && (hasIcon || hasText) ? spacing : 0) + checkSize.height();
          auto y = optItem->rect.y() + std::max(0, optItem->rect.height() - contentH) / 2;

          if (se == SE_ItemViewItemDecoration) {
            return hasIcon ? QRect{ optItem->rect.x() + (optItem->rect.width() - iconSize.width()) / 2, y,
              iconSize.width(), iconSize.height() }
                           : QRect{};
          }

          if (hasIcon) {
            y += iconSize.height() + (hasText || hasCheck ? spacing : 0);
          }
          if (se == SE_ItemViewItemText) {
            return hasText ? QRect{ optItem->rect.x() + hPadding, y, optItem->rect.width() - 2 * hPadding, textH }
                           : QRect{};
          }

          if (hasText) {
            y += textH + (hasCheck ? spacing : 0);
          }
          return hasCheck ? QRect{ optItem->rect.x() + (optItem->rect.width() - checkSize.width()) / 2, y,
            checkSize.width(), checkSize.height() }
                          : QRect{};
        } else {
          auto x = optItem->rect.x() + hPadding;
          const auto contentY = optItem->rect.y();
          const auto contentH = optItem->rect.height();
          const auto contentRight = optItem->rect.x() + optItem->rect.width() - hPadding;

          if (se == SE_ItemViewItemCheckIndicator) {
            return hasCheck
                     ? QRect{ x, contentY + (contentH - checkSize.height()) / 2, checkSize.width(), checkSize.height() }
                     : QRect{};
          }

          if (hasCheck) {
            x += checkSize.width() + spacing;
          }
          if (se == SE_ItemViewItemDecoration) {
            return hasIcon
                     ? QRect{ x, contentY + (contentH - iconSize.height()) / 2, iconSize.width(), iconSize.height() }
                     : QRect{};
          }

          if (hasIcon) {
            x += iconSize.width() + spacing;
          }
          return hasText ? QRect{ x, contentY, std::max(0, contentRight - x), contentH } : QRect{};
        }
      }
      break;
    case SE_TreeViewDisclosureItem:
      break;
    case SE_LineEditContents:
      if (const auto* optFrame = qstyleoption_cast<const QStyleOptionFrame*>(opt)) {
        const auto borderW = optFrame->lineWidth;
        const auto hMargin = _impl->theme.spacing / 2;
        const auto rect = optFrame->rect.adjusted(borderW + hMargin, borderW, -borderW - hMargin, -borderW);
        //r = visualRect(opt->direction, opt->rect, r);
        return rect;
      }
      return {};
    case SE_FrameContents:
      break;
    case SE_DockWidgetCloseButton:
      break;
    case SE_DockWidgetFloatButton:
      break;
    case SE_DockWidgetTitleBarText:
      break;
    case SE_DockWidgetIcon:
      break;
    case SE_CheckBoxLayoutItem:
    case SE_ComboBoxLayoutItem:
    case SE_DateTimeEditLayoutItem:
    case SE_LabelLayoutItem:
    case SE_ProgressBarLayoutItem:
    case SE_PushButtonLayoutItem:
    case SE_RadioButtonLayoutItem:
    case SE_SliderLayoutItem:
    case SE_SpinBoxLayoutItem:
    case SE_ToolButtonLayoutItem:
    case SE_FrameLayoutItem:
    case SE_GroupBoxLayoutItem:
      break;

    // TabWidget
    case SE_TabWidgetLayoutItem:
      break;
    case SE_TabWidgetTabBar:
      break;
    case SE_TabWidgetTabPane:
      break;
    case SE_TabWidgetTabContents:
      break;
    case SE_TabWidgetLeftCorner:
      break;
    case SE_TabWidgetRightCorner:
      break;

    // TabBar
    case SE_TabBarTearIndicatorLeft: {
      const auto& rect = opt->rect;
      const auto shadowW = _impl->theme.spacing * 3;
      if (QlementineStyleImpl::isTabShapeVertical(QlementineStyleImpl::tabBarShape(w))) {
        return { rect.x(), rect.y(), rect.width(), shadowW };
      }
      return { rect.x(), rect.y(), shadowW, rect.height() };
    }
    case SE_TabBarTearIndicatorRight: {
      const auto& rect = opt->rect;
      const auto scrollButtonsW = _impl->theme.controlHeightMedium * 2 + _impl->theme.spacing * 3;
      const auto shadowW = _impl->theme.spacing * 3;
      if (QlementineStyleImpl::isTabShapeVertical(QlementineStyleImpl::tabBarShape(w))) {
        return { rect.x(), rect.y() + rect.height() - shadowW - scrollButtonsW, rect.width(),
          shadowW + scrollButtonsW };
      }
      return { rect.x() + rect.width() - shadowW - scrollButtonsW, rect.y(), shadowW + scrollButtonsW, rect.height() };
    }
    case SE_TabBarTabLeftButton:
      // Button on the left of a tab.
      if (const auto* optTab = qstyleoption_cast<const QStyleOptionTab*>(opt)) {
        const auto localRect = QlementineStyleImpl::tabLocalRect(optTab->shape, optTab->rect);
        const auto isVertical = QlementineStyleImpl::isTabShapeVertical(optTab->shape);
        const auto buttonSize = isVertical ? optTab->leftButtonSize.transposed() : optTab->leftButtonSize;
        const auto paddingTop = _impl->theme.tabBarPaddingTop;
        const auto spacing = _impl->theme.spacing;
        const auto padding = _impl->tabExtraPadding(optTab, w);
        const auto x = localRect.x() + spacing + padding.left() + _impl->tabContentInset(optTab->shape);
        const auto y = localRect.y() + paddingTop + (localRect.height() - paddingTop - buttonSize.height()) / 2;
        return QlementineStyleImpl::tabLocalTransform(optTab->shape, optTab->rect)
          .mapRect(QRect{ x, y, buttonSize.width(), buttonSize.height() });
      }
      return {};
    case SE_TabBarTabRightButton:
      // Button on the right of a tab (close button).
      if (const auto* optTab = qstyleoption_cast<const QStyleOptionTab*>(opt)) {
        const auto localRect = QlementineStyleImpl::tabLocalRect(optTab->shape, optTab->rect);
        const auto isVertical = QlementineStyleImpl::isTabShapeVertical(optTab->shape);
        const auto buttonSize = isVertical ? optTab->rightButtonSize.transposed() : optTab->rightButtonSize;
        const auto spacing = _impl->theme.spacing;
        const auto paddingTop = _impl->theme.tabBarPaddingTop;
        const auto padding = _impl->tabExtraPadding(optTab, w);
        const auto x = localRect.x() + localRect.width() - spacing - buttonSize.width() - padding.right()
                       - _impl->tabContentInset(optTab->shape);
        const auto y = localRect.y() + paddingTop + (localRect.height() - paddingTop - buttonSize.height()) / 2;
        return QlementineStyleImpl::tabLocalTransform(optTab->shape, optTab->rect)
          .mapRect(QRect{ x, y, buttonSize.width(), buttonSize.height() });
      }
      return {};
    case SE_TabBarTabText:
      if (const auto* optTab = qstyleoption_cast<const QStyleOptionTab*>(opt)) {
        const auto localRect = QlementineStyleImpl::tabLocalRect(optTab->shape, optTab->rect);
        const auto isVertical = QlementineStyleImpl::isTabShapeVertical(optTab->shape);
        const auto spacing = _impl->theme.spacing;
        const auto leftButtonWidth = isVertical ? optTab->leftButtonSize.height() : optTab->leftButtonSize.width();
        const auto rightButtonWidth = isVertical ? optTab->rightButtonSize.height() : optTab->rightButtonSize.width();
        const auto leftButtonW = leftButtonWidth > 0 ? leftButtonWidth + spacing : 0;
        const auto rightButtonW = rightButtonWidth > 0 ? rightButtonWidth + spacing : 0;
        const auto padding = _impl->tabExtraPadding(optTab, w);
        const auto inset = _impl->tabContentInset(optTab->shape);
        const auto x = localRect.x() + padding.left() + spacing + leftButtonW + inset;
        const auto y = localRect.y() + _impl->theme.tabBarPaddingTop;
        const auto width =
          localRect.width() - leftButtonW - padding.left() - rightButtonW - padding.right() - spacing * 2 - inset * 2;
        const auto height = localRect.height() - _impl->theme.tabBarPaddingTop;
        return QlementineStyleImpl::tabLocalTransform(optTab->shape, optTab->rect)
          .mapRect(QRect{ x, y, width, height });
      }
      return {};
    case SE_TabBarScrollLeftButton: {
      const auto& rect = opt->rect;
      const auto spacing = _impl->theme.spacing;
      const auto width = _impl->theme.controlHeightMedium + static_cast<int>(spacing * 1.5);
      const auto height = _impl->theme.controlHeightLarge + spacing;
      if (QlementineStyleImpl::isTabShapeVertical(QlementineStyleImpl::tabBarShape(w))) {
        return { rect.x(), rect.y() + rect.height() - 2 * width, height, width };
      }
      const auto x = rect.x() + rect.width() - 2 * width;
      const auto y = rect.y();
      return { x, y, width, height };
    }
    case SE_TabBarScrollRightButton: {
      const auto& rect = opt->rect;
      const auto spacing = _impl->theme.spacing;
      const auto width = _impl->theme.controlHeightMedium + static_cast<int>(spacing * 1.5);
      const auto height = _impl->theme.controlHeightLarge + spacing;
      if (QlementineStyleImpl::isTabShapeVertical(QlementineStyleImpl::tabBarShape(w))) {
        return { rect.x(), rect.y() + rect.height() - width + spacing / 2, height, width };
      }
      const auto x = rect.x() + rect.width() - width + spacing / 2;
      const auto y = rect.y();
      return { x, y, width, height };
    }
    case SE_ToolBarHandle:
      //qDebug() << "SE_ToolBarHandle";
      break;
    case SE_ShapedFrameContents:
      break;
    default:
      break;
  }
  return QCommonStyle::subElementRect(se, opt, w);
}

void QlementineStyle::drawComplexControl(
  ComplexControl cc, const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const {
  QPainterStateGuard stateGuard(p);
  p->setRenderHint(QPainter::SmoothPixmapTransform, true);
  switch (cc) {
    case CC_SpinBox:
      if (const auto* spinboxOpt = qstyleoption_cast<const QStyleOptionSpinBox*>(opt)) {
        const auto* parentWidget = w ? w->parentWidget() : nullptr;
        const auto isTabCellEditor =
          parentWidget && qobject_cast<const QAbstractItemView*>(parentWidget->parentWidget());

        p->setRenderHint(QPainter::Antialiasing, true);
        const auto spinBoxEnabled = spinboxOpt->state.testFlag(State_Enabled);
        if (spinboxOpt->buttonSymbols != QAbstractSpinBox::NoButtons) {
          const auto radius = static_cast<double>(_impl->theme.borderRadius);
          const auto upButtonRect = subControlRect(cc, opt, SC_SpinBoxUp, w);
          if (upButtonRect.isValid()) {
            const auto upButtonActive = spinboxOpt->activeSubControls.testFlag(SC_SpinBoxUp);
            const auto upButtonPath = getMultipleRadiusesRectPath(
              upButtonRect, isTabCellEditor ? RadiusesF{ 0. } : RadiusesF{ 0., radius, 0., 0. });
            const auto upButtonEnabled =
              spinBoxEnabled && spinboxOpt->stepEnabled.testFlag(QAbstractSpinBox::StepUpEnabled);
            const auto upButtonHovered = upButtonActive;
            const auto upButtonPressed = upButtonActive && spinboxOpt->state.testFlag(State_Sunken);
            const auto upMouse = getMouseState(upButtonPressed, upButtonHovered, upButtonEnabled);
            const auto& upButtonBgColor = spinBoxButtonBackgroundColor(upMouse);
            const auto& currentColor =
              _impl->animations.animateBackgroundColor(w, upButtonBgColor, _impl->theme.animationDuration);

            // Draw background.
            p->setPen(Qt::NoPen);
            p->setBrush(currentColor);
            p->drawPath(upButtonPath);

            // Draw icon.
            const auto& fgColor = spinBoxButtonForegroundColor(upMouse);
            const auto& currentFgColor =
              _impl->animations.animateForegroundColor(w, fgColor, _impl->theme.animationDuration);
            const auto iconSize = _impl->theme.iconSize / 2;
            const auto translateY = _impl->theme.borderWidth;
            p->setPen(QPen(currentFgColor, iconPenWidth, Qt::SolidLine, Qt::FlatCap));
            p->setBrush(Qt::NoBrush);
            drawSpinBoxArrowIndicator(
              upButtonRect.translated(0, translateY), p, spinboxOpt->buttonSymbols, SC_SpinBoxUp, iconSize);
          }

          const auto downButtonRect = subControlRect(cc, opt, SC_SpinBoxDown, w);
          if (downButtonRect.isValid()) {
            const auto downButtonActive = spinboxOpt->activeSubControls.testFlag(SC_SpinBoxDown);
            const auto downButtonPath = getMultipleRadiusesRectPath(downButtonRect, RadiusesF{ 0., 0., radius, 0. });
            const auto downButtonEnabled =
              spinBoxEnabled && spinboxOpt->stepEnabled.testFlag(QAbstractSpinBox::StepDownEnabled);
            const auto downButtonHovered = downButtonActive;
            const auto downButtonPressed = downButtonActive && spinboxOpt->state.testFlag(State_Sunken);
            const auto downMouse = getMouseState(downButtonPressed, downButtonHovered, downButtonEnabled);
            const auto& downButtonBgColor = spinBoxButtonBackgroundColor(downMouse);
            const auto& currentColor =
              _impl->animations.animateBackgroundColor2(w, downButtonBgColor, _impl->theme.animationDuration);

            // Draw background.
            p->setPen(Qt::NoPen);
            p->setBrush(currentColor);
            p->drawPath(downButtonPath);

            // Draw icon.
            const auto& fgColor = spinBoxButtonForegroundColor(downMouse);
            const auto& currentFgColor =
              _impl->animations.animateForegroundColor2(w, fgColor, _impl->theme.animationDuration);
            const auto iconSize = _impl->theme.iconSize / 2;
            p->setPen(QPen(currentFgColor, iconPenWidth, Qt::SolidLine, Qt::FlatCap));
            p->setBrush(Qt::NoBrush);
            drawSpinBoxArrowIndicator(downButtonRect, p, spinboxOpt->buttonSymbols, SC_SpinBoxDown, iconSize);
          }
        }
      }
      return;
    case CC_ComboBox:
      if (const auto* comboBoxOpt = qstyleoption_cast<const QStyleOptionComboBox*>(opt)) {
        // When the combobox is editable, we draw the shape of a QLineEdit.
        // When it's not, we draw the shape of a QPushButton.
        if (comboBoxOpt->editable) {
          // Simulate an arrow button.
          const auto arrowButtonRect = subControlRect(CC_ComboBox, comboBoxOpt, SC_ComboBoxArrow, w);
          QStyleOptionRoundedButton buttonOpt;
          buttonOpt.rect = arrowButtonRect;
          buttonOpt.fontMetrics = comboBoxOpt->fontMetrics;
          buttonOpt.palette = comboBoxOpt->palette;
          buttonOpt.state = comboBoxOpt->state;
          buttonOpt.state.setFlag(QStyle::StateFlag::State_On, false);
          buttonOpt.features.setFlag(QStyleOptionButton::Flat, !comboBoxOpt->frame);
          buttonOpt.radiuses = RadiusesF{ 0., _impl->theme.borderRadius };
          drawControl(CE_PushButtonBevel, &buttonOpt, p, w);

          // NB: CE_ComboBoxLabel won't be called for an editable QComboBox,
          // because the foreground content is drawn by the QLineEdit within the QComboBox.
          // We still want the arrow indicator, so we have to draw it here.
          // Non-editable ComboBox foreground drawing is done in CE_ComboBoxLabel.
          {
            const auto mouse = getMouseState(comboBoxOpt->state);
            const auto& fgColor = comboBoxForegroundColor(mouse);
            const auto& currentFgColor =
              _impl->animations.animateForegroundColor(w, fgColor, _impl->theme.animationDuration);

            const auto indicatorSize = _impl->theme.iconSize;
            const auto indicatorX = arrowButtonRect.x() + (arrowButtonRect.width() - indicatorSize.width()) / 2;
            const auto indicatorY = arrowButtonRect.y() + (arrowButtonRect.height() - indicatorSize.height()) / 2;
            const auto indicatorRect = QRect{ QPoint{ indicatorX, indicatorY }, indicatorSize };

            if (qobject_cast<const QDateTimeEdit*>(w)) {
              const auto pixelRatio = getPixelRatio(w);
              const auto& icon = _impl->getStandardIconExt(StandardPixmapExt::SP_Calendar, indicatorSize * pixelRatio);
              drawIcon(indicatorRect, p, icon, mouse, CheckState::Checked, w, true, currentFgColor);
            } else {
              p->setBrush(Qt::NoBrush);
              p->setPen(QPen(currentFgColor, iconPenWidth, Qt::SolidLine, Qt::FlatCap));
              drawComboBoxIndicator(indicatorRect, p);
            }
          }
        } else {
          const auto* parentWidget = w ? w->parentWidget() : nullptr;
          const auto isTabCellEditor =
            parentWidget && qobject_cast<const QAbstractItemView*>(parentWidget->parentWidget());

          // ComboBox background and border (same as a Button).
          QStyleOptionRoundedButton buttonOpt;
          buttonOpt.rect = comboBoxOpt->rect;
          buttonOpt.fontMetrics = comboBoxOpt->fontMetrics;
          buttonOpt.palette = comboBoxOpt->palette;
          buttonOpt.state = comboBoxOpt->state;
          buttonOpt.state.setFlag(QStyle::StateFlag::State_On, false);
          buttonOpt.features.setFlag(QStyleOptionButton::Flat, !comboBoxOpt->frame);
          buttonOpt.radiuses = RadiusesF{ isTabCellEditor ? 0. : _impl->theme.borderRadius };
          drawControl(CE_PushButtonBevel, &buttonOpt, p, w);
        }
      }
      return;
    case CC_ScrollBar:
      if (const auto* scrollBarOpt = qstyleoption_cast<const QStyleOptionSlider*>(opt)) {
        // NB: no animation for the scrollbar handle, because it should be the content offset
        // that should be animated.
        const auto mouse = getMouseState(scrollBarOpt->state);
        const auto horizontal = scrollBarOpt->orientation == Qt::Horizontal;
        const auto thickness = getScrollBarThickness(mouse);
        const auto currentThickness =
          _impl->animations.animateProgress(w, thickness, _impl->theme.animationDuration * 2);
        const auto scrollBarMargin = _impl->theme.scrollBarMargin;

        // Groove.
        const auto grooveRect = subControlRect(CC_ScrollBar, scrollBarOpt, SC_ScrollBarGroove, w);
        const auto currentGrooveRect =
          horizontal ? QRectF(grooveRect.x(), grooveRect.y() + grooveRect.height() - currentThickness,
                         grooveRect.width(), currentThickness)
                     : QRectF(grooveRect.x() + grooveRect.width() - currentThickness, grooveRect.y(), currentThickness,
                         grooveRect.height());

        const auto& grooveColor = scrollBarGrooveColor(mouse);
        const auto& currentGrooveColor =
          _impl->animations.animateBackgroundColor(w, grooveColor, _impl->theme.animationDuration * 2);
        const auto grooveRadius = scrollBarMargin <= 0 ? 0.
                                  : horizontal         ? currentGrooveRect.height() / 2.
                                                       : currentGrooveRect.width() / 2.;
        p->setRenderHint(QPainter::Antialiasing, true);
        p->setPen(Qt::NoPen);
        p->setBrush(currentGrooveColor);
        p->drawRoundedRect(currentGrooveRect, grooveRadius, grooveRadius);

        // Handle.
        const auto handleRect = subControlRect(CC_ScrollBar, scrollBarOpt, SC_ScrollBarSlider, w);
        if (!handleRect.isEmpty()) {
          const auto currentHandleRect =
            horizontal ? QRectF(handleRect.x(), handleRect.y() + handleRect.height() - currentThickness,
                           handleRect.width(), currentThickness)
                       : QRectF(handleRect.x() + handleRect.width() - currentThickness, handleRect.y(),
                           currentThickness, handleRect.height());
          const auto handleMouse = getScrollBarHandleState(scrollBarOpt->state, scrollBarOpt->activeSubControls);
          const auto& handleColor = scrollBarHandleColor(handleMouse);
          const auto& currentHandleColor =
            _impl->animations.animateBackgroundColor2(w, handleColor, _impl->theme.animationDuration);
          const auto handleRadius = horizontal ? currentHandleRect.height() / 2. : currentHandleRect.width() / 2.;
          p->setBrush(currentHandleColor);
          p->drawRoundedRect(currentHandleRect, handleRadius, handleRadius);
        }
      }
      return;
    case CC_Slider:
      if (const auto* sliderOpt = qstyleoption_cast<const QStyleOptionSlider*>(opt)) {
        const auto progress = sliderOpt->sliderPosition;
        // If the user is dragging the handle, we shorten the animation to ensure it correctly follows
        // the mouse cursor with a quick interpolation (otherwise, it snaps and don't animate).
        const auto handleActive =
          sliderOpt->state.testFlag(State_Sunken) && sliderOpt->activeSubControls == SubControl::SC_SliderHandle;
        const auto duration = handleActive ? _impl->theme.sliderAnimationDuration : _impl->theme.animationDuration;
        const auto currentProgress = _impl->animations.animateProgress(w, progress, duration);

        const auto min = sliderOpt->minimum;
        const auto max = sliderOpt->maximum;
        const auto widgetMouse = getMouseState(sliderOpt->state);
        const auto mouse = widgetMouse == MouseState::Disabled ? MouseState::Disabled : MouseState::Normal;
        const auto handleRect = QlementineStyleImpl::sliderHandleRect(*this, *sliderOpt, currentProgress, w);
        const auto disabled = mouse == MouseState::Disabled;

        // Draw tickmarks.
        if (sliderOpt->subControls.testFlag(SC_SliderTickmarks) && sliderOpt->tickPosition != QSlider::NoTicks) {
          const auto tickmarksRect = subControlRect(CC_Slider, opt, SC_SliderTickmarks, w);
          const auto tickThickness = _impl->theme.sliderTickThickness;
          const auto& tickColor = sliderTickColor(mouse);

          // Little trick to avoid having two colors with alpha<255 above one another.
          if (disabled) {
            p->save();
            auto clipRegion = QRegion{ tickmarksRect };
            clipRegion = clipRegion.subtracted(handleRect.adjusted(1, 0, -1, 0));
            p->setClipRegion(clipRegion);
          }

          // TODO other tick positions.
          drawSliderTickMarks(p, tickmarksRect, tickColor, min, max, sliderOpt->tickInterval, tickThickness,
            sliderOpt->singleStep, sliderOpt->pageStep);
          if (disabled) {
            p->restore();
          }
        }

        // Draw groove and value.
        const auto grooveRect = subControlRect(CC_Slider, opt, SC_SliderGroove, w);

        if (sliderOpt->subControls.testFlag(SC_SliderGroove) && grooveRect.isValid()) {
          const auto& grooveColor = sliderGrooveColor(mouse);
          const auto& valueColor = sliderValueColor(mouse);
          const auto radius = grooveRect.height() / 2.;

          // Little trick to avoid having two colors with alpha<255 above one another.
          if (disabled) {
            p->save();
            auto clipRegion = QRegion{ grooveRect };
            clipRegion = clipRegion.subtracted(handleRect.adjusted(1, 0, -1, 0));
            p->setClipRegion(clipRegion);
          }
          drawRoundedRect(p, grooveRect, grooveColor, radius);
          if (disabled) {
            p->restore();
          }
          const auto valueRect = grooveRect.adjusted(0, 0, -handleRect.width() + 1, 0);
          drawProgressBarValueRect(p, valueRect, valueColor, min, max, currentProgress, radius);
        }

        // Draw handle.
        if (sliderOpt->subControls.testFlag(SC_SliderHandle) && handleRect.isValid()) {
          static QPixmap dropShadowPixmap;
          const auto handleMouse = sliderOpt->activeSubControls == QStyle::SC_SliderHandle ? widgetMouse : mouse;
          const auto& handleBgColor = sliderHandleColor(handleMouse);
          const auto& currentHandleBgColor =
            _impl->animations.animateForegroundColor(w, handleBgColor, _impl->theme.animationDuration);

          p->setRenderHint(QPainter::Antialiasing, true);

          // Create drop shadow and keep it in cache.
          if (dropShadowPixmap.isNull()) {
            QPixmap inputPixmap(handleRect.size());
            inputPixmap.fill(Qt::transparent);
            {
              QPainter tmpPainter(&inputPixmap);
              tmpPainter.setRenderHint(QPainter::Antialiasing, true);
              tmpPainter.setPen(Qt::NoPen);
              tmpPainter.setBrush(Qt::black);
              tmpPainter.drawEllipse(QRect{ QPoint{ 0, 0 }, handleRect.size() });
            }
            constexpr auto dropShadowBlurRadius = 2.;
            dropShadowPixmap =
              qlementine::getDropShadowPixmap(inputPixmap, dropShadowBlurRadius, _impl->theme.shadowColor3);
          }

          // Draw drop shadow centered below handle.
          {
            constexpr auto dropShadowOffsetY = 0.5;
            const auto dropShadowX = handleRect.x() + (handleRect.width() - dropShadowPixmap.width()) / 2;
            const auto dropShadowY =
              handleRect.y() + (handleRect.height() - dropShadowPixmap.height()) / 2. + dropShadowOffsetY;
            const auto compModebackup = p->compositionMode();
            p->setCompositionMode(QPainter::CompositionMode::CompositionMode_Multiply);
            p->drawPixmap(dropShadowX, int(dropShadowY), dropShadowPixmap);
            p->setCompositionMode(compModebackup);
          }

          p->setPen(Qt::NoPen);
          p->setBrush(currentHandleBgColor);
          p->drawEllipse(handleRect);
        }
      }
      return;
    case CC_ToolButton:
      if (const auto* toolbuttonOpt = qstyleoption_cast<const QStyleOptionToolButton*>(opt)) {
        const auto hasMenu = toolbuttonOpt->features.testFlag(QStyleOptionToolButton::HasMenu);
        const auto menuIsOnSeparateButton =
          hasMenu && toolbuttonOpt->features.testFlag(QStyleOptionToolButton::ToolButtonFeature::MenuButtonPopup);

        const auto isMouseOver = toolbuttonOpt->state.testFlag(State_MouseOver);
        const auto isPressed = toolbuttonOpt->state.testFlag(State_Sunken);
        const auto* parentTabBar = qobject_cast<QTabBar*>(w->parentWidget());
        const auto isTabBarScrollButton = parentTabBar != nullptr && toolbuttonOpt->arrowType != Qt::NoArrow;
        const auto radius = _impl->theme.borderRadius;
        const auto buttonActive = toolbuttonOpt->activeSubControls.testFlag(SC_ToolButton);
        const auto menuButtonActive =
          menuIsOnSeparateButton && toolbuttonOpt->activeSubControls.testFlag(SC_ToolButtonMenu);
        const auto mouse = getMouseState(toolbuttonOpt->state);

        const auto buttonRect = subControlRect(CC_ToolButton, opt, SC_ToolButton, w);
        const auto menuButtonRect = subControlRect(CC_ToolButton, opt, SC_ToolButtonMenu, w);

        // Tweak the state.
        auto buttonState = toolbuttonOpt->state;
        if (menuIsOnSeparateButton) {
          buttonState.setFlag(State_MouseOver, (isMouseOver && buttonActive) || (isPressed && menuButtonActive));
          buttonState.setFlag(State_Sunken, isPressed && buttonActive);
          buttonState.setFlag(State_Raised, isMouseOver && menuButtonActive);
        }

        // Main button.
        {
          auto buttonOpt = QStyleOptionToolButton(*toolbuttonOpt);
          buttonOpt.state = buttonState;

          // Special case for QTabBar.
          if (isTabBarScrollButton) {
            buttonOpt.state.setFlag(State_Raised, true);

            // Draw an opaque background to hide tabs below.
            const auto isLeftButton = toolbuttonOpt->arrowType == Qt::ArrowType::LeftArrow
                                      || toolbuttonOpt->arrowType == Qt::ArrowType::UpArrow;
            const auto tabBarShape = parentTabBar->shape();
            const auto isVertical = QlementineStyleImpl::isTabShapeVertical(tabBarShape);
            const auto tabBarState = parentTabBar->isEnabled() ? MouseState::Normal : MouseState::Disabled;
            if (parentTabBar->documentMode() || isLeftButton) {
              p->fillRect(toolbuttonOpt->rect, tabBarBackgroundColor(tabBarState));
            } else {
              const auto bgRadius = _impl->theme.borderRadius * 1.5;
              drawRoundedRect(p, toolbuttonOpt->rect, tabBarBackgroundColor(tabBarState),
                QlementineStyleImpl::tabBarRadiuses(tabBarShape, bgRadius, QlementineStyleImpl::TabBarPart::End));
            }

            // Rect.
            const auto spacing = _impl->theme.spacing;
            const auto buttonSize = QSize{ _impl->theme.controlHeightMedium, _impl->theme.controlHeightMedium };
            const auto buttonX = isVertical     ? buttonRect.x() + (buttonRect.width() - buttonSize.width()) / 2
                                 : isLeftButton ? buttonRect.x() + buttonRect.width() - buttonSize.width() - spacing / 2
                                                : buttonRect.x() + spacing / 2;
            const auto buttonY = !isVertical ? buttonRect.y() + (buttonRect.height() - buttonSize.height()) / 2
                                 : isLeftButton
                                   ? buttonRect.y() + buttonRect.height() - buttonSize.height() - spacing / 2
                                   : buttonRect.y() + spacing / 2;
            buttonOpt.rect = QRect{ QPoint{ buttonX, buttonY }, buttonSize };

            // The arrows are drawn pointing left/right, so rotate them to point up/down on vertical tab bars.
            QPainterStateGuard rotationGuard(p);
            if (isVertical) {
              const auto center = QPointF(buttonOpt.rect.center()) + QPointF(0.5, 0.5);
              p->translate(center);
              p->rotate(90.);
              p->translate(-center);
            }

            // Icon.
            const auto stdIcon = isLeftButton ? SP_ArrowLeft : SP_ArrowRight;
            buttonOpt.icon = standardIcon(stdIcon, &buttonOpt, w);

            drawPrimitive(PE_PanelButtonTool, &buttonOpt, p, w);
            drawControl(CE_ToolButtonLabel, &buttonOpt, p, w);

          } else {
            // Background.
            buttonOpt.rect = menuIsOnSeparateButton ? buttonRect : opt->rect;
            drawPrimitive(PE_PanelButtonTool, &buttonOpt, p, w);

            // Foreground.
            buttonOpt.rect = buttonRect;
            drawControl(CE_ToolButtonLabel, &buttonOpt, p, w);
          }
        }

        // Menu arrow.
        if (menuIsOnSeparateButton) {
          const auto& menuButtonRadiuses = hasMenu ? RadiusesF{ 0., radius, radius, 0. } : RadiusesF{};
          auto menuButtonState = toolbuttonOpt->state;
          menuButtonState.setFlag(State_MouseOver, (isMouseOver && menuButtonActive) || (isPressed && buttonActive));
          menuButtonState.setFlag(State_Sunken, isPressed && menuButtonActive);
          menuButtonState.setFlag(State_Raised, isMouseOver && buttonActive);

          // Background.
          const auto menuButtonMouse = getToolButtonMouseState(menuButtonState);
          const auto role = getColorRole(toolbuttonOpt->state, false);
          const auto& bgColor = toolButtonBackgroundColor(menuButtonMouse, role);
          const auto& currentColor =
            _impl->animations.animateBackgroundColor2(w, bgColor, _impl->theme.animationDuration);
          drawRoundedRect(p, menuButtonRect, currentColor, menuButtonRadiuses);

          // Line.
          const auto lineW = _impl->theme.borderWidth;
          const auto& lineColor = toolButtonSeparatorColor(mouse, role);
          const auto lineX = buttonRect.x() + buttonRect.width() - lineW / 2.;
          const auto lineMargin = 0; //_impl->theme.spacing / 2.;
          const auto lineY1 = static_cast<double>(buttonRect.y() + lineMargin);
          const auto lineY2 = static_cast<double>(buttonRect.y() + buttonRect.height() - lineMargin);
          const auto lineP1 = QPointF{ lineX, lineY1 };
          const auto lineP2 = QPointF{ lineX, lineY2 };
          p->setBrush(Qt::NoBrush);
          p->setPen(QPen(lineColor, lineW, Qt::SolidLine, Qt::FlatCap));
          p->drawLine(lineP1, lineP2);

          // Arrow.
          const auto& arrowSize = _impl->theme.iconSize;
          const auto arrowX = menuButtonRect.x() + (menuButtonRect.width() - arrowSize.width()) / 2;
          const auto arrowY = menuButtonRect.y() + (menuButtonRect.height() - arrowSize.height()) / 2;
          const auto arrowRect = QRect{ arrowX, arrowY, arrowSize.width(), arrowSize.height() };
          const auto& arrowColor = toolButtonForegroundColor(menuButtonMouse, role);
          const auto& currentArrowColor =
            _impl->animations.animateForegroundColor2(w, arrowColor, _impl->theme.animationDuration);
          const auto path = getMenuIndicatorPath(arrowRect);
          p->setPen(QPen(currentArrowColor, iconPenWidth, Qt::SolidLine, Qt::RoundCap));
          p->drawPath(path);
        } else if (hasMenu) {
          // Arrow.
          const auto spacing = _impl->theme.spacing;
          const auto& arrowSize = _impl->theme.iconSize;
          const auto arrowX = menuButtonRect.x() + (menuButtonRect.width() - arrowSize.width()) / 2 - spacing;
          const auto arrowY = menuButtonRect.y() + (menuButtonRect.height() - arrowSize.height()) / 2;
          const auto arrowRect = QRect{ arrowX, arrowY, arrowSize.width(), arrowSize.height() };
          const auto& arrowColor = toolButtonForegroundColor(mouse, getColorRole(toolbuttonOpt->state, false));
          const auto& currentArrowColor =
            _impl->animations.animateForegroundColor(w, arrowColor, _impl->theme.animationDuration);
          const auto path = getMenuIndicatorPath(arrowRect);
          p->setPen(QPen(currentArrowColor, iconPenWidth, Qt::SolidLine, Qt::RoundCap));
          p->drawPath(path);
        }
      }
      return;
    case CC_TitleBar:
      break;
    case CC_Dial:
      if (const auto* dialOpt = qstyleoption_cast<const QStyleOptionSlider*>(opt)) {
        const auto min = dialOpt->minimum;
        const auto max = dialOpt->maximum;
        const auto mouse = getMouseState(dialOpt->state);

        // Draw tickmarks.
        if (dialOpt->subControls.testFlag(SC_DialTickmarks)) {
          const auto tickmarksRect = subControlRect(cc, opt, SC_DialTickmarks, w);
          const auto tickThickness = _impl->theme.dialMarkThickness;
          const auto& tickColor = dialTickColor(mouse);
          const auto tickLength = _impl->theme.dialTickLength;
          const auto minArcLength = dialOpt->notchTarget * 2;
          drawDialTickMarks(p, tickmarksRect, tickColor, min, max, tickThickness, tickLength, dialOpt->singleStep,
            dialOpt->pageStep, int(minArcLength));
        }

        const auto progress = dialOpt->sliderPosition;
        // If the user is dragging the handle, we shorten the animation to ensure it correctly follows
        // the mouse cursor with a quick interpolation (otherwise, it snaps and don't animate).
        const auto handleActive =
          dialOpt->state.testFlag(State_Sunken) && dialOpt->activeSubControls == SubControl::SC_DialHandle;
        const auto duration = handleActive ? _impl->theme.sliderAnimationDuration : _impl->theme.animationDuration;
        const auto currentProgress = _impl->animations.animateProgress(w, progress, duration);

        // Dial shape.
        const auto dialRect = subControlRect(cc, opt, SC_DialGroove, w);
        const auto& bgColor = dialBackgroundColor(mouse);
        const auto& handleColor = dialHandleColor(mouse);
        const auto& grooveColor = dialGrooveColor(mouse);
        const auto& valueColor = dialValueColor(mouse);
        const auto& markColor = dialMarkColor(mouse);
        const auto& currentHandleColor =
          _impl->animations.animateBackgroundColor(w, handleColor, _impl->theme.animationDuration);

        drawDial(p, dialRect, dialOpt->minimum, dialOpt->maximum, currentProgress, bgColor, currentHandleColor,
          grooveColor, valueColor, markColor, _impl->theme.dialGrooveThickness, _impl->theme.dialMarkLength,
          _impl->theme.dialMarkThickness);
      }
      return;
    case CC_GroupBox:
      if (const auto* groupBoxOpt = qstyleoption_cast<const QStyleOptionGroupBox*>(opt)) {
        // Checkbox
        if (groupBoxOpt->subControls.testFlag(SC_GroupBoxCheckBox)) {
          const auto checkBoxRect = subControlRect(CC_GroupBox, opt, SC_GroupBoxCheckBox, w);
          QStyleOptionButton checkBoxOpt;
          checkBoxOpt.QStyleOption::operator=(*groupBoxOpt);
          checkBoxOpt.rect = checkBoxRect;
          drawPrimitive(PE_IndicatorCheckBox, &checkBoxOpt, p, w);
        }

        // Title
        if (groupBoxOpt->subControls.testFlag(SC_GroupBoxLabel)) {
          const auto textRect = subControlRect(CC_GroupBox, opt, SC_GroupBoxLabel, w);
          const auto& font = _impl->theme.fontH5;
          const auto fm = QFontMetrics(font);
          const auto elidedText =
            fm.elidedText(groupBoxOpt->text, Qt::ElideRight, textRect.width(), Qt::TextSingleLine);
          const auto mouse = getMouseState(groupBoxOpt->state);
          const auto& textColor = groupBoxTitleColor(mouse, w);
          constexpr auto textFlags =
            Qt::AlignVCenter | Qt::AlignBaseline | Qt::TextSingleLine | Qt::AlignLeft | Qt::TextHideMnemonic;
          p->setFont(font);
          p->setPen(textColor);
          p->setRenderHint(QPainter::Antialiasing, true);
          p->drawText(textRect, textFlags, elidedText);
        }

        // Frame
        const auto hasFrame = !groupBoxOpt->features.testFlag(QStyleOptionFrame::Flat);
        if (hasFrame) {
          const auto frameRect = subControlRect(CC_GroupBox, opt, SC_GroupBoxFrame, w);
          QStyleOptionFrame frameOpt;
          frameOpt.QStyleOption::operator=(*groupBoxOpt);
          frameOpt.features = groupBoxOpt->features;
          frameOpt.state = groupBoxOpt->state;
          frameOpt.rect = frameRect;

          // If the groupbox is disabled (because its parent is disabled), or if it is not checked,
          // tweak the state to reflect that.
          const auto checked = qlementine::getCheckState(groupBoxOpt->state);
          if (checked == CheckState::NotChecked && groupBoxOpt->subControls.testFlag(SC_GroupBoxCheckBox)) {
            frameOpt.state.setFlag(QStyle::State_Enabled, false);
          }
          drawPrimitive(PE_FrameGroupBox, &frameOpt, p, w);
        }
      }
      return;
    case CC_MdiControls:
      break;
    default:
      break;
  }

  QCommonStyle::drawComplexControl(cc, opt, p, w);
}

QStyle::SubControl QlementineStyle::hitTestComplexControl(
  ComplexControl cc, const QStyleOptionComplex* opt, const QPoint& pos, const QWidget* w) const {
  switch (cc) {
    case CC_SpinBox:
      if (const auto* optSpinBox = qstyleoption_cast<const QStyleOptionSpinBox*>(opt)) {
        if (optSpinBox->buttonSymbols != QAbstractSpinBox::NoButtons) {
          if (optSpinBox->subControls.testFlag(SC_SpinBoxUp)) {
            const auto upButtonRect = subControlRect(cc, opt, SC_SpinBoxUp, w);
            if (upButtonRect.contains(pos))
              return SC_SpinBoxUp;
          }

          if (optSpinBox->subControls.testFlag(SC_SpinBoxDown)) {
            const auto downButtonRect = subControlRect(cc, opt, SC_SpinBoxDown, w);
            if (downButtonRect.contains(pos))
              return SC_SpinBoxDown;
          }
        }

        const auto editFieldRect = subControlRect(cc, opt, SC_SpinBoxEditField, w);
        if (editFieldRect.contains(pos))
          return SC_SpinBoxEditField;

        const auto frameRect = subControlRect(cc, opt, SC_SpinBoxFrame, w);
        if (frameRect.contains(pos))
          return SC_SpinBoxFrame;
      }
      return SC_None;
    case CC_ComboBox:
      if (qstyleoption_cast<const QStyleOptionComboBox*>(opt)) {
        // NB: Don't hit-test for the popup (SC_ComboBoxListBoxPopup) because
        // it's useless and would potentially block some clicks.

        const auto editFieldRect = subControlRect(cc, opt, SC_ComboBoxEditField, w);
        if (editFieldRect.isValid() && editFieldRect.contains(pos)) {
          return SC_ComboBoxEditField;
        }

        const auto arrowRect = subControlRect(cc, opt, SC_ComboBoxArrow, w);
        if (arrowRect.isValid() && arrowRect.contains(pos)) {
          return SC_ComboBoxArrow;
        }

        const auto frameRect = subControlRect(cc, opt, SC_ComboBoxFrame, w);
        if (frameRect.isValid() && frameRect.contains(pos)) {
          return SC_ComboBoxFrame;
        }
      }
      return SC_None;
    case CC_ScrollBar:
      if (qstyleoption_cast<const QStyleOptionSlider*>(opt)) {
        // Keep the scrollbar handle testing at the top so the handle has priority.
        const auto sliderRect = subControlRect(cc, opt, SC_ScrollBarSlider, w);
        if (sliderRect.isValid() && sliderRect.contains(pos)) {
          return SC_ScrollBarSlider;
        }

        const auto addLineRect = subControlRect(cc, opt, SC_ScrollBarAddLine, w);
        if (addLineRect.isValid() && addLineRect.contains(pos)) {
          return SC_ScrollBarAddLine;
        }
        const auto subLineRect = subControlRect(cc, opt, SC_ScrollBarSubLine, w);
        if (subLineRect.isValid() && subLineRect.contains(pos)) {
          return SC_ScrollBarSubLine;
        }

        const auto addPageRect = subControlRect(cc, opt, SC_ScrollBarAddPage, w);
        if (addPageRect.isValid() && addPageRect.contains(pos)) {
          return SC_ScrollBarAddPage;
        }

        const auto subPageRect = subControlRect(cc, opt, SC_ScrollBarSubPage, w);
        if (subPageRect.isValid() && subPageRect.contains(pos)) {
          return SC_ScrollBarSubPage;
        }

        const auto firstRect = subControlRect(cc, opt, SC_ScrollBarFirst, w);
        if (firstRect.isValid() && firstRect.contains(pos)) {
          return SC_ScrollBarFirst;
        }

        const auto lastRect = subControlRect(cc, opt, SC_ScrollBarLast, w);
        if (lastRect.isValid() && lastRect.contains(pos)) {
          return SC_ScrollBarLast;
        }

        const auto grooveRect = subControlRect(cc, opt, SC_ScrollBarGroove, w);
        if (grooveRect.isValid() && grooveRect.contains(pos)) {
          return SC_ScrollBarGroove;
        }
      }
      return SC_None;
    case CC_Slider:
      if (const auto* optSlider = qstyleoption_cast<const QStyleOptionSlider*>(opt)) {
        const auto handleRect = subControlRect(cc, optSlider, SC_SliderHandle, w);
        if (handleRect.isValid() && handleRect.contains(pos)) {
          return SC_SliderHandle;
        }

        const auto grooveRect = subControlRect(cc, optSlider, SC_SliderGroove, w);
        const auto& clickRect = !handleRect.isValid()
                                  ? grooveRect
                                  : QRect{ grooveRect.x(), handleRect.y(), grooveRect.width(), handleRect.height() };
        if (clickRect.isValid() && clickRect.contains(pos)) {
          return SC_SliderGroove;
        }
      }
      return SC_None;
    case CC_ToolButton:
      if (qstyleoption_cast<const QStyleOptionToolButton*>(opt)) {
        const auto buttonRect = subControlRect(cc, opt, SC_ToolButton, w);
        if (buttonRect.isValid() && buttonRect.contains(pos)) {
          return SC_ToolButton;
        }

        const auto menuButtonRect = subControlRect(cc, opt, SC_ToolButtonMenu, w);
        if (menuButtonRect.isValid() && menuButtonRect.contains(pos)) {
          return SC_ToolButtonMenu;
        }
      }
      return SC_None;
    case CC_Dial:
      if (qstyleoption_cast<const QStyleOptionSlider*>(opt)) {
        const auto handleRect = subControlRect(cc, opt, SC_DialHandle, w);
        if (handleRect.isValid() && handleRect.contains(pos)) {
          return SC_DialHandle;
        }

        const auto grooveRect = subControlRect(cc, opt, SC_DialGroove, w);
        if (grooveRect.isValid() && grooveRect.contains(pos)) {
          return SC_DialGroove;
        }

        const auto tickMarksRect = subControlRect(cc, opt, SC_DialTickmarks, w);
        if (tickMarksRect.isValid() && tickMarksRect.contains(pos)) {
          return SC_DialTickmarks;
        }
      }
      return SC_None;
    case CC_GroupBox:
      if (qstyleoption_cast<const QStyleOptionGroupBox*>(opt)) {
        // Here we cheat to avoid having a gap between the CheckBox and the Label where it
        // doesn't hit any SubControl. We want it to always hit the CheckBox.
        const auto checkBoxRect = subControlRect(cc, opt, SC_GroupBoxCheckBox, w);
        const auto labelRect = subControlRect(cc, opt, SC_GroupBoxLabel, w);
        const auto titleRect = checkBoxRect.united(labelRect);
        if (titleRect.isValid() && titleRect.contains(pos)) {
          return SC_GroupBoxCheckBox;
        }

        const auto contentsRect = subControlRect(cc, opt, SC_GroupBoxContents, w);
        if (contentsRect.isValid() && contentsRect.contains(pos)) {
          return SC_GroupBoxContents;
        }

        const auto frameRect = subControlRect(cc, opt, SC_GroupBoxFrame, w);
        if (frameRect.isValid() && frameRect.contains(pos)) {
          return SC_GroupBoxFrame;
        }
      }
      return SC_None;
    case CC_TitleBar:
      // TODO
      break;
    case CC_MdiControls:
      // TODO
      break;
    default:
      break;
  }

  return QCommonStyle::hitTestComplexControl(cc, opt, pos, w);
}

QRect QlementineStyle::subControlRect(
  ComplexControl cc, const QStyleOptionComplex* opt, SubControl sc, const QWidget* w) const {
  switch (cc) {
    case CC_SpinBox:
      if (const auto* spinboxOpt = qstyleoption_cast<const QStyleOptionSpinBox*>(opt)) {
        switch (sc) {
          case SC_SpinBoxUp:
            if (spinboxOpt->buttonSymbols != QAbstractSpinBox::NoButtons) {
              const auto iconDimension = pixelMetric(PM_ButtonIconSize);
              const auto buttonW = iconDimension + 2 * _impl->theme.borderWidth;
              const auto& totalRect = spinboxOpt->rect;
              const auto buttonH = totalRect.height() / 2;
              const auto buttonX = totalRect.right() - buttonW;
              const auto buttonY = totalRect.top();
              return QRect{ buttonX, buttonY, buttonW, buttonH };
            } else {
              return {};
            }
            break;
          case SC_SpinBoxDown:
            if (spinboxOpt->buttonSymbols != QAbstractSpinBox::NoButtons) {
              const auto iconDimension = pixelMetric(PM_ButtonIconSize);
              const auto buttonW = iconDimension + 2 * _impl->theme.borderWidth;
              const auto& totalRect = spinboxOpt->rect;
              const auto buttonH = totalRect.height() / 2;
              const auto buttonX = totalRect.right() - buttonW;
              const auto buttonY = totalRect.bottom() + 1 - buttonH; // cf. Qt documentation.
              return QRect{ buttonX, buttonY, buttonW, buttonH };
            } else {
              return {};
            }
            break;
          case SC_SpinBoxEditField:
            if (spinboxOpt->buttonSymbols != QAbstractSpinBox::NoButtons) {
              const auto iconDimension = pixelMetric(PM_ButtonIconSize);
              const auto buttonW = iconDimension + 2 * _impl->theme.borderWidth + 1;
              const auto& totalRect = spinboxOpt->rect;
              return QRect{ totalRect.x(), totalRect.y(), totalRect.width() - buttonW, totalRect.height() };
            } else {
              return spinboxOpt->rect;
            }
            break;
          case SC_SpinBoxFrame:
            return opt->rect;
            break;
          default:
            break;
        }
      }
      return {};
    case CC_ComboBox:
      if (const auto* comboBoxOpt = qstyleoption_cast<const QStyleOptionComboBox*>(opt)) {
        switch (sc) {
          case SC_ComboBoxArrow: {
            // Not only the rect for the arrow icon, but the rect for the whole clickable zone,
            // in which the arrow will be drawn at the center.
            const auto indicatorSize = _impl->theme.iconSize;
            const auto hPadding = _impl->theme.spacing;
            const auto buttonW = indicatorSize.width() + hPadding * 2;
            const auto buttonH = comboBoxOpt->rect.height();
            const auto buttonX = comboBoxOpt->rect.x() + comboBoxOpt->rect.width() - buttonW;
            const auto buttonY = comboBoxOpt->rect.y();
            return QRect{ buttonX, buttonY, buttonW, buttonH };
          } break;
          case SC_ComboBoxEditField: {
            if (comboBoxOpt->editable) {
              const auto hasIcon = !comboBoxOpt->currentIcon.isNull();
              const auto indicatorSize = _impl->theme.iconSize;
              const auto spacing = _impl->theme.spacing;
              const auto shiftX = hasIcon ? static_cast<int>(spacing * 2.5) : 0;
              const auto indicatorButtonW = spacing * 2 + indicatorSize.width();
              const auto editFieldW = comboBoxOpt->rect.width() - indicatorButtonW + shiftX;
              return QRect{ comboBoxOpt->rect.x() - shiftX, comboBoxOpt->rect.y(), editFieldW,
                comboBoxOpt->rect.height() };
            } else {
              return QRect{};
            }
          } break;
          case SC_ComboBoxFrame: {
            const auto frameH = _impl->theme.controlHeightLarge;
            const auto frameW = comboBoxOpt->rect.width();
            const auto frameX = comboBoxOpt->rect.x();
            const auto frameY = comboBoxOpt->rect.y() + (comboBoxOpt->rect.height() - frameH) / 2;
            return QRect{ frameX, frameY, frameW, frameH };
          } break;
          case SC_ComboBoxListBoxPopup: {
            const auto contentMarginH = pixelMetric(PM_MenuHMargin);
            const auto contentMarginV = pixelMetric(PM_MenuVMargin);
            const auto shadowWidth = _impl->theme.spacing;
            const auto borderWidth = _impl->theme.borderWidth;
            const auto width = std::max(opt->rect.width(), w->width());
            const auto height = opt->rect.height() + 12; // Not possible to change height here.
            const auto x = opt->rect.x() - shadowWidth - borderWidth - contentMarginH;
            const auto y = opt->rect.y() - shadowWidth - borderWidth - contentMarginV / 2; // TODO remove hardcoded
            return { x, y, width, height };
          } break;
          default:
            break;
        }
      }
      return {};
    case CC_ScrollBar:
      if (const auto* scrollBarOpt = qstyleoption_cast<const QStyleOptionSlider*>(opt)) {
        const auto horizontal = scrollBarOpt->orientation == Qt::Horizontal;
        const auto& rect = scrollBarOpt->rect;

        switch (sc) {
          case SC_ScrollBarAddPage: {
            const auto totalLength = horizontal ? rect.width() : rect.height();
            const auto handleCenter = sliderPositionFromValue(scrollBarOpt->minimum, scrollBarOpt->maximum,
              scrollBarOpt->sliderPosition, totalLength, scrollBarOpt->upsideDown);
            return horizontal ? QRect(rect.x(), rect.y(), handleCenter, rect.height())
                              : QRect(rect.x(), rect.y(), rect.width(), handleCenter);
          } break;
          case SC_ScrollBarSubPage: {
            const auto totalLength = horizontal ? rect.width() : rect.height();
            const auto handleCenter = sliderPositionFromValue(scrollBarOpt->minimum, scrollBarOpt->maximum,
              scrollBarOpt->sliderPosition, totalLength, scrollBarOpt->upsideDown);
            return horizontal ? QRect(rect.x() + handleCenter, rect.y(), rect.width() - handleCenter, rect.height())
                              : QRect(rect.x(), rect.y() + handleCenter, rect.width(), rect.height() - handleCenter);
          } break;
          case SC_ScrollBarSlider: {
            // Compute slider length.
            if (scrollBarOpt->maximum != scrollBarOpt->minimum) {
              const auto range = scrollBarOpt->maximum - scrollBarOpt->minimum;
              const auto margin = _impl->theme.scrollBarMargin;
              auto maxLength = horizontal ? rect.width() - 2 * margin : rect.height() - 2 * margin;
              auto minLength = pixelMetric(PM_ScrollBarSliderMin, scrollBarOpt, w);
              if (minLength > maxLength) {
                std::swap(maxLength, minLength);
              }
              const auto length = std::max(
                0., static_cast<double>((scrollBarOpt->pageStep * maxLength)) / (range + scrollBarOpt->pageStep));
              const auto handleLength = std::clamp(static_cast<int>(length), minLength, maxLength);
              const auto handleStart = sliderPositionFromValue(scrollBarOpt->minimum, scrollBarOpt->maximum,
                scrollBarOpt->sliderPosition, maxLength - handleLength, scrollBarOpt->upsideDown);
              return horizontal ? QRect(rect.x() + margin + handleStart, rect.y(), handleLength, rect.height() - margin)
                                : QRect(rect.x(), rect.y() + margin + handleStart, rect.width() - margin, handleLength);
            } else {
              return rect;
            }
          } break;
          case SC_ScrollBarGroove: {
            const auto margin = _impl->theme.scrollBarMargin;
            return horizontal ? QRect(rect.x() + margin, rect.y(), rect.width() - 2 * margin, rect.height() - margin)
                              : QRect(rect.x(), rect.y() + margin, rect.width() - margin, rect.height() - 2 * margin);
          } break;
          case SC_ScrollBarAddLine:
          case SC_ScrollBarSubLine:
          case SC_ScrollBarFirst:
          case SC_ScrollBarLast:
          default:
            // Not handled by this QStyle.
            return {};
        }
      }
      return {};
    case CC_Slider:
      if (const auto* sliderOpt = qstyleoption_cast<const QStyleOptionSlider*>(opt)) {
        switch (sc) {
          case SC_SliderGroove: {
            const auto grooveW = opt->rect.width();
            const auto grooveH = _impl->theme.sliderGrooveHeight;
            const auto grooveX = opt->rect.x();
            const auto grooveY = opt->rect.y() + (opt->rect.height() - grooveH) / 2;
            return QRect{ grooveX, grooveY, grooveW, grooveH };
          } break;
          case SC_SliderHandle: {
            return QlementineStyleImpl::sliderHandleRect(
              *this, *sliderOpt, static_cast<qreal>(sliderOpt->sliderPosition), w);
          } break;
          case SC_SliderTickmarks:
            switch (sliderOpt->tickPosition) {
              case QSlider::TickPosition::TicksAbove: {
                const auto grooveRect = subControlRect(cc, opt, SubControl::SC_SliderGroove, w);
                const auto handleThickness = pixelMetric(PM_SliderLength);
                const auto tickOffset = pixelMetric(PM_SliderTickmarkOffset);
                const auto tickMarksX = opt->rect.x() + handleThickness / 2;
                const auto tickMarksH = _impl->theme.sliderTickSize;
                const auto tickMarksY = grooveRect.top() - tickOffset - tickMarksH;
                const auto tickMarksW = grooveRect.width() - handleThickness;
                return QRect{ tickMarksX, tickMarksY, tickMarksW, tickMarksH };
              } break;
              // TODO other tick positions.
              default:
                break;
            }
            break;
          default:
            return {};
        }
      }
      return {};
    case CC_ToolButton:
      if (const auto* toolButtonOpt = qstyleoption_cast<const QStyleOptionToolButton*>(opt)) {
        const auto& rect = toolButtonOpt->rect;

        const auto hasMenu = toolButtonOpt->features.testFlag(QStyleOptionToolButton::HasMenu);
        const auto menuIsOnSeparateButton =
          toolButtonOpt->features.testFlag(QStyleOptionToolButton::ToolButtonFeature::MenuButtonPopup);

        const auto& iconSize = toolButtonOpt->iconSize;
        const auto separatorW = _impl->theme.borderWidth;
        const auto spacing = _impl->theme.spacing;
        const auto menuButtonW =
          hasMenu ? (menuIsOnSeparateButton ? separatorW + iconSize.width() + spacing / 2 : iconSize.width()) : 0;
        const auto buttonW = rect.width() - menuButtonW;
        switch (sc) {
          case SC_ToolButton:
            return QRect{ rect.x(), rect.y(), buttonW, rect.height() };
          case SC_ToolButtonMenu:
            return QRect{ rect.x() + rect.width() - menuButtonW, rect.y(), menuButtonW, rect.height() };
          default:
            return {};
        }
      }
      return {};
    case CC_TitleBar:
      if (qstyleoption_cast<const QStyleOptionTitleBar*>(opt)) {
        switch (sc) {
          case SC_TitleBarSysMenu:
            return {};
          case SC_TitleBarMinButton:
            return {};
          case SC_TitleBarMaxButton:
            return {};
          case SC_TitleBarCloseButton:
            return {};
          case SC_TitleBarNormalButton:
            return {};
          case SC_TitleBarShadeButton:
            return {};
          case SC_TitleBarUnshadeButton:
            return {};
          case SC_TitleBarContextHelpButton:
            return {};
          case SC_TitleBarLabel:
            return {};
          default:
            return {};
        }
      }
      return {};
    case CC_Dial:
      if (const auto* dialOpt = qstyleoption_cast<const QStyleOptionSlider*>(opt)) {
        const auto& totalRect = dialOpt->rect;
        const auto hasTicks = dialOpt->subControls.testFlag(SC_DialTickmarks);
        switch (sc) {
          case SC_DialHandle:
          case SC_DialGroove: {
            const auto tickSpace = hasTicks ? _impl->theme.dialTickLength + _impl->theme.dialTickSpacing : 0;
            const auto minDimension = std::max(0, std::min(totalRect.width(), totalRect.height()) - tickSpace * 2);
            const auto dialX = totalRect.x() + (totalRect.width() - minDimension) / 2;
            const auto dialY = totalRect.y() + (totalRect.height() - minDimension) / 2;
            return QRect{ dialX, dialY, minDimension, minDimension };
          } break;
          case SC_DialTickmarks: {
            if (!hasTicks)
              return QRect{};

            const auto minDimension = std::max(0, std::min(totalRect.width(), totalRect.height()));
            const auto ticksX = totalRect.x() + (totalRect.width() - minDimension) / 2;
            const auto ticksY = totalRect.y() + (totalRect.height() - minDimension) / 2;
            return QRect{ ticksX, ticksY, minDimension, minDimension };
          } break;
          default:
            return {};
        }
      }
      return {};
    case CC_GroupBox:
      if (const auto* groupBoxOpt = qstyleoption_cast<const QStyleOptionGroupBox*>(opt)) {
        const auto& rect = groupBoxOpt->rect;
        const auto hasTitle = groupBoxOpt->subControls.testFlag(SC_GroupBoxLabel);
        const auto hasCheckbox = groupBoxOpt->subControls.testFlag(SC_GroupBoxCheckBox);
        const auto hasFrame = !groupBoxOpt->features.testFlag(QStyleOptionFrame::Flat);
        const auto labelH =
          hasTitle ? std::max(_impl->theme.controlHeightMedium, QFontMetrics(_impl->theme.fontH5).height()) : 0;
        const auto titleBottomSpacing = hasFrame && (hasTitle || hasCheckbox) ? _impl->theme.spacing / 2 : 0;
        const auto& checkBoxSize = hasCheckbox ? _impl->theme.iconSize : QSize{ 0, 0 };
        const auto titleH = hasTitle || hasCheckbox ? std::max(labelH, checkBoxSize.height()) : 0;
        const auto leftPadding = hasTitle || hasCheckbox ? _impl->theme.spacing : 0;

        switch (sc) {
            // TODO handle other kinds of Qt::Alignment like right-aligned or centered.
          case SC_GroupBoxCheckBox:
            if (groupBoxOpt->subControls.testFlag(SC_GroupBoxCheckBox)) {
              const auto x = rect.x();
              const auto y = rect.y() + (titleH - checkBoxSize.height()) / 2;
              return QRect{ QPoint{ x, y }, checkBoxSize };
            }
            return {};
          case SC_GroupBoxLabel:
            // TODO handle other kinds of Qt::Alignment like right-aligned or centered.
            if (groupBoxOpt->subControls.testFlag(SC_GroupBoxLabel)) {
              const auto spacing = hasCheckbox ? _impl->theme.spacing : 0;
              const auto x = rect.x() + checkBoxSize.width() + spacing;
              const auto y = rect.y();
              const auto labelW = rect.width() - checkBoxSize.width() - spacing;
              return QRect{ x, y, labelW, titleH };
            }
            return {};
          case SC_GroupBoxContents:
            /*if (groupBoxOpt->subControls.testFlag(SC_GroupBoxContents))*/ {
              const auto x = rect.x() + leftPadding;
              const auto y = rect.y() + titleH + titleBottomSpacing;
              const auto width = rect.width() - leftPadding;
              const auto height = rect.height() - titleH - titleBottomSpacing;
              return QRect{ x, y, width, height };
            }
            //return {};
          case SC_GroupBoxFrame:
            /*if (groupBoxOpt->subControls.testFlag(SC_GroupBoxFrame))*/ {
              const auto x = rect.x() + leftPadding;
              const auto y = rect.y() + titleH + titleBottomSpacing;
              const auto width = rect.width() - leftPadding;
              const auto height = rect.height() - titleH - titleBottomSpacing;
              return QRect{ x, y, width, height };
            }
            //return {};
          default:
            break;
        }
      }
      return {};
    case CC_MdiControls:
      switch (sc) {
        case SC_MdiMinButton:
          return {};
        case SC_MdiNormalButton:
          return {};
        case SC_MdiCloseButton:
          return {};
        default:
          break;
      }
      break;
    default:
      break;
  }

  return QCommonStyle::subControlRect(cc, opt, sc, w);
}

QSize QlementineStyle::sizeFromContents(
  ContentsType ct, const QStyleOption* opt, const QSize& contentSize, const QWidget* widget) const {
  switch (ct) {
    case CT_PushButton:
      if (const auto* optButton = buttonStyleOption(opt)) {
        const auto hasIcon = !optButton->icon.isNull();
        const auto hasText = !optButton->text.isEmpty();
        const auto hasMenu = optButton->features.testFlag(QStyleOptionButton::HasMenu);

        auto contentWidth = 0;
        if (hasText) {
          contentWidth += qlementine::textWidth(optButton->fontMetrics, optButton->text);
        }
        if (hasIcon) {
          contentWidth += optButton->iconSize.width();
          if (hasText) {
            contentWidth += _impl->theme.spacing;
          }
        }
        if (hasMenu) {
          contentWidth += optButton->iconSize.width();
          contentWidth += _impl->theme.spacing;
        }

        const auto maxSize = widget->maximumSize();
        const auto maxW = maxSize.width();
        const auto maxH = maxSize.height();
        const auto padding = pixelMetric(PM_ButtonMargin, opt, widget);
        auto [paddingLeft, paddingRight] = getHPaddings(hasIcon, hasText, hasMenu, padding);

        // Keep visual balance for text+icon QPushButton without menu indicator.
        if (hasText && hasIcon && !hasMenu) {
          paddingLeft = padding * 2;
          paddingRight = padding * 2;
        }

        const auto defaultH = _impl->theme.controlHeightLarge;
        auto w = std::max(defaultH, contentWidth + paddingLeft + paddingRight);
        if (maxW != QWIDGETSIZE_MAX && maxW > -1) {
          w = std::min(w, maxW);
        }
        auto h = std::max(defaultH, contentSize.height() + padding);
        if (maxH != QWIDGETSIZE_MAX && maxH > -1) {
          h = std::min(h, maxH);
        }
        return QSize{ w, h };
      }
      break;
    case CT_CheckBox:
    case CT_RadioButton:
      if (const auto* optButton = buttonStyleOption(opt)) {
        QSize actualContentSize(contentSize);

        if (!optButton->icon.isNull()) {
          // QCheckBox adds an hardcoded spacing if there is an icon.
          actualContentSize.rwidth() -= hardcodedButtonSpacing;

          // Add our own spacing only if there is both icon and text.
          if (optButton->text.isEmpty()) {
            actualContentSize.rwidth() = 0;
          } else {
            actualContentSize.rwidth() += _impl->theme.spacing;
          }
        }

        // Add space for the indicator.
        const auto indicatorSize = pixelMetric(PM_IndicatorWidth, opt, widget);
        const auto indicatorSpacing = pixelMetric(PM_CheckBoxLabelSpacing, opt, widget);
        actualContentSize.rwidth() += indicatorSize + indicatorSpacing;
        actualContentSize.rheight() = std::max(actualContentSize.height(), indicatorSize);

        const auto verticalMargin = pixelMetric(PM_ButtonMargin, opt, widget) / 2;
        const auto w = actualContentSize.width();
        const auto h = std::max(_impl->theme.controlHeightMedium, actualContentSize.height() + verticalMargin);
        return QSize{ w, h };
      }
      break;
    case CT_ToolButton:
      if (const auto* optToolButton = qstyleoption_cast<const QStyleOptionToolButton*>(opt)) {
        const auto spacing = _impl->theme.spacing;
        const auto& iconSize = optToolButton->iconSize;

        // Special cases.
        if (widget->inherits("QLineEditIconButton")) {
          return _impl->theme.iconSize;
        } else if (widget->inherits("QMenuBarExtension")) {
          const auto extent = pixelMetric(PM_ToolBarExtensionExtent);
          return QSize{ extent, extent };
        } else if (const auto* parentTabBar = qobject_cast<QTabBar*>(widget->parentWidget())) {
          const auto w = _impl->theme.controlHeightMedium + static_cast<int>(spacing * 1.5);
          const auto h = _impl->theme.controlHeightLarge + spacing;
          return QlementineStyleImpl::isTabShapeVertical(parentTabBar->shape()) ? QSize{ h, w } : QSize{ w, h };
        }

        const auto buttonStyle = optToolButton->toolButtonStyle;
        const auto hasMenu = optToolButton->features.testFlag(QStyleOptionToolButton::ToolButtonFeature::HasMenu);
        const auto menuIsOnSeparateButton =
          hasMenu && optToolButton->features.testFlag(QStyleOptionToolButton::ToolButtonFeature::MenuButtonPopup);

        const auto separatorW = menuIsOnSeparateButton ? _impl->theme.borderWidth : 0;
        const auto menuIndicatorW = hasMenu ? separatorW + iconSize.width() + spacing / 2 : 0;

        switch (buttonStyle) {
          case Qt::ToolButtonStyle::ToolButtonTextOnly: {
            const auto textW =
              optToolButton->fontMetrics.boundingRect(optToolButton->rect, Qt::AlignCenter, optToolButton->text)
                .width();
            const auto leftPadding = spacing * 2;
            const auto rightPadding = hasMenu ? spacing : spacing * 2;
            const auto w = leftPadding + textW + rightPadding + menuIndicatorW;
            const auto h = iconSize.height() < _impl->theme.controlHeightLarge
                             ? _impl->theme.controlHeightLarge
                             : iconSize.height() + _impl->theme.spacing;
            return QSize{ w, h };
          }
          case Qt::ToolButtonStyle::ToolButtonIconOnly: {
            const auto w = iconSize.width() + spacing * 2 + menuIndicatorW;
            const auto h = iconSize.height() < _impl->theme.controlHeightLarge
                             ? _impl->theme.controlHeightLarge
                             : iconSize.height() + _impl->theme.spacing;
            return QSize{ w, h };
          }
          case Qt::ToolButtonStyle::ToolButtonTextUnderIcon: {
            constexpr auto maxTextW = 150;
            const auto textW = qMin(maxTextW,
              optToolButton->fontMetrics.boundingRect(optToolButton->rect, Qt::AlignCenter, optToolButton->text)
                .width());
            const auto textH = optToolButton->fontMetrics.height();
            const auto hPadding = _impl->theme.spacing;
            const auto vPadding = _impl->theme.spacing;
            const auto w = hPadding * 2 + qMax(iconSize.width(), textW) + menuIndicatorW;
            const auto h = vPadding * 2 + iconSize.height() + spacing + textH;
            return QSize{ w, h };
          }
          case Qt::ToolButtonStyle::ToolButtonTextBesideIcon: {
            const auto iconW = iconSize.width();
            const auto textW =
              optToolButton->fontMetrics.boundingRect(optToolButton->rect, Qt::AlignCenter, optToolButton->text)
                .width();
            const auto leftPadding = spacing;
            const auto rightPadding = hasMenu ? spacing : spacing * 2;
            const auto w = leftPadding + iconW + spacing + textW + rightPadding + menuIndicatorW;
            const auto h = iconSize.height() < _impl->theme.controlHeightLarge
                             ? _impl->theme.controlHeightLarge
                             : iconSize.height() + _impl->theme.spacing;
            return QSize{ w, h };
          }
          default:
            return QSize{};
        }
      }
      break;
    case CT_ComboBox:
      if (const auto* optComboBox = qstyleoption_cast<const QStyleOptionComboBox*>(opt)) {
        // Check if the ComboBox is inside a QTableView/QTreeView.
        const auto* parentWidget = widget ? widget->parentWidget() : nullptr;
        const auto* parentParentWidget = parentWidget ? parentWidget->parentWidget() : nullptr;
        const auto isTabCellEditor = qobject_cast<const QAbstractItemView*>(parentParentWidget) != nullptr;

        const auto h = _impl->theme.controlHeightLarge;
        auto w = isTabCellEditor ? optComboBox->rect.size().width() : contentSize.width();

        // Hack hardcoded values in Qt source code.
        if (!optComboBox->currentIcon.isNull()) {
          // QComboBox adds an hardcoded spacing if there is an icon.
          w -= hardcodedButtonSpacing;

          // Add our own spacing only if there is both icon and text.
          if (optComboBox->currentText.isEmpty()) {
            w = 0;
          } else {
            w += _impl->theme.spacing;
          }
        }

        // Add space to compute correct sizeHint.
        if (!isTabCellEditor) {
          // Add space for indicator (NB: this is the arrow on the right, not the icon).
          const auto indicatorSize = _impl->theme.iconSize;
          w += _impl->theme.spacing + indicatorSize.width();

          // Add space for padding.
          const auto framePadding = pixelMetric(PM_ComboBoxFrameWidth, optComboBox, widget);
          const auto horizontalMargin = pixelMetric(PM_ButtonMargin, opt, widget);
          w += horizontalMargin + framePadding * 2;
        }

        return QSize{ w, h };
      }
      break;
    case CT_Splitter:
      break;
    case CT_ProgressBar:
      if (const auto* optProgressBar = qstyleoption_cast<const QStyleOptionProgressBar*>(opt)) {
        const auto indeterminate = optProgressBar->maximum == 0 && optProgressBar->minimum == 0;
        const auto showText = !indeterminate && optProgressBar->textVisible;
        const auto& maximumText = indeterminate ? QString{} : QString("%1%").arg(optProgressBar->maximum);
        const auto labelW =
          showText ? optProgressBar->fontMetrics.boundingRect(optProgressBar->rect, Qt::AlignRight, maximumText).width()
                   : 0;
        const auto labelH = showText ? optProgressBar->fontMetrics.height() : 0;
        const auto spacing = _impl->theme.spacing;
        const auto barH = _impl->theme.progressBarGrooveHeight;
        const auto defaultH = _impl->theme.controlHeightMedium;
        const auto h = std::min(defaultH, std::max(labelH, barH));
        const auto w = _impl->theme.controlDefaultWidth + (showText ? spacing + labelW : 0);
        return QSize{ w, h };
      }
      break;
    case CT_MenuItem:
      if (const auto* optMenuItem = qstyleoption_cast<const QStyleOptionMenuItem*>(opt)) {
        if (optMenuItem->menuItemType == QStyleOptionMenuItem::Separator) {
          const auto h = _impl->theme.spacing + _impl->theme.borderWidth;
          return QSize{ h, h };

        } else if (optMenuItem->menuItemType == QStyleOptionMenuItem::Normal
                   || optMenuItem->menuItemType == QStyleOptionMenuItem::SubMenu) {
          const auto hPadding = _impl->theme.spacing;
          const auto vPadding = _impl->theme.spacing / 2;
          const auto iconSize = _impl->theme.iconSize;
          const auto spacing = _impl->theme.spacing;
          const auto& fm = optMenuItem->fontMetrics;
          const auto [label, shortcut] = getMenuLabelAndShortcut(optMenuItem->text);
          const auto labelW = fm.boundingRect(optMenuItem->rect, Qt::AlignLeft, label).width();

          // Submenu arrow.
          const auto hasArrow = optMenuItem->menuItemType == QStyleOptionMenuItem::SubMenu;
          const auto arrowW = hasArrow ? spacing + iconSize.width() : spacing;

          // Shortcut. NB: Some difficulties to understand what's going on. Qt changes the width so here's a hack.
          const auto hasShortcut = shortcut.length() > 0;
          const auto reservedShortcutW = optMenuItem->reservedShortcutWidth;
          const auto shortcutTextWidth = hasShortcut ? fm.boundingRect(shortcut).width() : 0;
          const auto shortcutW = std::max(reservedShortcutW, shortcutTextWidth);

          // Icon.
          const auto iconW =
            !QCoreApplication::testAttribute(Qt::AA_DontShowIconsInMenus) && optMenuItem->maxIconWidth > 0
              ? optMenuItem->maxIconWidth + spacing
              : 0;

          // Check or Radio.
          const auto hasCheck =
            optMenuItem->menuHasCheckableItems || optMenuItem->checkType != QStyleOptionMenuItem::NotCheckable;
          const auto checkW = hasCheck ? iconSize.width() + spacing : 0;

          const auto w = std::max(0, hPadding + checkW + iconW + labelW + shortcutW + arrowW + hPadding);
          const auto h = std::max(_impl->theme.controlHeightMedium, iconSize.height() + vPadding);
          return { w, h };
        }
        return QSize{};
      }
      break;
    case CT_MenuBarItem: {
      const auto hPadding = _impl->theme.spacing;
      const auto vPadding = _impl->theme.spacing / 2;
      const auto h = std::max(_impl->theme.iconSize.height() + _impl->theme.spacing, contentSize.height());
      auto s = contentSize.grownBy(QMargins(hPadding, vPadding, hPadding, vPadding));
      s.rheight() = h;
      return s;
    } break;
    case CT_MenuBar:
      return contentSize;
    case CT_Menu:
      return contentSize;
    case CT_TabBarTab:
      if (const auto* optTab = qstyleoption_cast<const QStyleOptionTab*>(opt)) {
        // TODO Handle expanding tab bar.
        // TODO Choose min/max values according to available space and total tab count.
        // Don't make tabs too long or too short.
        // The size is computed in the local frame of the tab, then transposed if the tab is vertical.
        const auto spacing = _impl->theme.spacing;
        const auto isVertical = QlementineStyleImpl::isTabShapeVertical(optTab->shape);
        const auto h = _impl->theme.controlHeightLarge + spacing;

        auto w = spacing * 2;

        // Button on the left.
        if (!optTab->leftButtonSize.isEmpty()) {
          w += (isVertical ? optTab->leftButtonSize.height() : optTab->leftButtonSize.width()) + spacing;
        }

        // Button on the right.
        if (!optTab->rightButtonSize.isEmpty()) {
          w += (isVertical ? optTab->rightButtonSize.height() : optTab->rightButtonSize.width()) + spacing;
        }

        // Icon.
        if (!optTab->icon.isNull() && !optTab->iconSize.isEmpty()) {
          w += optTab->iconSize.width() + spacing;
        }

        // Text.
        if (!optTab->text.isEmpty()) {
          w += qlementine::textWidth(optTab->fontMetrics, optTab->text);
        }

        // Clamp tab size.
        auto tabMaxWidth = _impl->theme.tabBarTabMaxWidth;
        auto tabMinWidth = _impl->theme.tabBarTabMinWidth;
        if (tabMinWidth > tabMaxWidth) {
          std::swap(tabMinWidth, tabMaxWidth);
        }
        if (tabMaxWidth > 0) {
          w = std::min(w, tabMaxWidth);
        }
        if (tabMinWidth > 0) {
          w = std::max(w, tabMinWidth);
        }

        const auto padding = _impl->tabExtraPadding(optTab, widget);
        w += padding.left() + padding.right() + _impl->tabContentInset(optTab->shape) * 2;

        return isVertical ? QSize{ h, w } : QSize{ w, h };
      }
      break;
    case CT_Slider:
      if (const auto* optSlider = qstyleoption_cast<const QStyleOptionSlider*>(opt)) {
        const auto& rect = optSlider->rect;
        return optSlider->orientation == Qt::Horizontal ? QSize{ rect.width(), _impl->theme.controlHeightMedium }
                                                        : QSize{ _impl->theme.controlHeightMedium, rect.height() };
      }
      break;
    case CT_ScrollBar:
      // TODO
      //return opt->rect.size();
      break;
    case CT_LineEdit:
      if (qstyleoption_cast<const QStyleOptionFrame*>(opt)) {
        // Use contentSize (font-metrics-based) rather than optFrame->rect (current widget
        // geometry) so that sizeHint/minimumSizeHint report a content-driven width instead
        // of echoing back the widget's existing size.
        const auto w = contentSize.width() - 2 * hardcodedLineEditHMargin;
        const auto h = _impl->theme.controlHeightLarge;
        const auto* parent = widget->parentWidget();
        const auto* treeView = parent ? qobject_cast<const QAbstractItemView*>(parent->parentWidget()) : nullptr;
        return treeView ? contentSize : QSize{ w, h };
      }
      break;
    case CT_SpinBox:
      if (const auto* optSpinbox = qstyleoption_cast<const QStyleOptionSpinBox*>(opt)) {
        const auto isDateTimeEdit = qobject_cast<const QDateTimeEdit*>(widget) != nullptr;
        const auto hasButtons = optSpinbox->buttonSymbols != QAbstractSpinBox::NoButtons;
        const auto buttonW = isDateTimeEdit || hasButtons ? _impl->theme.controlHeightLarge : 0;
        const auto dateTimeWidth = isDateTimeEdit ? _impl->theme.iconSize.width() : 0;
        const auto borderW = optSpinbox->frame ? pixelMetric(PM_SpinBoxFrameWidth, opt, widget) : 0;
        return QSize{ contentSize.width() + buttonW + dateTimeWidth + 2 * borderW, _impl->theme.controlHeightLarge };
      }
      break;
    case CT_SizeGrip:
      break;
    case CT_TabWidget:
      break;
    case CT_DialogButtons:
      break;
    case CT_HeaderSection:
      if (const auto* optHeader = qstyleoption_cast<const QStyleOptionHeader*>(opt)) {
        const auto spacing = _impl->theme.spacing;
        const auto headerIsSelected = true;
        auto font = QFont(widget->font());
        if (headerIsSelected) {
          font.setBold(true);
        }
        const auto lineW = _impl->theme.borderWidth;
        const auto iconExtent = pixelMetric(PM_SmallIconSize, opt);
        const auto fm = QFontMetrics(font);
        const auto textW = qlementine::textWidth(fm, optHeader->text);
        const auto& icon = optHeader->icon;
        const auto hasIcon = !icon.isNull();
        const auto iconW = hasIcon ? iconExtent + spacing : 0;
        const auto hasArrow = optHeader->sortIndicator != QStyleOptionHeader::SortIndicator::None;
        const auto arrowW = hasArrow ? iconExtent + spacing : 0;
        const auto paddingH = pixelMetric(PM_HeaderMargin);
        const auto paddingV = paddingH / 2;
        const auto textH = fm.height();
        const auto w = lineW + paddingH + iconW + textW + arrowW + paddingH + lineW;
        const auto h = lineW + paddingV + std::max(iconExtent, textH) + paddingV + lineW;
        return QSize{ w, h };
      }
      break;
    case CT_GroupBox:
      if (const auto* groupBoxOpt = qstyleoption_cast<const QStyleOptionGroupBox*>(opt)) {
        const auto hasTitle = groupBoxOpt->subControls.testFlag(SC_GroupBoxLabel);
        const auto hasCheckbox = groupBoxOpt->subControls.testFlag(SC_GroupBoxCheckBox);
        const auto hasFrame = !groupBoxOpt->features.testFlag(QStyleOptionFrame::Flat);
        const auto fm = QFontMetrics(_impl->theme.fontH5);
        const auto labelH =
          hasTitle ? std::max(_impl->theme.controlHeightMedium, QFontMetrics(_impl->theme.fontH5).height()) : 0;
        const auto labelW = fm.boundingRect(groupBoxOpt->rect, Qt::AlignLeft, groupBoxOpt->text).width();
        const auto& checkBoxSize = _impl->theme.iconSize;
        const auto titleBottomSpacing = hasFrame && (hasTitle || hasCheckbox) ? _impl->theme.spacing / 2 : 0;
        const auto titleH = hasTitle || hasCheckbox ? std::max(labelH, checkBoxSize.height()) : 0;
        const auto spacing = _impl->theme.spacing;
        const auto titleW = checkBoxSize.width() + spacing + labelW;
        const auto leftPadding = hasTitle || hasCheckbox ? _impl->theme.spacing : 0;
        const auto w = std::max(contentSize.width() + leftPadding, titleW);
        const auto h = titleH + titleBottomSpacing + contentSize.height();
        return QSize{ w, h };
      }
      break;
    case CT_MdiControls:
      break;
    case CT_ItemViewItem:
      if (const auto* optItem = qstyleoption_cast<const QStyleOptionViewItem*>(opt)) {
        const auto& features = optItem->features;
        const auto spacing = _impl->theme.spacing;
        const auto hPadding = spacing;

        const auto hasIcon = features.testFlag(QStyleOptionViewItem::HasDecoration) && !optItem->icon.isNull();
        const auto& iconSize = hasIcon ? optItem->decorationSize : QSize{ 0, 0 };

        const auto hasText = features.testFlag(QStyleOptionViewItem::HasDisplay) && !optItem->text.isEmpty();
        const auto textH = hasText ? optItem->fontMetrics.height() : 0;

        const auto hasCheck = features.testFlag(QStyleOptionViewItem::HasCheckIndicator);
        const auto& checkSize = hasCheck ? _impl->theme.iconSize : QSize{ 0, 0 };

        const auto* listView = qobject_cast<const QListView*>(widget);
        const auto isList = listView != nullptr;
        const auto iconMode = (isList && listView->viewMode() == QListView::IconMode);

        auto font = QFont(widget->font());
        const auto fm = QFontMetrics(font);
        const auto textW = qlementine::textWidth(fm, optItem->text);
        const auto defaultH = _impl->theme.controlHeightLarge;

        if (iconMode) {
          constexpr auto maxTextWInIconMode = 100;
          const auto maxContentW = std::max({
            std::min(textW, maxTextWInIconMode),
            iconSize.width(),
            checkSize.width(),
          });
          auto w = 2 * hPadding + maxContentW;
          const auto hasContentAboveCheck = hasIcon || hasText;
          auto h = std::max({
            iconSize.height() + (hasIcon && hasText ? spacing : 0) + textH
              + (hasCheck && hasContentAboveCheck ? spacing : 0) + checkSize.height(),
            defaultH,
          });

          const auto gridSize = listView ? listView->gridSize() : QSize{ 0, 0 };
          w = qMax(gridSize.width(), w);
          h = qMax(gridSize.height(), h);
          return QSize{ w, h };
        } else {
          const auto sideBySideIconWidth = (iconSize.width() > 0 ? iconSize.width() + spacing : 0);
          const auto sideBySideCheckWidth = (checkSize.width() > 0 ? checkSize.width() + spacing : 0);
          const auto w = textW + 2 * hPadding + sideBySideIconWidth + sideBySideCheckWidth;
          const auto h = std::max({
            iconSize.height() + spacing,
            textH + spacing,
            defaultH,
          });
          return QSize{ w, h };
        }
      }
      break;
    default:
      break;
  }
  return QCommonStyle::sizeFromContents(ct, opt, contentSize, widget);
}

int QlementineStyle::pixelMetric(PixelMetric m, const QStyleOption* opt, const QWidget* w) const {
  switch (m) {
    // Icons.
    case PM_SmallIconSize:
      return _impl->theme.iconSize.height();
    case PM_LargeIconSize:
      return _impl->theme.iconSizeLarge.height();

    // Button.
    case PM_ButtonMargin:
      return _impl->theme.spacing;
    case PM_ButtonDefaultIndicator:
      return _impl->theme.iconSize.width();
    case PM_MenuButtonIndicator:
      return _impl->theme.iconSize.width();
    case PM_ButtonShiftHorizontal:
    case PM_ButtonShiftVertical:
      return 0;
    case PM_ButtonIconSize:
      return _impl->theme.iconSize.height();

    // LineEdit.
    case PM_LineEditIconMargin:
      return _impl->theme.spacing;
    case PM_LineEditIconSize:
      return _impl->theme.iconSize.height();

    // Frame.
    case PM_DefaultFrameWidth:
      // Hack for QLineEdit. This is the only way to know if we have to draw a border or not.
      // See: https://github.com/qt/qtbase/blob/dev/src/widgets/widgets/qlineedit.cpp#L81C65-L81C85
      if (qobject_cast<const QLineEdit*>(w)) {
        return 1;
      }
      // Prevent QWidgets that contain or inherit QFrame to have a border.
      return 0;

    // ComboBox.
    case PM_ComboBoxFrameWidth:
      return _impl->theme.spacing;

    // SpinBox.
    case PM_SpinBoxFrameWidth:
      return _impl->theme.borderWidth;

    // Slider.
    case PM_SliderControlThickness:
      return _impl->theme.controlHeightMedium;
    case PM_SliderThickness:
      return _impl->theme.iconSize.height();
    case PM_SliderLength:
      return _impl->theme.iconSize.width();
    case PM_SliderTickmarkOffset:
      return _impl->theme.sliderTickSpacing;
    case PM_SliderSpaceAvailable:
      if (const auto* optSlider = qstyleoption_cast<const QStyleOptionSlider*>(opt)) {
        return optSlider->rect.width() - pixelMetric(PM_SliderLength, optSlider, w);
      }
      break;
    case PM_MaximumDragDistance:
      return -1;

    // DockWidgets.
    case PM_DockWidgetSeparatorExtent:
      break;
    case PM_DockWidgetHandleExtent:
      break;
    case PM_DockWidgetFrameWidth:
      break;

    // TabBar.
    case PM_TabBarTabOverlap:
      return static_cast<int>(_impl->theme.borderRadius);
    case PM_TabBarTabHSpace:
      return 0;
    case PM_TabBarTabVSpace:
      return 0;
    case PM_TabBarBaseHeight:
      return _impl->theme.controlHeightLarge + _impl->theme.spacing;
    case PM_TabBarBaseOverlap:
      return 0;
    case PM_TabBarTabShiftHorizontal:
      return 0;
    case PM_TabBarTabShiftVertical:
      return 0;
    case PM_TabBarScrollButtonWidth:
      return _impl->theme.controlHeightLarge + static_cast<int>(_impl->theme.spacing * 1.5);
    case PM_TabBar_ScrollButtonOverlap:
      return 0;
    case PM_TabBarIconSize:
      return _impl->theme.iconSize.height();
    case PM_TabCloseIndicatorWidth:
    case PM_TabCloseIndicatorHeight:
      return _impl->theme.controlHeightMedium;

    // ProgressBar.
    case PM_ProgressBarChunkWidth:
      return 0;

    // Splitter.
    case PM_SplitterWidth:
      return 1;
    // TitleBar.
    case PM_TitleBarHeight:
      break;
    case PM_TitleBarButtonIconSize:
      break;
    case PM_TitleBarButtonSize:
      break;

    // Menu.
    case PM_MenuScrollerHeight:
      // Scroller is the part where the user can click to scroll the menu when it is too big.
      return _impl->theme.controlHeightSmall;
    case PM_MenuHMargin:
    case PM_MenuVMargin: {
      // Keep some space between the items and the frame.
      const auto borderW = qobject_cast<const QMenu*>(w) ? 1 : 0;
      return _impl->theme.spacing / 2 + borderW;
    }
    case PM_MenuPanelWidth:
      // Keep some space for drop shadow.
      return blurRadiusNecessarySpace(_impl->theme.spacing);
    case PM_MenuTearoffHeight:
      // Tear off is the part of the menu that is clickable to detach the menu.
      return _impl->theme.controlHeightSmall;
    case PM_MenuDesktopFrameWidth:
      // TODO What is it?
      break;
    case PM_SubMenuOverlap:
      return 0;

    // MenuBar.
    case PM_MenuBarPanelWidth:
      return _impl->theme.borderWidth; // Let this to any value to ensure bg is drawn.
    case PM_MenuBarItemSpacing:
      return 0;
    case PM_MenuBarVMargin:
      return 0;
    case PM_MenuBarHMargin:
      return 0;

    // Indicators.
    case PM_IndicatorWidth:
    case PM_ExclusiveIndicatorWidth:
      return _impl->theme.iconSize.width();
    case PM_IndicatorHeight:
    case PM_ExclusiveIndicatorHeight:
      return _impl->theme.iconSize.height();

    // Dialog.
    case PM_MessageBoxIconSize:
      return _impl->theme.iconSizeLarge.height();

    // Mdi.
    case PM_MdiSubWindowFrameWidth:
      break;
    case PM_MdiSubWindowMinimizedWidth:
      break;

    // ToolBar.
    case PM_ToolBarFrameWidth:
      return isInTitleBar(w) ? 0 : _impl->theme.borderWidth;
    case PM_ToolBarHandleExtent:
      return _impl->theme.spacing / 2;
    case PM_ToolBarItemSpacing:
      return _impl->theme.spacing / 2;
    case PM_ToolBarItemMargin:
      return _impl->theme.spacing;
    case PM_ToolBarSeparatorExtent:
      return _impl->theme.spacing * 2;
    case PM_ToolBarExtensionExtent:
      return _impl->theme.iconSize.height() + _impl->theme.spacing;
    case PM_ToolBarIconSize:
      return _impl->theme.iconSize.height();

    // SpinBox.
    case PM_SpinBoxSliderHeight:
      break; // ?

    // ItemView.
    case PM_IconViewIconSize:
      return pixelMetric(PM_LargeIconSize, opt, w);
    case PM_ListViewIconSize:
      return pixelMetric(PM_SmallIconSize, opt, w);
    case PM_HeaderDefaultSectionSizeHorizontal:
      return static_cast<int>(_impl->theme.controlDefaultWidth * 1.5);
    case PM_HeaderDefaultSectionSizeVertical:
      return _impl->theme.controlHeightMedium;

    // Focus.
    case PM_FocusFrameVMargin:
    case PM_FocusFrameHMargin:
      // This is used in QFocusFrame to compute its size.
      // Allow place for bounce animation.
      return 2 * _impl->theme.focusBorderWidth;

    // ToolTip.
    case PM_ToolTipLabelFrameWidth:
      return _impl->theme.spacing / 2;

    // CheckBox.
    case PM_RadioButtonLabelSpacing:
    case PM_CheckBoxLabelSpacing:
      return _impl->theme.spacing;

    // Grip.
    case PM_SizeGripSize:
      break;

    // Dock.
    case PM_DockWidgetTitleMargin:
      break;
    case PM_DockWidgetTitleBarButtonMargin:
      break;

    // Layout.
    case PM_LayoutLeftMargin:;
    case PM_LayoutTopMargin:
    case PM_LayoutRightMargin:
    case PM_LayoutBottomMargin:
      return _impl->theme.spacing * 2;
    case PM_LayoutHorizontalSpacing:
    case PM_LayoutVerticalSpacing:
      return _impl->theme.spacing;

    // Common.
    case PM_TextCursorWidth:
      return 1;

    // ScrollView.
    case PM_ScrollBarExtent:
      return _impl->theme.scrollBarThicknessFull + _impl->theme.scrollBarMargin;
    case PM_ScrollBarSliderMin:
      return _impl->theme.controlHeightLarge;
    case PM_ScrollView_ScrollBarSpacing:
      return 0;
    case PM_ScrollView_ScrollBarOverlap:
      return true;

    // TreeView/TableView.
    case PM_TreeViewIndentation:
      return static_cast<int>(_impl->theme.spacing * 2.5);
    case PM_HeaderMargin:
      return _impl->theme.spacing; // Header horizontal padding.
    case PM_HeaderMarkSize:
      return _impl->theme.iconSize.height();
    case PM_HeaderGripMargin:
      break; // ???

    default:
      break;
  }
  return QCommonStyle::pixelMetric(m, opt, w);
}

int QlementineStyle::styleHint(StyleHint sh, const QStyleOption* opt, const QWidget* w, QStyleHintReturn* shret) const {
  switch (sh) {
    // Text
    case SH_EtchDisabledText:
      return false;
    case SH_DitherDisabledText:
      return false;

    // Widget
    case SH_Widget_ShareActivation:
      break;
    case SH_Widget_Animate: // deprecated
    case SH_Widget_Animation_Duration:
      return _impl->theme.animationDuration;

    // Workspace
    case SH_Workspace_FillSpaceOnMaximize:
      return true;

    // ScrollBar
    case SH_ScrollBar_MiddleClickAbsolutePosition:
      return false;
    case SH_ScrollBar_ScrollWhenPointerLeavesControl:
      return true;
    case SH_ScrollBar_LeftClickAbsolutePosition:
      return true;
    case SH_ScrollBar_ContextMenu:
      return false;
    case SH_ScrollBar_RollBetweenButtons:
      return false;
    case SH_ScrollView_FrameOnlyAroundContents:
      return false;
    case SH_ScrollBar_Transient:
      return true;

    // TabBar
    case SH_TabBar_SelectMouseType:
      return QEvent::MouseButtonPress;
    case SH_TabBar_Alignment:
      return Qt::AlignLeft;
    case SH_TabBar_ElideMode:
      // Let not the QTabBar handle the style a text already elided.
      return Qt::ElideNone;
    case SH_TabBar_CloseButtonPosition:
      return QTabBar::RightSide;
    case SH_TabBar_ChangeCurrentDelay:
      return 500;
    case SH_TabBar_PreferNoArrows:
      return false;
    case SH_TabWidget_DefaultTabPosition:
      return QTabWidget::North;

    // Slider
    case SH_Slider_SnapToValue:
      return true;
    case SH_Slider_SloppyKeyEvents:
      return false;
    case SH_Slider_StopMouseOverSlider:
      return true;
    case SH_Slider_AbsoluteSetButtons:
      return Qt::LeftButton;
    case SH_Slider_PageSetButtons:
      return Qt::LeftButton;

    // Dialogs
    case SH_ProgressDialog_CenterCancelButton:
      return false;
    case SH_ProgressDialog_TextLabelAlignment:
      return Qt::AlignLeft;
    case SH_PrintDialog_RightAlignButtons:
      return true;
    case SH_FontDialog_SelectAssociatedText:
      return false;

    // DialogButtonBox
    case SH_DialogButtons_DefaultButton:
      break; // Let automatic.
    case SH_DialogButtonLayout:
      break; // Let the platform decide.
    case SH_DialogButtonBox_ButtonsHaveIcons:
      return false;

    // MessageBox
    case SH_MessageBox_TextInteractionFlags:
      return Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse;
    case SH_MessageBox_CenterButtons:
      return false;
    case SH_MessageBox_UseBorderForButtonSpacing:
      return 0;

    // Menu
    case SH_Menu_AllowActiveAndDisabled:
      return false;
    case SH_Menu_SpaceActivatesItem:
      return true;
    case SH_Menu_SubMenuPopupDelay:
      return 300;
    case SH_Menu_MouseTracking:
      return true;
    case SH_Menu_Scrollable:
      return true;
    case SH_Menu_SloppySubMenus:
      break;
    case SH_Menu_FillScreenWithScroll:
      return true;
    case SH_Menu_KeyboardSearch:
      return true;
    case SH_Menu_SelectionWrap:
      return true;
    case SH_Menu_Mask:
      break;
    case SH_Menu_FlashTriggeredItem:
      return true;
    case SH_Menu_FadeOutOnHide:
      return true;
    case SH_Menu_SupportsSections:
      return false;
    case SH_Menu_SubMenuUniDirection:
      break;
    case SH_Menu_SubMenuUniDirectionFailCount:
      break;
    case SH_Menu_SubMenuSloppySelectOtherActions:
      break;
    case SH_Menu_SubMenuSloppyCloseTimeout:
      break;
    case SH_Menu_SubMenuResetWhenReenteringParent:
      break;
    case SH_Menu_SubMenuDontStartSloppyOnLeave:
      break;
    case SH_UnderlineShortcut:
      break;

    // MenuBar
    case SH_MenuBar_MouseTracking:
      return true;
    case SH_MenuBar_AltKeyNavigation:
      return true;
    case SH_DrawMenuBarSeparator:
      return true;
    case SH_MainWindow_SpaceBelowMenuBar:
      return false;

    // ComboBox
    case SH_ComboBox_ListMouseTracking:
      return true;
    case SH_ComboBox_Popup:
      return true;
    case SH_ComboBox_LayoutDirection:
      break;
    case SH_ComboBox_PopupFrameStyle:
      return QFrame::StyledPanel | QFrame::Plain;
    case SH_ComboBox_UseNativePopup:
      return false;
    case SH_ComboBox_AllowWheelScrolling:
      return false;

    // TitleBar
    case SH_TitleBar_NoBorder:
      break;
    case SH_TitleBar_ModifyNotification:
      break;
    case SH_TitleBar_AutoRaise:
      break;
    case SH_TitleBar_ShowToolTipsOnButtons:
      return true;

    // TextFields
    case SH_BlinkCursorWhenTextSelected:
      return true;
    case SH_RichText_FullWidthSelection:
      break;
    case SH_TextControl_FocusIndicatorTextCharFormat:
      break;

    // GroupBox
    case SH_GroupBox_TextLabelVerticalAlignment:
      return Qt::AlignVCenter;
    case SH_GroupBox_TextLabelColor:
      return int(_impl->theme.secondaryColor.rgba());

    // Table
    case SH_Table_GridLineColor:
      return int(tableLineColor().rgba());
    case SH_Header_ArrowAlignment:
      return Qt::AlignRight | Qt::AlignVCenter;

    // SpinBox
    case SH_SpinBox_AnimateButton:
      return true;
    case SH_SpinBox_KeyPressAutoRepeatRate:
      return 75;
    case SH_SpinBox_ClickAutoRepeatRate:
      return 75;
    case SH_SpinBox_ButtonsInsideFrame:
      return false;
    case SH_SpinBox_StepModifier:
      return Qt::ControlModifier;
    case SH_SpinBox_ClickAutoRepeatThreshold:
      return 500;
    case SH_SpinControls_DisableOnBounds:
      return true;

    // ToolBox
    case SH_ToolBox_SelectedPageTitleBold:
      return true;

    // Button
    case SH_Button_FocusPolicy:
      return Qt::TabFocus;

    // Masks
    case SH_FocusFrame_Mask:
      if (w) {
        if (auto* mask = qstyleoption_cast<QStyleHintReturnMask*>(shret)) {
          const auto focusBorderW = _impl->theme.focusBorderWidth;
          const auto widgetRect = w->rect();
          const auto extendedRect =
            widgetRect.marginsAdded(QMargins(focusBorderW, focusBorderW, focusBorderW, focusBorderW));
          mask->region = extendedRect;
          return 1;
        }
      }
      return 0;
    case SH_RubberBand_Mask:
      break;
    case SH_WindowFrame_Mask:
      break;

    // Dial
    case SH_Dial_BackgroundRole:
      break;

    // ItemView
    case SH_ItemView_ChangeHighlightOnFocus:
      return true;
    case SH_ItemView_EllipsisLocation:
      return Qt::AlignTrailing;
    case SH_ItemView_ShowDecorationSelected:
      return true;
    case SH_ItemView_ActivateItemOnSingleClick:
      return true;
    case SH_ItemView_MovementWithoutUpdatingSelection:
      return true;
    case SH_ItemView_ArrowKeysNavigateIntoChildren:
      return true;
    case SH_ItemView_PaintAlternatingRowColorsForEmptyArea:
      return true;
    case SH_ItemView_DrawDelegateFrame:
      return false;
    case SH_ItemView_ScrollMode:
      return QAbstractItemView::ScrollPerPixel;

    // ListView
    case SH_ListViewExpand_SelectMouseType:
      break; // ???

    // LineEdit
    case SH_LineEdit_PasswordCharacter:
      return QChar(0x2022).unicode(); // Bullet.
    case SH_LineEdit_PasswordMaskDelay:
      return 0;

    // FocusFrame
    case SH_FocusFrame_AboveWidget:
      return true;

    // Wizard
    case SH_WizardStyle:
      break;

    // DockWidget
    case SH_DockWidget_ButtonsHaveFrame:
      break;

    // Misc.
    case SH_RequestSoftwareInputPanel:
      return RSIP_OnMouseClick;

    // ToolBar
    case SH_ToolBar_Movable:
      return false;
    case SH_ToolButtonStyle:
      return Qt::ToolButtonStyle::ToolButtonIconOnly;
    case SH_ToolButton_PopupDelay:
      break;

    // FormLayout
    case SH_FormLayoutFieldGrowthPolicy:
      return QFormLayout::AllNonFixedFieldsGrow;
    case SH_FormLayoutFormAlignment:
      return Qt::AlignLeft;
    case SH_FormLayoutLabelAlignment:
      return Qt::AlignLeft;
    case SH_FormLayoutWrapPolicy:
      return QFormLayout::WrapLongRows;

    // ToolTip
    case SH_ToolTip_Mask:
      //if (auto* mask = qstyleoption_cast<QStyleHintReturnMask*>(shret)) {
      //  mask->region = windowMask(opt->rect, opts.round > ROUND_SLIGHT);
      //  return true;
      //}
      break;
    case SH_ToolTip_WakeUpDelay:
      return 700;
    case SH_ToolTip_FallAsleepDelay:
      return 2500;
    case SH_ToolTipLabel_Opacity:
      return 255; // TODO

    // Splitter
    case SH_Splitter_OpaqueResize:
      return true;

    default:
      break;
  }
  return QCommonStyle::styleHint(sh, opt, w, shret);
}

QIcon QlementineStyle::standardIcon(StandardPixmap sp, const QStyleOption* opt, const QWidget* w) const {
  switch (sp) {
    case SP_MessageBoxQuestion:
    case SP_MessageBoxInformation:
    case SP_MessageBoxCritical:
    case SP_MessageBoxWarning: {
      const auto extent = pixelMetric(PM_LargeIconSize) * 4;
      return _impl->getStandardIcon(sp, QSize(extent, extent));
    }
    case SP_ToolBarHorizontalExtensionButton:
    case SP_ToolBarVerticalExtensionButton:
    case SP_ArrowLeft:
    case SP_ArrowRight:
    case SP_LineEditClearButton:
      return _impl->getStandardIcon(sp, _impl->theme.iconSize);
    default:
      break;
  }
  return QCommonStyle::standardIcon(sp, opt, w);
}

QPalette QlementineStyle::standardPalette() const {
  return _impl->theme.palette;
}

QPixmap QlementineStyle::standardPixmap(StandardPixmap sp, const QStyleOption* opt, const QWidget* w) const {
  return QCommonStyle::standardPixmap(sp, opt, w);
}

QPixmap QlementineStyle::generatedIconPixmap(QIcon::Mode im, const QPixmap& pixmap, const QStyleOption* opt) const {
  return QCommonStyle::generatedIconPixmap(im, pixmap, opt);
}

int QlementineStyle::layoutSpacing(QSizePolicy::ControlType c1, QSizePolicy::ControlType c2, Qt::Orientation o,
  const QStyleOption* opt, const QWidget* w) const {
  return QCommonStyle::layoutSpacing(c1, c2, o, opt, w);
}

void QlementineStyle::polish(QPalette& palette) {
  QCommonStyle::polish(palette);
  palette = _impl->theme.palette;
}

void QlementineStyle::polish(QApplication* app) {
  QCommonStyle::polish(app);
  if (!_impl->previousAppFont) {
    _impl->previousAppFont = app->font();
  }
  app->setFont(_impl->theme.fontRegular);

  QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, false);
  QCoreApplication::setAttribute(Qt::AA_DontShowShortcutsInContextMenus, false);

  QApplication::setEffectEnabled(Qt::UIEffect::UI_AnimateMenu, true);
  QApplication::setEffectEnabled(Qt::UIEffect::UI_FadeMenu, true);
  QApplication::setEffectEnabled(Qt::UIEffect::UI_AnimateCombo, true);
  QApplication::setEffectEnabled(Qt::UIEffect::UI_AnimateTooltip, true);
  QApplication::setEffectEnabled(Qt::UIEffect::UI_FadeTooltip, true);
}

void QlementineStyle::unpolish(QApplication* app) {
  QCommonStyle::unpolish(app);
  if (_impl->previousAppFont) {
    app->setFont(*_impl->previousAppFont);
    _impl->previousAppFont.reset();
  }
}

void QlementineStyle::polish(QWidget* w) {
  if (!w)
    return;

  QCommonStyle::polish(w);

  // Ensure we only polish a widget once, otherwise we might
  // end up with multiple event filters on the same widget.
  auto& polishedWidgetInfo = _impl->polishedWidgets.ensure(w);
  const auto eventFiltersInstalled = !polishedWidgetInfo.eventFilters.empty();

  // Not yet ideal, but still better than a huge fonction.
  polishToolTip(w);
  polishLineEditIconButton(*this, _impl->animations, w, polishedWidgetInfo, eventFiltersInstalled);
  polishCommonWidget(w);
  polishExternalFocusFrame(w, _impl->polishedWidgets, polishedWidgetInfo, eventFiltersInstalled);
  polishMenu(w, _impl->polishedWidgets, polishedWidgetInfo, eventFiltersInstalled);
  polishComboBoxPopup(w, *this);
  polishVerticalCompression(w);
  polishMouseWheelBlocker(w, _impl->polishedWidgets, polishedWidgetInfo, eventFiltersInstalled);
  polishComboBox(*this, w);
  polishTabBar(w, _impl->polishedWidgets, polishedWidgetInfo, eventFiltersInstalled);

  if (auto* label = qobject_cast<QLabel*>(w)) {
    const auto labelObjName = label->objectName();
    const auto isInformativeLabel = labelObjName == QStringLiteral("qt_msgbox_informativelabel");
    if (isInformativeLabel) {
      label->setForegroundRole(QPalette::ColorRole::BrightText);
    }
  }

  polishMessageBox(w, *this);
  polishScrollArea(w);
  polishSlider(w);
  polishTextEditors(w, _impl->polishedWidgets, polishedWidgetInfo, eventFiltersInstalled);
  polishLineEditMenus(w, _impl->polishedWidgets, polishedWidgetInfo, eventFiltersInstalled);
  polishDestructionTracking(w, this, _impl->polishedWidgets, polishedWidgetInfo, eventFiltersInstalled);
}

void QlementineStyle::unpolish(QWidget* w) {
  QCommonStyle::unpolish(w);
  unpolishWidget(*this, w, _impl->polishedWidgets);
}

/* QStyle extended enums. */

void QlementineStyle::drawPrimitiveExt(
  PrimitiveElementExt pe, const QStyleOption* opt, QPainter* p, const QWidget* w) const {
  QPainterStateGuard stateGuard(p);
  p->setRenderHint(QPainter::SmoothPixmapTransform, true);
  const auto* optButton = buttonStyleOption(opt);
  const auto* optCommandButton = qstyleoption_cast<const QStyleOptionCommandLinkButton*>(opt);
  switch (pe) {
    case PrimitiveElementExt::PE_CommandButtonPanel:
      if (optButton) {
        const auto radius = _impl->theme.borderRadius;
        const auto mouse = getMouseState(optButton->state);
        const auto isDefault = optButton->features.testFlag(QStyleOptionButton::DefaultButton);
        const auto role = getColorRole(optButton->state, isDefault);
        const auto& bgColor = commandButtonBackgroundColor(mouse, role);
        const auto& currentColor = _impl->animations.animateBackgroundColor(w, bgColor, _impl->theme.animationDuration);
        p->setPen(Qt::NoPen);
        p->setBrush(currentColor);
        p->setRenderHint(QPainter::Antialiasing, true);
        p->drawRoundedRect(optButton->rect, radius, radius);
      }
      return;
    case PrimitiveElementExt::PE_CommandButtonLabel:
      if (optButton) {
        p->setRenderHint(QPainter::Antialiasing, true);
        p->setBrush(Qt::NoBrush);

        const auto& rect = optButton->rect;
        const auto spacing = _impl->theme.spacing;
        const auto mouse = getMouseState(optButton->state);
        const auto checked = getCheckState(optButton->state);
        const auto isDefault = optButton->features.testFlag(QStyleOptionButton::DefaultButton);
        const auto role = getColorRole(optButton->state, isDefault);

        auto availableX = rect.x();
        auto availableW = rect.width();

        const auto& icon = optButton->icon;
        if (!icon.isNull()) {
          const auto& iconSize = optButton->iconSize;
          const auto iconX = availableX;
          const auto iconY = rect.y() + (rect.height() - iconSize.height()) / 2;
          const auto iconRect = QRect{ QPoint{ iconX, iconY }, iconSize };
          const auto& pixmap = getPixmap(icon, iconSize, mouse, checked, w);

          if (!pixmap.isNull() && !iconRect.isEmpty()) {
            const auto& iconColor = commandButtonIconColor(mouse, role);
            const auto& colorizedPixmap = getColorizedPixmap(pixmap, w, iconColor, iconColor, mouse);

            // The pixmap may be smaller than the requested size, so we center it in the rect by default.
            const auto pixmapPixelRatio = colorizedPixmap.devicePixelRatio();
            const auto targetW = static_cast<int>((qreal) colorizedPixmap.width() / (pixmapPixelRatio));
            const auto targetH = static_cast<int>((qreal) colorizedPixmap.height() / (pixmapPixelRatio));
            const auto targetX = iconRect.x() + (iconRect.width() - targetW) / 2;
            const auto targetY = iconRect.y() + (iconRect.height() - targetH) / 2;
            const auto targetRect = QRect{ targetX, targetY, targetW, targetH };
            p->drawPixmap(targetRect, colorizedPixmap);

            const auto iconSpace = iconSize.width() + spacing * 2;
            availableX += iconSpace;
            availableW -= iconSpace;
          }
        }

        if (availableW < 0) {
          return;
        }

        const auto& text = optButton->text;
        const auto description = optCommandButton ? optCommandButton->description : QString{};
        const auto hasText = !text.isEmpty();
        const auto hasDescription = !description.isEmpty();
        const auto& fm = optButton->fontMetrics;
        const auto& boldFm = _impl->fontMetricsBold ? *_impl->fontMetricsBold : fm;
        const auto vSpacing = hasText && hasDescription ? spacing / 4 : 0;
        const auto textH = hasText ? boldFm.height() : 0;
        const auto descriptionH = hasDescription ? fm.height() : 0;
        const auto totalTextH = textH + vSpacing + descriptionH;
        const auto totalTextY = rect.y() + (rect.height() - totalTextH) / 2;
        constexpr auto textFlags =
          Qt::AlignVCenter | Qt::AlignBaseline | Qt::TextSingleLine | Qt::AlignLeft | Qt::TextHideMnemonic;

        const auto backupFont = QFont{ p->font() };
        if (hasText) {
          const auto textX = availableX;
          const auto textY = totalTextY;
          const auto textRect = QRect{ textX, textY, availableW, textH };
          const auto& textColor = commandButtonTextColor(mouse, role);
          const auto elidedText = boldFm.elidedText(text, Qt::ElideRight, availableW, Qt::TextSingleLine);
          p->setFont(_impl->theme.fontBold);
          p->setPen(textColor);
          p->drawText(textRect, textFlags, elidedText);
        }

        if (hasDescription) {
          const auto descriptionX = availableX;
          const auto descriptionY = totalTextY + textH + vSpacing;
          const auto descriptionRect = QRect{ descriptionX, descriptionY, availableW, descriptionH };
          const auto& descriptionColor = commandButtonDescriptionColor(mouse, role);
          const auto elidedDescription = fm.elidedText(description, Qt::ElideRight, availableW, Qt::TextSingleLine);
          p->setFont(_impl->theme.fontRegular);
          p->setPen(descriptionColor);
          p->drawText(descriptionRect, textFlags, elidedDescription);
        }

        p->setFont(backupFont);
      }
      return;
    default:
      break;
  }
}

QIcon QlementineStyle::standardIconExt(StandardPixmapExt sp, const QStyleOption* opt, const QWidget* w) const {
  Q_UNUSED(opt);
  Q_UNUSED(w);
  return _impl->getStandardIconExt(sp, _impl->theme.iconSize);
}

QSize QlementineStyle::sizeFromContentsExt(
  ContentsTypeExt ct, const QStyleOption* opt, const QSize& s, const QWidget* w) const {
  Q_UNUSED(s);
  Q_UNUSED(w);

  const auto* optButton = buttonStyleOption(opt);
  const auto* optCommandButton = qstyleoption_cast<const QStyleOptionCommandLinkButton*>(opt);
  switch (ct) {
    case ContentsTypeExt::CT_CommandButton:
      if (optButton) {
        const auto iconSize = optButton->iconSize;
        const auto& icon = optButton->icon;
        const auto spacing = _impl->theme.spacing;
        const auto hPadding = spacing * 2;
        const auto vPadding = spacing;
        const auto description = optCommandButton ? optCommandButton->description : QString{};
        const auto hasText = !optButton->text.isEmpty();
        const auto hasDescription = !description.isEmpty();
        const auto vSpacing = hasText && hasDescription ? spacing / 4 : 0;
        const auto iconW = icon.isNull() ? 0 : iconSize.width() + spacing * 2;
        const auto& fm = optButton->fontMetrics;
        const auto& boldFm = _impl->fontMetricsBold ? *_impl->fontMetricsBold : fm;
        const auto textW = boldFm.boundingRect(optButton->rect, Qt::AlignLeft, optButton->text).width();
        const auto descriptionW = fm.boundingRect(optButton->rect, Qt::AlignLeft, description).width();
        const auto width = hPadding * 2 + iconW + std::max(textW, descriptionW);
        const auto textH = hasText ? boldFm.height() : 0;
        const auto descriptionH = hasDescription ? fm.height() : 0;
        const auto height =
          vPadding * 2 + std::max(icon.isNull() ? 0 : iconSize.height(), textH + descriptionH + vSpacing);
        return QSize{ width, height };
      }
      break;
    default:
      break;
  }
  return {};
}

void QlementineStyle::drawControlExt(
  ControlElementExt ce, const QStyleOption* opt, QPainter* p, const QWidget* w) const {
  QPainterStateGuard stateGuard(p);
  p->setRenderHint(QPainter::SmoothPixmapTransform, true);
  const auto* optButton = buttonStyleOption(opt);
  const auto* optCommandButton = qstyleoption_cast<const QStyleOptionCommandLinkButton*>(opt);
  switch (ce) {
    case ControlElementExt::CE_CommandButton:
      if (optButton) {
        // Button background and border.
        drawPrimitiveExt(PrimitiveElementExt::PE_CommandButtonPanel, opt, p, w);

        // Button foreground (text, descrption and icon).
        const auto spacing = _impl->theme.spacing;
        const auto hPadding = spacing * 2;
        const auto vPadding = spacing;
        const auto fgRect = optButton->rect.marginsRemoved(QMargins{ hPadding, vPadding, hPadding, vPadding });
        auto optLabel = optCommandButton ? QStyleOptionCommandLinkButton{ *optCommandButton }
                                         : QStyleOptionCommandLinkButton{ *optButton };
        optLabel.rect = fgRect;
        drawPrimitiveExt(PrimitiveElementExt::PE_CommandButtonLabel, &optLabel, p, w);
      }
      return;
    default:
      return;
  }
}

int QlementineStyle::pixelMetricExt(PixelMetricExt m, const QStyleOption* opt, const QWidget* w) const {
  Q_UNUSED(opt);
  Q_UNUSED(w);

  switch (m) {
    // Icons.
    case PixelMetricExt::PM_MediumIconSize:
      return _impl->theme.iconSizeMedium.height();
    default:
      return 0;
  }
}

/* Theme-related methods. */

QColor const& QlementineStyle::color(MouseState const mouse, ColorRole const role) const {
  const auto primary = role == ColorRole::Primary;

  switch (mouse) {
    case MouseState::Pressed:
      return primary ? _impl->theme.primaryColorPressed : _impl->theme.secondaryColorPressed;
    case MouseState::Hovered:
      return primary ? _impl->theme.primaryColorHovered : _impl->theme.secondaryColorHovered;
    case MouseState::Disabled:
      return primary ? _impl->theme.primaryColorDisabled : _impl->theme.secondaryColorDisabled;
    case MouseState::Transparent:
      return primary ? _impl->theme.primaryColorTransparent : _impl->theme.secondaryColorTransparent;
    case MouseState::Normal:
    default:
      return primary ? _impl->theme.primaryColor : _impl->theme.secondaryColor;
  }
}

QColor const& QlementineStyle::frameBackgroundColor(MouseState const mouse) const {
  if (mouse == MouseState::Disabled)
    return _impl->theme.backgroundColorMainTransparent;
  else
    return _impl->theme.backgroundColorMain1;
}

QColor const& QlementineStyle::buttonBackgroundColor(
  MouseState const mouse, ColorRole const role, const QWidget* w) const {
  Q_UNUSED(w)
  const auto primary = role == ColorRole::Primary;

  switch (mouse) {
    case MouseState::Pressed:
      return primary ? _impl->theme.primaryColorPressed : _impl->theme.neutralColorPressed;
    case MouseState::Hovered:
      return primary ? _impl->theme.primaryColorHovered : _impl->theme.neutralColorHovered;
    case MouseState::Disabled:
      return primary ? _impl->theme.primaryColorDisabled : _impl->theme.neutralColorDisabled;
    case MouseState::Transparent:
      return primary ? _impl->theme.primaryColorTransparent : _impl->theme.neutralColorTransparent;
    case MouseState::Normal:
    default:
      return primary ? _impl->theme.primaryColor : _impl->theme.neutralColor;
  }
}

QColor const& QlementineStyle::buttonForegroundColor(
  MouseState const mouse, ColorRole const role, const QWidget* w) const {
  Q_UNUSED(w)
  const auto primary = role == ColorRole::Primary;

  switch (mouse) {
    case MouseState::Pressed:
      return primary ? _impl->theme.primaryColorForegroundPressed : _impl->theme.secondaryColor;
    case MouseState::Hovered:
      return primary ? _impl->theme.primaryColorForegroundHovered : _impl->theme.secondaryColor;
    case MouseState::Disabled:
      return primary ? _impl->theme.primaryColorForegroundDisabled : _impl->theme.secondaryColorDisabled;
    case MouseState::Transparent:
    case MouseState::Normal:
    default:
      return primary ? _impl->theme.primaryColorForeground : _impl->theme.secondaryColor;
  }
}

QColor const& QlementineStyle::toolButtonBackgroundColor(MouseState const mouse, ColorRole const role) const {
  const auto primary = role == ColorRole::Primary;

  switch (mouse) {
    case MouseState::Pressed:
      return primary ? _impl->theme.primaryColorPressed : _impl->theme.neutralColorHovered;
    case MouseState::Hovered:
      return primary ? _impl->theme.primaryColorHovered : _impl->theme.neutralColor;
    case MouseState::Disabled:
      return primary ? _impl->theme.primaryColorDisabled : _impl->theme.neutralColorTransparent;
    case MouseState::Transparent:
    case MouseState::Normal:
    default:
      return primary ? _impl->theme.primaryColor : _impl->theme.neutralColorTransparent;
  }
}

QColor const& QlementineStyle::toolButtonForegroundColor(MouseState const mouse, ColorRole const role) const {
  const auto primary = role == ColorRole::Primary;

  switch (mouse) {
    case MouseState::Disabled:
      return primary ? _impl->theme.primaryColorForegroundDisabled : _impl->theme.secondaryColorDisabled;
    default:
      return primary ? _impl->theme.primaryColorForeground : _impl->theme.secondaryColor;
  }
}

QColor const& QlementineStyle::toolButtonSeparatorColor(MouseState const mouse, ColorRole const role) const {
  Q_UNUSED(role)
  switch (mouse) {
    case MouseState::Pressed:
      return _impl->theme.neutralColorPressed;
    case MouseState::Hovered:
      return _impl->theme.neutralColorHovered;
    case MouseState::Normal:
      return _impl->theme.neutralColor;
    case MouseState::Disabled:
    default:
      return _impl->theme.neutralColorDisabled;
  }
}

QColor const& QlementineStyle::commandButtonBackgroundColor(MouseState const mouse, ColorRole const role) const {
  return buttonBackgroundColor(mouse, role);
}

QColor const& QlementineStyle::commandButtonTextColor(MouseState const mouse, ColorRole const role) const {
  return toolButtonForegroundColor(mouse, role);
}

QColor const& QlementineStyle::commandButtonDescriptionColor(MouseState const mouse, ColorRole const role) const {
  const auto primary = role == ColorRole::Primary;

  switch (mouse) {
    case MouseState::Disabled:
      return primary ? _impl->theme.primaryColorForegroundDisabled : _impl->theme.secondaryAlternativeColorDisabled;
    default:
      return primary ? _impl->theme.primaryColorForegroundDisabled : _impl->theme.secondaryAlternativeColor;
  }
}

QColor const& QlementineStyle::commandButtonIconColor(MouseState const mouse, ColorRole const role) const {
  return commandButtonTextColor(mouse, role);
}

QColor const& QlementineStyle::checkButtonBackgroundColor(MouseState const mouse, CheckState const checked) const {
  switch (checked) {
    case CheckState::Checked:
    case CheckState::Indeterminate:
      return buttonBackgroundColor(mouse, ColorRole::Primary);
    case CheckState::NotChecked:
    default:
      switch (mouse) {
        case MouseState::Pressed:
          return _impl->theme.backgroundColorMain3;
        case MouseState::Disabled:
          return _impl->theme.backgroundColorMain2;
        default:
          return _impl->theme.backgroundColorMain1;
      }
  }
}

QColor const& QlementineStyle::checkButtonForegroundColor(MouseState const mouse, CheckState const checked) const {
  Q_UNUSED(checked)
  return buttonForegroundColor(mouse, ColorRole::Primary);
}

QColor const& QlementineStyle::checkButtonBorderColor(
  MouseState const mouse, FocusState const focus, CheckState const checked) const {
  switch (checked) {
    case CheckState::Checked:
    case CheckState::Indeterminate:
      return checkButtonBackgroundColor(mouse, checked);
    case CheckState::NotChecked:
    default:
      if (focus == FocusState::Focused)
        return _impl->theme.primaryColor;

      switch (mouse) {
        case MouseState::Hovered:
          return _impl->theme.borderColorHovered;
        case MouseState::Pressed:
          return _impl->theme.borderColorPressed;
        case MouseState::Disabled:
          return _impl->theme.borderColorDisabled;
        default:
          return _impl->theme.borderColor;
      }
  }
}

QColor const& QlementineStyle::radioButtonBackgroundColor(MouseState const mouse, CheckState const checked) const {
  return checkButtonBackgroundColor(mouse, checked);
}

QColor const& QlementineStyle::radioButtonForegroundColor(MouseState const mouse, CheckState const checked) const {
  return checkButtonForegroundColor(mouse, checked);
}

QColor const& QlementineStyle::radioButtonBorderColor(
  MouseState const mouse, FocusState const focus, CheckState const checked) const {
  return checkButtonBorderColor(mouse, focus, checked);
}

QColor const& QlementineStyle::comboBoxBackgroundColor(MouseState const mouse) const {
  return buttonBackgroundColor(mouse, ColorRole::Secondary);
}

QColor const& QlementineStyle::comboBoxForegroundColor(MouseState const mouse) const {
  return buttonForegroundColor(mouse, ColorRole::Secondary);
}

QColor const& QlementineStyle::comboBoxTextColor(MouseState const mouse, Status const status, const QWidget* w) const {
  Q_UNUSED(w);
  switch (status) {
    case Status::Error:
      return _impl->theme.statusColorError;
    case Status::Warning:
      return _impl->theme.statusColorWarning;
    case Status::Success:
      return _impl->theme.statusColorSuccess;
    case Status::Info:
    case Status::Default:
    default:
      return comboBoxForegroundColor(mouse);
  }
}

QColor const& QlementineStyle::spinBoxBackgroundColor(MouseState const mouse) const {
  return textFieldBackgroundColor(mouse, Status::Default);
}

QColor const& QlementineStyle::spinBoxBorderColor(MouseState const mouse, FocusState const focus) const {
  return textFieldBorderColor(mouse, focus, Status::Default);
}

QColor const& QlementineStyle::spinBoxButtonBackgroundColor(MouseState const mouse) const {
  return buttonBackgroundColor(mouse, ColorRole::Secondary);
}

QColor const& QlementineStyle::spinBoxButtonForegroundColor(MouseState const mouse) const {
  return buttonForegroundColor(mouse, ColorRole::Secondary);
}

QColor const& QlementineStyle::listItemRowBackgroundColor(
  MouseState const mouse, AlternateState const alternate) const {
  const auto isAlternate = alternate == AlternateState::Alternate;
  const auto isEnabled = mouse != MouseState::Disabled;
  return _impl->theme.palette.color(isEnabled ? QPalette::ColorGroup::Normal : QPalette::ColorGroup::Disabled,
    isAlternate ? QPalette::ColorRole::AlternateBase : QPalette::ColorRole::Base);
}

QColor QlementineStyle::listItemBackgroundColor(MouseState const mouse, SelectionState const selected,
  FocusState const focus, ActiveState const active, const QModelIndex& index, const QWidget* widget) const {
  Q_UNUSED(index)
  Q_UNUSED(widget)
  const auto isSelected = selected == SelectionState::Selected;
  const auto isActive = active == ActiveState::Active && focus == FocusState::Focused;

  if (isActive) {
    switch (mouse) {
      case MouseState::Pressed:
        return isSelected ? _impl->theme.primaryColor : _impl->theme.neutralColor;
      case MouseState::Hovered:
        return isSelected ? _impl->theme.primaryColor : _impl->theme.neutralColorDisabled;
      case MouseState::Disabled:
        return isSelected ? _impl->theme.primaryColorDisabled : _impl->theme.neutralColorTransparent;
      case MouseState::Transparent:
      case MouseState::Normal:
      default:
        return isSelected ? _impl->theme.primaryColor : _impl->theme.neutralColorTransparent;
    }
  } else {
    switch (mouse) {
      case MouseState::Pressed:
        return _impl->theme.neutralColor;
      case MouseState::Hovered:
        return isSelected ? _impl->theme.neutralColor : _impl->theme.neutralColorDisabled;
      case MouseState::Disabled:
        return isSelected ? _impl->theme.neutralColor : _impl->theme.neutralColorTransparent;
      case MouseState::Transparent:
      case MouseState::Normal:
      default:
        return isSelected ? _impl->theme.neutralColor : _impl->theme.neutralColorTransparent;
    }
  }
}

QColor const& QlementineStyle::listItemForegroundColor(
  MouseState const mouse, SelectionState const selected, FocusState const focus, ActiveState const active) const {
  Q_UNUSED(focus)
  const auto isSelected = selected == SelectionState::Selected;
  const auto isActive = active == ActiveState::Active;

  if (isActive) {
    switch (mouse) {
      case MouseState::Disabled:
        return isSelected ? _impl->theme.primaryColorForegroundDisabled : _impl->theme.secondaryColorDisabled;
      case MouseState::Hovered:
      case MouseState::Pressed:
      case MouseState::Transparent:
      case MouseState::Normal:
      default:
        return isSelected ? _impl->theme.primaryColorForeground : _impl->theme.secondaryColor;
    }
  } else {
    switch (mouse) {
      case MouseState::Disabled:
        return _impl->theme.secondaryColorDisabled;
      case MouseState::Hovered:
      case MouseState::Pressed:
      case MouseState::Transparent:
      case MouseState::Normal:
      default:
        return _impl->theme.secondaryColor;
    }
  }
}

// Returns whether an icon in an item view should be colorized with a color.
// Subclasses can override this to customize the behavior depending on the index or state.
AutoIconColor QlementineStyle::listItemAutoIconColor(MouseState const mouse, SelectionState const selected,
  FocusState const focus, ActiveState const active, const QModelIndex& index, const QWidget* widget) const {
  Q_UNUSED(mouse)
  Q_UNUSED(selected)
  Q_UNUSED(focus)
  Q_UNUSED(active)
  Q_UNUSED(index)
  return autoIconColor(widget);
}

QColor const& QlementineStyle::listItemCaptionForegroundColor(
  MouseState const mouse, SelectionState const selected, FocusState const focus, ActiveState const active) const {
  Q_UNUSED(focus)
  const auto isSelected = selected == SelectionState::Selected;
  const auto isActive = active == ActiveState::Active;

  if (isActive) {
    switch (mouse) {
      case MouseState::Disabled:
        return isSelected ? _impl->theme.primaryColorForegroundDisabled
                          : _impl->theme.secondaryAlternativeColorDisabled;
      case MouseState::Hovered:
      case MouseState::Pressed:
      case MouseState::Transparent:
      case MouseState::Normal:
      default:
        return isSelected ? _impl->theme.primaryColorForeground : _impl->theme.secondaryAlternativeColor;
    }
  } else {
    switch (mouse) {
      case MouseState::Disabled:
        return _impl->theme.secondaryAlternativeColorDisabled;
      case MouseState::Hovered:
      case MouseState::Pressed:
      case MouseState::Transparent:
      case MouseState::Normal:
      default:
        return _impl->theme.secondaryAlternativeColor;
    }
  }
}

QColor const& QlementineStyle::listItemCheckButtonBackgroundColor(
  MouseState const mouse, CheckState const checked, SelectionState const selected, ActiveState const active) const {
  Q_UNUSED(active)
  const auto isChecked = checked != CheckState::NotChecked;
  const auto isEnabled = mouse != MouseState::Disabled;
  switch (selected) {
    case SelectionState::Selected:
      if (isEnabled)
        return isChecked ? _impl->theme.primaryAlternativeColor : _impl->theme.backgroundColorMain1;
      else
        return isChecked ? _impl->theme.primaryAlternativeColorDisabled : _impl->theme.neutralColorDisabled;
    case SelectionState::NotSelected:
    default:
      if (isEnabled)
        return isChecked ? _impl->theme.primaryColor : _impl->theme.backgroundColorMain1;
      else
        return isChecked ? _impl->theme.primaryColorDisabled : _impl->theme.backgroundColorMain2;
  }
}

QColor const& QlementineStyle::listItemCheckButtonBorderColor(
  MouseState const mouse, CheckState const checked, SelectionState const selected, ActiveState const active) const {
  Q_UNUSED(active)
  const auto isChecked = checked != CheckState::NotChecked;
  const auto isEnabled = mouse != MouseState::Disabled;
  switch (selected) {
    case SelectionState::Selected:
      if (isEnabled)
        return isChecked ? _impl->theme.primaryAlternativeColorTransparent : _impl->theme.primaryColor;
      else
        return isChecked ? _impl->theme.primaryAlternativeColorTransparent : _impl->theme.borderColorTransparent;
    case SelectionState::NotSelected:
    default:
      if (isEnabled)
        return isChecked ? _impl->theme.primaryColor : _impl->theme.borderColor;
      else
        return isChecked ? _impl->theme.primaryColorDisabled : _impl->theme.borderColorDisabled;
  }
}

QColor const& QlementineStyle::listItemCheckButtonForegroundColor(
  MouseState const mouse, CheckState const checked, SelectionState const selected, ActiveState const active) const {
  Q_UNUSED(selected)
  Q_UNUSED(active)
  return checkButtonForegroundColor(mouse, checked);
}

QColor const& QlementineStyle::cellItemFocusBorderColor(
  FocusState const focus, SelectionState const selected, ActiveState const active) const {
  Q_UNUSED(active)
  if (selected == SelectionState::Selected)
    return focus == FocusState::Focused ? _impl->theme.neutralColorPressed : _impl->theme.neutralColorTransparent;
  else
    return focus == FocusState::Focused ? _impl->theme.primaryColor : _impl->theme.primaryColorTransparent;
}

QColor const& QlementineStyle::menuBackgroundColor() const {
  return _impl->theme.backgroundColorMain1;
}

QColor const& QlementineStyle::menuBorderColor() const {
  return _impl->theme.borderColor;
}

QColor const& QlementineStyle::menuSeparatorColor() const {
  return _impl->theme.borderColorDisabled;
}

QColor const& QlementineStyle::menuItemBackgroundColor(MouseState const mouse) const {
  switch (mouse) {
    case MouseState::Hovered:
      return _impl->theme.primaryColor;
    case MouseState::Pressed:
      return _impl->theme.primaryColorHovered;
    case MouseState::Disabled:
    case MouseState::Transparent:
    case MouseState::Normal:
    default:
      return _impl->theme.primaryColorTransparent;
  }
}

QColor const& QlementineStyle::menuItemForegroundColor(MouseState const mouse) const {
  switch (mouse) {
    case MouseState::Hovered:
      return _impl->theme.primaryColorForegroundHovered;
    case MouseState::Pressed:
      return _impl->theme.primaryColorForegroundPressed;
    case MouseState::Disabled:
      return _impl->theme.secondaryColorDisabled;
    case MouseState::Transparent:
    case MouseState::Normal:
    default:
      return _impl->theme.secondaryColor;
  }
}

QColor const& QlementineStyle::menuItemSecondaryForegroundColor(MouseState const mouse) const {
  switch (mouse) {
    case MouseState::Hovered:
      return _impl->theme.primaryColorForegroundHovered;
    case MouseState::Pressed:
      return _impl->theme.primaryColorForegroundPressed;
    case MouseState::Disabled:
      return _impl->theme.secondaryAlternativeColorDisabled;
    case MouseState::Transparent:
    case MouseState::Normal:
    default:
      return _impl->theme.secondaryAlternativeColor;
  }
}

QColor const& QlementineStyle::menuBarBackgroundColor() const {
  return _impl->theme.backgroundColorMain2;
}

QColor const& QlementineStyle::menuBarBorderColor() const {
  return _impl->theme.borderColor;
}

QColor const& QlementineStyle::menuBarItemBackgroundColor(MouseState const mouse, SelectionState const selected) const {
  Q_UNUSED(selected)
  switch (mouse) {
    case MouseState::Hovered:
      return _impl->theme.neutralColorDisabled;
    case MouseState::Pressed:
      return _impl->theme.neutralColor;
    case MouseState::Disabled:
    case MouseState::Transparent:
    case MouseState::Normal:
    default:
      return _impl->theme.neutralColorTransparent;
  }
}

QColor const& QlementineStyle::menuBarItemForegroundColor(MouseState const mouse, SelectionState const selected) const {
  Q_UNUSED(selected)
  switch (mouse) {
    case MouseState::Hovered:
      return _impl->theme.secondaryColor;
    case MouseState::Pressed:
      return _impl->theme.secondaryColor;
    case MouseState::Disabled:
      return _impl->theme.secondaryColorDisabled;
    case MouseState::Transparent:
    case MouseState::Normal:
    default:
      return _impl->theme.secondaryColor;
  }
}

QColor const& QlementineStyle::tabBarBackgroundColor(MouseState const mouse) const {
  return mouse == MouseState::Disabled ? _impl->theme.backgroundColorMain3 : _impl->theme.backgroundColorTabBar;
}

QColor const& QlementineStyle::tabBarShadowColor() const {
  return _impl->theme.shadowColor1;
}

QColor const& QlementineStyle::tabBarBottomShadowColor() const {
  return _impl->theme.shadowColor1;
}

QColor const& QlementineStyle::tabBackgroundColor(MouseState const mouse, SelectionState const selected) const {
  const auto isSelected = selected == SelectionState::Selected;
  const auto& selectedTabColor = _impl->theme.backgroundColorMain2;
  const auto& hoverTabColor = _impl->theme.neutralColor;
  const auto& defaultTabColor = _impl->theme.backgroundColorMainTransparent;

  switch (mouse) {
    case MouseState::Hovered:
      return isSelected ? selectedTabColor : hoverTabColor;
    case MouseState::Pressed:
      return _impl->theme.backgroundColorMain2;
    case MouseState::Normal:
      return isSelected ? selectedTabColor : defaultTabColor;
    case MouseState::Disabled:
    case MouseState::Transparent:
    default:
      return defaultTabColor;
  }
}

QColor const& QlementineStyle::tabForegroundColor(MouseState const mouse, SelectionState const selected) const {
  Q_UNUSED(selected)
  return buttonForegroundColor(mouse, ColorRole::Secondary);
}

QColor QlementineStyle::tabTextColor(
  MouseState const mouse, SelectionState const selected, const QStyleOptionTab* optTab, const QWidget* w) const {
  Q_UNUSED(optTab);
  Q_UNUSED(w);
  return tabForegroundColor(mouse, selected);
}

QColor const& QlementineStyle::tabCloseButtonBackgroundColor(
  MouseState const mouse, SelectionState const selected) const {
  const auto isSelected = selected == SelectionState::Selected;
  switch (mouse) {
    case MouseState::Pressed:
      return isSelected ? _impl->theme.neutralColorPressed : _impl->theme.semiTransparentColor4;
    case MouseState::Hovered:
      return isSelected ? _impl->theme.neutralColor : _impl->theme.semiTransparentColor2;
    case MouseState::Normal:
    case MouseState::Disabled:
    case MouseState::Transparent:
    default:
      return isSelected ? _impl->theme.neutralColorTransparent : _impl->theme.semiTransparentColorTransparent;
  }
}

QColor const& QlementineStyle::tabCloseButtonForegroundColor(
  MouseState const mouse, SelectionState const selected) const {
  switch (mouse) {
    case MouseState::Pressed:
    case MouseState::Hovered:
    case MouseState::Normal:
      return _impl->theme.secondaryColor;
    case MouseState::Disabled:
    case MouseState::Transparent:
      return _impl->theme.secondaryColorTransparent;
    default:
      return selected == SelectionState::Selected ? _impl->theme.secondaryColor
                                                  : _impl->theme.secondaryColorTransparent;
  }
}

QColor const& QlementineStyle::tabBarScrollButtonBackgroundColor(MouseState const mouse) const {
  switch (mouse) {
    case MouseState::Pressed:
      return _impl->theme.semiTransparentColor4;
    case MouseState::Hovered:
      return _impl->theme.semiTransparentColor2;
    case MouseState::Normal:
    case MouseState::Disabled:
    case MouseState::Transparent:
    default:
      return _impl->theme.semiTransparentColorTransparent;
  }
}

QColor const& QlementineStyle::progressBarGrooveColor(MouseState const mouse) const {
  return mouse == MouseState::Disabled ? _impl->theme.neutralColorDisabled : _impl->theme.neutralColor;
}

QColor const& QlementineStyle::progressBarValueColor(MouseState const mouse) const {
  return mouse == MouseState::Disabled ? _impl->theme.primaryColorDisabled : _impl->theme.primaryColor;
}

QColor const& QlementineStyle::textFieldBackgroundColor(MouseState const mouse, Status const status) const {
  Q_UNUSED(status);
  if (mouse == MouseState::Disabled)
    return _impl->theme.backgroundColorMain3;
  else
    return _impl->theme.backgroundColorMain1;
}

QColor const& QlementineStyle::textFieldBorderColor(
  MouseState const mouse, FocusState const focus, Status const status) const {
  if (mouse == MouseState::Disabled) {
    return _impl->theme.borderColorDisabled;
  } else {
    switch (status) {
      case Status::Error:
        if (focus == FocusState::Focused || mouse == MouseState::Hovered || mouse == MouseState::Pressed)
          return _impl->theme.statusColorErrorHovered;
        else
          return _impl->theme.statusColorError;
      case Status::Warning:
        if (focus == FocusState::Focused || mouse == MouseState::Hovered || mouse == MouseState::Pressed)
          return _impl->theme.statusColorWarningHovered;
        else
          return _impl->theme.statusColorWarning;
      case Status::Success:
        if (focus == FocusState::Focused || mouse == MouseState::Hovered || mouse == MouseState::Pressed)
          return _impl->theme.statusColorSuccessHovered;
        else
          return _impl->theme.statusColorSuccess;
      case Status::Info:
      case Status::Default:
      default:
        if (focus == FocusState::Focused || mouse == MouseState::Hovered || mouse == MouseState::Pressed)
          return _impl->theme.primaryColor;
        else
          return _impl->theme.borderColor;
    }
  }
}

QColor const& QlementineStyle::textFieldForegroundColor(MouseState const mouse) const {
  if (mouse == MouseState::Disabled)
    return _impl->theme.secondaryColorDisabled;
  else
    return _impl->theme.secondaryColor;
}

QColor const& QlementineStyle::sliderGrooveColor(MouseState const mouse) const {
  if (mouse == MouseState::Disabled)
    return _impl->theme.neutralColorDisabled;
  else
    return _impl->theme.neutralColor;
}

QColor const& QlementineStyle::sliderValueColor(MouseState const mouse) const {
  if (mouse == MouseState::Disabled)
    return _impl->theme.primaryColorDisabled;
  else
    return _impl->theme.primaryColor;
}

QColor const& QlementineStyle::sliderHandleColor(MouseState const mouse) const {
  if (mouse == MouseState::Disabled)
    return _impl->theme.neutralColorDisabled;
  else if (mouse == MouseState::Pressed)
    return _impl->theme.primaryColorForegroundPressed;
  else if (mouse == MouseState::Hovered)
    return _impl->theme.primaryColorForegroundHovered;
  else
    return _impl->theme.primaryColorForeground;
}

QColor const& QlementineStyle::sliderTickColor(MouseState const mouse) const {
  if (mouse == MouseState::Disabled)
    return _impl->theme.borderColorDisabled;
  else
    return _impl->theme.borderColor;
}

QColor const& QlementineStyle::dialHandleColor(MouseState const mouse) const {
  if (mouse == MouseState::Disabled)
    return _impl->theme.neutralColorDisabled;
  else
    return _impl->theme.neutralColor;
}

QColor const& QlementineStyle::dialGrooveColor(MouseState const mouse) const {
  if (mouse == MouseState::Disabled)
    return _impl->theme.neutralColorDisabled;
  else
    return _impl->theme.neutralColorPressed;
}

QColor const& QlementineStyle::dialValueColor(MouseState const mouse) const {
  if (mouse == MouseState::Disabled)
    return _impl->theme.primaryColorDisabled;
  else
    return _impl->theme.primaryColor;
}

QColor const& QlementineStyle::dialTickColor(MouseState const mouse) const {
  if (mouse == MouseState::Disabled)
    return _impl->theme.neutralColorDisabled;
  else
    return _impl->theme.neutralColorPressed;
}

QColor const& QlementineStyle::dialMarkColor(MouseState const mouse) const {
  if (mouse == MouseState::Disabled)
    return _impl->theme.secondaryColorDisabled;
  else
    return _impl->theme.secondaryColor;
}

QColor const& QlementineStyle::dialBackgroundColor(MouseState const mouse) const {
  if (mouse == MouseState::Disabled)
    return _impl->theme.neutralColorDisabled;
  else
    return _impl->theme.neutralColorPressed;
}

QColor const& QlementineStyle::labelForegroundColor(MouseState const mouse, const QWidget* w) const {
  Q_UNUSED(w);
  if (mouse == MouseState::Disabled)
    return _impl->theme.secondaryColorDisabled;
  else
    return _impl->theme.secondaryColor;
}

QColor const& QlementineStyle::labelCaptionForegroundColor(MouseState const mouse) const {
  if (mouse == MouseState::Disabled)
    return _impl->theme.secondaryAlternativeColorDisabled;
  else
    return _impl->theme.secondaryAlternativeColor;
}

QColor const& QlementineStyle::iconForegroundColor(MouseState const mouse, ColorRole const role) const {
  if (mouse == MouseState::Disabled)
    return role == ColorRole::Primary ? _impl->theme.primaryColorForegroundDisabled
                                      : _impl->theme.secondaryColorForegroundDisabled;
  else
    return role == ColorRole::Primary ? _impl->theme.primaryColorForeground : _impl->theme.secondaryColorForeground;
}

QColor const& QlementineStyle::toolBarBackgroundColor() const {
  return _impl->theme.backgroundColorMain2;
}

QColor const& QlementineStyle::toolBarBorderColor() const {
  return _impl->theme.borderColor;
}

QColor const& QlementineStyle::toolBarSeparatorColor() const {
  return _impl->theme.secondaryColorDisabled;
}

QColor const& QlementineStyle::toolTipBackgroundColor() const {
  return _impl->toolTipMatchesTheme ? _impl->theme.backgroundColorMain1 : _impl->theme.secondaryColor;
}

QColor const& QlementineStyle::toolTipBorderColor() const {
  return _impl->toolTipMatchesTheme ? _impl->theme.borderColor : _impl->theme.secondaryColorPressed;
}

QColor const& QlementineStyle::toolTipForegroundColor() const {
  return _impl->toolTipMatchesTheme ? _impl->theme.secondaryColor : _impl->theme.secondaryColorForeground;
}

QColor const& QlementineStyle::scrollBarGrooveColor(MouseState const mouse) const {
  switch (mouse) {
    case MouseState::Hovered:
    case MouseState::Pressed:
      return _impl->theme.semiTransparentColor4;
    default:
      return _impl->theme.semiTransparentColorTransparent;
  }
}

QColor const& QlementineStyle::scrollBarHandleColor(MouseState const mouse) const {
  switch (mouse) {
    case MouseState::Hovered:
      return _impl->theme.secondaryAlternativeColorHovered;
    case MouseState::Pressed:
      return _impl->theme.secondaryAlternativeColorPressed;
    case MouseState::Disabled:
      return _impl->theme.semiTransparentColor1;
    case MouseState::Normal:
    case MouseState::Transparent:
    default:
      return _impl->theme.semiTransparentColor4;
  }
}

int QlementineStyle::getScrollBarThickness(MouseState const mouse) const {
  switch (mouse) {
    case MouseState::Hovered:
    case MouseState::Pressed:
      return _impl->theme.scrollBarThicknessFull;
    default:
      return _impl->theme.scrollBarThicknessSmall;
  }
}

QColor const& QlementineStyle::groupBoxTitleColor(MouseState const mouse, const QWidget* w) const {
  return labelForegroundColor(mouse, w);
}

QColor QlementineStyle::groupBoxBackgroundColor(MouseState const mouse) const {
  if (mouse == MouseState::Disabled) {
    return _impl->theme.backgroundColorMainTransparent;
  } else {
    return getColorSourceOver(_impl->theme.backgroundColorMain2,
      colorWithAlphaF(_impl->theme.backgroundColorMain3, _impl->theme.backgroundColorMain3.alphaF() * .75));
  }
}

QColor const& QlementineStyle::groupBoxBorderColor(MouseState const mouse) const {
  return mouse == MouseState::Disabled ? _impl->theme.borderColorDisabled : _impl->theme.borderColor;
}

QColor const& QlementineStyle::statusColor(Status const status, MouseState const mouse) const {
  switch (status) {
    case Status::Success:
      switch (mouse) {
        case MouseState::Disabled:
          return _impl->theme.statusColorSuccessDisabled;
        case MouseState::Pressed:
          return _impl->theme.statusColorSuccessPressed;
        case MouseState::Hovered:
          return _impl->theme.statusColorSuccessHovered;
        default:
          return _impl->theme.statusColorSuccess;
      }
    case Status::Warning:
      switch (mouse) {
        case MouseState::Disabled:
          return _impl->theme.statusColorWarningDisabled;
        case MouseState::Pressed:
          return _impl->theme.statusColorWarningPressed;
        case MouseState::Hovered:
          return _impl->theme.statusColorWarningHovered;
        default:
          return _impl->theme.statusColorWarning;
      }
    case Status::Error:
      switch (mouse) {
        case MouseState::Disabled:
          return _impl->theme.statusColorErrorDisabled;
        case MouseState::Pressed:
          return _impl->theme.statusColorErrorPressed;
        case MouseState::Hovered:
          return _impl->theme.statusColorErrorHovered;
        default:
          return _impl->theme.statusColorError;
      }
    case Status::Default:
    case Status::Info:
    default:
      switch (mouse) {
        case MouseState::Disabled:
          return _impl->theme.statusColorInfoDisabled;
        case MouseState::Pressed:
          return _impl->theme.statusColorInfoPressed;
        case MouseState::Hovered:
          return _impl->theme.statusColorInfoHovered;
        default:
          return _impl->theme.statusColorInfo;
      }
  }
}

QColor const& QlementineStyle::statusColorForeground(const Status, const MouseState mouse) const {
  switch (mouse) {
    case MouseState::Disabled:
      return _impl->theme.statusColorForegroundDisabled;
    case MouseState::Pressed:
      return _impl->theme.statusColorForegroundPressed;
    case MouseState::Hovered:
      return _impl->theme.statusColorForegroundHovered;
    default:
      return _impl->theme.statusColorForeground;
  }
}

QColor QlementineStyle::focusBorderColor(Status status) const {
  if (status == Status::Default)
    return _impl->theme.focusColor;

  const auto& statusColor = this->statusColor(status, MouseState::Normal);
  const auto focusAlpha = _impl->theme.focusColor.alpha();
  auto statusFocusColor = QColor{
    statusColor.red(),
    statusColor.green(),
    statusColor.blue(),
    focusAlpha,
  };
  statusFocusColor = statusFocusColor.lighter(110);

  return statusFocusColor;
}

QColor const& QlementineStyle::frameBorderColor() const {
  return _impl->theme.borderColorDisabled;
}

QColor const& QlementineStyle::separatorColor() const {
  return _impl->theme.borderColor;
}

const QFont& QlementineStyle::fontForTextRole(TextRole role) const {
  switch (role) {
    case TextRole::Caption:
      return _impl->theme.fontCaption;
    case TextRole::H1:
      return _impl->theme.fontH1;
    case TextRole::H2:
      return _impl->theme.fontH2;
    case TextRole::H3:
      return _impl->theme.fontH3;
    case TextRole::H4:
      return _impl->theme.fontH4;
    case TextRole::H5:
      return _impl->theme.fontH5;
    default:
      return _impl->theme.fontRegular;
  }
}

QPalette QlementineStyle::paletteForTextRole(TextRole role) const {
  auto result = QPalette{ _impl->theme.palette };

  const auto& textColor = colorForTextRole(role, MouseState::Normal);
  const auto& textColorDisabled = colorForTextRole(role, MouseState::Disabled);

  result.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Text, textColor);
  result.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::WindowText, textColor);
  result.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::BrightText, textColor);

  result.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, textColorDisabled);
  result.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, textColorDisabled);
  result.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, textColorDisabled);

  return result;
}

QColor const& QlementineStyle::switchGrooveColor(MouseState const mouse, CheckState const checked) const {
  return checkButtonBackgroundColor(mouse, checked);
}

QColor const& QlementineStyle::switchGrooveBorderColor(
  MouseState const mouse, FocusState const focus, CheckState const checked) const {
  return checkButtonBorderColor(mouse, focus, checked);
}

QColor const& QlementineStyle::switchHandleColor(MouseState const mouse, CheckState const checked) const {
  const auto primary = checked != CheckState::NotChecked;

  switch (mouse) {
    case MouseState::Pressed:
      return primary ? _impl->theme.primaryColorForegroundPressed : _impl->theme.secondaryColorPressed;
    case MouseState::Hovered:
      return primary ? _impl->theme.primaryColorForegroundHovered : _impl->theme.secondaryColorHovered;
    case MouseState::Disabled:
      return primary ? _impl->theme.primaryColorForegroundDisabled : _impl->theme.secondaryColorDisabled;
    case MouseState::Transparent:
    case MouseState::Normal:
    default:
      return primary ? _impl->theme.primaryColorForeground : _impl->theme.secondaryColor;
  }
}

QColor const& QlementineStyle::tableHeaderBgColor(MouseState const mouse, CheckState const checked) const {
  Q_UNUSED(checked)

  switch (mouse) {
    case MouseState::Pressed:
      return _impl->theme.neutralColorHovered;
    case MouseState::Hovered:
      return _impl->theme.neutralColor;
    case MouseState::Disabled:
      return _impl->theme.neutralColor;
    case MouseState::Transparent:
    case MouseState::Normal:
    default:
      return _impl->theme.backgroundColorMain3;
  }
}

QColor const& QlementineStyle::tableHeaderFgColor(MouseState const mouse, CheckState const) const {
  switch (mouse) {
    case MouseState::Disabled:
      return _impl->theme.secondaryColorDisabled;
    default:
      return _impl->theme.secondaryColor;
  }
}

QColor const& QlementineStyle::tableLineColor() const {
  return _impl->theme.borderColor;
}

QColor const& QlementineStyle::colorForTextRole(TextRole role, MouseState const mouse) const {
  switch (role) {
    case TextRole::Caption:
      return mouse == MouseState::Disabled ? _impl->theme.secondaryAlternativeColorDisabled
                                           : _impl->theme.secondaryAlternativeColor;
    case TextRole::H1:
    case TextRole::H2:
    case TextRole::H3:
    case TextRole::H4:
    case TextRole::H5:
    case TextRole::Default:
    default:
      return mouse == MouseState::Disabled ? _impl->theme.secondaryColorDisabled : _impl->theme.secondaryColor;
  }
}

int QlementineStyle::pixelSizeForTextRole(TextRole role) const {
  switch (role) {
    case TextRole::Caption:
      return _impl->theme.fontSizeS1;
    case TextRole::H1:
      return _impl->theme.fontSizeH1;
    case TextRole::H2:
      return _impl->theme.fontSizeH2;
    case TextRole::H3:
      return _impl->theme.fontSizeH3;
    case TextRole::H4:
      return _impl->theme.fontSizeH4;
    case TextRole::H5:
      return _impl->theme.fontSizeH5;
    default:
      return _impl->theme.fontSize;
  }
}

Status QlementineStyle::widgetStatus(QWidget const* widget) const {
  if (!widget) {
    return Status::Default;
  }

  // QFocusFrame: delegate to the focused widget.
  if (const auto* focusFrame = qobject_cast<const QFocusFrame*>(widget)) {
    if (const auto* focusedWidget = focusFrame->widget()) {
      return widgetStatus(focusedWidget);
    }
    return Status::Default;
  }

  // LineEdit custom widget.
  if (const auto* qlementineLineEdit = qobject_cast<const qlementine::LineEdit*>(widget)) {
    return qlementineLineEdit->status();
  }

  // PlainTextEdit custom widget.
  if (const auto* qlementineTextEdit = qobject_cast<const qlementine::PlainTextEdit*>(widget)) {
    return qlementineTextEdit->status();
  }

  // Generic widget with Qt property.
  if (const auto statusVariant = widget->property(Property_Status);
    statusVariant.isValid() && statusVariant.canConvert<Status>()) {
    return statusVariant.value<Status>();
  }

  return Status::Default;
}

void QlementineStyle::setWidgetStatus(QWidget* widget, Status status) {
  if (!widget) {
    return;
  }

  // LineEdit custom widget.
  if (auto* qlementineLineEdit = qobject_cast<qlementine::LineEdit*>(widget)) {
    qlementineLineEdit->setStatus(status);
    return;
  }

  // PlainTextEdit custom widget.
  if (auto* qlementineTextEdit = qobject_cast<qlementine::PlainTextEdit*>(widget)) {
    qlementineTextEdit->setStatus(status);
    return;
  }

  // Generic widget: use Qt property.
  const auto currentStatus = widget->property(Property_Status);
  const auto currentStatusValue =
    currentStatus.isValid() && currentStatus.canConvert<Status>() ? currentStatus.value<Status>() : Status::Default;

  if (currentStatusValue != status) {
    if (status == Status::Default) {
      widget->setProperty(Property_Status, QVariant());
    } else {
      widget->setProperty(Property_Status, QVariant::fromValue(status));
    }
    widget->update();
  }
}

QColor const& QlementineStyle::statusBarBackgroundColor() const {
  return _impl->theme.backgroundColorMain2;
}

QColor const& QlementineStyle::statusBarBorderColor() const {
  return _impl->theme.borderColor;
}

QColor const& QlementineStyle::statusBarSeparatorColor() const {
  return _impl->theme.secondaryColorDisabled;
}

QColor const& QlementineStyle::splitterColor(const MouseState mouse) const {
  switch (mouse) {
    case MouseState::Normal:
      return _impl->theme.borderColor;
    case MouseState::Hovered:
      return _impl->theme.primaryColor;
    case MouseState::Pressed:
      return _impl->theme.primaryColorPressed;
    case MouseState::Disabled:
    default:
      return _impl->theme.borderColorTransparent;
  }
}
} // namespace oclero::qlementine
