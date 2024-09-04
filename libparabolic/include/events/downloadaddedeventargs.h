#ifndef DOWNLOADADDEDEVENTARGS_H
#define DOWNLOADADDEDEVENTARGS_H

#include <filesystem>
#include <string>
#include <libnick/events/eventargs.h>
#include "models/downloadstatus.h"

namespace Nickvision::TubeConverter::Shared::Events
{
    /**
     * @brief Event arguments for when a download is added.
     */
    class DownloadAddedEventArgs : public Nickvision::Events::EventArgs
    {
    public:
        /**
         * @brief Constructs a DownloadAddedEventArgs.
         * @param id The Id of the download
         * @param path The expected path of the download
         * @param url The URL of the download
         * @param status The status of the download
         */
        DownloadAddedEventArgs(int id, const std::filesystem::path& path, const std::string& url, Models::DownloadStatus status);
        /**
         * @brief Gets the Id of the download.
         * @return The Id of the download
         */
        int getId() const;
        /**
         * @brief Gets the expected path of the download.
         * @return The expected path of the download
         */
        const std::filesystem::path& getPath() const;
        /**
         * @brief Gets the URL of the download.
         * @return The URL of the download
         */
        const std::string& getUrl() const;
        /**
         * @brief Gets the status of the download.
         * @return The status of the download
         */
        Models::DownloadStatus getStatus() const;

    private:
        int m_id;
        std::filesystem::path m_path;
        std::string m_url;
        Models::DownloadStatus m_status;
    };
}

#endif //DOWNLOADADDEDEVENTARGS_H