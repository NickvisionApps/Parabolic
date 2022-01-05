#ifndef INFOBAR_H
#define INFOBAR_H

#include <string>
#include <gtkmm.h>

namespace NickvisionTubeConverter::Controls
{
    class InfoBar : public Gtk::InfoBar
    {
    public:
        InfoBar();
        void showMessage(const std::string& title, const std::string& message, Gtk::MessageType messageType = Gtk::MessageType::INFO);

    private:
        Gtk::Box m_mainBox;
        Gtk::Label m_lblTitle;
        Gtk::Label m_lblMessage;
    };
}

#endif // INFOBAR_H
