#ifndef FORMAT_H
#define FORMAT_H

#include <compare>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include "audio.h"
#include "file.h"
#include "subtitle.h"
#include "translation.h"
#include "video.h"

namespace parabolic::format
{
    class Format
    {
    public:
        Format() = default;
        Format(std::string id, std::string protocol, file::Kind kind);
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
        [[nodiscard]] file::Kind kind() const;
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
        file::Kind m_kind{ file::Kind::Unknown };
        std::optional<double> m_bitrate;
        std::optional<audio::Language> m_audio_language;
        std::optional<video::Codec> m_video_codec;
        std::optional<audio::Codec> m_audio_codec;
        std::optional<video::Resolution> m_video_resolution;
        std::optional<video::FrameRate> m_frame_rate;
    };

    class Options
    {
    public:
        [[nodiscard]] bool include_super_resolutions() const;
        void set_include_super_resolutions(bool value);
        [[nodiscard]] audio::Codec preferred_audio_codec() const;
        void set_preferred_audio_codec(audio::Codec value);
        [[nodiscard]] video::FrameRate preferred_frame_rate() const;
        void set_preferred_frame_rate(video::FrameRate value);
        [[nodiscard]] subtitle::Type preferred_subtitle_format() const;
        void set_preferred_subtitle_format(subtitle::Type value);
        [[nodiscard]] video::Codec preferred_video_codec() const;
        void set_preferred_video_codec(video::Codec value);
        bool operator==(const Options& rhs) const = default;
        friend void from_json(const nlohmann::json& json, Options& options);
        friend void to_json(nlohmann::json& json, const Options& options);

    private:
        bool m_include_super_resolutions{ true };
        audio::Codec m_preferred_audio_codec{ audio::Codec::Any };
        video::FrameRate m_preferred_frame_rate{ video::FrameRate::Any };
        subtitle::Type m_preferred_subtitle_format{ subtitle::Type::Any };
        video::Codec m_preferred_video_codec{ video::Codec::Any };
    };
}

#endif
