#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <gtkmm.h>
#include "../models/configuration.h"

namespace NickvisionTubeConverter::Views
{
    class SettingsDialog : public Gtk::Dialog
    {
    public:
        SettingsDialog(Gtk::Window& parent);
        ~SettingsDialog();

    private:
        NickvisionTubeConverter::Models::Configuration m_configuration;
        Gtk::ScrolledWindow m_scroll;
        Gtk::Box m_mainBox;
        Gtk::Label m_lblGeneral;
        Gtk::ListBox m_listGeneral;
        Gtk::Label m_lblMaxNumberOfActiveDownloads;
        Gtk::ComboBoxText m_cmbMaxNumberOfActiveDownloads;
    };
}

#endif // SETTINGSDIALOG_H
