#include "helpers/gtkhelpers.h"
#include <adwaita.h>

namespace Nickvision::TubeConverter::GNOME::Helpers
{
    void GtkHelpers::addToBox(GtkBox* box, GtkWidget* widget, bool addSeparator)
    {
        if(addSeparator && gtk_widget_get_first_child(GTK_WIDGET(box)) != nullptr)
        {
            gtk_box_append(box, gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
        }
        gtk_box_append(box, widget);
    }

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

    void GtkHelpers::moveFromBox(GtkBox* oldBox, GtkBox* newBox, GtkWidget* widget, bool addSeparator)
    {
        GtkWidget* oldSeparator{ gtk_widget_get_prev_sibling(widget) };
        if(oldSeparator == nullptr)
        {
            oldSeparator = gtk_widget_get_next_sibling(widget);
        }
        if(oldSeparator && GTK_IS_SEPARATOR(oldSeparator))
        {
            gtk_box_remove(oldBox, oldSeparator);
        }
        gtk_box_remove(oldBox, widget);
        if(addSeparator && gtk_widget_get_first_child(GTK_WIDGET(newBox)) != nullptr)
        {
            gtk_box_append(newBox, gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
        }
        gtk_box_append(newBox, widget);
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