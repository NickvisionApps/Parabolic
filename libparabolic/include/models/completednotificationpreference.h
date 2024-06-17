#ifndef COMPLETEDNOTIFICATIONPREFERENCE_H
#define COMPLETEDNOTIFICATIONPREFERENCE_H

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief Notification preferences for completed downloads.
     */
    enum class CompletedNotificationPreference
    {
        ForEach = 0,
        AllCompleted,
        Never
    };
}

#endif //COMPLETEDNOTIFICATIONPREFERENCE_H