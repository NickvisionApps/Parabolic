// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <oclero/qlementine/widgets/TitleBar.hpp>

#include <QFileSystemWatcher>
#include <QObject>
#include <QString>
#include <QTimer>

#ifdef QLEMENTINE_WITH_DBUS
class QDBusVariant;
#endif

namespace oclero::qlementine {
/// Resolves the caption button layout preferred by the user and watches it for changes.
/// GNOME and other desktops use the settings portal (org.gnome.desktop.wm.preferences/button-layout),
/// KDE uses ButtonsOnLeft and ButtonsOnRight from kwinrc. The default is `appmenu:close` on GNOME, and minimize,
/// maximize and close on the right on other desktops. Without QtDBus, the portal is not used.
/// A single instance is shared by every window.
class LinuxButtonLayout : public QObject {
  Q_OBJECT

public:
  /// Created on first use and destroyed with the QCoreApplication.
  static LinuxButtonLayout& instance();

  CaptionButtonLayout layout() const;
  bool isGnome() const;

Q_SIGNALS:
  void changed();

private:
  explicit LinuxButtonLayout(QObject* parent);

#ifdef QLEMENTINE_WITH_DBUS
  Q_SLOT void onSettingChanged(const QString& settingNamespace, const QString& key, const QDBusVariant& value);
#endif
  void refresh();

  bool _isGnome{ false };
  bool _isKde{ false };
  QString _kwinrcPath;
  CaptionButtonLayout _layout;
  QFileSystemWatcher _watcher;
  QTimer _debounceTimer;
};
} // namespace oclero::qlementine
