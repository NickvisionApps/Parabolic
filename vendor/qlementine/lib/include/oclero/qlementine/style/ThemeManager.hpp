// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <QObject>
#include <QColor>
#include <QPointer>

#include <vector>

#include <oclero/qlementine/qlementine_export.h>
#include <oclero/qlementine/style/Theme.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>

namespace oclero::qlementine {
class QLEMENTINE_EXPORT ThemeManager : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString currentTheme READ currentTheme WRITE setCurrentTheme NOTIFY currentThemeChanged)
  Q_PROPERTY(int themeCount READ themeCount NOTIFY themeCountChanged)
  Q_PROPERTY(bool useSystemAccentColor READ useSystemAccentColor WRITE setUseSystemAccentColor NOTIFY
               useSystemAccentColorChanged)

public:
  explicit ThemeManager(QObject* parent = nullptr);
  explicit ThemeManager(QlementineStyle* style, QObject* parent = nullptr);
  ~ThemeManager() override = default;

  QlementineStyle* style() const;
  void setStyle(QlementineStyle* style);

  const std::vector<Theme>& themes() const;
  void addTheme(const Theme& theme);

  void loadDirectory(const QString& path);

  QString currentTheme() const;
  void setCurrentTheme(const QString& key);

  int themeCount() const;

  /// Whether the primary colors of the current theme are overridden by the system accent color.
  /// The accent color is captured when the ThemeManager is created, before any Qlementine palette is applied.
  bool useSystemAccentColor() const;
  void setUseSystemAccentColor(bool useSystemAccentColor);

  Q_SLOT void setNextTheme();
  Q_SLOT void setPreviousTheme();

  int themeIndex(const QString& key) const;

  int currentThemeIndex() const;
  void setCurrentThemeIndex(int index);

Q_SIGNALS:
  void currentThemeChanged();
  void themeCountChanged();
  void useSystemAccentColorChanged();

private:
  void synchronizeThemeOnStyle();

private:
  std::vector<Theme> _themes;
  QPointer<QlementineStyle> _style{ nullptr };
  QColor _systemAccentColor;
  bool _useSystemAccentColor{ false };
  int _currentIndex{ -1 };
};
} // namespace oclero::qlementine
