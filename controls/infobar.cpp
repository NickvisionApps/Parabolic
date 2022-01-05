#include "infobar.h"

namespace NickvisionTubeConverter::Controls
{
    InfoBar::InfoBar()
    {
        //==Signals==//
        signal_response().connect([&](int response)
        {
           hide();
        });
        //==Layout==//
        m_mainBox.set_orientation(Gtk::Orientation::HORIZONTAL);
        m_lblMessage.set_margin_start(6);
        m_mainBox.append(m_lblTitle);
        m_mainBox.append(m_lblMessage);
        add_child(m_mainBox);
        hide();
    }

    void InfoBar::showMessage(const std::string& title, const std::string& message, Gtk::MessageType messageType)
    {
        m_lblTitle.set_markup("<b>" + title + "</b>");
        m_lblMessage.set_text(message);
        set_show_close_button(true);
        set_message_type(messageType);
        show();
    }
}
