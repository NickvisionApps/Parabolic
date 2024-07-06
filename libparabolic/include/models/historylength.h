#ifndef HISTORYLENGTH_H
#define HISTORYLENGTH_H

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief Lengths of how long to keep historic items.
     */
    enum class HistoryLength
    {
        Never = 0,
        OneDay = 1,
        OneWeek = 7,
        OneMonth = 30,
        ThreeMonths = 90,
        Forever = 365
    };
}

#endif //HISTORYLENGTH_H