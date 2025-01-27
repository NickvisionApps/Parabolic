#include "helpers/gtkhelpers.h"

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
        g_idle_add(+[](gpointer data) -> int
        {
            std::function<void()>* function{ static_cast<std::function<void()>*>(data) };
            (*function)();
            delete function;
            return G_SOURCE_REMOVE;
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

    void GtkHelpers::setComboRowModel(AdwComboRow* row, const std::vector<std::string>& strs, const std::string& selected, bool allowEllipse)
    {
        size_t selectedIndex{ 0 };
        GtkStringList* list{ gtk_string_list_new(nullptr) };
        for(size_t i = 0; i < strs.size(); i++)
        {
            const std::string str{ strs[i] };
            gtk_string_list_append(list, str.c_str());
            if(str == selected)
            {
                selectedIndex = i;
            }
        }
        if(!allowEllipse)
        {
            GtkListItemFactory* factory{ gtk_signal_list_item_factory_new() };
            g_signal_connect(factory, "setup", G_CALLBACK(+[](GtkListItemFactory* factory, GtkListItem* item, gpointer data)
            {
                GtkLabel* lbl{ GTK_LABEL(gtk_label_new(nullptr)) };
                gtk_widget_set_halign(GTK_WIDGET(lbl), GTK_ALIGN_START);
                gtk_label_set_ellipsize(lbl, PANGO_ELLIPSIZE_NONE);
                gtk_list_item_set_child(item, GTK_WIDGET(lbl));
            }), nullptr);
            g_signal_connect(factory, "bind", G_CALLBACK(+[](GtkListItemFactory* factory, GtkListItem* item, gpointer data)
            {
                GtkStringList* list{ GTK_STRING_LIST(data) };
                GtkLabel* lbl{ GTK_LABEL(gtk_list_item_get_child(item)) };
                gtk_label_set_label(lbl, gtk_string_list_get_string(list, gtk_list_item_get_position(item)));
            }), list);
            adw_combo_row_set_factory(row, factory);
        }
        adw_combo_row_set_model(row, G_LIST_MODEL(list));
        adw_combo_row_set_selected(row, selectedIndex);
    }
}
