#pragma once

#include <string>
#include "subtitle.h"

namespace parabolic::file
{
    enum class Type
    {
        VideoGeneric,
        MP4,
        WEBM,
        MKV,
        MOV,
        AVI,
        AudioGeneric,
        MP3,
        M4A,
        OPUS,
        FLAC,
        WAV,
        OGG
    };

    std::string dot_extension(Type type);

    bool is_audio(Type type);

    bool is_generic(Type type);

    bool is_video(Type type);

    bool should_recode(Type type);

    bool supports_subtitle(Type type, subtitle::Format format);

    bool supports_thumbnails(Type type);
}
