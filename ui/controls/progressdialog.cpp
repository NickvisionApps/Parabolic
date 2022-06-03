#include "progressdialog.h"

using namespace NickvisionTubeConverter::UI;
using namespace NickvisionTubeConverter::UI::Controls;

ProgressDialog::ProgressDialog(GtkWidget* parent, const std::string& description, const std::function<void()>& work, const std::function<void()>& then) : Widget{"/org/nickvision/tubeconverter/ui/controls/progressdialog.xml", "adw_progDialog"}, m_work{work}, m_then{then}, m_isFinished{false}
{
    //==Dialog==//
    gtk_window_set_transient_for(GTK_WINDOW(m_gobj), GTK_WINDOW(parent));
    g_timeout_add(50, [](void* data) -> int 
    { 
        ProgressDialog* dialog = reinterpret_cast<ProgressDialog*>(data);
        bool result = dialog->timeout();
        if(!result)
        {
            delete dialog;
        }
        return result;
    }, this);
    //==Description==//
    gtk_label_set_markup(GTK_LABEL(gtk_builder_get_object(m_builder, "gtk_lblDescription")), std::string("<b>" + description + "</b>").c_str());
    //==Thread==//
    m_thread = std::jthread{[&]()
    {
        m_work();
        std::lock_guard<std::mutex> lock{m_mutex};
        m_isFinished = true;
    }};
}

void ProgressDialog::show()
{
    std::lock_guard<std::mutex> lock{m_mutex};
    if(!m_isFinished)
    {
        gtk_widget_show(m_gobj);
    }
}

bool ProgressDialog::timeout()
{
    std::lock_guard<std::mutex> lock{m_mutex};
    gtk_progress_bar_pulse(GTK_PROGRESS_BAR(gtk_builder_get_object(m_builder, "gtk_progBar")));
    if(m_isFinished)
    {
        m_then();
        gtk_window_destroy(GTK_WINDOW(m_gobj));
        return false;
    }
    return true;
}
