#include "events/downloadcompletedeventargs.h"

using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::Shared::Events
{
    DownloadCompletedEventArgs::DownloadCompletedEventArgs(int id, DownloadStatus status, const std::filesystem::path& path, bool showNotification)
        : m_id{ id },
        m_status{ status },
        m_path{ path },
        m_showNotification{ showNotification }
    {
        
    }

    int DownloadCompletedEventArgs::getId() const
    {
        return m_id;
    }

    DownloadStatus DownloadCompletedEventArgs::getStatus() const
    {
        return m_status;
    }

    const std::filesystem::path& DownloadCompletedEventArgs::getPath() const
    {
        return m_path;
    }

    bool DownloadCompletedEventArgs::getShowNotification() const
    {
        return m_showNotification;
    }
}