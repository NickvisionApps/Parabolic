#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <filesystem>
#include <string>
#include <libnick/events/event.h>
#include <libnick/events/parameventargs.h>
#include <libnick/system/process.h>
#include "downloadoptions.h"
#include "downloaderoptions.h"
#include "downloadprogresschangedeventargs.h"
#include "downloadstatus.h"

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
        Events::Event<DownloadProgressChangedEventArgs>& progressChanged();
        /**
         * @brief Gets the event for when the download is completed.
         * @return The completed event
         */
        Events::Event<Events::ParamEventArgs<DownloadStatus>>& completed();
        /**
         * @brief Gets the path of the download.
         * @return The path of the download
         */
        std::filesystem::path getPath() const;
        /**
         * @brief Gets the status of the download.
         * @return The status of the download
         */
        DownloadStatus getStatus() const;
        /**
         * @brief Starts the download.
         * @brief Python must first be started via PythonHelpers::start().
         * @brief downloaderOptions The DownloaderOptions
         */
        void start(const DownloaderOptions& downloaderOptions);
        /**
         * @brief Stops the download.
         * @brief Python must first be started via PythonHelpers::start().
         */
        void stop();

    private:
        DownloadOptions m_options;
        std::string m_id;
        DownloadStatus m_status;
        std::string m_filename;
        std::filesystem::path m_tempDirPath;
        std::filesystem::path m_logFilePath;
        Events::Event<DownloadProgressChangedEventArgs> m_progressChanged;
        Events::Event<Events::ParamEventArgs<DownloadStatus>> m_completed;
    };
}

#endif //DOWNLOAD_H