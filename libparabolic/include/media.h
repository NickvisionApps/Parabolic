#pragma once

#include <compare>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include "translator.h"

namespace parabolic
{
    enum class AudioCodec
    {
        Any,
        FLAC,
        WAV,
        OPUS,
        AAC,
        MP4A,
        MP3
    };

    enum class FrameRate
    {
        Any,
        Fps24,
        Fps30,
        Fps60
    };

    enum class MediaType
    {
        Video,
        Audio,
        Image,
        Unknown
    };

    enum class VideoCodec
    {
        Any,
        H265,
        VP9,
        AV01,
        H264,
    };

    class VideoResolution
    {
    public:
        VideoResolution();
        VideoResolution(int width, int height);
        static VideoResolution& best();
        static VideoResolution& worst();
        [[nodiscard]] static std::optional<VideoResolution> parse(const std::string& value, const Translator& translator);
        [[nodiscard]] int width() const;
        [[nodiscard]] int height() const;
        [[nodiscard]] std::string str(const Translator& translator) const;
        std::strong_ordering operator<=>(const VideoResolution& rhs) const;
        bool operator==(const VideoResolution& rhs) const;
        friend void from_json(const nlohmann::json& json, VideoResolution& resolution);
        friend void to_json(nlohmann::json& json, const VideoResolution& resolution);

    private:
        int m_width;
        int m_height;
    };

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

    inline void to_json(nlohmann::json& json, const VideoResolution& resolution)
    {
        json = nlohmann::json{
            {"width", resolution.m_width},
            {"height", resolution.m_height}
        };
    }
}