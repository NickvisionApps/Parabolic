#include "models/downloadprogressstate.h"

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

    const std::string& DownloadProgressState::getLog() const
    {
        return m_log;
    }

    void DownloadProgressState::setLog(const std::string& log)
    {
        m_log = log;
    }
}