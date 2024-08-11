#include "helpers/gtkhelpers.h"
#include <adwaita.h>

namespace Nickvision::TubeConverter::GNOME::Helpers
{
    void GtkHelpers::dispatchToMainThread(const std::function<void()>& func)
    {
        g_main_context_invoke(g_main_context_default(), +[](gpointer data) -> int
        {
            std::function<void()>* function{ static_cast<std::function<void()>*>(data) };
            (*function)();
            delete function;
            return false;
        }, new std::function<void()>(func));
    }

    void GtkHelpers::setAccelForAction(GtkApplication* app, const char* action, const char* accel)
    {
        const char* accels[2] { accel, nullptr };
        gtk_application_set_accels_for_action(app, action, accels);
    }

    void GtkHelpers::setComboRowModel(AdwComboRow* row, const std::vector<std::string>& strs)
    {
        GtkStringList* list{ gtk_string_list_new(nullptr) };
        for(const std::string& str : strs)
        {
            gtk_string_list_append(list, str.c_str());
        }
        adw_combo_row_set_model(row, G_LIST_MODEL(list));
        adw_combo_row_set_selected(row, 0);
    }
}