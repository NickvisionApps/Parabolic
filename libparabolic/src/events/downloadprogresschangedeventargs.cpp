#include "events/downloadprogresschangedeventargs.h"
#include <chrono>
#include <format>
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
        m_etaStr = m_eta == -1 ? _("N/A") : std::format("{:%T}", std::chrono::round<std::chrono::milliseconds>(std::chrono::duration<int>(eta)));
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
