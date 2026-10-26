#ifndef FILE_H
#define FILE_H

#include <chrono>
#include <compare>
#include <cstdint>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include "subtitle.h"

namespace parabolic::file
{
    enum class Kind : std::uint8_t
    {
        Video,
        Audio,
        Image,
        Unknown,
    };

    enum class Type : std::uint8_t
    {
        VideoGeneric,
        Mp4,
        Webm,
        Mkv,
        Mov,
        Avi,
        AudioGeneric,
        Mp3,
        M4a,
        Opus,
        Flac,
        Wav,
        Ogg,
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

    [[nodiscard]] std::string_view dot_extension(Type type);
    [[nodiscard]] bool is_audio(Type type);
    [[nodiscard]] bool is_generic(Type type);
    [[nodiscard]] bool is_video(Type type);
    [[nodiscard]] bool should_recode(Type type);
    [[nodiscard]] bool supports_subtitle(Type type, subtitle::Type subtitle_type);
    [[nodiscard]] bool supports_thumbnails(Type type);
}

#endif
