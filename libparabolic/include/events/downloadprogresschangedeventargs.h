#ifndef DOWNLOADPROGRESSCHANGEDEVENTARGS_H
#define DOWNLOADPROGRESSCHANGEDEVENTARGS_H

#include <string>
#include <libnick/events/eventargs.h>

namespace Nickvision::TubeConverter::Shared::Events
{
    /**
     * @brief Event arguments for when a download's progress changes.
     */
    class DownloadProgressChangedEventArgs : public Nickvision::Events::EventArgs
    {
    public:
        /**
         * @brief Constructs a DownloadProgressChangedEventArgs.
         * @param id The Id of the download
         * @param log The log of the download
         * @param progress The progress of the download (between 0 and 1, or nan for indeterminate)
         * @param speed The speed of the download (in byes per second)
         * @param eta The eta of the download (in seconds)
         */
        DownloadProgressChangedEventArgs(int id, const std::string& log, double progress = 1.0, double speed = 0.0, int eta = 0);
        /**
         * @brief Gets the Id of the download.
         * @return The Id of the download
         */
        int getId() const;
        /**
         * @brief Gets the log of the download.
         * @return The log of the download
         */
        const std::string& getLog() const;
        /**
         * @brief Gets the progress of the download.
         * @brief The progress either be between 0 and 1, or nan for indeterminate.
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
         * @brief Gets the eta of the download (in seconds).
         * @return The eta of the download (in seconds)
         */
        int getEta() const;
        /**
         * @brief Gets the string representation of the eta.
         * @return The eta string representation
         */
        const std::string& getEtaStr() const;

    private:
        int m_id;
        std::string m_log;
        double m_progress;
        double m_speed;
        std::string m_speedStr;
        int m_eta;
        std::string m_etaStr;
    };
}

#endif //DOWNLOADPROGRESSCHANGEDEVENTARGS_H
