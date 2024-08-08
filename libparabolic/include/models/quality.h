#ifndef QUALITY_H
#define QUALITY_H

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief Qualities for a download.
     */
    enum class Quality
    {
        Best = 0,
        Good,
        Worst,
    };
}

#endif //QUALITY_H