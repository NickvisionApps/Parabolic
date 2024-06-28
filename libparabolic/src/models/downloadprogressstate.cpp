#include "models/downloadprogressstate.h"
#include <cmath>
#include <format>
#include <libnick/helpers/codehelpers.h>
#include <libnick/localization/gettext.h>

using namespace Nickvision::Helpers;

namespace Nickvision::TubeConverter::Shared::Models
{
    DownloadProgressState::DownloadProgressState()
        : m_status{ DownloadProgressStatus::Processing },
        m_progress{ 0.0 },
        m_speed{ 0.0 }
    {

    }

    DownloadProgressStatus DownloadProgressState::getStatus() const
    {
        return m_status;
    }

    void DownloadProgressState::setStatus(DownloadProgressStatus status)
    {
        m_status = status;
    }

    double DownloadProgressState::getProgress() const
    {
        return m_progress;
    }

    void DownloadProgressState::setProgress(double progress)
    {
        m_progress = progress;
    }

    double DownloadProgressState::getSpeed() const
    {
        return m_speed;
    }

    void DownloadProgressState::setSpeed(double speed)
    {
        m_speed = speed;
    }

    std::string DownloadProgressState::getSpeedStr() const
    {
        static constexpr double pow2{ 1024 * 1024 };
        static constexpr double pow3{ 1024 * 1024 * 1024 };
        if(m_speed > pow3)
        {
            return std::vformat(_("%.1f GiB/s"), std::make_format_args(CodeHelpers::unmove(m_speed / pow3)));
        }
        else if(m_speed > pow2)
        {
            return std::vformat(_("%.1f MiB/s"), std::make_format_args(CodeHelpers::unmove(m_speed / pow2)));
        }
        else if(m_speed > 1024)
        {
            return std::vformat(_("%.1f KiB/s"), std::make_format_args(CodeHelpers::unmove(m_speed / 1024)));
        }
        else
        {
            return std::vformat(_("%.1f B/s"), std::make_format_args(m_speed));
        }
    }

    const std::string& DownloadProgressState::getLog() const
    {
        return m_log;
    }

    void DownloadProgressState::setLog(const std::string& log)
    {
        m_log = log;
    }
}