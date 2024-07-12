#include "events/downloadcompletedeventargs.h"

using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::Shared::Events
{
    DownloadCompletedEventArgs::DownloadCompletedEventArgs(const std::string& id, const std::filesystem::path& path, DownloadStatus status, bool showNotification)
        : m_id{ id },
        m_path{ path },
        m_status{ status },
        m_showNotification{ showNotification }
    {
        
    }

    const std::string& DownloadCompletedEventArgs::getId() const
    {
        return m_id;
    }

    const std::filesystem::path& DownloadCompletedEventArgs::getPath() const
    {
        return m_path;
    }

    DownloadStatus DownloadCompletedEventArgs::getStatus() const
    {
        return m_status;
    }

    bool DownloadCompletedEventArgs::getShowNotification() const
    {
        return m_showNotification;
    }
}