#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <libnick/app/datafilebase.h>
#include <libnick/app/windowgeometry.h>
#include <libnick/update/version.h>
#include <libnick/update/versiontype.h>
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
         * @param isPortable The isPortable to pass to the DataFileBase
         */
        Configuration(const std::string& key, const std::string& appName, bool isPortable);
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
         * @brief Gets the preferred translation language for the application.
         * @return The preferred translation language
         * @return An empty string to use the system language
         * @return "C" to not use translations
         */
        std::string getTranslationLanguage() const;
        /**
         * @brief Sets the preferred translation language for the application.
         * @param language The new preferred translation language
         */
        void setTranslationLanguage(const std::string& language);
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
         * @brief Gets the preferred update type for the application.
         * @return The preferred update type
         */
        Update::VersionType getPreferredUpdateType() const;
        /**
         * @brief Sets the preferred update type for the application.
         * @param type The new preferred update type
         */
        void setPreferredUpdateType(Update::VersionType type);
        /**
         * @brief Gets the installed version of yt-dlp.
         * @return The installed version of yt-dlp
         * @return An empty Version object if yt-dlp is using the bundled version
         */
        Update::Version getInstalledYtdlpVersion() const;
        /**
         * @brief Sets the installed version of yt-dlp.
         * @param version The new installed version of yt-dlp
         */
        void setInstalledYtdlpVersion(const Update::Version& version);
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
    };
}

#endif //CONFIGURATION_H
