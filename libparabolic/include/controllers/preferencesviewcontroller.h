#ifndef PREFERENCESVIEWCONTROLLER_H
#define PREFERENCESVIEWCONTROLLER_H

#include <filesystem>
#include <string>
#include "models/browsers.h"
#include "models/configuration.h"
#include "models/completednotificationpreference.h"
#include "models/downloaderoptions.h"
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
         */
        PreferencesViewController(Models::Configuration& configuration);
        /**
         * @brief Gets the cookies extension url for a browser.
         * @param browser The browser
         * @return The cookies extension url
         */
        const std::string& getCookiesExtensionUrl(Models::Browsers browser) const;
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
         * @brief Gets the completed notification preference for downloads.
         * @return The completed notification preference
         */
        Models::CompletedNotificationPreference getCompletedNotificationPreference() const;
        /**
         * @brief Sets the completed notification preference for downloads.
         * @param preference The new completed notification preference
         */
        void setCompletedNotificationPreference(Models::CompletedNotificationPreference preference);
        /**
         * @brief Gets whether or not to prevent the system from suspending when downloading.
         * @return True to prevent the system from suspending, else false
         */
        bool getPreventSuspendWhenDownloading() const;
        /**
         * @brief Sets whether or not to prevent the system from suspending when downloading.
         * @param prevent True to prevent the system from suspending, else false
         */
        void setPreventSuspendWhenDownloading(bool prevent);
        /**
         * @brief Gets whether or not to disallow conversions.
         * @return True to disallow conversions, else false
         */
        bool getDisallowConversions() const;
        /**
         * @brief Sets whether or not to disallow conversions.
         * @param disallowConversions True to disallow conversions, else false
         */
        void setDisallowConversions(bool disallowConversions);
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
         * @brief Saves the current configuration to disk.
         */
        void saveConfiguration();

    private:
        Models::Configuration& m_configuration;
    };
}

#endif //PREFERENCESVIEWCONTROLLER_H