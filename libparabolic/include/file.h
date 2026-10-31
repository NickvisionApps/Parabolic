#ifndef FILE_H
#define FILE_H

#include <chrono>
#include <compare>
#include <cstdint>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include "audio.h"
#include "subtitle.h"
#include "translation.h"
#include "video.h"

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

    class Format
    {
    public:
        Format() = default;
        Format(std::string id, std::string protocol, Kind kind);
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
        [[nodiscard]] Kind kind() const;
        [[nodiscard]] std::optional<double> bitrate() const;
        [[nodiscard]] std::optional<audio::Language> audio_language() const;
        [[nodiscard]] std::optional<video::Codec> video_codec() const;
        [[nodiscard]] std::optional<audio::Codec> audio_codec() const;
        [[nodiscard]] std::optional<video::Resolution> video_resolution() const;
        [[nodiscard]] std::optional<video::FrameRate> frame_rate() const;
        [[nodiscard]] std::string str(const translation::Translator& translator) const;
        std::strong_ordering operator<=>(const Format& rhs) const;
        bool operator==(const Format& rhs) const;
        friend void from_json(const nlohmann::json& json, Format& format);
        friend void to_json(nlohmann::json& json, const Format& format);

    private:
        std::string m_id;
        std::string m_protocol;
        std::string m_extension;
        unsigned long m_bytes{ 0 };
        Kind m_kind{ Kind::Unknown };
        std::optional<double> m_bitrate;
        std::optional<audio::Language> m_audio_language;
        std::optional<video::Codec> m_video_codec;
        std::optional<audio::Codec> m_audio_codec;
        std::optional<video::Resolution> m_video_resolution;
        std::optional<video::FrameRate> m_frame_rate;
    };

    class TimeFrame
    {
    public:
        TimeFrame() = default;
        TimeFrame(std::chrono::seconds start, std::chrono::seconds end);
        [[nodiscard]] std::chrono::seconds start() const;
        [[nodiscard]] std::chrono::seconds end() const;
        [[nodiscard]] std::string str() const;
        std::strong_ordering operator<=>(const TimeFrame& rhs) const = default;
        friend void from_json(const nlohmann::json& json, TimeFrame& time_frame);
        friend void to_json(nlohmann::json& json, const TimeFrame& time_frame);

    private:
        std::chrono::seconds m_start{ 0 };
        std::chrono::seconds m_end{ 0 };
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
