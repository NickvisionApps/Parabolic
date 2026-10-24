#pragma once

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

    enum class VideoCodec
    {
        Any,
        H265,
        VP9,
        AV01,
        H264,
    };
}