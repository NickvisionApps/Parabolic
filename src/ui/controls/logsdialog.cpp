#include "logsdialog.hpp"
#include "../../helpers/translation.hpp"

using namespace NickvisionTubeConverter::UI::Controls;

LogsDialog::LogsDialog(GtkWindow* parent, const std::string& title, const std::string& logs, const std::string& cancelText, const std::string& destructiveText, const std::string& suggestedText) : MessageDialog(parent, title, "", cancelText, destructiveText, suggestedText)
{
    //Logs
    m_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_widget_set_size_request(m_box, 500, 500);
    m_scrolledWindow = gtk_scrolled_window_new();
    gtk_box_append(GTK_BOX(m_box), m_scrolledWindow);
    m_textView = gtk_text_view_new();
    gtk_widget_set_vexpand(m_textView, TRUE);
    gtk_style_context_add_class(gtk_widget_get_style_context(m_textView), "card");
    m_textBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textView));
    gtk_text_buffer_set_text(m_textBuffer, logs.c_str(), -1);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(m_textView), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(m_textView), TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(m_scrolledWindow), m_textView);
    m_copyButton = gtk_button_new();
    gtk_button_set_label(GTK_BUTTON(m_copyButton), _("Copy to clipboard"));
    gtk_widget_add_css_class(m_copyButton, "pill");
    gtk_widget_set_halign(m_copyButton, GTK_ALIGN_END);
    g_signal_connect(m_copyButton, "clicked", G_CALLBACK((void (*)(GtkButton*, gpointer))[](GtkButton*, gpointer data) { reinterpret_cast<LogsDialog*>(data)->onCopyToClipboard(); }), this);
    gtk_box_append(GTK_BOX(m_box), m_copyButton);
    adw_message_dialog_set_extra_child(ADW_MESSAGE_DIALOG(m_gobj), m_box);
}

void LogsDialog::onCopyToClipboard() {
    GtkTextIter start, end;
    gtk_text_buffer_get_iter_at_offset(m_textBuffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(m_textBuffer, &end, -1);
    gdk_clipboard_set_text(gdk_display_get_clipboard(gdk_display_get_default()), gtk_text_buffer_get_text(m_textBuffer, &start, &end, FALSE));
}
