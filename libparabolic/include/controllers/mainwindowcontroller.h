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
#include <libnick/notifications/notificationsenteventargs.h>
#include <libnick/notifications/shellnotificationsenteventargs.h>
#include <libnick/system/suspendinhibitor.h>
#include <libnick/taskbar/taskbaritem.h>
#include <libnick/update/updater.h>
#include <libnick/update/version.h>
#include "controllers/adddownloaddialogcontroller.h"
#include "controllers/credentialdialogcontroller.h"
#include "controllers/keyringdialogcontroller.h"
#include "controllers/preferencesviewcontroller.h"
#include "models/downloadmanager.h"
#include "models/historicdownload.h"
#include "models/startupinformation.h"
#include "models/theme.h"
#include "models/ytdlpmanager.h"

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
         * @brief Gets the event for when an application update is available.
         * @return The application update available event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<Nickvision::Update::Version>>& appUpdateAvailable();
        /**
         * @brief Gets the event for when an application update's progress is changed.
         * @return The application update progress changed event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<double>>& appUpdateProgressChanged();
        /**
         * @brief Gets the event for when the history is changed.
         * @return The history changed event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<std::vector<Models::HistoricDownload>>>& historyChanged();
        /**
         * @brief Gets the event for when a download is added.
         * @return The download added event
         */
        Nickvision::Events::Event<Events::DownloadAddedEventArgs>& downloadAdded();
        /**
         * @brief Gets the event for when a download is completed.
         * @return The download completed event
         */
        Nickvision::Events::Event<Events::DownloadCompletedEventArgs>& downloadCompleted();
        /**
         * @brief Gets the event for when a download's progress is changed.
         * @return The download progress changed event
         */
        Nickvision::Events::Event<Events::DownloadProgressChangedEventArgs>& downloadProgressChanged();
        /**
         * @brief Gets the event for when a download is stopped.
         * @return The download stopped event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<int>>& downloadStopped();
        /**
         * @brief Gets the event for when a download is paused.
         * @return The download paused event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<int>>& downloadPaused();
        /**
         * @brief Gets the event for when a download is resumed.
         * @return The download resumed event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<int>>& downloadResumed();
        /**
         * @brief Gets the event for when a download is retried.
         * @return The download retried event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<int>>& downloadRetried();
        /**
         * @brief Gets the event for when a download is started from the queue.
         * @return The download started from queue event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<int>>& downloadStartedFromQueue();
        /**
         * @brief Gets the event for when a credential is needed for a download.
         * @return The download credential needed event
         */
        Nickvision::Events::Event<Events::DownloadCredentialNeededEventArgs>& downloadCredentialNeeded();
        /**
         * @brief Gets the AppInfo object for the application
         * @return The current AppInfo object
         */
        const Nickvision::App::AppInfo& getAppInfo() const;
        /**
         * @brief Gets the preferred theme for the application.
         * @return The preferred theme
         */
        Models::Theme getTheme();
        /**
         * @brief Gets the preferred update type for the application.
         * @return The preferred update type
         */
        Update::VersionType getPreferredUpdateType();
        /**
         * @brief Sets the preferred update type for the application.
         * @param type The preferred update type
         */
        void setPreferredUpdateType(Update::VersionType type);
        /**
         * @brief Sets whether or not to show the disclaimer on startup.
         * @param showDisclaimerOnStartup True to show the disclaimer, else false
         */
        void setShowDisclaimerOnStartup(bool showDisclaimerOnStartup);
        /**
         * @brief Sets whether or not the MainWindow is active.
         * @param isWindowActive True if the MainWindow is active, else false
         */
        void setIsWindowActive(bool isWindowActive);
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
        std::string getHelpUrl(const std::string& pageName = "index");
        /**
         * @brief Gets an AddDownloadDialogController.
         * @return The AddDownloadDialogController
         */
        std::shared_ptr<AddDownloadDialogController> createAddDownloadDialogController();
        /**
         * @brief Gets a CredentialDialogController.
         * @param args The DownloadCredentialNeededEventArgs
         * @return The CredentialDialogController
         */
        std::shared_ptr<CredentialDialogController> createCredentialDialogController(const Events::DownloadCredentialNeededEventArgs& args);
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
        const Models::StartupInformation& startup(HWND hwnd);
