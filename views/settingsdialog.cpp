#include "settingsdialog.h"

namespace NickvisionTubeConverter::Views
{
    SettingsDialog::SettingsDialog(Gtk::Window& parent) : Gtk::Dialog("Settings", parent, true, true)
    {
        //==Settings==//
        set_default_size(600, 500);
        set_resizable(false);
        set_hide_on_close(true);
        //==General Section==//
        m_lblGeneral.set_markup("<b>General</b>");
        m_lblGeneral.set_halign(Gtk::Align::START);
        m_lblGeneral.set_margin_start(20);
        m_lblGeneral.set_margin_top(20);
        m_listGeneral.set_selection_mode(Gtk::SelectionMode::NONE);
        m_listGeneral.set_margin_top(6);
        m_listGeneral.set_margin_start(20);
        m_listGeneral.set_margin_end(20);
        m_lblMaxNumberOfActiveDownloads.set_label("Max Number of Active Downloads");
        m_lblMaxNumberOfActiveDownloads.set_halign(Gtk::Align::START);
        m_lblMaxNumberOfActiveDownloads.set_margin_start(2);
        for(int i = 1; i < 11; i++)
        {
            m_cmbMaxNumberOfActiveDownloads.append(std::to_string(i));
        }
        m_listGeneral.append(m_lblMaxNumberOfActiveDownloads);
        m_listGeneral.append(m_cmbMaxNumberOfActiveDownloads);
        //==Layout==//
        m_mainBox.set_orientation(Gtk::Orientation::VERTICAL);
        m_mainBox.append(m_lblGeneral);
        m_mainBox.append(m_listGeneral);
        m_scroll.set_child(m_mainBox);
        set_child(m_scroll);
        //==Load Configuration==//
        m_cmbMaxNumberOfActiveDownloads.set_active(m_configuration.getMaxNumberOfActiveDownloads() - 1);
    }

    SettingsDialog::~SettingsDialog()
    {
        m_configuration.setMaxNumberOfActiveDownloads(m_cmbMaxNumberOfActiveDownloads.get_active_row_number() + 1);
        m_configuration.save();
    }
}
