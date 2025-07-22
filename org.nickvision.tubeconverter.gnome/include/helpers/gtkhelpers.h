#ifndef GTKHELPERS_H
#define GTKHELPERS_H

#include <functional>
#include <string>
#include <vector>
#include <adwaita.h>

namespace Nickvision::TubeConverter::GNOME::Helpers::GtkHelpers
{
    /**
     * @brief Runs the function on the main UI thread.
     * @param function The function to run
     */
    void dispatchToMainThread(const std::function<void()>& function);
    /**
     * @brief Sets the accelerator for an action.
     * @param app The GtkApplication
     * @param action The action detailed name
     * @param accel The accelerator
     */
    void setAccelForAction(GtkApplication* app, const char* action, const char* accel);
    /**
     * @brief Sets the model for a combo row.
     * @param row The combo row
     * @param strs The strings to set
     * @param selected An optional string that should be selected
     * @param allowEllipse Whether or not to allow ellipses in the combo row
     */
    void setComboRowModel(AdwComboRow* row, const std::vector<std::string>& strs, const std::string& selected, bool allowEllipse = true);
    /**
     * @brief Sets the model for a combo row.
     * @param row The combo row
     * @param strs The strings to set
     * @param selected An optional index that should be selected
     * @param allowEllipse Whether or not to allow ellipses in the combo row
     */
    void setComboRowModel(AdwComboRow* row, const std::vector<std::string>& strs, const size_t& selected, bool allowEllipse = true);
}

#endif //GTKHELPERS_H
