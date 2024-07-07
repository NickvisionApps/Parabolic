#include "models/downloadprogresschangedeventargs.h"
#include <cmath>
#include <format>
#include <libnick/helpers/codehelpers.h>
#include <libnick/localization/gettext.h>

using namespace Nickvision::Helpers;

namespace Nickvision::TubeConverter::Shared::Models
{
    DownloadProgressChangedEventArgs::DownloadProgressChangedEventArgs(DownloadProgressStatus status, double progress, double speed, const std::string& log)
        : m_status{ status },
        m_progress{ progress },
        m_speed{ speed },
        m_log{ log }
    {
        static constexpr double pow2{ 1024 * 1024 };
        static constexpr double pow3{ 1024 * 1024 * 1024 };
        if(m_speed > pow3)
        {
            m_speedStr = std::vformat(_("%.1f GiB/s"), std::make_format_args(CodeHelpers::unmove(m_speed / pow3)));
        }
        else if(m_speed > pow2)
        {
            m_speedStr = std::vformat(_("%.1f MiB/s"), std::make_format_args(CodeHelpers::unmove(m_speed / pow2)));
        }
        else if(m_speed > 1024)
        {
            m_speedStr = std::vformat(_("%.1f KiB/s"), std::make_format_args(CodeHelpers::unmove(m_speed / 1024)));
        }
        else
        {
            m_speedStr = std::vformat(_("%.1f B/s"), std::make_format_args(m_speed));
        }
    }

    DownloadProgressStatus DownloadProgressChangedEventArgs::getStatus() const
    {
        return m_status;
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