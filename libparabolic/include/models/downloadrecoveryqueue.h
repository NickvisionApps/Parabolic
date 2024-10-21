#ifndef DOWNLOADRECOVERYQUEUE_H
#define DOWNLOADRECOVERYQUEUE_H

#include <libnick/app/datafilebase.h>

namespace Nickvision::TubeConverter::Shared::Models
{
    class DownloadRecoveryQueue : public Nickvision::App::DataFileBase
    {
    public:
        /**
         * @brief Constructs a DownloadRecoveryQueue.
         * @param key The key to pass to the DataFileBase
         * @param appName The name of the application to pass to the DataFileBase
         */
        DownloadRecoveryQueue(const std::string& key, const std::string& appName);
    };
}

#endif //DOWNLOADRECOVERYQUEUE_H