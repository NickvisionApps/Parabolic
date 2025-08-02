#ifndef YTDLPMANAGER_H
#define YTDLPMANAGER_H

#include <filesystem>
#include <libnick/events/event.h>
#include <libnick/update/updater.h>
#include <libnick/update/version.h>
#include "models/configuration.h"
#include "models/startupinformation.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A manager for yt-dlp and its dependencies.
     */
    class YtdlpManager
    {
    public:
        /**
         * @brief Constructs a YtdlpManager.
         */
        YtdlpManager(Configuration& config);
        /**
         * @brief Gets the path to the yt-dlp executable.
         * @return The path to the yt-dlp executable
         */
        const std::filesystem::path& getExecutablePath() const;
        /**
         * @brief Checks for a yt-dlp update and sends a notification with the "ytdlp" action if one is available.
         */
        void checkForUpdates();
        /**
         * @brief Downloads the latest version of yt-dlp in the background.
         * @brief Will send a notification if the update fails.
         * @brief YtdlpManager::checkForUpdates() must be called before this method.
         */
        void downloadUpdate();

    private:
        mutable std::mutex m_mutex;
        Configuration& m_config;
        mutable Update::Updater m_updater;
        Update::Version m_bundledYtdlpVersion;
        Update::Version m_latestYtdlpVersion;
    };
}

#endif //YTDLPMANAGER_H