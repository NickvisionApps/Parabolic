#ifndef DOWNLOADCOMPLETEDEVENTARGS_H
#define DOWNLOADCOMPLETEDEVENTARGS_H

#include <filesystem>
#include <string>
#include <libnick/events/eventargs.h>
#include "models/downloadstatus.h"

namespace Nickvision::TubeConverter::Shared::Events
{
    /**
     * @brief Event arguments for when a download is completed.
     */
    class DownloadCompletedEventArgs : public Nickvision::Events::EventArgs
    {
    public:
        /**
         * @brief Constructs a DownloadCompletedEventArgs.
         * @param id The Id of the download
         * @param path The path of the download
         * @param status The status of the download
         * @param showNotification Whether or not to show a notification
         */
        DownloadCompletedEventArgs(int id, const std::filesystem::path& path, Models::DownloadStatus status, bool showNotification);
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
         * @brief Gets the status of the download.
         * @return The status of the download
         */
        Models::DownloadStatus getStatus() const;
        /**
         * @brief Gets whether or not to show a notification.
         * @return True if show a notification, false if not
         */
        bool getShowNotification() const;

    private:
        int m_id;
        std::filesystem::path m_path;
        Models::DownloadStatus m_status;
        bool m_showNotification;
    };
}

#endif //DOWNLOADCOMPLETEDEVENTARGS_H