#ifndef DOWNLOADPROGRESSSTATE_H
#define DOWNLOADPROGRESSSTATE_H

#include <string>
#include "downloadprogressstatus.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model of the progress state of a download.
     */
    class DownloadProgressState
    {
    public:
        /**
         * @brief Construct a DownloadProgressState.
         */
        DownloadProgressState();
        /**
         * @brief Gets the status of the download.
         * @return The status of the download
         */
        DownloadProgressStatus getStatus() const;
        /**
         * @brief Sets the status of the download.
         * @param status The status of the download
         */
        void setStatus(DownloadProgressStatus status);
        /**
         * @brief Gets the progress of the download.
         * @return The progress of the download
         */
        double getProgress() const;
        /**
         * @brief Sets the progress of the download.
         * @param progress The progress of the download
         */
        void setProgress(double progress);
        /**
         * @brief Gets the speed of the download (in byes per second).
         * @return The speed of the download (in byes per second)
         */
        double getSpeed() const;
        /**
         * @brief Sets the speed of the download (in byes per second).
         * @param speed The speed of the download (in byes per second)
         */
        void setSpeed(double speed);
        /**
         * @brief Gets the string representation of the speed.
         * @return The speed string representation
         */
        std::string getSpeedStr() const;
        /**
         * @brief Gets the log of the download.
         * @return The log of the download
         */
        const std::string& getLog() const;
        /**
         * @brief Sets the log of the download.
         * @param log The log of the download
         */
        void setLog(const std::string& log);

    private:
        DownloadProgressStatus m_status;
        double m_progress;
        double m_speed;
        std::string m_log;
    };
}

#endif //DOWNLOADPROGRESSSTATE_H