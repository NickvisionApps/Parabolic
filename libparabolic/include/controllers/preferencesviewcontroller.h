#ifndef PREFERENCESVIEWCONTROLLER_H
#define PREFERENCESVIEWCONTROLLER_H

#include <string>
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
         */
        PreferencesViewController() = default;
        /**
         * @brief Gets the application's id.
         * @return The app id
         */
        const std::string& getId() const;
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
         * @brief Saves the current configuration to disk.
         */
        void saveConfiguration();
    };
}

#endif //PREFERENCESVIEWCONTROLLER_H