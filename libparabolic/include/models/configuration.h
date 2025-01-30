#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <libnick/app/datafilebase.h>
#include <libnick/app/windowgeometry.h>
#include "completednotificationpreference.h"
#include "downloaderoptions.h"
#include "theme.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model for the configuration of the application.
     */
    class Configuration : public Nickvision::App::DataFileBase
    {
    public:
        /**
         * @brief Constructs a Configuration.
         * @param key The key to pass to the DataFileBase
         * @param appName The name of the application to pass to the DataFileBase
         */
        Configuration(const std::string& key, const std::string& appName);
        /**
         * @brief Gets the preferred theme for the application.
         * @return The preferred theme
         */
        Theme getTheme() const;
        /**
         * @brief Sets the preferred theme for the application.
         * @param theme The new preferred theme
         */
        void setTheme(Theme theme);
        /**
         * @brief Gets the window geometry for the application.
         * @return The window geometry
         */
        App::WindowGeometry getWindowGeometry() const;
        /**
         * @brief Sets the window geometry for the application.
         * @param geometry The new window geometry
         */
        void setWindowGeometry(const App::WindowGeometry& geometry);
        /**
         * @brief Gets whether or not to automatically check for application updates.
         * @return True to automatically check for updates, else false
         */
        bool getAutomaticallyCheckForUpdates() const;
        /**
         * @brief Sets whether or not to automatically check for application updates.
         * @param check Whether or not to automatically check for updates
         */
        void setAutomaticallyCheckForUpdates(bool check);
        /**
         * @brief Gets the completed notification preference for downloads.
         * @return The completed notification preference
         */
        CompletedNotificationPreference getCompletedNotificationPreference() const;
        /**
         * @brief Sets the completed notification preference for downloads.
         * @param preference The new completed notification preference
         */
        void setCompletedNotificationPreference(CompletedNotificationPreference preference);
        /**
         * @brief Gets whether or not to prevent the system from suspending while Parabolic is running.
         * @return True to prevent the system from suspending, else false
         */
        bool getPreventSuspend() const;
        /**
         * @brief Sets whether or not to prevent the system from suspending while Parabolic is running.
         * @param prevent True to prevent the system from suspending, else false
         */
        void setPreventSuspend(bool prevent);
        /**
         * @brief Gets whether or not to recover crashed downloads.
         * @return True to recover crashed downloads, else false
         */
        bool getRecoverCrashedDownloads() const;
        /**
         * @brief Sets whether or not to recover crashed downloads.
         * @param recoverCrashedDownloads True to recover crashed downloads, else false
         */
        void setRecoverCrashedDownloads(bool recoverCrashedDownloads);
        /**
         * @brief Gets the downloader options.
         * @return The downloader options
         */
        DownloaderOptions getDownloaderOptions() const;
        /**
         * @brief Sets the downloader options.
         * @param downloaderOptions The new downloader options
         */
        void setDownloaderOptions(const DownloaderOptions& downloaderOptions);
        /**
         * @brief Gets whether or not to show the disclaimer on startup.
         * @return True to show the disclaimer, else false
         */
        bool getShowDisclaimerOnStartup() const;
        /**
         * @brief Sets whether or not to show the disclaimer on startup.
         * @param showDisclaimerOnStartup True to show the disclaimer, else false
         */
        void setShowDisclaimerOnStartup(bool showDisclaimerOnStartup);
        /**
         * @brief Gets whether or not to show the generic disclaimer.
         * @return True to show the disclaimer, else false
         */
        bool getShowGenericDisclaimer() const;
        /**
         * @brief Sets whether or not to show the generic disclaimer.
         * @param showDisclaimer True to show the disclaimer, else false
         */
        void setShowGenericDisclaimer(bool showDisclaimer);
        /**
         * @brief Gets whether or not to download immediately after validation.
         * @return True to download immediately after validation, else false
         */
        bool getDownloadImmediatelyAfterValidation() const;
        /**
         * @brief Sets whether or not to download immediately after validation.
         * @param downloadImmediatelyAfterValidation True to download immediately after validation, else false
         */
        void setDownloadImmediatelyAfterValidation(bool downloadImmediatelyAfterValidation);
    };
}

#endif //CONFIGURATION_H
