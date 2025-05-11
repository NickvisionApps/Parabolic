#include "models/urlinfo.h"
#include <filesystem>
#include <stdexcept>

namespace Nickvision::TubeConverter::Shared::Models
{
    UrlInfo::UrlInfo(const std::string& url, boost::json::object info)
        : m_url{ url },
        m_title{ info["title"].is_string() ? info["title"].as_string().c_str() : "" },
        m_isPlaylist{ false }
    {
        boost::json::array entries = info["entries"].is_array() ? info["entries"].as_array() : boost::json::array();
        if(!entries.empty())
        {
            m_isPlaylist = true;
            int pos{ 0 };
            for(const boost::json::value& entry : entries)
            {
                if(!entry.is_object())
                {
                    continue;
                }
                boost::json::object obj = entry.as_object();
                obj["limit_characters"] = info["limit_characters"];
                obj["include_media_id_in_title"] = info["include_media_id_in_title"];
                obj["include_auto_generated_subtitles"] = info["include_auto_generated_subtitles"];
                obj["preferred_video_codec"] = info["preferred_video_codec"];
                obj["preferred_audio_codec"] = info["preferred_audio_codec"];
                obj["suggested_save_folder"] = info["suggested_save_folder"];
                obj["is_part_of_playlist"] = true;
                obj["playlist_position"] = ++pos;
                m_media.push_back({ obj });
            }
        }
        else
        {
            m_media.push_back({ info });
        }
    }

    UrlInfo::UrlInfo(const std::string& url, const std::string& title, const std::vector<UrlInfo>& urlInfos)
        : m_url{ url },
        m_title{ title },
        m_isPlaylist{ false }
    {
        if(urlInfos.size() == 0)
        {
            throw std::invalid_argument("The list of UrlInfos must not be empty");
        }
        m_isPlaylist = urlInfos.size() > 1 ? true : urlInfos[0].isPlaylist();
        for(const UrlInfo& urlInfo : urlInfos)
        {
            for(size_t i = 0; i < urlInfo.count(); i++)
            {
                m_media.push_back(urlInfo.get(i));
            }
        }
    }

    const std::string& UrlInfo::getUrl() const
    {
        return m_url;
    }

    const std::string& UrlInfo::getTitle() const
    {
        return m_title;
    }

    bool UrlInfo::isPlaylist() const
    {
        return m_isPlaylist;
    }

    size_t UrlInfo::count() const
    {
        return m_media.size();
    }

    Media& UrlInfo::get(const size_t index)
    {
        return m_media[index];
    }

    const Media& UrlInfo::get(const size_t index) const
    {
        return m_media[index];
    }

    Media& UrlInfo::operator[](const size_t index)
    {
        return m_media[index];
    }

    const Media& UrlInfo::operator[](const size_t index) const
    {
        return m_media[index];
    }
}
