#if (defined(_WIN32) && !defined(_CRT_SECURE_NO_WARNINGS))
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef MAINWINDOWCONTROLLER_H
#define MAINWINDOWCONTROLLER_H

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <libnick/app/appinfo.h>
#include <libnick/app/datafilemanager.h>
#include <libnick/app/windowgeometry.h>
#include <libnick/events/event.h>
#include <libnick/events/parameventargs.h>
#include <libnick/keyring/keyring.h>
#include <libnick/logging/logger.h>
#include <libnick/network/networkmonitor.h>
#include <libnick/notifications/notificationsenteventargs.h>
#include <libnick/notifications/shellnotificationsenteventargs.h>
#include <libnick/system/suspendinhibitor.h>
#include <libnick/taskbar/taskbaritem.h>
#include <libnick/update/updater.h>
#include "controllers/adddownloaddialogcontroller.h"
#include "controllers/keyringdialogcontroller.h"
#include "controllers/preferencesviewcontroller.h"
#include "models/downloadmanager.h"
#include "models/historicdownload.h"
#include "models/theme.h"

namespace Nickvision::TubeConverter::Shared::Controllers
{
    /**
     * @brief A controller for a MainWindow.
     */
    class MainWindowController
    {
    public:
        /**
         * @brief Constructs a MainWindowController.
         * @param args A list of argument strings for the application
         */
        MainWindowController(const std::vector<std::string>& args);
        /**
         * @brief Gets the Saved event for the application's configuration.
         * @return The configuration Saved event
         */
        Nickvision::Events::Event<Nickvision::Events::EventArgs>& configurationSaved();
        /**
         * @brief Gets the event for when a notification is sent.
         * @return The notification sent event
         */
        Nickvision::Events::Event<Nickvision::Notifications::NotificationSentEventArgs>& notificationSent();
        /**
         * @brief Gets the event for when a shell notification is sent.
         * @return The shell notification sent event
         */
        Nickvision::Events::Event<Nickvision::Notifications::ShellNotificationSentEventArgs>& shellNotificationSent();
        /**
         * @brief Gets the event for when the disclaimer is triggered.
         * @return The disclaimer triggered event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<std::string>>& disclaimerTriggered();
        /**
         * @brief Gets the event for when the ability to download is changed.
         * @return The download ability changed event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<bool>>& downloadAbilityChanged();
        /**
         * @brief Gets the AppInfo object for the application
         * @return The current AppInfo object
         */
        const Nickvision::App::AppInfo& getAppInfo() const;
        /**
         * @brief Gets the DownloadManager for the application.
         * @return The DownloadManager
         */
        Models::DownloadManager& getDownloadManager();
        /**
         * @brief Gets the preferred theme for the application.
         * @return The preferred theme
         */
        Models::Theme getTheme();
        /**
         * @brief Sets whether or not to show the disclaimer on startup.
         * @param showDisclaimerOnStartup True to show the disclaimer, else false
         */
        void setShowDisclaimerOnStartup(bool showDisclaimerOnStartup);
        /**
         * @brief Gets the debugging information for the application.
         * @param extraInformation Extra, ui-specific, information to include in the debug info statement
         * @return The application's debug information
         */
        std::string getDebugInformation(const std::string& extraInformation = "") const;
        /**
         * @brief Gets whether or not the application can be shut down.
         * @return True if can shut down, else false
         */
        bool canShutdown() const;
        /**
         * @brief Gets the help url for a specific page.
         * @param pageName The name of the page to get the help url for
         * @return The help url for the page
         */
        std::string getHelpUrl(const std::string& pageName);
        /**
         * @brief Gets whether or not a download can be started.
         * @return True if can download, else false
         */
        bool canDownload() const;
        /**
         * @brief Gets an AddDownloadDialogController.
         * @return The AddDownloadDialogController
         */
        std::shared_ptr<AddDownloadDialogController> createAddDownloadDialogController();
        /**
         * @brief Gets a KeyringDialogController.
         * @return The KeyringDialogController
         */
        std::shared_ptr<KeyringDialogController> createKeyringDialogController();
        /**
         * @brief Gets a PreferencesViewController.
         * @return The PreferencesViewController
         */
        std::shared_ptr<PreferencesViewController> createPreferencesViewController();
        /**
         * @brief Starts the application.
         * @brief Will only have an effect on the first time called.
         * @return The WindowGeometry to use for the application window at startup
         */
#ifdef _WIN32
        Nickvision::App::WindowGeometry startup(HWND hwnd);
#elif defined(__linux__)
        Nickvision::App::WindowGeometry startup(const std::string& desktopFile);
#endif
        /**
         * @brief Shuts down the application.
         * @param geometry The window geometry to save
         */
        void shutdown(const Nickvision::App::WindowGeometry& geometry);
        /**
         * @brief Checks for an application update and sends a notification if one is available.
         */
        void checkForUpdates();
#ifdef _WIN32
        /**
         * @brief Downloads and installs the latest application update in the background.
         * @brief Will send a notification if the update fails.
         * @brief MainWindowController::checkForUpdates() must be called before this method.
         */
        void windowsUpdate();
#endif
        /**
         * @brief Logs a system message.
         * @param level The severity level of the message
         * @param message The message to log
         * @param source The source location of the log message
         */
        void log(Logging::LogLevel level, const std::string& message, const std::source_location& source = std::source_location::current());

    private:
        /**
         * @brief Handles when the configuration is saved.
         */
        void onConfigurationSaved();
        /**
         * @brief Handles when the network connection state is changed.
         */
        void onNetworkStateChanged(const Network::NetworkStateChangedEventArgs& args);
        bool m_started;
        std::vector<std::string> m_args;
        Nickvision::App::AppInfo m_appInfo;
        Nickvision::App::DataFileManager m_dataFileManager;
        Nickvision::Logging::Logger m_logger;
        std::shared_ptr<Nickvision::Update::Updater> m_updater;
        Nickvision::Taskbar::TaskbarItem m_taskbar;
        Nickvision::Network::NetworkMonitor m_networkMonitor;
        Nickvision::System::SuspendInhibitor m_suspendInhibitor;
        Nickvision::Keyring::Keyring m_keyring;
        Models::DownloadManager m_downloadManager;
        Nickvision::Events::Event<Nickvision::Notifications::NotificationSentEventArgs> m_notificationSent;
        Nickvision::Events::Event<Nickvision::Notifications::ShellNotificationSentEventArgs> m_shellNotificationSent;
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<std::string>> m_disclaimerTriggered;
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<bool>> m_downloadAbilityChanged;
    };
}

#endif //MAINWINDOWCONTROLLER_H
