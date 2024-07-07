#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <filesystem>
#include <string>
#include <libnick/events/event.h>
#include <libnick/events/parameventargs.h>
#include <pybind11/embed.h>
#include "downloadoptions.h"
#include "downloaderoptions.h"
#include "downloadprogresschangedeventargs.h"

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
         * @brief The event's parameter is a boolean indicating whether or not the download was successful.
         * @return The completed event
         */
        Events::Event<Events::ParamEventArgs<bool>>& completed();
        /**
         * @brief Starts the download.
         * @brief Python must first be started via PythonHelpers::start().
         * @brief downloaderOptions The DownloaderOptions
         * @return True if started, else false
         */
        bool start(const DownloaderOptions& downloaderOptions);
        /**
         * @brief Stops the download.
         * @brief Python must first be started via PythonHelpers::start().
         * @return True if stopped, else false
         */
        bool stop();

    private:
        /**
         * @brief Invokes the progress changed event with the updated log.
         */
        void invokeLogUpdate();
        /**
         * @brief Invokes the progress changed event with the updated progress.
         */
        void progressHook();
        DownloadOptions m_options;
        std::string m_id;
        std::filesystem::path m_tempDirPath;
        std::filesystem::path m_logFilePath;
        pybind11::object m_logFileHandle;
        bool m_isRunning;
        bool m_isDone;
        bool m_isSuccess;
        bool m_wasStopped;
        std::string m_filename;
        Events::Event<DownloadProgressChangedEventArgs> m_progressChanged;
        Events::Event<Events::ParamEventArgs<bool>> m_completed;
    };
}

#endif //DOWNLOAD_H