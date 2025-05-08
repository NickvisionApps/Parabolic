#include "events/downloadprogresschangedeventargs.h"
#include <cmath>
#include <libnick/localization/gettext.h>

namespace Nickvision::TubeConverter::Shared::Events
{
    DownloadProgressChangedEventArgs::DownloadProgressChangedEventArgs(int id, double progress, double speed, const std::string& log)
        : m_id{ id },
        m_progress{ progress > 1 ? 1 : progress},
        m_speed{ speed },
        m_log{ log }
    {
        static constexpr double pow2{ 1024 * 1024 };
        static constexpr double pow3{ 1024 * 1024 * 1024 };
        if(m_speed > pow3)
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
    }

    int DownloadProgressChangedEventArgs::getId() const
    {
        return m_id;
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

    const std::string& DownloadProgressChangedEventArgs::getLog() const
    {
        return m_log;
    }
}
