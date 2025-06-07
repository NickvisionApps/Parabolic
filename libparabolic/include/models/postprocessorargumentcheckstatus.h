#ifndef POSTPROCESSORARGUMENTCHECKSTATUS_H
#define POSTPROCESSORARGUMENTCHECKSTATUS_H

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief Statuses for when a post processor argument is validated.
     */
    enum class PostProcessorArgumentCheckStatus
    {
        Valid,
        EmptyName,
        ExistingName,
        EmptyArgs
    };
}

#endif //POSTPROCESSORARGUMENTCHECKSTATUS_H
