#include "events/downloadaddedeventargs.h"

using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::Shared::Events
{
    DownloadAddedEventArgs::DownloadAddedEventArgs(int id, const std::filesystem::path& path, const std::string& url, DownloadStatus status)
        : m_id{ id },
        m_path{ path },
        m_url{ url },
        m_status{ status }
    {
        
    }

    int DownloadAddedEventArgs::getId() const
    {
        return m_id;
    }

    const std::filesystem::path& DownloadAddedEventArgs::getPath() const
    {
        return m_path;
    }

    const std::string& DownloadAddedEventArgs::getUrl() const
    {
        return m_url;
    }

    DownloadStatus DownloadAddedEventArgs::getStatus() const
    {
        return m_status;
    }
}