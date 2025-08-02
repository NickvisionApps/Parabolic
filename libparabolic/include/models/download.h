#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <libnick/events/event.h>
#include <libnick/system/process.h>
#include "downloadoptions.h"
#include "downloaderoptions.h"
#include "downloadstatus.h"
#include "events/downloadcompletedeventargs.h"
#include "events/downloadprogresschangedeventargs.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model of a download.
     */
    class Download
    {
    public:
        /**
         * @brief Constructs a Download.
         * @brief options The DownloadOptions
         */
        Download(const DownloadOptions& options);
        /**
         * @brief Destructs a Download.
         * @brief This will stop the download if it is running.
         */
        ~Download();
        /**
         * @brief Gets the event for when the download's progress is changed.
         * @return The progress changed event
         */
        Nickvision::Events::Event<Events::DownloadProgressChangedEventArgs>& progressChanged();
        /**
         * @brief Gets the event for when the download is completed.
         * @return The completed event
         */
        Nickvision::Events::Event<Events::DownloadCompletedEventArgs>& completed();
        /**
         * @brief Gets the Id of the download.
         * @return The Id of the download
         */
        int getId();
        /**
         * @brief Gets the url of the download.
         * @return The url of the download
         */
        const std::string& getUrl() const;
        /**
         * @brief Gets the options of the download.
         * @return The options of the download
         */
        const DownloadOptions& getOptions() const;
        /**
         * @brief Gets the status of the download.
         * @return The status of the download
         */
        DownloadStatus getStatus() const;
        /**
         * @brief Gets the path of the download.
         * @return The path of the download
         */
        const std::filesystem::path& getPath() const;
        /**
         * @brief Gets the log of the download.
         * @return The log of the download
         */
        const std::string& getLog() const;
        /**
         * @brief Starts the download.
         * @param ytdlpExecutable The path to the yt-dlp executable
         * @param downloaderOptions The DownloaderOptions
         */
        void start(const std::filesystem::path& ytdlpExecutable, const DownloaderOptions& downloaderOptions);
        /**
         * @brief Stops the download.
         */
        void stop();
        /**
         * @brief Pauses the download.
         */
        void pause();
        /**
         * @brief Resumes the download.
         */
        void resume();

    private:
        /**
         * @brief Watches the download process for progress.
         */
        void watch();
        /**
         * @brief Handles when the underlying process exits.
         * @brief args The ProcessExitedEventArgs
         */
        void onProcessExit(const System::ProcessExitedEventArgs& args);
        mutable std::mutex m_mutex;
        int m_id;
        DownloadOptions m_options;
        DownloadStatus m_status;
        std::filesystem::path m_path;
        std::shared_ptr<System::Process> m_process;
        Nickvision::Events::Event<Events::DownloadProgressChangedEventArgs> m_progressChanged;
        Nickvision::Events::Event<Events::DownloadCompletedEventArgs> m_completed;
    };
}

#endif //DOWNLOAD_H
