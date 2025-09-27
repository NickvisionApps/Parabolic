#include "events/downloadprogresschangedeventargs.h"
#include <chrono>
#include <libnick/localization/gettext.h>

namespace Nickvision::TubeConverter::Shared::Events
{
    DownloadProgressChangedEventArgs::DownloadProgressChangedEventArgs(int id, const std::string& log, double progress, double speed, int eta)
        : m_id{ id },
        m_log{ log },
        m_progress{ progress > 1 ? 1 : progress},
        m_speed{ speed },
        m_eta{ eta }
    {
        static constexpr double pow2{ 1024 * 1024 };
        static constexpr double pow3{ 1024 * 1024 * 1024 };
        if(m_speed == 0)
        {
            m_speedStr = _("0 B/s");
        }
        else if(m_speed > pow3)
        {
            m_speedStr = _f("{:.2f} GiB/s", m_speed / pow3);
        }
        else if(m_speed > pow2)
        {
            m_speedStr = _f("{:.2f} MiB/s", m_speed / pow2);
        }
        else if(m_speed > 1024)
        {
            m_speedStr = _f("{:.2f} KiB/s", m_speed / 1024.0);
        }
        else
        {
            m_speedStr = _f("{:.2f} B/s", m_speed);
        }
        if(m_eta == -1)
        {
            m_etaStr = _("Unknown time left");
        }
        else
        {
            std::chrono::seconds totalSeconds{ std::chrono::round<std::chrono::seconds>(std::chrono::duration<int>(m_eta)) };
            std::chrono::hours hours{ std::chrono::duration_cast<std::chrono::hours>(totalSeconds) };
            totalSeconds -= hours;
            std::chrono::minutes minutes{ std::chrono::duration_cast<std::chrono::minutes>(totalSeconds) };
            totalSeconds -= minutes;
            std::chrono::seconds seconds{ std::chrono::duration_cast<std::chrono::seconds>(totalSeconds) };
            std::string remainingStr;
            if(m_eta == 0)
            {
                remainingStr = _("0 seconds");
            }
            else
            {
                if(hours.count())
                {
                    remainingStr += _fn("{} hour", "{} hours", hours.count(), hours.count());
                }
                if(hours.count() || minutes.count())
                {
                    if(hours.count())
                    {
                        remainingStr = _f("{} and ", remainingStr);
                    }
                    remainingStr += _fn("{} minute", "{} minutes", minutes.count(), minutes.count());
                }
                if(hours.count() || minutes.count() || seconds.count())
                {
                    if(hours.count() || minutes.count())
                    {
                        remainingStr = _f("{} and ", remainingStr);
                    }
                    remainingStr += _fn("{} second", "{} seconds", seconds.count(), seconds.count());
                }
            }
            m_etaStr = _fn("{} left", "{} left", hours.count() + minutes.count() + seconds.count(), remainingStr);
        }
    }

    int DownloadProgressChangedEventArgs::getId() const
    {
        return m_id;
    }

    const std::string& DownloadProgressChangedEventArgs::getLog() const
    {
        return m_log;
    }

    double DownloadProgressChangedEventArgs::getProgress() const
    {
        return m_progress;
    }

    double DownloadProgressChangedEventArgs::getSpeed() const
    {
        return m_speed;
    }

    const std::string& DownloadProgressChangedEventArgs::getSpeedStr() const
    {
        return m_speedStr;
    }

    int DownloadProgressChangedEventArgs::getEta() const
    {
        return m_eta;
    }

    const std::string& DownloadProgressChangedEventArgs::getEtaStr() const
    {
        return m_etaStr;
    }
}
