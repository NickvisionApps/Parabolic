#include "longmessagedialog.hpp"

using namespace NickvisionTubeConverter::UI::Controls;

LongMessageDialog::LongMessageDialog(GtkWindow* parent, const std::string& title, const std::string& description, const std::string& cancelText, const std::string& destructiveText, const std::string& suggestedText) : MessageDialog(parent, title, "", cancelText, destructiveText, suggestedText)
{
    //Description
    m_scrolledWindow = gtk_scrolled_window_new();
    gtk_widget_set_size_request(m_scrolledWindow, 500, 500);
    m_lblDescription = gtk_label_new(description.c_str());
    gtk_label_set_wrap(GTK_LABEL(m_lblDescription), true);
    gtk_label_set_justify(GTK_LABEL(m_lblDescription), GTK_JUSTIFY_CENTER);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(m_scrolledWindow), m_lblDescription);
    adw_message_dialog_set_extra_child(ADW_MESSAGE_DIALOG(m_gobj), m_scrolledWindow);
}
