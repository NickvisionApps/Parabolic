#ifndef GTKHELPERS_H
#define GTKHELPERS_H

#include <functional>

namespace Nickvision::TubeConverter::GNOME::Helpers::GtkHelpers
{
    /**
     * @brief Runs the function on the main UI thread.
     * @param function The function to run 
     */
    void dispatchToMainThread(const std::function<void()>& function);
}

#endif //GTKHELPERS_H