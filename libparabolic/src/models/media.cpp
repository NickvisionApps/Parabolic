#include "models/media.h"
#include <libnick/helpers/stringhelpers.h>

using namespace Nickvision::Helpers;

namespace Nickvision::TubeConverter::Shared::Models
{
    static bool hasFormats(const std::vector<Format> formats, MediaType type)
    {
        for(const Format& format : formats)
        {
            if(!format.isFormatValue(FormatValue::None) && format.getType() == type)
            {
                return true;
            }
        }
        return false;
    }

    Media::Media(boost::json::object info)
        : m_playlistPosition{ -1 },
        m_timeFrame{ std::chrono::seconds(0), std::chrono::seconds(0) }
    {
        //Parse base information
        if(info.contains("is_part_of_playlist"))
        {
            m_url = info.contains("url") ? (info["url"].is_string() ? info["url"].as_string() : "") : (info["webpage_url"].is_string() ? info["webpage_url"].as_string().c_str() : "");
            m_playlistPosition = info["playlist_position"].is_int64() ? static_cast<int>(info["playlist_position"].as_int64()) : -1;
        }
        else
        {
            m_url = info.contains("webpage_url") ? (info["webpage_url"].is_string() ? info["webpage_url"].as_string() : "") : (info["url"].is_string() ? info["url"].as_string() : "");
        }
        m_title = info["title"].is_string() ? info["title"].as_string() : "Media";
        if(info["include_media_id_in_title"].is_bool() && info["include_media_id_in_title"].as_bool() && info["display_id"].is_string())
        {
            m_title += " [" + std::string(info["display_id"].as_string()) + "]";
        }
        m_title = StringHelpers::normalizeForFilename(m_title, info["limit_characters"].is_bool() ? info["limit_characters"].as_bool() : false);
        if(info.contains("duration"))
        {
            m_timeFrame = { std::chrono::seconds(0), std::chrono::seconds{ info["duration"].is_double() ? static_cast<int>(info["duration"].as_double()) : (info["duration"].is_int64() ? static_cast<int>(info["duration"].as_int64()) : 0) } };
        }
        //Parse formats
        bool hasVideoFormat{ false };
        bool hasAudioFormat{ false };
        if(info.contains("formats") && info["formats"].is_array())
        {
            VideoCodec preferredVideoCodec{ VideoCodec::Any };
            AudioCodec preferredAudioCodec{ AudioCodec::Any };
            if(info["preferred_video_codec"].is_int64())
            {
                preferredVideoCodec = static_cast<VideoCodec>(info["preferred_video_codec"].as_int64());
            }
            if(info["preferred_audio_codec"].is_int64())
            {
                preferredAudioCodec = static_cast<AudioCodec>(info["preferred_audio_codec"].as_int64());
            }
            std::vector<Format> skippedFormats;
            for(const boost::json::value& format : info["formats"].as_array())
            {
                if(!format.is_object())
                {
                    continue;
                }
                Format f{ format.as_object() };
                if(f.getType() != MediaType::Image)
                {
                    if(f.getType() == MediaType::Video)
                    {
                        hasVideoFormat = true;
                    }
                    else if(f.getType() == MediaType::Audio)
                    {
                        hasAudioFormat = true;
                    }
                    if(f.getVideoCodec() && preferredVideoCodec != VideoCodec::Any && f.getVideoCodec().value() != preferredVideoCodec)
                    {
                        skippedFormats.push_back(f);
                        continue;
                    }
                    if(f.getAudioCodec() && preferredAudioCodec != AudioCodec::Any && f.getAudioCodec().value() != preferredAudioCodec)
                    {
                        skippedFormats.push_back(f);
                        continue;
                    }
                    m_formats.push_back(f);
                }
            }
            if(m_formats.size() == 0 && skippedFormats.size() > 0)
            {
                m_formats = skippedFormats;
            }
            else if(!hasFormats(m_formats, MediaType::Video) && hasFormats(skippedFormats, MediaType::Video))
            {
                for(const Format& format : skippedFormats)
                {
                    if(!format.isFormatValue(FormatValue::None) && format.getType() == MediaType::Video)
                    {
                        m_formats.push_back(format);
                    }
                }
            }
            else if(!hasFormats(m_formats, MediaType::Audio) && hasFormats(skippedFormats, MediaType::Audio))
            {
                for(const Format& format : skippedFormats)
                {
                    if(!format.isFormatValue(FormatValue::None) && format.getType() == MediaType::Audio)
                    {
                        m_formats.push_back(format);
                    }
                }
            }
        }
        std::sort(m_formats.begin(), m_formats.end());
        //Parse automatic subtitles
        if(info["include_auto_generated_subtitles"].is_bool() && info["include_auto_generated_subtitles"].as_bool() && info.contains("automatic_captions") && info["automatic_captions"].is_object())
        {
            for(const boost::json::key_value_pair& caption : info["automatic_captions"].as_object())
            {
                m_subtitles.push_back({ caption.key(), true });
            }
        }
        //Parse subtitles
        if(info.contains("subtitles") && info["subtitles"].is_object())
        {
            for(const boost::json::key_value_pair& subtitle : info["subtitles"].as_object())
            {
                if(subtitle.key() != "live_chat")
                {
                    m_subtitles.push_back({ subtitle.key(), false });
                }
            }
        }
        std::sort(m_subtitles.begin(), m_subtitles.end());
        //Type
        if(hasVideoFormat && hasAudioFormat)
        {
            m_type = MediaType::Video;
            m_formats.insert(m_formats.begin(), { FormatValue::Worst, MediaType::Audio });
            m_formats.insert(m_formats.begin(), { FormatValue::Best, MediaType::Audio });
        }
        else if(hasAudioFormat)
        {
            m_type = MediaType::Audio;
        }
        else
        {
            m_type = MediaType::Video;
        }
        m_formats.insert(m_formats.begin(), { FormatValue::Worst, m_type });
        m_formats.insert(m_formats.begin(), { FormatValue::Best, m_type });
        m_formats.insert(m_formats.begin(), { FormatValue::None, MediaType::Video });
        m_formats.insert(m_formats.begin(), { FormatValue::None, MediaType::Audio });
        //Suggested save folder
        if(info["suggested_save_folder"].is_string())
        {
            m_suggestedSaveFolder = info["suggested_save_folder"].as_string().c_str();
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

    int Media::getPlaylistPosition() const
    {
        return m_playlistPosition;
    }

    MediaType Media::getType() const
    {
        return m_type;
    }

    const TimeFrame& Media::getTimeFrame() const
    {
        return m_timeFrame;
    }

    const std::vector<Format>& Media::getFormats() const
    {
        return m_formats;
    }

    const std::vector<SubtitleLanguage>& Media::getSubtitles() const
    {
        return m_subtitles;
    }

    const std::filesystem::path& Media::getSuggestedSaveFolder() const
    {
        return m_suggestedSaveFolder;
    }

    bool Media::hasVideoFormats() const
    {
        return hasFormats(m_formats, MediaType::Video);
    }

    bool Media::hasAudioFormats() const
    {
        return hasFormats(m_formats, MediaType::Audio);
    }
}
