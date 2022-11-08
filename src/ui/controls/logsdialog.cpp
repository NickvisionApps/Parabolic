#include "logsdialog.hpp"

using namespace NickvisionTubeConverter::UI::Controls;

LogsDialog::LogsDialog(GtkWindow* parent, const std::string& title, const std::string& logs, const std::string& cancelText, const std::string& destructiveText, const std::string& suggestedText) : MessageDialog(parent, title, "", cancelText, destructiveText, suggestedText)
{
    //Logs
    m_scrolledWindow = gtk_scrolled_window_new();
    gtk_widget_set_size_request(m_scrolledWindow, 500, 500);
    m_textView = gtk_text_view_new();
    m_textBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textView));
    gtk_text_buffer_set_text(m_textBuffer, logs.c_str(), -1);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(m_textView), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(m_textView), TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(m_scrolledWindow), m_textView);
    adw_message_dialog_set_extra_child(ADW_MESSAGE_DIALOG(m_gobj), m_scrolledWindow);
}
