#include "comboboxdialog.hpp"
#include "../../helpers/translation.hpp"

using namespace NickvisionTubeConverter::UI::Controls;

ComboBoxDialog::ComboBoxDialog(GtkWindow* parent, const std::string& title, const std::string& description, const std::string& rowTitle, const std::vector<std::string>& choices) : m_choices{ choices }, m_response{ "cancel" }, m_gobj{ adw_message_dialog_new(parent, title.c_str(), description.c_str()) }
{
    //Dialog Settings
    gtk_window_set_hide_on_close(GTK_WINDOW(m_gobj), true);
    adw_message_dialog_add_responses(ADW_MESSAGE_DIALOG(m_gobj), "cancel", _("Cancel"), "ok", _("OK"), nullptr);
    adw_message_dialog_set_response_appearance(ADW_MESSAGE_DIALOG(m_gobj), "ok", ADW_RESPONSE_SUGGESTED);
    adw_message_dialog_set_default_response(ADW_MESSAGE_DIALOG(m_gobj), "cancel");
    adw_message_dialog_set_close_response(ADW_MESSAGE_DIALOG(m_gobj), "cancel");
    g_signal_connect(m_gobj, "response", G_CALLBACK((void (*)(AdwMessageDialog*, gchar*, gpointer))([](AdwMessageDialog*, gchar* response, gpointer data) { reinterpret_cast<ComboBoxDialog*>(data)->setResponse({ response }); })), this);
    //Preferences Group
    m_preferencesGroup = adw_preferences_group_new();
    //Choices
    m_rowChoices = adw_combo_row_new();
    gtk_widget_set_size_request(m_rowChoices, 420, -1);
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(m_rowChoices), rowTitle.c_str());
    GtkStringList* lstChoices = gtk_string_list_new(nullptr);
    for(const std::string& s : m_choices)
    {
        gtk_string_list_append(lstChoices, s.c_str());
    }
    adw_combo_row_set_model(ADW_COMBO_ROW(m_rowChoices), G_LIST_MODEL(lstChoices));
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(m_preferencesGroup), m_rowChoices);
    g_object_unref(lstChoices);
    //Layout
    adw_message_dialog_set_extra_child(ADW_MESSAGE_DIALOG(m_gobj), m_preferencesGroup);
}

GtkWidget* ComboBoxDialog::gobj()
{
    return m_gobj;
}

std::string ComboBoxDialog::run()
{
    gtk_widget_show(m_gobj);
    while(gtk_widget_is_visible(m_gobj))
    {
        g_main_context_iteration(g_main_context_default(), false);
    }
    std::string result{ m_response == "ok" ? m_choices[adw_combo_row_get_selected(ADW_COMBO_ROW(m_rowChoices))] : ""};
    gtk_window_destroy(GTK_WINDOW(m_gobj));
    return result;
}

void ComboBoxDialog::setResponse(const std::string& response)
{
    m_response = response;
}
