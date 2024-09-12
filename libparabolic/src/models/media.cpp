#include "models/media.h"
#include <libnick/helpers/stringhelpers.h>

using namespace Nickvision::Helpers;

namespace Nickvision::TubeConverter::Shared::Models
{
    Media::Media(boost::json::object info)
        : m_timeFrame{ std::chrono::seconds(0), std::chrono::seconds(0) },
        m_hasSubtitles{ false }
    {
        //Parse base information
        m_url = info.contains("url") ? (info["url"].is_string() ? info["url"].as_string() : "") : (info["webpage_url"].is_string() ? info["webpage_url"].as_string().c_str() : "");
        m_title = info["title"].is_string() ? info["title"].as_string() : "Media";
        m_title = StringHelpers::normalizeForFilename(m_title, info["limit_characters"].is_bool() ? info["limit_characters"].as_bool() : false);
        if(info.contains("duration"))
        {
            m_timeFrame = { std::chrono::seconds(0), std::chrono::seconds{ info["duration"].is_double() ? static_cast<int>(info["duration"].as_double()) : (info["duration"].is_int64() ? static_cast<int>(info["duration"].as_int64()) : 0) } };
        }
        if(info.contains("subtitles"))
        {
            boost::json::object subtitles = info["subtitles"].is_object() ? info["subtitles"].as_object() : boost::json::object();
            if(!subtitles.empty())
            {
                m_hasSubtitles = !(subtitles.size() == 1 && subtitles.contains("live_chat"));
            }
        }
        //Parse formats
        if(info.contains("formats") && info["formats"].is_array())
        {
            for(const boost::json::value& format : info["formats"].as_array())
            {
                if(!format.is_object())
                {
                    continue;
                }
                m_formats.push_back({ format.as_object() });
            }
        }
        //Parse automatic subtitles
        if(info.contains("automatic_captions") && info["automatic_captions"].is_object())
        {
            for(const boost::json::key_value_pair& caption : info["automatic_captions"].as_object())
            {
                m_automaticSubtitles.push_back(caption.key());
            }
        }
        //Parse subtitles
        if(info.contains("subtitles") && info["subtitles"].is_object())
        {
            for(const boost::json::key_value_pair& subtitle : info["subtitles"].as_object())
            {
                m_subtitles.push_back(subtitle.key());
            }
        }
        //Type
        m_type = MediaType::Audio;
        for(const Format& format : m_formats)
        {
            if(format.getType() == MediaType::Video)
            {
                m_type = MediaType::Video;
                break;
            }
        }
    }

    const std::string& Media::getUrl() const
    {
        return m_url;
    }

    const std::string& Media::getTitle() const
    {
        return m_title;
    }

    MediaType Media::getType() const
    {
        return m_type;
    }

    const TimeFrame& Media::getTimeFrame() const
    {
        return m_timeFrame;
    }

    bool Media::hasSubtitles() const
    {
        return m_hasSubtitles;
    }

    const std::vector<Format>& Media::getFormats() const
    {
        return m_formats;
    }

    const std::vector<std::string>& Media::getAutomaticSubtitles() const
    {
        return m_automaticSubtitles;
    }

    const std::vector<std::string>& Media::getSubtitles() const
    {
        return m_subtitles;
    }
}