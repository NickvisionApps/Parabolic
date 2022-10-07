#include "messagedialog.hpp"

using namespace NickvisionTubeConverter::UI::Controls;

MessageDialog::MessageDialog(GtkWindow* parent, const std::string& title, const std::string& description, const std::string& cancelText, const std::string& destructiveText, const std::string& suggestedText) : m_response{ MessageDialogResponse::Cancel }, m_gobj{ adw_message_dialog_new(parent, title.c_str(), description.c_str()) }
{
    //Dialog Settings
    gtk_window_set_hide_on_close(GTK_WINDOW(m_gobj), true);
    if(!cancelText.empty())
    {
        adw_message_dialog_add_response(ADW_MESSAGE_DIALOG(m_gobj), "cancel", cancelText.c_str());
        adw_message_dialog_set_default_response(ADW_MESSAGE_DIALOG(m_gobj), "cancel");
        adw_message_dialog_set_close_response(ADW_MESSAGE_DIALOG(m_gobj), "cancel");
    }
    if(!destructiveText.empty())
    {
        adw_message_dialog_add_response(ADW_MESSAGE_DIALOG(m_gobj), "destructive", destructiveText.c_str());
        adw_message_dialog_set_response_appearance(ADW_MESSAGE_DIALOG(m_gobj), "destructive", ADW_RESPONSE_DESTRUCTIVE);
    }
    if(!suggestedText.empty())
    {
        adw_message_dialog_add_response(ADW_MESSAGE_DIALOG(m_gobj), "suggested", suggestedText.c_str());
        adw_message_dialog_set_response_appearance(ADW_MESSAGE_DIALOG(m_gobj), "suggested", ADW_RESPONSE_SUGGESTED);
    }
    g_signal_connect(m_gobj, "response", G_CALLBACK((void (*)(AdwMessageDialog*, gchar*, gpointer))([](AdwMessageDialog*, gchar* response, gpointer data) { reinterpret_cast<MessageDialog*>(data)->setResponse({ response }); })), this);
}

GtkWidget* MessageDialog::gobj()
{
    return m_gobj;
}

MessageDialogResponse MessageDialog::run()
{
    gtk_widget_show(m_gobj);
    while(gtk_widget_is_visible(m_gobj))
    {
        g_main_context_iteration(g_main_context_default(), false);
    }
    gtk_window_destroy(GTK_WINDOW(m_gobj));
    return m_response;
}

void MessageDialog::setResponse(const std::string& response)
{
    if(response == "destructive")
    {
        m_response = MessageDialogResponse::Destructive;
    }
    else if(response == "suggested")
    {
        m_response = MessageDialogResponse::Suggested;
    }
    else
    {
        m_response = MessageDialogResponse::Cancel;
    }
}
