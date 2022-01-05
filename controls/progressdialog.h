#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <string>
#include <thread>
#include <functional>
#include <gtkmm.h>

namespace NickvisionTubeConverter::Controls
{
    class ProgressDialog : public Gtk::Dialog
    {
    public:
        ProgressDialog(Gtk::Window& parent, const std::string& description, const std::function<void()>& work);
        ~ProgressDialog();
        void show();

    private:
        std::function<void()> m_work;
        bool m_isFinished;
        std::jthread m_thread;
        //==UI==//
        sigc::connection m_connectionTimeout;
        Gtk::Box m_mainBox;
        Gtk::Label m_lblDescription;
        Gtk::ProgressBar m_progBar;
        //==Slots==//
        bool timeout();
    };
}

#endif // PROGRESSDIALOG_H
