// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include "LinuxButtonLayout.hpp"

#ifdef QLEMENTINE_WITH_DBUS
#  include <QDBusConnection>
#  include <QDBusMessage>
#  include <QDBusVariant>
#endif
#include <QCoreApplication>
#include <QFileInfo>
#include <QGuiApplication>
#include <QPointer>
#include <QSettings>
#include <QStandardPaths>

#include <utility>

namespace oclero::qlementine {
namespace {
#ifdef QLEMENTINE_WITH_DBUS
const auto portalService = QStringLiteral("org.freedesktop.portal.Desktop");
const auto portalPath = QStringLiteral("/org/freedesktop/portal/desktop");
const auto portalInterface = QStringLiteral("org.freedesktop.portal.Settings");
const auto gnomeNamespace = QStringLiteral("org.gnome.desktop.wm.preferences");
const auto gnomeKey = QStringLiteral("button-layout");
#endif

// Tokens: close, minimize, maximize. "appmenu" is ignored because current GNOME versions don't display it in the
// title bar, and unknown tokens are ignored too.
QList<CaptionButton> parseGnomeSide(const QString& side) {
  auto result = QList<CaptionButton>{};
  const auto tokens = side.split(',', Qt::SkipEmptyParts);
  for (const auto& token : tokens) {
    const auto name = token.trimmed();
    if (name == QLatin1String("close")) {
      result.append(CaptionButton::Close);
    } else if (name == QLatin1String("minimize")) {
      result.append(CaptionButton::Minimize);
    } else if (name == QLatin1String("maximize")) {
      result.append(CaptionButton::Maximize);
    }
  }
  return result;
}

// Letters: M (menu), I (minimize), A (maximize), X (close). Other buttons are ignored.
QList<CaptionButton> parseKdeButtons(const QString& letters) {
  auto result = QList<CaptionButton>{};
  for (const auto letter : letters) {
    if (letter == QLatin1Char('M')) {
      result.append(CaptionButton::Menu);
    } else if (letter == QLatin1Char('I')) {
      result.append(CaptionButton::Minimize);
    } else if (letter == QLatin1Char('A')) {
      result.append(CaptionButton::Maximize);
    } else if (letter == QLatin1Char('X')) {
      result.append(CaptionButton::Close);
    }
  }
  return result;
}
} // namespace

LinuxButtonLayout::LinuxButtonLayout(QObject* parent)
  : QObject(parent) {
  // Desktop detection.
  const auto desktops = qEnvironmentVariable("XDG_CURRENT_DESKTOP").toUpper().split(':', Qt::SkipEmptyParts);
  _isGnome = desktops.contains(QLatin1String("GNOME"));
  _isKde = desktops.contains(QLatin1String("KDE"));

  if (_isKde) {
    // KDE saves kwinrc with an atomic rename, so the file is added again after each change.
    _kwinrcPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QStringLiteral("/kwinrc");
    _debounceTimer.setSingleShot(true);
    _debounceTimer.setInterval(150);
    connect(&_debounceTimer, &QTimer::timeout, this, [this]() {
      if (!_watcher.files().contains(_kwinrcPath)) {
        _watcher.addPath(_kwinrcPath);
      }
      refresh();
    });
    connect(&_watcher, &QFileSystemWatcher::directoryChanged, &_debounceTimer, qOverload<>(&QTimer::start));
    connect(&_watcher, &QFileSystemWatcher::fileChanged, &_debounceTimer, qOverload<>(&QTimer::start));
    _watcher.addPath(QFileInfo(_kwinrcPath).absolutePath());
    _watcher.addPath(_kwinrcPath);
  }
#ifdef QLEMENTINE_WITH_DBUS
  else {
    QDBusConnection::sessionBus().connect(portalService, portalPath, portalInterface, QStringLiteral("SettingChanged"),
      this, SLOT(onSettingChanged(QString, QString, QDBusVariant)));
  }
#endif

  refresh();
}

LinuxButtonLayout& LinuxButtonLayout::instance() {
  static QPointer<LinuxButtonLayout> sharedInstance;
  if (!sharedInstance) {
    sharedInstance = new LinuxButtonLayout(QCoreApplication::instance());
  }
  return *sharedInstance;
}

CaptionButtonLayout LinuxButtonLayout::layout() const {
  return _layout;
}

bool LinuxButtonLayout::isGnome() const {
  return _isGnome;
}

#ifdef QLEMENTINE_WITH_DBUS
void LinuxButtonLayout::onSettingChanged(const QString& settingNamespace, const QString& key, const QDBusVariant&) {
  if (settingNamespace == gnomeNamespace && key == gnomeKey) {
    refresh();
  }
}
#endif

void LinuxButtonLayout::refresh() {
  // Default: close button only on GNOME and KDE, minimize, maximize and close on other desktops. All on the right.
  auto result = CaptionButtonLayout{};
  if (_isGnome || _isKde) {
    result.right = { CaptionButton::Close };
  } else {
    result.right = { CaptionButton::Minimize, CaptionButton::Maximize, CaptionButton::Close };
  }

  if (_isKde) {
    QSettings settings(_kwinrcPath, QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("org.kde.kdecoration2"));
    result.left = parseKdeButtons(settings.value(QStringLiteral("ButtonsOnLeft"), QStringLiteral("MS")).toString());
    result.right = parseKdeButtons(settings.value(QStringLiteral("ButtonsOnRight"), QStringLiteral("HIAX")).toString());
  }
#ifdef QLEMENTINE_WITH_DBUS
  else if (QDBusConnection::sessionBus().isConnected()) {
    auto message = QDBusMessage::createMethodCall(portalService, portalPath, portalInterface, QStringLiteral("Read"));
    message << gnomeNamespace << gnomeKey;
    const auto reply = QDBusConnection::sessionBus().call(message, QDBus::Block, 250);
    if (reply.type() == QDBusMessage::ReplyMessage && !reply.arguments().isEmpty()) {
      // The value can be wrapped in several variants.
      auto value = reply.arguments().constFirst();
      while (value.metaType() == QMetaType::fromType<QDBusVariant>()) {
        value = value.value<QDBusVariant>().variant();
      }
      const auto text = value.toString();
      if (!text.isEmpty()) {
        // Without a colon, GNOME puts every button on the left.
        const auto sides = text.split(':');
        result.left = parseGnomeSide(sides.at(0));
        result.right = sides.size() > 1 ? parseGnomeSide(sides.at(1)) : QList<CaptionButton>{};
      }
    }
  }
#endif

  if (QGuiApplication::isRightToLeft()) {
    std::swap(result.left, result.right);
  }

  if (result.left != _layout.left || result.right != _layout.right) {
    _layout = result;
    Q_EMIT changed();
  }
}
} // namespace oclero::qlementine
