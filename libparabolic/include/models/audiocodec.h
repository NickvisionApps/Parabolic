#ifndef AUDIOCODEC_H
#define AUDIOCODEC_H

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief Types of supported audio codecs.
     */
    enum class AudioCodec
    {
        Any = 0,
        FLAC,
        WAV,
        OPUS,
        AAC,
        MP4A,
        MP3
    };
}

#endif //AUDIOCODEC_H
