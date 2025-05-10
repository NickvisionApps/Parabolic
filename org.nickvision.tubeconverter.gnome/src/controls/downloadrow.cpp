#include "controls/downloadrow.h"
#include <cmath>
#include <format>
#include <libnick/localization/gettext.h>

using namespace Nickvision::Events;
using namespace Nickvision::TubeConverter::Shared::Events;
using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::GNOME::Controls
{
    DownloadRow::DownloadRow(const DownloadAddedEventArgs& args, GtkWindow* parent)
        : ControlBase{ parent, "download_row" },
        m_id{ args.getId() },
        m_log{ "" },
        m_path{ args.getPath() },
        m_isPaused{ false }
    {
        //Load
        gtk_widget_add_css_class(m_builder.get<GtkWidget>("statusIcon"), "stopped");
        gtk_image_set_from_icon_name(m_builder.get<GtkImage>("statusIcon"), "folder-download-symbolic");
        gtk_label_set_text(m_builder.get<GtkLabel>("fileNameLabel"), m_path.filename().string().c_str());
        if(args.getStatus() == DownloadStatus::Queued)
        {
            gtk_label_set_text(m_builder.get<GtkLabel>("statusLabel"), _("Queued"));
        }
        else if(args.getStatus() == DownloadStatus::Running)
        {
            gtk_label_set_text(m_builder.get<GtkLabel>("statusLabel"), _("Starting"));
            gtk_widget_set_sensitive(m_builder.get<GtkWidget>("cmdToClipboardButton"), true);
        }
        else
        {
            gtk_label_set_text(m_builder.get<GtkLabel>("statusLabel"), _("Unknown"));
        }
        adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("buttonsViewStack"), "downloading");
        adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("progViewStack"), "running");
        //Signals
        g_signal_connect(m_builder.get<GObject>("pauseResumeButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<DownloadRow*>(data)->pauseResume(); }), this);
        g_signal_connect(m_builder.get<GObject>("stopButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<DownloadRow*>(data)->stop(); }), this);
        g_signal_connect(m_builder.get<GObject>("playButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<DownloadRow*>(data)->play(); }), this);
        g_signal_connect(m_builder.get<GObject>("openButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<DownloadRow*>(data)->openFolder(); }), this);
        g_signal_connect(m_builder.get<GObject>("retryButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<DownloadRow*>(data)->retry(); }), this);
        g_signal_connect(m_builder.get<GObject>("cmdToClipboardButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<DownloadRow*>(data)->cmdToClipboard(); }), this);
        g_signal_connect(m_builder.get<GObject>("logToClipboardButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<DownloadRow*>(data)->logToClipboard(); }), this);
    }

    int DownloadRow::getId()
    {
        return m_id;
    }

    const std::string& DownloadRow::getLog()
    {
        return m_log;
    }

    Event<ParamEventArgs<int>>& DownloadRow::stopped()
    {
        return m_stopped;
    }

    Event<ParamEventArgs<int>>& DownloadRow::paused()
    {
        return m_paused;
    }

    Event<ParamEventArgs<int>>& DownloadRow::resumed()
    {
        return m_resumed;
    }

    Event<ParamEventArgs<int>>& DownloadRow::retried()
    {
        return m_retried;
    }

    Event<ParamEventArgs<int>>& DownloadRow::commandToClipboardRequested()
    {
        return m_commandToClipboardRequested;
    }

    void DownloadRow::setProgressState(const DownloadProgressChangedEventArgs& args)
    {
        gtk_image_set_from_icon_name(m_builder.get<GtkImage>("statusIcon"), "folder-download-symbolic");
        adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("progViewStack"), "running");
        if(std::isnan(args.getProgress()))
        {
            gtk_label_set_text(m_builder.get<GtkLabel>("statusLabel"), _("Processing"));
            gtk_progress_bar_pulse(m_builder.get<GtkProgressBar>("progBar"));
        }
        else
        {
            gtk_progress_bar_set_fraction(m_builder.get<GtkProgressBar>("progBar"), args.getProgress());
            gtk_label_set_text(m_builder.get<GtkLabel>("statusLabel"), std::format("{} | {}", _("Running"), args.getSpeedStr()).c_str());
        }
        if(args.getLog() != m_log)
        {
            m_log = args.getLog();
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(m_builder.get<GtkTextView>("logView")), m_log.c_str(), m_log.size());
            GtkAdjustment* vadjustment{ gtk_scrolled_window_get_vadjustment(m_builder.get<GtkScrolledWindow>("logScroll")) };
            gtk_adjustment_set_value(vadjustment, gtk_adjustment_get_upper(vadjustment));
        }
    }

    void DownloadRow::setCompleteState(const DownloadCompletedEventArgs& args)
    {
        m_path = args.getPath();
        gtk_label_set_text(m_builder.get<GtkLabel>("fileNameLabel"), m_path.filename().string().c_str());
        gtk_progress_bar_set_fraction(m_builder.get<GtkProgressBar>("progBar"), 1.0);
        gtk_widget_remove_css_class(m_builder.get<GtkWidget>("statusIcon"), "stopped");
        if(args.getStatus() == DownloadStatus::Error)
        {
            gtk_widget_add_css_class(m_builder.get<GtkWidget>("statusIcon"), "error");
            gtk_image_set_from_icon_name(m_builder.get<GtkImage>("statusIcon"), "process-stop-symbolic");
            gtk_label_set_text(m_builder.get<GtkLabel>("statusLabel"), _("Error"));
            adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("buttonsViewStack"), "error");
            gtk_level_bar_set_value(m_builder.get<GtkLevelBar>("levelBar"), 0.0);
        }
        else if(args.getStatus() == DownloadStatus::Success)
        {
            gtk_widget_add_css_class(m_builder.get<GtkWidget>("statusIcon"), "success");
            gtk_image_set_from_icon_name(m_builder.get<GtkImage>("statusIcon"), "emblem-ok-symbolic");
            gtk_label_set_text(m_builder.get<GtkLabel>("statusLabel"), _("Success"));
            adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("buttonsViewStack"), "success");
            gtk_level_bar_set_value(m_builder.get<GtkLevelBar>("levelBar"), 1.0);
        }
        adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("progViewStack"), "done");
    }

    void DownloadRow::setStopState()
    {
        gtk_progress_bar_set_fraction(m_builder.get<GtkProgressBar>("progBar"), 1.0);
        gtk_image_set_from_icon_name(m_builder.get<GtkImage>("statusIcon"), "media-playback-stop-symbolic");
        gtk_label_set_text(m_builder.get<GtkLabel>("statusLabel"), _("Stopped"));
        adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("buttonsViewStack"), "error");
        adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("progViewStack"), "done");
        gtk_level_bar_set_value(m_builder.get<GtkLevelBar>("levelBar"), 0.0);
    }

    void DownloadRow::setPauseState()
    {
        gtk_image_set_from_icon_name(m_builder.get<GtkImage>("statusIcon"), "media-playback-pause-symbolic");
        gtk_label_set_text(m_builder.get<GtkLabel>("statusLabel"), _("Paused"));
        gtk_button_set_icon_name(m_builder.get<GtkButton>("pauseResumeButton"), "media-playback-start-symbolic");
        gtk_widget_set_tooltip_text(m_builder.get<GtkWidget>("pauseResumeButton"), _("Resume"));
    }

    void DownloadRow::setResumeState()
    {
        gtk_button_set_icon_name(m_builder.get<GtkButton>("pauseResumeButton"), "media-playback-pause-symbolic");
        gtk_widget_set_tooltip_text(m_builder.get<GtkWidget>("pauseResumeButton"), _("Pause"));
    }

    void DownloadRow::setStartFromQueueState()
    {
        gtk_widget_add_css_class(m_builder.get<GtkWidget>("statusIcon"), "stopped");
        gtk_image_set_from_icon_name(m_builder.get<GtkImage>("statusIcon"), "folder-download-symbolic");
        gtk_label_set_text(m_builder.get<GtkLabel>("statusLabel"), _("Running"));
        gtk_widget_set_sensitive(m_builder.get<GtkWidget>("cmdToClipboardButton"), true);
    }

    void DownloadRow::pauseResume()
    {
        if(m_isPaused)
        {
            m_resumed.invoke({ m_id });
        }
        else
        {
            m_paused.invoke({ m_id });
        }
        m_isPaused = !m_isPaused;
    }

    void DownloadRow::stop()
    {
        m_stopped.invoke({ m_id });
    }

    void DownloadRow::play()
    {
        GtkFileLauncher* launcher{ gtk_file_launcher_new(g_file_new_for_path(m_path.string().c_str())) };
        gtk_file_launcher_launch(launcher, m_parent, nullptr, GAsyncReadyCallback(+[](GObject* source, GAsyncResult* res, gpointer)
        {
            gtk_file_launcher_launch_finish(GTK_FILE_LAUNCHER(source), res, nullptr);
            g_object_unref(source);
        }), nullptr);
    }

    void DownloadRow::openFolder()
    {
        GtkFileLauncher* launcher{ gtk_file_launcher_new(g_file_new_for_path(m_path.string().c_str())) };
        gtk_file_launcher_open_containing_folder(launcher, m_parent, nullptr, GAsyncReadyCallback(+[](GObject* source, GAsyncResult* res, gpointer)
        {
            gtk_file_launcher_open_containing_folder_finish(GTK_FILE_LAUNCHER(source), res, nullptr);
            g_object_unref(source);
        }), nullptr);
    }

    void DownloadRow::retry()
    {
        m_retried.invoke({ m_id });
    }

    void DownloadRow::cmdToClipboard()
    {
        m_commandToClipboardRequested.invoke({ m_id });
    }

    void DownloadRow::logToClipboard()
    {
        gdk_clipboard_set_text(gdk_display_get_clipboard(gdk_display_get_default()), m_log.c_str());
    }
}
