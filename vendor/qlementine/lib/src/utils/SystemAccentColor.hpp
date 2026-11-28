// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <QColor>
#include <QObject>

#ifdef QLEMENTINE_WITH_DBUS
#  include <QDBusVariant>
#endif

namespace oclero::qlementine {
QColor systemAccentColor(bool dark);
Qt::ColorScheme querySystemColorScheme();

class SystemAccentWatcher : public QObject {
  Q_OBJECT

public:
  static SystemAccentWatcher& instance();

Q_SIGNALS:
  void changed();

private:
  SystemAccentWatcher();

#ifdef QLEMENTINE_WITH_DBUS
  Q_SLOT void onSettingChanged(const QString& settingNamespace, const QString& key, const QDBusVariant& value);
#endif
};
} // namespace oclero::qlementine
