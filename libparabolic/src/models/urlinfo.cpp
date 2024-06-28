#include "models/urlinfo.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    UrlInfo::UrlInfo(const std::string& url)
        : m_url(url),
        m_isPlaylist(false)
    {

    }

    std::optional<UrlInfo> UrlInfo::fetch(const std::string& url)
    {
        return std::nullopt;
    }

    const std::string& UrlInfo::getUrl() const
    {
        return m_url;
    }

    bool UrlInfo::isPlaylist() const
    {
        return m_isPlaylist;
    }

    const std::vector<Media>& UrlInfo::getMedia() const
    {
        return m_media;
    }
}