#include "discovery.h"
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include "audio.h"
#include "subtitle.h"
#include "video.h"

namespace parabolic::discovery
{
    bool Options::include_media_id_in_title() const
    {
        return m_include_media_id_in_title;
    }

    void Options::set_include_media_id_in_title(bool value)
    {
        m_include_media_id_in_title = value;
    }

    bool Options::include_super_resolutions() const
    {
        return m_include_super_resolutions;
    }

    void Options::set_include_super_resolutions(bool value)
    {
        m_include_super_resolutions = value;
    }

    bool Options::limit_characters() const
    {
        return m_limit_characters;
    }

    void Options::set_limit_characters(bool value)
    {
        m_limit_characters = value;
    }

    audio::Codec Options::preferred_audio_codec() const
    {
        return m_preferred_audio_codec;
    }

    void Options::set_preferred_audio_codec(audio::Codec value)
    {
        m_preferred_audio_codec = value;
    }

    video::FrameRate Options::preferred_frame_rate() const
    {
        return m_preferred_frame_rate;
    }

    void Options::set_preferred_frame_rate(video::FrameRate value)
    {
        m_preferred_frame_rate = value;
    }

    subtitle::Type Options::preferred_subtitle_format() const
    {
        return m_preferred_subtitle_format;
    }

    void Options::set_preferred_subtitle_format(subtitle::Type value)
    {
        m_preferred_subtitle_format = value;
    }

    video::Codec Options::preferred_video_codec() const
    {
        return m_preferred_video_codec;
    }

    void Options::set_preferred_video_codec(video::Codec value)
    {
        m_preferred_video_codec = value;
    }

    const Options& Manager::options() const
    {
        return m_options;
    }

    void Manager::set_options(const Options& options)
    {
        m_options = options;
    }

    void from_json(const nlohmann::json& json, Options& options)
    {
        if (json.contains("includeMediaIdInTitle") && json["includeMediaIdInTitle"].is_boolean())
        {
            options.m_include_media_id_in_title = json.at("includeMediaIdInTitle").get<bool>();
        }
        else if (json.contains("IncludeMediaIdInTitle") && json["IncludeMediaIdInTitle"].is_boolean())
        {
            options.m_include_media_id_in_title = json.at("IncludeMediaIdInTitle").get<bool>();
        }
        if (json.contains("includeSuperResolutions") && json["includeSuperResolutions"].is_boolean())
        {
            options.m_include_super_resolutions = json.at("includeSuperResolutions").get<bool>();
        }
        else if (json.contains("IncludeSuperResolutions") && json["IncludeSuperResolutions"].is_boolean())
        {
            options.m_include_super_resolutions = json.at("IncludeSuperResolutions").get<bool>();
        }
        if (json.contains("limitCharacters") && json["limitCharacters"].is_boolean())
        {
            options.m_limit_characters = json.at("limitCharacters").get<bool>();
        }
        else if (json.contains("LimitCharacters") && json["LimitCharacters"].is_boolean())
        {
            options.m_limit_characters = json.at("LimitCharacters").get<bool>();
        }
        if (json.contains("preferredAudioCodec") && json["preferredAudioCodec"].is_number_integer())
        {
            int value{ json.at("preferredAudioCodec").get<int>() };
            if (value >= static_cast<int>(audio::Codec::Any) && value <= static_cast<int>(audio::Codec::Mp3))
            {
                options.m_preferred_audio_codec = static_cast<audio::Codec>(value);
            }
        }
        else if (json.contains("PreferredAudioCodec") && json["PreferredAudioCodec"].is_number_integer())
        {
            int value{ json.at("PreferredAudioCodec").get<int>() };
            if (value >= static_cast<int>(audio::Codec::Any) && value <= static_cast<int>(audio::Codec::Mp3))
            {
                options.m_preferred_audio_codec = static_cast<audio::Codec>(value);
            }
        }
        if (json.contains("preferredFrameRate") && json["preferredFrameRate"].is_number_integer())
        {
            int value{ json.at("preferredFrameRate").get<int>() };
            if (value >= static_cast<int>(video::FrameRate::Any) && value <= static_cast<int>(video::FrameRate::Fps60))
            {
                options.m_preferred_frame_rate = static_cast<video::FrameRate>(value);
            }
        }
        else if (json.contains("PreferredFrameRate") && json["PreferredFrameRate"].is_number_integer())
        {
            int value{ json.at("PreferredFrameRate").get<int>() };
            if (value >= static_cast<int>(video::FrameRate::Any) && value <= static_cast<int>(video::FrameRate::Fps60))
            {
                options.m_preferred_frame_rate = static_cast<video::FrameRate>(value);
            }
        }
        if (json.contains("preferredSubtitleFormat") && json["preferredSubtitleFormat"].is_number_integer())
        {
            int value{ json.at("preferredSubtitleFormat").get<int>() };
            if (value >= static_cast<int>(subtitle::Type::Any) && value <= static_cast<int>(subtitle::Type::Lrc))
            {
                options.m_preferred_subtitle_format = static_cast<subtitle::Type>(value);
            }
        }
        else if (json.contains("PreferredSubtitleFormat") && json["PreferredSubtitleFormat"].is_number_integer())
        {
            int value{ json.at("PreferredSubtitleFormat").get<int>() };
            if (value >= static_cast<int>(subtitle::Type::Any) && value <= static_cast<int>(subtitle::Type::Lrc))
            {
                options.m_preferred_subtitle_format = static_cast<subtitle::Type>(value);
            }
        }
        if (json.contains("preferredVideoCodec") && json["preferredVideoCodec"].is_number_integer())
        {
            int value{ json.at("preferredVideoCodec").get<int>() };
            if (value >= static_cast<int>(video::Codec::Any) && value <= static_cast<int>(video::Codec::H264))
            {
                options.m_preferred_video_codec = static_cast<video::Codec>(value);
            }
        }
        else if (json.contains("PreferredVideoCodec") && json["PreferredVideoCodec"].is_number_integer())
        {
            int value{ json.at("PreferredVideoCodec").get<int>() };
            if (value >= static_cast<int>(video::Codec::Any) && value <= static_cast<int>(video::Codec::H264))
            {
                options.m_preferred_video_codec = static_cast<video::Codec>(value);
            }
        }
    }

    void to_json(nlohmann::json& json, const Options& options)
    {
        json = nlohmann::json{
            { "includeMediaIdInTitle", options.m_include_media_id_in_title },
            { "includeSuperResolutions", options.m_include_super_resolutions },
            { "limitCharacters", options.m_limit_characters },
            { "preferredAudioCodec", static_cast<int>(options.m_preferred_audio_codec) },
            { "preferredFrameRate", static_cast<int>(options.m_preferred_frame_rate) },
            { "preferredSubtitleFormat", static_cast<int>(options.m_preferred_subtitle_format) },
            { "preferredVideoCodec", static_cast<int>(options.m_preferred_video_codec) },
        };
    }
}
