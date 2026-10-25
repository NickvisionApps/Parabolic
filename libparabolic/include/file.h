#pragma once

#include <cstdint>
#include <string>
#include "subtitle.h"

namespace parabolic::file
{
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

    std::string dot_extension(Type type);

    bool is_audio(Type type);

    bool is_generic(Type type);

    bool is_video(Type type);

    bool should_recode(Type type);

    bool supports_subtitle(Type type, subtitle::Format format);

    bool supports_thumbnails(Type type);
}
