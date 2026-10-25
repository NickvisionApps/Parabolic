#pragma once

#include <chrono>
#include <compare>
#include <cstdint>
#include <limits>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
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
        friend void from_json(const nlohmann::json& json, Format& format);
        friend void to_json(nlohmann::json& json, const Format& format);

    private:
        std::string m_id;
        std::string m_protocol;
        std::string m_extension;
        unsigned long m_bytes{ 0 };
        Type m_type;
        std::optional<double> m_bitrate;
        std::optional<AudioLanguage> m_audio_language;
        std::optional<VideoCodec> m_video_codec;
        std::optional<AudioCodec> m_audio_codec;
        std::optional<VideoResolution> m_video_resolution;
        std::optional<FrameRate> m_frame_rate;
    };

    inline void from_json(const nlohmann::json& json, Format& format)
    {
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
