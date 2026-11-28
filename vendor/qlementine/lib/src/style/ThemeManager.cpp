// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/style/ThemeManager.hpp>

#include "../utils/SystemAccentColor.hpp"

#include <QDir>
#include <QEvent>
#include <QGuiApplication>
#include <QPalette>
#include <QStyleHints>

namespace oclero::qlementine {
ThemeManager::ThemeManager(QObject* parent)
  : ThemeManager(nullptr, parent) {}

ThemeManager::ThemeManager(QlementineStyle* style, QObject* parent)
  : QObject(parent)
  , _systemAccentColor(QGuiApplication::palette().color(QPalette::Accent)) {
  qApp->installEventFilter(this);
  connect(&SystemAccentWatcher::instance(), &SystemAccentWatcher::changed, this, &ThemeManager::onSystemAccentChanged);
  connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, this, &ThemeManager::onSystemAccentChanged);
  connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, this, &ThemeManager::systemColorSchemeChanged);
  setStyle(style);
}

QlementineStyle* ThemeManager::style() const {
  return _style;
}

void ThemeManager::setStyle(QlementineStyle* style) {
  if (style != _style) {
    _style = style;
    synchronizeThemeOnStyle();
    Q_EMIT currentThemeChanged();
    Q_EMIT themeCountChanged();
  }
}

const std::vector<Theme>& ThemeManager::themes() const {
  return _themes;
}

void ThemeManager::addTheme(const Theme& theme) {
  _themes.emplace_back(theme);
  Q_EMIT themeCountChanged();
  if (_currentIndex < 0) {
    setCurrentThemeIndex(0);
  }
}

void ThemeManager::loadDirectory(const QString& path) {
  QDir dir(path);
  if (!dir.exists())
    return;

  dir.setFilter(QDir::Filter::Files | QDir::Filter::NoDotAndDotDot);
  dir.setSorting(QDir::SortFlag::Name | QDir::SortFlag::IgnoreCase);
  const auto files = dir.entryInfoList();
  for (const auto& file : files) {
    QFileInfo fileInfo(file);
    if (fileInfo.suffix().toLower() == QStringLiteral("json")) {
      const auto themeOpt = Theme::fromJsonPath(file.absoluteFilePath());
      if (themeOpt.has_value()) {
        addTheme(themeOpt.value());
      }
    }
  }
}

QString ThemeManager::currentTheme() const {
  if (_currentIndex > -1 && _currentIndex < themeCount()) {
    return _themes.at(_currentIndex).meta.name;
  }
  return {};
}

void ThemeManager::setCurrentTheme(const QString& key) {
  const auto index = themeIndex(key);
  setCurrentThemeIndex(index);
}

int ThemeManager::currentThemeIndex() const {
  return _currentIndex;
}

void ThemeManager::setCurrentThemeIndex(int index) {
  if (index > -1 && index < themeCount()) {
    if (index != _currentIndex) {
      _currentIndex = index;
      synchronizeThemeOnStyle();
      Q_EMIT currentThemeChanged();
    }
  }
}

bool ThemeManager::useSystemAccentColor() const {
  return _useSystemAccentColor;
}

void ThemeManager::setUseSystemAccentColor(bool useSystemAccentColor) {
  if (useSystemAccentColor != _useSystemAccentColor) {
    _useSystemAccentColor = useSystemAccentColor;
    synchronizeThemeOnStyle();
    Q_EMIT useSystemAccentColorChanged();
  }
}

int ThemeManager::themeCount() const {
  return static_cast<int>(_themes.size());
}

void ThemeManager::setNextTheme() {
  if (themeCount() > 1) {
    // Wrap.
    const auto next = (_currentIndex + 1) % themeCount();
    setCurrentThemeIndex(next);
  }
}

void ThemeManager::setPreviousTheme() {
  if (themeCount() > 1) {
    // Wrap.
    auto previous = _currentIndex - 1;
    if (previous < 0)
      previous = themeCount() - 1;
    setCurrentThemeIndex(previous);
  }
}

int ThemeManager::themeIndex(const QString& key) const {
  const auto it = std::find_if(_themes.begin(), _themes.end(), [&key](const auto& theme) {
    return theme.meta.name == key;
  });
  if (it != _themes.end())
    return static_cast<int>(std::distance(_themes.begin(), it));
  return -1;
}

bool ThemeManager::eventFilter(QObject* watched, QEvent* event) {
  if (watched == qApp && (event->type() == QEvent::ApplicationPaletteChange || event->type() == QEvent::ThemeChange)) {
    onSystemAccentChanged();
  }
  return QObject::eventFilter(watched, event);
}

void ThemeManager::onSystemAccentChanged() {
  if (!_useSystemAccentColor || _currentIndex < 0 || _currentIndex >= themeCount()) {
    return;
  }
  if (resolveAccentColor(_themes.at(_currentIndex)) != _appliedAccentColor) {
    synchronizeThemeOnStyle();
  }
}

QColor ThemeManager::resolveAccentColor(const Theme& theme) const {
  const auto isDarkTheme = theme.backgroundColorMain1.lightnessF() < 0.5;
  const auto nativeAccentColor = systemAccentColor(isDarkTheme);
  return nativeAccentColor.isValid() ? nativeAccentColor : _systemAccentColor;
}

void ThemeManager::synchronizeThemeOnStyle() {
  if (!_style)
    return;

  if (_themes.empty())
    return;

  if (_currentIndex >= 0 && _currentIndex < themeCount()) {
    const auto& theme = _themes.at(_currentIndex);
    if (_useSystemAccentColor) {
      _appliedAccentColor = resolveAccentColor(theme);
      _style->setTheme(theme.withAccentColor(_appliedAccentColor));
    } else {
      _appliedAccentColor = QColor();
      _style->setTheme(theme);
    }
  } else {
    addTheme(_style->theme());
    setCurrentThemeIndex(themeCount() - 1);
  }
}
} // namespace oclero::qlementine
