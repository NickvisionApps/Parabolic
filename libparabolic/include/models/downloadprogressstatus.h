#ifndef DOWNLOADPROGRESSSTATUS_H
#define DOWNLOADPROGRESSSTATUS_H

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief Statuses for the progress of a download.
     */
    enum class DownloadProgressStatus
    {
        Processing,
        Downloading,
        DownloadingAria,
        DownloadingFFmpeg,
        Other
    };
}

#endif //DOWNLOADPROGRESSSTATUS_H