#elif defined(__linux__)
        const Models::StartupInformation& startup(const std::string& desktopFile);
#else
        const Models::StartupInformation& startup();
#endif
        /**
         * @brief Shuts down the application.
         * @param geometry The window geometry to save
         */
        void shutdown(const Nickvision::App::WindowGeometry& geometry);
#ifdef _WIN32
        /**
         * @brief Starts downloading and installing the latest application update for Windows in the background.
         */
        void startWindowsUpdate();
#endif
        /**
         * @brief Downloads the latest yt-dlp update in the background.
         * @brief Will send a notification if the update fails.
         */
        void ytdlpUpdate();
        /**
         * @brief Gets the count of remaining downloads.
         * @return The count of remaining downloads
         */
        size_t getRemainingDownloadsCount() const;
        /**
         * @brief Gets the count of downloading downloads.
         * @return The count of downloading downloads
         */
        size_t getDownloadingCount() const;
        /**
         * @brief Gets the count of queued downloads.
         * @return The count of queued downloads
         */
        size_t getQueuedCount() const;
        /**
         * @brief Gets the count of completed downloads.
         * @return The count of completed downloads
         */
        size_t getCompletedCount() const;
        /**
         * @brief Recovers all available recoverable downloads.
         * @param clearInstead Whether or not to clear the recoverable downloads instead of recovering them
         * @return The number of recovered downloads
         */
        void recoverDownloads(bool clearInstead);
        /**
         * @brief Clears the download history.
         * @brief This method invokes the historyChanged event.
         */
        void clearHistory();
        /**
         * @brief Removes a historic download from the history.
         * @brief This method invokes the historyChanged event.
         * @param download The historic download to remove
         */
        void removeHistoricDownload(const Models::HistoricDownload& download);
        /**
         * @brief Requests that a download be stopped.
         * @brief This will invoke the downloadStopped event if stopped successfully.
         * @param id The id of the download to stop
         */
        void stopDownload(int id);
        /**
         * @brief Requests that a download be paused.
         * @brief This will invoke the downloadPaused event if stopped successfully.
         * @param id The id of the download to pause
         */
        void pauseDownload(int id);
        /**
         * @brief Requests that a download be resumed.
         * @brief This will invoke the downloadResumed event if stopped successfully.
         * @param id The id of the download to resume
         */
        void resumeDownload(int id);
        /**
         * @brief Requests that a download be retried.
         * @brief This will invoke the downloadRetried event if retried successfully.
         * @param id The id of the download to retry
         */
        void retryDownload(int id);
        /**
         * @brief Requests that all downloads be stopped.
         * @brief This will invoke the downloadStopped event for each download stopped.
         */
        void stopAllDownloads();
        /**
         * @brief Requests that all failed downloads be retried.
         * @brief This will invoke the downloadRetried event for each download retried.
         */
        void retryFailedDownloads();
        /**
         * @brief Clears all downloads from the queue.
         * @return The ids of the downloads cleared
         */
        std::vector<int> clearQueuedDownloads();
        /**
         * @brief Clears all completed downloads.
         * @return The ids of the downloads cleared
         */
        std::vector<int> clearCompletedDownloads();

    private:
        /**
         * @brief Handles when the configuration is saved.
         */
        void onConfigurationSaved();
        /**
         * @brief Handles when a download is completed.
         * @param args DownloadCompletedEventArgs
         */
        void onDownloadCompleted(const Events::DownloadCompletedEventArgs& args);
        bool m_started;
        std::vector<std::string> m_args;
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<Nickvision::Update::Version>> m_appUpdateAvailable;
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<double>> m_appUpdateProgressChanged;
        Nickvision::App::AppInfo m_appInfo;
        Nickvision::App::DataFileManager m_dataFileManager;
        std::shared_ptr<Nickvision::Update::Updater> m_updater;
        Nickvision::Taskbar::TaskbarItem m_taskbar;
        Nickvision::System::SuspendInhibitor m_suspendInhibitor;
        Nickvision::Keyring::Keyring m_keyring;
        Models::DownloadManager m_downloadManager;
        bool m_isWindowActive;
    };
}

#endif //MAINWINDOWCONTROLLER_H
