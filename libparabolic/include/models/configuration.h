#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <filesystem>
#include <string>
#include <libnick/app/configurationbase.h>
#include <libnick/app/windowgeometry.h>
#include "completednotificationpreference.h"
#include "downloaderoptions.h"
#include "mediafiletype.h"
#include "theme.h"
#include "videoresolution.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model for the configuration of the application.
     */
    class Configuration : public Nickvision::App::ConfigurationBase
    {
    public:
        /**
         * @brief Constructs a Configuration.
         * @param key The key to pass to the ConfigurationBase
         */
        Configuration(const std::string& key);
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
        DownloaderOptions getDownloaderOptions() const;
        /**
         * @brief Sets the downloader options.
         * @param downloaderOptions The new downloader options
         */
        void setDownloaderOptions(const DownloaderOptions& downloaderOptions);
        /**
         * @brief Gets the previous save folder.
         * @return The previous save folder
         */
        std::filesystem::path getPreviousSaveFolder() const;
        /**
         * @brief Sets the previous save folder.
         * @param previousSaveFolder The new previous save folder
         */
        void setPreviousSaveFolder(const std::filesystem::path& previousSaveFolder);
        /**
         * @brief Gets the previous media file type.
         * @return The previous media file type
         */
        MediaFileType getPreviousMediaFileType() const;
        /**
         * @brief Sets the previous media file type.
         * @param previousMediaFileType The new previous media file type
         */
        void setPreviousMediaFileType(const MediaFileType& previousMediaFileType);
        /**
         * @brief Gets the previous generic media file type.
         * @return The previous generic media file type
         */
        MediaFileType getPreviousGenericMediaFileType() const;
        /**
         * @brief Sets the previous generic media file type.
         * @param previousGenericMediaFileType The new previous generic media file type
         */
        void setPreviousGenericMediaFileType(const MediaFileType& previousGenericMediaFileType);
        /**
         * @brief Gets the previous video resolution.
         * @return The previous video resolution
         */
        VideoResolution getPreviousVideoResolution() const;
        /**
         * @brief Sets the previous video resolution.
         * @param previousVideoResolution The new previous video resolution
         */
        void setPreviousVideoResolution(const VideoResolution& previousVideoResolution);
        /**
         * @brief Gets the previous subtitle state.
         * @return The previous subtitle state
         */
        bool getPreviousSubtitleState() const;
        /**
         * @brief Sets the previous subtitle state.
         * @param previousSubtitleState The new previous subtitle state
         */
        void setPreviousSubtitleState(bool previousSubtitleState);
        /**
         * @brief Gets the previous prefer AV1 state.
         * @return The previous prefer AV1 state
         */
        bool getPreviousPreferAV1State() const;
        /**
         * @brief Sets the previous prefer AV1 state.
         * @param previousPreferAV1State The new previous prefer AV1 state
         */
        void setPreviousPreferAV1State(bool previousPreferAV1State);
        /**
         * @brief Gets whether or not to number titles in playlists.
         * @return True to number titles, else false
         */
        bool getNumberTitles() const;
        /**
         * @brief Sets whether or not to number titles in playlists.
         * @param numberTitles True to number titles, else false
         */
        void setNumberTitles(bool numberTitles);
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