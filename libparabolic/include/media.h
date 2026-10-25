#pragma once

#include <chrono>
#include <compare>
#include <cstdint>
#include <format>
#include <limits>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include "string_manip.h"
#include "translator.h"

namespace parabolic::media
{
    enum class AudioCodec : std::uint8_t
    {
        Any,
        Flac,
        Wav,
        Opus,
        Aac,
        Mp4a,
        Mp3,
    };

    enum class FrameRate : std::uint8_t
    {
        Any,
        Fps24,
        Fps30,
        Fps60,
    };

    enum class Type : std::uint8_t
    {
        Video,
        Audio,
        Image,
        Unknown,
    };

    enum class VideoCodec : std::uint8_t
    {
        Any,
        H265,
        Vp9,
        Av01,
        H264,
    };

    class AudioLanguage
    {
    public:
        AudioLanguage(std::string language, bool audio_description);
        [[nodiscard]] const std::string& language() const;
        [[nodiscard]] bool has_audio_description() const;

    private:
        std::string m_language;
        bool m_audio_description;
    };

    class TimeFrame
    {
    public:
        TimeFrame(std::chrono::seconds start, std::chrono::seconds end);
        [[nodiscard]] std::chrono::seconds start() const;
        [[nodiscard]] std::chrono::seconds end() const;
        [[nodiscard]] std::string str() const;
        std::strong_ordering operator<=>(const TimeFrame& rhs) const = default;
        friend void from_json(const nlohmann::json& json, TimeFrame& time_frame);
        friend void to_json(nlohmann::json& json, const TimeFrame& time_frame);

    private:
        std::chrono::seconds m_start;
        std::chrono::seconds m_end;
    };

    class VideoResolution
    {
    public:
        VideoResolution() = default;
        VideoResolution(int width, int height);
        static VideoResolution& best();
        static VideoResolution& worst();
        [[nodiscard]] static std::optional<VideoResolution> parse(std::string_view value);
        [[nodiscard]] int width() const;
        [[nodiscard]] int height() const;
        [[nodiscard]] std::string str(const translator::Translator& translator) const;
        std::strong_ordering operator<=>(const VideoResolution& rhs) const;
        bool operator==(const VideoResolution& rhs) const;
        friend void from_json(const nlohmann::json& json, VideoResolution& resolution);
        friend void to_json(nlohmann::json& json, const VideoResolution& resolution);

    private:
        int m_width{ std::numeric_limits<int>::max() };
        int m_height{ std::numeric_limits<int>::max() };
    };

    class Format
    {
    public:
        Format() = default;
        Format(std::string id, std::string protocol, Type type);
        [[nodiscard]] static Format& best_video();
        [[nodiscard]] static Format& best_audio();
        [[nodiscard]] static Format& worst_video();
        [[nodiscard]] static Format& worst_audio();
        [[nodiscard]] static Format& none_video();
        [[nodiscard]] static Format& none_audio();
        [[nodiscard]] const std::string& id() const;
        [[nodiscard]] const std::string& protocol() const;
        [[nodiscard]] const std::string& extension() const;
        [[nodiscard]] unsigned long bytes() const;
        [[nodiscard]] Type type() const;
        [[nodiscard]] std::optional<double> bitrate() const;
        [[nodiscard]] std::optional<AudioLanguage> audio_language() const;
        [[nodiscard]] std::optional<VideoCodec> video_codec() const;
        [[nodiscard]] std::optional<AudioCodec> audio_codec() const;
        [[nodiscard]] std::optional<VideoResolution> video_resolution() const;
        [[nodiscard]] std::optional<FrameRate> frame_rate() const;
        [[nodiscard]] std::string str(const translator::Translator& translator) const;
        std::strong_ordering operator<=>(const Format& rhs) const;
        bool operator==(const Format& rhs) const;
        friend void from_json(const nlohmann::json& json, Format& format);
        friend void to_json(nlohmann::json& json, const Format& format);

    private:
        std::string m_id;
        std::string m_protocol;
        std::string m_extension;
        unsigned long m_bytes{ 0 };
        Type m_type{ Type::Unknown };
        std::optional<double> m_bitrate;
        std::optional<AudioLanguage> m_audio_language;
        std::optional<VideoCodec> m_video_codec;
        std::optional<AudioCodec> m_audio_codec;
        std::optional<VideoResolution> m_video_resolution;
        std::optional<FrameRate> m_frame_rate;
    };

