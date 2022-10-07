#include "entrydialog.hpp"

using namespace NickvisionTubeConverter::UI::Controls;

EntryDialog::EntryDialog(GtkWindow* parent, const std::string& title, const std::string& description, const std::string& entryTitle) : m_response{ "cancel" }, m_gobj{ adw_message_dialog_new(parent, title.c_str(), description.c_str()) }
{
    //Dialog Settings
    gtk_window_set_hide_on_close(GTK_WINDOW(m_gobj), true);
    adw_message_dialog_add_responses(ADW_MESSAGE_DIALOG(m_gobj), "cancel", "Cancel", "ok", "OK", nullptr);
    adw_message_dialog_set_response_appearance(ADW_MESSAGE_DIALOG(m_gobj), "ok", ADW_RESPONSE_SUGGESTED);
    adw_message_dialog_set_default_response(ADW_MESSAGE_DIALOG(m_gobj), "cancel");
    adw_message_dialog_set_close_response(ADW_MESSAGE_DIALOG(m_gobj), "cancel");
    g_signal_connect(m_gobj, "response", G_CALLBACK((void (*)(AdwMessageDialog*, gchar*, gpointer))([](AdwMessageDialog*, gchar* response, gpointer data) { reinterpret_cast<EntryDialog*>(data)->setResponse({ response }); })), this);
    //Preferences Group
    m_preferencesGroup = adw_preferences_group_new();
    //Choices
    m_rowEntry = adw_entry_row_new();
    gtk_widget_set_size_request(m_rowEntry, 420, -1);
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(m_rowEntry), entryTitle.c_str());
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(m_preferencesGroup), m_rowEntry);
    //Layout
    adw_message_dialog_set_extra_child(ADW_MESSAGE_DIALOG(m_gobj), m_preferencesGroup);
}

GtkWidget* EntryDialog::gobj()
{
    return m_gobj;
}

std::string EntryDialog::run()
{
    gtk_widget_show(m_gobj);
    while(gtk_widget_is_visible(m_gobj))
    {
        g_main_context_iteration(g_main_context_default(), false);
    }
    std::string result{ m_response == "ok" ? gtk_editable_get_text(GTK_EDITABLE(m_rowEntry)) : "" };
    gtk_window_destroy(GTK_WINDOW(m_gobj));
    return result;
}

void EntryDialog::setResponse(const std::string& response)
{
    m_response = response;
}
