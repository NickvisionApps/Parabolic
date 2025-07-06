#ifndef GTKHELPERS_H
#define GTKHELPERS_H

#include <functional>
#include <string>
#include <vector>
#include <adwaita.h>

namespace Nickvision::TubeConverter::GNOME::Helpers::GtkHelpers
{
    /**
     * @brief Adds a widget to a GtkBox.
     * @param box The box to add the widget to
     * @param widget The widget to add
     * @param addSeparator Whether or not to add a separator between the widgets
     */
    void addToBox(GtkBox* box, GtkWidget* widget, bool addSeparator = false);
    /**
     * @brief Runs the function on the main UI thread.
     * @param function The function to run 
     */
    void dispatchToMainThread(const std::function<void()>& function);
    /**
     * @brief Moves a widget from one box to another.
     * @param oldBox The old box
     * @param newBox The new box
     * @param widget The widget to move
     * @param addSeparator Whether or not to add a separator between the widgets
     */
    void moveFromBox(GtkBox* oldBox, GtkBox* newBox, GtkWidget* widget, bool addSeparator = false);
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
    void setComboRowModel(AdwComboRow* row, const std::vector<std::string>& strs, size_t selected, bool allowEllipse = true);
}

#endif //GTKHELPERS_H
