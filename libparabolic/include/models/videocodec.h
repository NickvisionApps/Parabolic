#ifndef VIDEOCODEC_H
#define VIDEOCODEC_H

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief Types of supported video codecs.
     */
    enum class VideoCodec
    {
        VP9 = 0,
        AV01 = 1,
        H264 = 2
    };
}

#endif //VIDEOCODEC_H