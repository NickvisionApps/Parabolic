#ifndef DOWNLOADRECOVERYQUEUE_H
#define DOWNLOADRECOVERYQUEUE_H

#include <unordered_map>
#include <utility>
#include <libnick/app/datafilebase.h>
#include "downloadoptions.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model of a queue of downloads that can be recovered.
     */
    class DownloadRecoveryQueue : public Nickvision::App::DataFileBase
    {
    public:
        /**
         * @brief Constructs a DownloadRecoveryQueue.
         * @brief This will load the recoverable downloads from disk.
         * @param key The key to pass to the DataFileBase
         * @param appName The name of the application to pass to the DataFileBase
         * @param isPortable The isPortable to pass to the DataFileBase
         */
        DownloadRecoveryQueue(const std::string& key, const std::string& appName, bool isPortable);
        /**
         * @brief Gets a list of downloads to recover.
         * @returns A list of DownloadOptions
         */
        std::unordered_map<int, std::pair<DownloadOptions, bool>>& getRecoverableDownloads();
        /**
         * @brief Adds a download to the recovery queue.
         * @param id The id of the download to add
         * @param downloadOptions The options of the download
         * @returns True if added, else false
         */
        bool add(int id, const DownloadOptions& downloadOptions);
        /**
         * @brief Removes a download from the recovery queue.
         * @param id The id of the download to remove
         * @returns True if removed, else false
         */
        bool remove(int id);
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
        std::unordered_map<int, std::pair<DownloadOptions, bool>> m_queue;
    };
}

#endif //DOWNLOADRECOVERYQUEUE_H
