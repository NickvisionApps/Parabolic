// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include "SystemAccentColor.hpp"

#include <QGuiApplication>
#include <QStyleHints>

#ifdef _WIN32
#  include <windows.h>
#elif defined(QLEMENTINE_WITH_DBUS)
#  include <QDBusArgument>
#  include <QDBusConnection>
#  include <QDBusMessage>
#  include <QDBusVariant>
#endif

namespace oclero::qlementine {
QColor systemAccentColor(bool dark) {
#ifdef _WIN32
  constexpr auto entryCount = 8;
  constexpr auto entrySize = 4;
  constexpr auto lightIndex = 1;
  constexpr auto darkIndex = 4;
  BYTE palette[entryCount * entrySize] = {};
  DWORD size = sizeof(palette);
  const auto status = RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent",
    L"AccentPalette", RRF_RT_REG_BINARY, nullptr, palette, &size);
  if (status == ERROR_SUCCESS && size >= sizeof(palette)) {
    const auto offset = (dark ? lightIndex : darkIndex) * entrySize;
    return QColor(palette[offset], palette[offset + 1], palette[offset + 2]);
  }
#elif defined(QLEMENTINE_WITH_DBUS)
  Q_UNUSED(dark);
  if (QDBusConnection::sessionBus().isConnected()) {
    auto message = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.portal.Desktop"),
      QStringLiteral("/org/freedesktop/portal/desktop"), QStringLiteral("org.freedesktop.portal.Settings"),
      QStringLiteral("Read"));
    message << QStringLiteral("org.freedesktop.appearance") << QStringLiteral("accent-color");
    const auto reply = QDBusConnection::sessionBus().call(message, QDBus::Block, 250);
    if (reply.type() == QDBusMessage::ReplyMessage && !reply.arguments().isEmpty()) {
      auto value = reply.arguments().constFirst();
      while (value.metaType() == QMetaType::fromType<QDBusVariant>()) {
        value = value.value<QDBusVariant>().variant();
      }
      if (value.metaType() == QMetaType::fromType<QDBusArgument>()) {
        const auto argument = value.value<QDBusArgument>();
        auto r = -1.;
        auto g = -1.;
        auto b = -1.;
        argument.beginStructure();
        argument >> r >> g >> b;
        argument.endStructure();
        if (r >= 0. && r <= 1. && g >= 0. && g <= 1. && b >= 0. && b <= 1.) {
          return QColor::fromRgbF(r, g, b);
        }
      }
    }
  }
#else
  Q_UNUSED(dark);
#endif
  return {};
}

Qt::ColorScheme querySystemColorScheme() {
#ifdef _WIN32
  DWORD usesLightTheme = 1;
  DWORD size = sizeof(usesLightTheme);
  const auto status = RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
    L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &usesLightTheme, &size);
  if (status == ERROR_SUCCESS) {
    return usesLightTheme == 0 ? Qt::ColorScheme::Dark : Qt::ColorScheme::Light;
  }
#elif defined(QLEMENTINE_WITH_DBUS)
  if (QDBusConnection::sessionBus().isConnected()) {
    auto message = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.portal.Desktop"),
      QStringLiteral("/org/freedesktop/portal/desktop"), QStringLiteral("org.freedesktop.portal.Settings"),
      QStringLiteral("Read"));
    message << QStringLiteral("org.freedesktop.appearance") << QStringLiteral("color-scheme");
    const auto reply = QDBusConnection::sessionBus().call(message, QDBus::Block, 250);
    if (reply.type() == QDBusMessage::ReplyMessage && !reply.arguments().isEmpty()) {
      auto value = reply.arguments().constFirst();
      while (value.metaType() == QMetaType::fromType<QDBusVariant>()) {
        value = value.value<QDBusVariant>().variant();
      }
      switch (value.toUInt()) {
        case 1:
          return Qt::ColorScheme::Dark;
        case 2:
          return Qt::ColorScheme::Light;
        default:
          break;
      }
    }
  }
#endif
  return QGuiApplication::styleHints()->colorScheme();
}

SystemAccentWatcher& SystemAccentWatcher::instance() {
  static SystemAccentWatcher watcher;
  return watcher;
}

SystemAccentWatcher::SystemAccentWatcher()
  : QObject(nullptr) {
#ifdef QLEMENTINE_WITH_DBUS
  QDBusConnection::sessionBus().connect(QStringLiteral("org.freedesktop.portal.Desktop"),
    QStringLiteral("/org/freedesktop/portal/desktop"), QStringLiteral("org.freedesktop.portal.Settings"),
    QStringLiteral("SettingChanged"), this, SLOT(onSettingChanged(QString, QString, QDBusVariant)));
#endif
}

#ifdef QLEMENTINE_WITH_DBUS
void SystemAccentWatcher::onSettingChanged(const QString& settingNamespace, const QString& key, const QDBusVariant&) {
  if (settingNamespace == QLatin1String("org.freedesktop.appearance")
      && (key == QLatin1String("accent-color") || key == QLatin1String("color-scheme"))) {
    Q_EMIT changed();
  }
}
#endif
} // namespace oclero::qlementine
