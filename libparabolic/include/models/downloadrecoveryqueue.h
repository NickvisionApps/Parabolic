#ifndef DOWNLOADRECOVERYQUEUE_H
#define DOWNLOADRECOVERYQUEUE_H

#include <unordered_map>
#include <libnick/app/datafilebase.h>
#include "downloadoptions.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    class DownloadRecoveryQueue : public Nickvision::App::DataFileBase
    {
    public:
        /**
         * @brief Constructs a DownloadRecoveryQueue.
         * @brief This will load the recoverable downloads from disk.
         * @param key The key to pass to the DataFileBase
         * @param appName The name of the application to pass to the DataFileBase
         */
        DownloadRecoveryQueue(const std::string& key, const std::string& appName);
        /**
         * @brief Gets a list of downloads to recover.
         * @returns A list of DownloadOptions
         */
        const std::unordered_map<int, DownloadOptions>& getRecoverableDownloads() const;
        /**
         * @brief Adds a download to the recovery queue.
         * @param id The ID of the download
         * @param downloadOptions The options of the download
         * @returns True if added, else false
         */
        bool addDownload(int id, const DownloadOptions& downloadOptions);
        /**
         * @brief Removes a download from the recovery queue.
         * @param id The ID of the download
         * @returns True if removed, else false
         */
        bool removeDownload(int id);
        /**
         * @brief Clears all downloads from the recovery queue.
         * @returns True if cleared, else false
         */
        bool clear();

    private:
        /**
         * Updates the recovery file on disk.
         */
        void updateDisk();
        std::unordered_map<int, DownloadOptions> m_recoverableDownloads;
    };
}

#endif //DOWNLOADRECOVERYQUEUE_H