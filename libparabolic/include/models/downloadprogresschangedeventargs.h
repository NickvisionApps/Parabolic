#ifndef DOWNLOADPROGRESSCHANGEDEVENTARGS_H
#define DOWNLOADPROGRESSCHANGEDEVENTARGS_H

#include <ostream>
#include <string>
#include <libnick/events/eventargs.h>

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
         * @param progress The progress of the download (between 0 and 1)
         * @param speed The speed of the download (in byes per second)
         * @param log The log of the download
         */
        DownloadProgressChangedEventArgs(double progress, double speed, const std::string& log);
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
        /**
         * @brief Outputs the DownloadProgressChangedEventArgs to an output stream.
         * @param os The output stream
         * @param media The DownloadProgressChangedEventArgs
         * @return The output stream
         */
        friend std::ostream& operator<<(std::ostream& os, const DownloadProgressChangedEventArgs& args);

    private:
        double m_progress;
        double m_speed;
        std::string m_speedStr;
        std::string m_log;
    };
}

#endif //DOWNLOADPROGRESSCHANGEDEVENTARGS_H