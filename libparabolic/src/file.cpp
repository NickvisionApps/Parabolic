#include "file.h"
#include <chrono>
#include <format>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include "subtitle.h"

namespace parabolic::file
{
    TimeFrame::TimeFrame(std::chrono::seconds start, std::chrono::seconds end)
        : m_start{ start },
          m_end{ end }
    {
    }

    std::chrono::seconds TimeFrame::start() const
    {
        return m_start;
    }

    std::chrono::seconds TimeFrame::end() const
    {
        return m_end;
    }

    std::string TimeFrame::str() const
    {
        return std::format("{:%H:%M:%S} - {:%H:%M:%S}", m_start, m_end);
    }

    std::string_view dot_extension(Type type)
    {
        switch (type)
        {
        case Type::Mp4:
            return ".mp4";
        case Type::Webm:
            return ".webm";
        case Type::Mkv:
            return ".mkv";
        case Type::Mov:
            return ".mov";
        case Type::Avi:
            return ".avi";
        case Type::Mp3:
            return ".mp3";
        case Type::M4a:
            return ".m4a";
        case Type::Opus:
            return ".opus";
        case Type::Flac:
            return ".flac";
        case Type::Wav:
            return ".wav";
        case Type::Ogg:
            return ".ogg";
        default:
            return "";
        }
    }

    bool is_audio(Type type)
    {
        switch (type)
        {
        case Type::AudioGeneric:
        case Type::Mp3:
        case Type::M4a:
        case Type::Opus:
        case Type::Flac:
        case Type::Wav:
        case Type::Ogg:
            return true;
        default:
            return false;
        }
    }

    bool is_generic(Type type)
    {
        switch (type)
        {
        case Type::AudioGeneric:
        case Type::VideoGeneric:
            return true;
        default:
            return false;
        }
    }

    bool is_video(Type type)
    {
        switch (type)
        {
        case Type::VideoGeneric:
        case Type::Mp4:
        case Type::Webm:
        case Type::Mkv:
        case Type::Mov:
        case Type::Avi:
            return true;
        default:
            return false;
        }
    }

    bool should_recode(Type type)
    {
        switch (type)
        {
        case Type::Webm:
        case Type::Mov:
        case Type::Avi:
            return true;
        default:
            return false;
        }
    }

    bool supports_subtitle(Type type, subtitle::Type subtitle_type)
    {
        switch (subtitle_type)
        {
        case subtitle::Type::Any:
        case subtitle::Type::Vtt:
            return is_video(type) && type != Type::Avi;
        case subtitle::Type::Srt:
            return is_video(type) && type != Type::Webm && type != Type::Avi;
        case subtitle::Type::Ass:
            return is_video(type) && type != Type::Mkv;
        case subtitle::Type::Lrc:
            return is_audio(type);
        default:
            return false;
        }
    }

    bool supports_thumbnails(Type type)
    {
        switch (type)
        {
        case Type::Mp4:
        case Type::Mkv:
        case Type::Mov:
        case Type::Mp3:
        case Type::M4a:
        case Type::Opus:
        case Type::Flac:
        case Type::Ogg:
            return true;
        default:
            return false;
        }
    }

    void from_json(const nlohmann::json& json, TimeFrame& time_frame)
    {
        if (json.contains("start") && json["start"].is_number_integer())
        {
            time_frame.m_start = std::chrono::seconds(json.at("start").get<int>());
        }
        else if (json.contains("Start") && json["Start"].is_number_integer())
        {
            time_frame.m_start = std::chrono::seconds(json.at("Start").get<int>());
        }
        if (json.contains("end") && json["end"].is_number_integer())
        {
            time_frame.m_end = std::chrono::seconds(json.at("end").get<int>());
        }
        else if (json.contains("End") && json["End"].is_number_integer())
        {
            time_frame.m_end = std::chrono::seconds(json.at("End").get<int>());
        }
    }

    void to_json(nlohmann::json& json, const TimeFrame& time_frame)
    {
        json = nlohmann::json{ { "start", time_frame.m_start.count() }, { "end", time_frame.m_end.count() } };
    }
}
