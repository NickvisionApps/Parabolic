#ifndef DOWNLOADPROGRESSCHANGEDEVENTARGS_H
#define DOWNLOADPROGRESSCHANGEDEVENTARGS_H

#include <string>
#include <libnick/events/eventargs.h>
#include "downloadprogressstatus.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief Event arguments for when a download's progress changes.
     */
    class DownloadProgressChangedEventArgs : public Events::EventArgs
    {
    public:
        /**
         * @brief Constructs a DownloadProgressChangedEventArgs.
         * @param status The DownloadProgressStatus
         * @param progress The progress of the download
         * @param speed The speed of the download (in byes per second)
         * @param log The log of the download
         */
        DownloadProgressChangedEventArgs(DownloadProgressStatus status, double progress, double speed, const std::string& log);
        /**
         * @brief Gets the status of the download.
         * @return The status of the download
         */
        DownloadProgressStatus getStatus() const;
        /**
         * @brief Gets the progress of the download.
         * @return The progress of the download
         */
        double getProgress() const;
        /**
         * @brief Gets the speed of the download (in byes per second).
         * @return The speed of the download (in byes per second)
         */
        double getSpeed() const;
        /**
         * @brief Gets the string representation of the speed.
         * @return The speed string representation
         */
        const std::string& getSpeedStr() const;
        /**
         * @brief Gets the log of the download.
         * @return The log of the download
         */
        const std::string& getLog() const;

    private:
        DownloadProgressStatus m_status;
        double m_progress;
        double m_speed;
        std::string m_speedStr;
        std::string m_log;
    };
}

#endif //DOWNLOADPROGRESSCHANGEDEVENTARGS_H