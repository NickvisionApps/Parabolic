#pragma once

#include <chrono>
#include <compare>
#include <cstdint>
#include <limits>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
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

    enum class MediaType : std::uint8_t
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

    class TimeFrame
    {
    public:
        TimeFrame(std::chrono::seconds start, std::chrono::seconds end);
        [[nodiscard]] std::chrono::seconds start() const;
        [[nodiscard]] std::chrono::seconds end() const;
        [[nodiscard]] std::string str() const;
        std::strong_ordering operator<=>(const TimeFrame& rhs) const = default;
        friend void from_json(const nlohmann::json& json, TimeFrame& timeFrame);
        friend void to_json(nlohmann::json& json, const TimeFrame& timeFrame);

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
        [[nodiscard]] static std::optional<VideoResolution> parse(const std::string& value,
                                                                  const translator::Translator& translator);
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

    inline void from_json(const nlohmann::json& json, TimeFrame& timeFrame)
    {
        if (json.contains("start") && json["start"].is_number_integer())
        {
            timeFrame.m_start = std::chrono::seconds(json.at("start").get<int>());
        }
        else if (json.contains("Start") && json["Start"].is_number_integer())
        {
            timeFrame.m_start = std::chrono::seconds(json.at("Start").get<int>());
        }
        if (json.contains("end") && json["end"].is_number_integer())
        {
            timeFrame.m_end = std::chrono::seconds(json.at("end").get<int>());
        }
        else if (json.contains("End") && json["End"].is_number_integer())
        {
            timeFrame.m_end = std::chrono::seconds(json.at("End").get<int>());
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

    inline void to_json(nlohmann::json& json, const TimeFrame& timeFrame)
    {
        json = nlohmann::json{ { "start", timeFrame.m_start.count() }, { "end", timeFrame.m_end.count() } };
    }

    inline void to_json(nlohmann::json& json, const VideoResolution& resolution)
    {
        json = nlohmann::json{ { "width", resolution.m_width }, { "height", resolution.m_height } };
    }
}
