#ifndef DOWNLOADSTATUS_H
#define DOWNLOADSTATUS_H

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief Statues of a download.
     */
    enum class DownloadStatus
    {
        Queued,
        Running,
        Paused,
        Stopped,
        Error,
        Success
    };
}

#endif //DOWNLOADSTATUS_H
