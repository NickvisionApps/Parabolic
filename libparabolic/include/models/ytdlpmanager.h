#ifndef YTDLPMANAGER_H
#define YTDLPMANAGER_H

#include <filesystem>
#include <libnick/events/event.h>
#include <libnick/events/parameventargs.h>
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
         * @brief Gets the event for when a yt-dlp update is available.
         * @return The yt-dlp update available event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<Nickvision::Update::Version>>& updateAvailable();
        /**
         * @brief Gets the event for when a yt-dlp update's progress is changed.
         * @return The yt-dlp update progress changed event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<double>>& updateProgressChanged();
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
         * @brief Starts to download the latest version of yt-dlp in the background.
         */
        void startUpdateDownload();

    private:
        mutable std::mutex m_mutex;
        Configuration& m_config;
        mutable Update::Updater m_updater;
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<Nickvision::Update::Version>> m_updateAvailable;
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<double>> m_updateProgressChanged;
        Update::Version m_bundledYtdlpVersion;
        Update::Version m_latestYtdlpVersion;
    };
}

#endif //YTDLPMANAGER_H