#ifndef SUBTITLEFORMAT_H
#define SUBTITLEFORMAT_H

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief Types of formats for subtitles.
     */
    enum class SubtitleFormat
    {
        Any = 0,
        VTT,
        SRT,
        ASS,
        LRC
    };
}

#endif //SUBTITLEFORMAT_H