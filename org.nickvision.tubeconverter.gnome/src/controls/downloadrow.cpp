#include "controls/downloadrow.h"
#include <libnick/localization/gettext.h>

using namespace Nickvision::Events;
using namespace Nickvision::TubeConverter::Shared::Events;
using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::GNOME::Controls
{
    DownloadRow::DownloadRow(const DownloadAddedEventArgs& args, GtkWindow* parent)
        : ControlBase{ parent, "download_row" },
        m_id{ args.getId() },
        m_log{ _("Starting download...") },
        m_path{ args.getPath() }
    {
        //Load
        gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(m_builder, "fileNameLabel")), m_path.filename().string().c_str());
        switch(args.getStatus())
        {
        case DownloadStatus::Queued:
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(m_builder, "statusLabel")), _("Queued"));
            break;
        case DownloadStatus::Running:
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(m_builder, "statusLabel")), _("Running"));
            break;
        }
        adw_view_stack_set_visible_child_name(ADW_VIEW_STACK(gtk_builder_get_object(m_builder, "buttonsViewStack")), "downloading");
        adw_view_stack_set_visible_child_name(ADW_VIEW_STACK(gtk_builder_get_object(m_builder, "progViewStack")), "downloading");
        //Signals
        g_signal_connect(gtk_builder_get_object(m_builder, "stopButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<DownloadRow*>(data)->stop(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "playButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<DownloadRow*>(data)->play(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "openButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<DownloadRow*>(data)->openFolder(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "retryButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<DownloadRow*>(data)->retry(); }), this);
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

    Event<ParamEventArgs<int>>& DownloadRow::retried()
    {
        return m_retried;
    }

    void DownloadRow::setProgressState(const DownloadProgressChangedEventArgs& args)
    {
        
    }

    void DownloadRow::setCompleteState(const DownloadCompletedEventArgs& args)
    {

    }

    void DownloadRow::setStopState()
    {

    }

    void DownloadRow::setStartFromQueueState()
    {

    }

    void DownloadRow::stop()
    {

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
        GtkFileLauncher* launcher{ gtk_file_launcher_new(g_file_new_for_path(m_path.parent_path().string().c_str())) };
        gtk_file_launcher_launch(launcher, m_parent, nullptr, GAsyncReadyCallback(+[](GObject* source, GAsyncResult* res, gpointer)
        {
            gtk_file_launcher_launch_finish(GTK_FILE_LAUNCHER(source), res, nullptr);
            g_object_unref(source);
        }), nullptr);
    }

    void DownloadRow::retry()
    {

    }
}