#ifndef PREFERENCESVIEWCONTROLLER_H
#define PREFERENCESVIEWCONTROLLER_H

#include <filesystem>
#include <string>
#include "models/configuration.h"
#include "models/downloaderoptions.h"
#include "models/downloadhistory.h"
#include "models/theme.h"

namespace Nickvision::TubeConverter::Shared::Controllers
{
    /**
     * @brief A controller for a PreferencesView.
     */
    class PreferencesViewController
    {
    public:
        /**
         * @brief Constructs a PreferencesViewController.
         * @param configuration The reference to the configuration to use
         * @param downloadHistory The reference to the download history to use
         */
        PreferencesViewController(Models::Configuration& configuration, Models::DownloadHistory& downloadHistory);
        /**
         * @brief Gets the maximum number of postprocessing threads allowed by the system.
         * @return The maximum number of postprocessing threads
         */
        int getMaxPostprocessingThreads() const;
        /**
         * @brief Gets the preferred theme for the application.
         * @return The preferred theme
         */
        Models::Theme getTheme() const;
        /**
         * @brief Sets the preferred theme for the application.
         * @param theme The new preferred theme
         */
        void setTheme(Models::Theme theme);
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
         * @brief Gets the downloader options.
         * @return The downloader options
         */
        Models::DownloaderOptions getDownloaderOptions() const;
        /**
         * @brief Sets the downloader options.
         * @param options The new downloader options
         */
        void setDownloaderOptions(const Models::DownloaderOptions& options);
        /**
         * @brief Gets the index of the selected download history length.
         * @return The download history length index
         */
        size_t getHistoryLengthIndex() const;
        /**
         * @brief Sets the index of the selected download history length.
         * @param length The new download history length index
         */
        void setHistoryLengthIndex(size_t length);
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
        /**
         * @brief Saves the current configuration to disk.
         */
        void saveConfiguration();

    private:
        Models::Configuration& m_configuration;
        Models::DownloadHistory& m_downloadHistory;
    };
}

#endif //PREFERENCESVIEWCONTROLLER_H
