#include "events/downloadcompletedeventargs.h"

using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::Shared::Events
{
    DownloadCompletedEventArgs::DownloadCompletedEventArgs(int id, DownloadStatus status, bool showNotification)
        : m_id{ id },
        m_status{ status },
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

    bool DownloadCompletedEventArgs::getShowNotification() const
    {
        return m_showNotification;
    }
}