    inline void from_json(const nlohmann::json& json, Format& format)
    {
        if (json.contains("format_id") && json["format_id"].is_string())
        {
            format.m_id = json.at("format_id").get<std::string>();
        }
        if (json.contains("protocol") && json["protocol"].is_string())
        {
            format.m_protocol = json.at("protocol").get<std::string>();
        }
        if (json.contains("ext") && json["ext"].is_string())
        {
            format.m_extension = json.at("ext").get<std::string>();
        }
        if (json.contains("filesize") && json["filesize"].is_number_unsigned())
        {
            format.m_bytes = json.at("filesize").get<unsigned long>();
        }
        if (json.contains("tbr") && json["tbr"].is_number())
        {
            format.m_bitrate = json.at("tbr").get<double>();
        }
        std::string note;
        std::string resolution;
        if (json.contains("format_note") && json["format_note"].is_string())
        {
            note = json.at("format_note").get<std::string>();
        }
        if (json.contains("resolution") && json["resolution"].is_string())
        {
            resolution = json.at("resolution").get<std::string>();
        }
        if (resolution == "audio only")
        {
            format.m_type = Type::Audio;
        }
        else
        {
            format.m_type = note == "storyboard" ? Type::Image : Type::Video;
            format.m_video_resolution = VideoResolution::parse(resolution);
        }
        if (json.contains("language") && json["language"].is_string() && !json["language"].get<std::string>().empty())
        {
            format.m_audio_language = { json.at("language").get<std::string>(),
                                        string_manip::lower(format.m_id).contains("audiodesc") };
        }
        if (json.contains("vcodec") && json["vcodec"].is_string())
        {
            std::string vcodec{ string_manip::lower(json.at("vcodec").get<std::string>()) };
            if (vcodec.contains("vp09") || vcodec.contains("vp9"))
            {
                format.m_video_codec = VideoCodec::Vp9;
            }
            else if (vcodec.contains("av01"))
            {
                format.m_video_codec = VideoCodec::Av01;
            }
            else if (vcodec.contains("avc1") || vcodec.contains("h264"))
            {
                format.m_video_codec = VideoCodec::H264;
            }
            else if (vcodec.contains("hevc") || vcodec.contains("h265"))
            {
                format.m_video_codec = VideoCodec::H265;
            }
        }
        if (json.contains("acodec") && json["acodec"].is_string())
        {
            std::string acodec{ string_manip::lower(json.at("acodec").get<std::string>()) };
            if (acodec.contains("flac") || acodec.contains("alac"))
            {
                format.m_audio_codec = AudioCodec::Flac;
            }
            else if (acodec.contains("wav") || acodec.contains("aiff"))
            {
                format.m_audio_codec = AudioCodec::Wav;
            }
            else if (acodec.contains("opus"))
            {
                format.m_audio_codec = AudioCodec::Opus;
            }
            else if (acodec.contains("aac"))
            {
                format.m_audio_codec = AudioCodec::Aac;
            }
            else if (acodec.contains("mp4a"))
            {
                format.m_audio_codec = AudioCodec::Mp4a;
            }
            else if (acodec.contains("mp3"))
            {
                format.m_audio_codec = AudioCodec::Mp3;
            }
        }
        if (json.contains("fps") && json["fps"].is_number())
        {
            double fps{ json.at("fps").get<double>() };
            if (fps == 24.0)
            {
                format.m_frame_rate = FrameRate::Fps24;
            }
            else if (fps == 30.0)
            {
                format.m_frame_rate = FrameRate::Fps30;
            }
            else if (fps == 60.0)
            {
                format.m_frame_rate = FrameRate::Fps60;
            }
        }
    }

    inline void from_json(const nlohmann::json& json, TimeFrame& time_frame)
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

    inline void from_json(const nlohmann::json& json, VideoResolution& resolution)
    {
        if (json.contains("width") && json["width"].is_number_integer())
        {
            resolution.m_width = json.at("width").get<int>();
        }
        else if (json.contains("Width") && json["Width"].is_number_integer())
        {
            resolution.m_width = json.at("Width").get<int>();
        }
        if (json.contains("height") && json["height"].is_number_integer())
        {
            resolution.m_height = json.at("height").get<int>();
        }
        else if (json.contains("Height") && json["Height"].is_number_integer())
        {
            resolution.m_height = json.at("Height").get<int>();
        }
    }

    inline void to_json(nlohmann::json& json, const Format& format)
    {
        json = nlohmann::json{
            { "format_id", format.m_id },
            { "protocol", format.m_protocol },
            { "ext", format.m_extension },
            { "filesize", format.m_bytes },
            { "format_note", format.m_type == Type::Image ? "storyboard" : "" },
            { "resolution", "" },
            { "vcodec", "none" },
            { "acodec", "none" },
        };
        if (format.m_type == Type::Audio)
        {
            json["resolution"] = "audio only";
        }
        else if (format.m_video_resolution)
        {
            json["resolution"] =
                    std::format("{}x{}", format.m_video_resolution->width(), format.m_video_resolution->height());
        }
        if (format.m_bitrate)
        {
            json["tbr"] = *format.m_bitrate;
        }
        if (format.m_audio_language)
        {
            json["language"] = format.m_audio_language->language();
        }
        if (format.m_video_codec)
        {
            switch (*format.m_video_codec)
            {
            case VideoCodec::Vp9:
                json["vcodec"] = "vp09";
                break;
            case VideoCodec::Av01:
                json["vcodec"] = "av01";
                break;
            case VideoCodec::H264:
                json["vcodec"] = "avc1";
                break;
            case VideoCodec::H265:
                json["vcodec"] = "hevc";
                break;
            default:
                break;
            }
        }
        if (format.m_audio_codec)
        {
            switch (*format.m_audio_codec)
            {
            case AudioCodec::Flac:
                json["acodec"] = "flac";
                break;
            case AudioCodec::Wav:
                json["acodec"] = "wav";
                break;
            case AudioCodec::Opus:
                json["acodec"] = "opus";
                break;
            case AudioCodec::Aac:
                json["acodec"] = "aac";
                break;
            case AudioCodec::Mp4a:
                json["acodec"] = "mp4a";
                break;
            case AudioCodec::Mp3:
                json["acodec"] = "mp3";
                break;
            default:
                break;
            }
        }
        if (format.m_frame_rate)
        {
            switch (*format.m_frame_rate)
            {
            case FrameRate::Fps24:
                json["fps"] = 24.0;
                break;
            case FrameRate::Fps30:
                json["fps"] = 30.0;
                break;
            case FrameRate::Fps60:
                json["fps"] = 60.0;
                break;
            default:
                break;
            }
        }
    }

    inline void to_json(nlohmann::json& json, const TimeFrame& time_frame)
    {
        json = nlohmann::json{ { "start", time_frame.m_start.count() }, { "end", time_frame.m_end.count() } };
    }

    inline void to_json(nlohmann::json& json, const VideoResolution& resolution)
    {
        json = nlohmann::json{ { "width", resolution.m_width }, { "height", resolution.m_height } };
    }
}
