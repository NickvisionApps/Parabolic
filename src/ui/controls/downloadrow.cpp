#include "downloadrow.hpp"
#include <future>
#include <regex>
#include "messagedialog.hpp"

using namespace NickvisionTubeConverter::Models;
using namespace NickvisionTubeConverter::UI::Controls;

DownloadRow::DownloadRow(GtkWindow* parent, const Download& download) : m_download{ download }, m_gobj{ adw_action_row_new() }, m_parent{ parent }
{
    //Row Settings
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(m_gobj), std::regex_replace(download.getSavePath(), std::regex("\\&"), "&amp;").c_str());
    adw_action_row_set_subtitle(ADW_ACTION_ROW(m_gobj), std::regex_replace(download.getVideoUrl(), std::regex("\\&"), "&amp;").c_str());
    //Status Image
    m_imgStatus = gtk_image_new_from_icon_name("folder-download-symbolic");
    gtk_image_set_pixel_size(GTK_IMAGE(m_imgStatus), 20);
    adw_action_row_add_prefix(ADW_ACTION_ROW(m_gobj), m_imgStatus);
    //Box Downloading
    m_boxDownloading = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    //ProgBar
    m_progBar = gtk_progress_bar_new();
    gtk_widget_set_valign(m_progBar, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(m_progBar, 300, -1);
    gtk_box_append(GTK_BOX(m_boxDownloading), m_progBar);
    //Box Done
    m_boxDone = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_valign(m_boxDone, GTK_ALIGN_CENTER);
    //LevelBar
    m_levelBar = gtk_level_bar_new();
    gtk_widget_set_valign(m_levelBar, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(m_levelBar, 300, -1);
    gtk_box_append(GTK_BOX(m_boxDone), m_levelBar);
    //View Logs Button
    m_btnViewLogs = gtk_button_new();
    gtk_widget_set_valign(m_btnViewLogs, GTK_ALIGN_CENTER);
    gtk_widget_set_sensitive(m_btnViewLogs, false);
    gtk_style_context_add_class(gtk_widget_get_style_context(m_btnViewLogs), "flat");
    gtk_button_set_icon_name(GTK_BUTTON(m_btnViewLogs), "dialog-information-symbolic");
    gtk_widget_set_tooltip_text(m_btnViewLogs, "View Logs");
    g_signal_connect(m_btnViewLogs, "clicked", G_CALLBACK((void (*)(GtkButton*, gpointer))[](GtkButton*, gpointer data) { reinterpret_cast<DownloadRow*>(data)->onViewLogs(); }), this);
    gtk_box_append(GTK_BOX(m_boxDone), m_btnViewLogs);
    //View Stack
    m_viewStack = adw_view_stack_new();
    adw_view_stack_add_named(ADW_VIEW_STACK(m_viewStack), m_boxDownloading, "downloading");
    adw_view_stack_add_named(ADW_VIEW_STACK(m_viewStack), m_boxDone, "done");
    adw_action_row_add_suffix(ADW_ACTION_ROW(m_gobj), m_viewStack);
}

GtkWidget* DownloadRow::gobj()
{
    return m_gobj;
}

void DownloadRow::start()
{
    std::future<bool> result{ std::async(std::launch::async, [&]() -> bool { return m_download.download(); }) };
    std::future_status status{ std::future_status::timeout };
    gtk_style_context_add_class(gtk_widget_get_style_context(m_imgStatus), "accent");
    while(status != std::future_status::ready)
    {
        gtk_progress_bar_pulse(GTK_PROGRESS_BAR(m_progBar));
        g_main_context_iteration(g_main_context_default(), false);
        status = result.wait_for(std::chrono::milliseconds(40));
    }
    bool successful{ result.get() };
    gtk_style_context_remove_class(gtk_widget_get_style_context(m_imgStatus), "accent");
    gtk_style_context_add_class(gtk_widget_get_style_context(m_imgStatus), successful ? "success" : "error");
    adw_view_stack_set_visible_child_name(ADW_VIEW_STACK(m_viewStack), "done");
    gtk_level_bar_set_value(GTK_LEVEL_BAR(m_levelBar), successful ? 1.0 : 0.0);
    gtk_widget_set_sensitive(m_btnViewLogs, true);
}

void DownloadRow::onViewLogs()
{
    MessageDialog messageDialog{ m_parent, "Logs", m_download.getLog(), "OK" };
    messageDialog.run();
}
