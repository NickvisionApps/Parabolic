#ifndef VIDEOCODEC_H
#define VIDEOCODEC_H

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief Types of supported video codecs.
     */
    enum class VideoCodec
    {
        Any = 0,
        VP9,
        AV01,
        H264,
        H265
    };
}

#endif //VIDEOCODEC_H
