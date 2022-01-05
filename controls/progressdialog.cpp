#include "progressdialog.h"

namespace NickvisionTubeConverter::Controls
{
    ProgressDialog::ProgressDialog(Gtk::Window& parent, const std::string& description, const std::function<void()>& work) : Gtk::Dialog("Please Wait", parent, true, true), m_work(work), m_isFinished(false)
    {
        //==Settings==//
        set_default_size(400, 120);
        set_resizable(false);
        set_hide_on_close(true);
        set_deletable(false);
        m_connectionTimeout = Glib::signal_timeout().connect(sigc::mem_fun(*this, &ProgressDialog::timeout), 50);
        //==Description==//
        m_lblDescription.set_markup("<b>" + description + "</b>");
        m_lblDescription.set_halign(Gtk::Align::START);
        m_lblDescription.set_margin(6);
        //==Progress Bar==//
        m_progBar.set_margin(10);
        //==Layout==//
        m_mainBox.set_orientation(Gtk::Orientation::VERTICAL);
        m_mainBox.append(m_lblDescription);
        m_mainBox.append(m_progBar);
        set_child(m_mainBox);
        //==Thread==//
        m_thread = std::jthread([&]()
        {
            m_work();
            m_isFinished = true;
        });
    }

    ProgressDialog::~ProgressDialog()
    {
        m_connectionTimeout.disconnect();
    }

    void ProgressDialog::show()
    {
        if(!m_isFinished)
        {
            Gtk::Dialog::show();
        }
    }

    bool ProgressDialog::timeout()
    {
        m_progBar.pulse();
        if(m_isFinished)
        {
            hide();
        }
        return true;
    }
}
