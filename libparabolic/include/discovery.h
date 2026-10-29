#ifndef DISCOVERY_H
#define DISCOVERY_H

#include <nlohmann/json_fwd.hpp>
#include "audio.h"
#include "subtitle.h"
#include "video.h"

namespace parabolic::discovery
{
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
