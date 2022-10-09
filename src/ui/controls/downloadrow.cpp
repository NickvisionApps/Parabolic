#include "downloadrow.hpp"
#include <future>
#include "messagedialog.hpp"
#include <iostream>

using namespace NickvisionTubeConverter::Models;
using namespace NickvisionTubeConverter::UI::Controls;

DownloadRow::DownloadRow(GtkWindow* parent, const Download& download) : m_download{ download }, m_gobj{ adw_action_row_new() }, m_parent{ parent }
{
    //Row Settings
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(m_gobj), m_download.getSavePath().c_str());
    adw_action_row_set_subtitle(ADW_ACTION_ROW(m_gobj), m_download.getVideoUrl().c_str());
    //Progress
    m_viewStackProgress = adw_view_stack_new();
    gtk_widget_set_valign(m_viewStackProgress, GTK_ALIGN_CENTER);
    m_progBar = gtk_progress_bar_new();
    gtk_widget_set_size_request(m_progBar, 300, -1);
    m_levelBar = gtk_level_bar_new();
    gtk_widget_set_size_request(m_levelBar, 300, -1);
    adw_view_stack_add_named(ADW_VIEW_STACK(m_viewStackProgress), m_progBar, "progBar");
    adw_view_stack_add_named(ADW_VIEW_STACK(m_viewStackProgress), m_levelBar, "levelBar");
    //View Log Button
    m_btnViewLogs = gtk_button_new();
    gtk_widget_set_valign(m_btnViewLogs, GTK_ALIGN_CENTER);
    gtk_widget_set_sensitive(m_btnViewLogs, false);
    gtk_style_context_add_class(gtk_widget_get_style_context(m_btnViewLogs), "flat");
    gtk_button_set_icon_name(GTK_BUTTON(m_btnViewLogs), "dialog-information-symbolic");
    gtk_widget_set_tooltip_text(m_btnViewLogs, "View Logs");
    g_signal_connect(m_btnViewLogs, "clicked", G_CALLBACK((void (*)(GtkButton*, gpointer))[](GtkButton*, gpointer data) { reinterpret_cast<DownloadRow*>(data)->onViewLogs(); }), this);
    //Box
    m_boxSuffix = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_box_append(GTK_BOX(m_boxSuffix), m_viewStackProgress);
    gtk_box_append(GTK_BOX(m_boxSuffix), m_btnViewLogs);
    adw_action_row_add_suffix(ADW_ACTION_ROW(m_gobj), m_boxSuffix);
}

GtkWidget* DownloadRow::gobj()
{
    return m_gobj;
}

void DownloadRow::start()
{
    std::future<bool> result{ std::async(std::launch::async, [&]() -> bool { return m_download.download(); }) };
    std::future_status status{ std::future_status::timeout };
    while(status != std::future_status::ready)
    {
        gtk_progress_bar_pulse(GTK_PROGRESS_BAR(m_progBar));
        g_main_context_iteration(g_main_context_default(), false);
        status = result.wait_for(std::chrono::milliseconds(40));
    }
    adw_view_stack_set_visible_child_name(ADW_VIEW_STACK(m_viewStackProgress), "levelBar");
    gtk_level_bar_set_value(GTK_LEVEL_BAR(m_levelBar), result.get() ? 1.0 : 0.0);
    gtk_widget_set_sensitive(m_btnViewLogs, true);
}

void DownloadRow::onViewLogs()
{
    MessageDialog messageDialog{ m_parent, "Logs", m_download.getLog(), "OK" };
    messageDialog.run();
}
