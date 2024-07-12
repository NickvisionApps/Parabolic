#include "events/downloadaddedeventargs.h"

using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::Shared::Events
{
    DownloadAddedEventArgs::DownloadAddedEventArgs(const std::string& id, const std::filesystem::path& path, DownloadStatus status)
        : m_id{ id },
        m_path{ path },
        m_status{ status }
    {
        
    }

    const std::string& DownloadAddedEventArgs::getId() const
    {
        return m_id;
    }

    const std::filesystem::path& DownloadAddedEventArgs::getPath() const
    {
        return m_path;
    }

    DownloadStatus DownloadAddedEventArgs::getStatus() const
    {
        return m_status;
    }
}