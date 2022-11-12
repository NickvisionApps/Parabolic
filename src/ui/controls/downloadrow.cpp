#include "downloadrow.hpp"
#include <future>
#include <regex>
#include "logsdialog.hpp"
#include "../../helpers/translation.hpp"

using namespace NickvisionTubeConverter::Models;
using namespace NickvisionTubeConverter::UI::Controls;

DownloadRow::DownloadRow(GtkWindow* parent, const std::shared_ptr<Download>& download) : m_download{ download }, m_gobj{ adw_action_row_new() }, m_parent{ parent }
{
    //Row Settings
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(m_gobj), std::regex_replace(m_download->getSavePath(), std::regex("\\&"), "&amp;").c_str());
    adw_action_row_set_subtitle(ADW_ACTION_ROW(m_gobj), std::regex_replace(m_download->getVideoUrl(), std::regex("\\&"), "&amp;").c_str());
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
    //Stop Button
    m_btnStop = gtk_button_new();
    gtk_widget_set_valign(m_btnStop, GTK_ALIGN_CENTER);
    gtk_widget_add_css_class(m_btnStop, "flat");
    gtk_button_set_icon_name(GTK_BUTTON(m_btnStop), "media-playback-stop-symbolic");
    gtk_widget_set_tooltip_text(m_btnStop, _("Stop Download"));
    g_signal_connect(m_btnStop, "clicked", G_CALLBACK((void (*)(GtkButton*, gpointer))[](GtkButton*, gpointer data) { reinterpret_cast<DownloadRow*>(data)->onStop(); }), this);
    gtk_box_append(GTK_BOX(m_boxDownloading), m_btnStop);
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
    gtk_widget_add_css_class(m_btnViewLogs, "flat");
    gtk_button_set_icon_name(GTK_BUTTON(m_btnViewLogs), "dialog-information-symbolic");
    gtk_widget_set_tooltip_text(m_btnViewLogs, _("View Logs"));
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

void DownloadRow::start(bool embedMetadata)
{
    std::future<bool> result{ std::async(std::launch::async, [&, embedMetadata]() -> bool { return m_download->download(embedMetadata); }) };
    std::future_status status{ std::future_status::timeout };
    gtk_widget_add_css_class(m_imgStatus, "accent");
    while(status != std::future_status::ready)
    {
        gtk_progress_bar_pulse(GTK_PROGRESS_BAR(m_progBar));
        g_main_context_iteration(g_main_context_default(), false);
        status = result.wait_for(std::chrono::milliseconds(40));
    }
    bool successful{ result.get() };
    gtk_widget_remove_css_class(m_imgStatus, "accent");
    gtk_widget_add_css_class(m_imgStatus, successful ? "success" : "error");
    adw_view_stack_set_visible_child_name(ADW_VIEW_STACK(m_viewStack), "done");
    gtk_level_bar_set_value(GTK_LEVEL_BAR(m_levelBar), successful ? 1.0 : 0.0);
}

void DownloadRow::onStop()
{
    m_download->stop();
}

void DownloadRow::onViewLogs()
{
    LogsDialog messageDialog{ m_parent, _("Logs"), m_download->getLog(), _("OK") };
    messageDialog.run();
}
