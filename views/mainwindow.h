#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm.h>
#include "../models/update/updater.h"
#include "../controls/headerbar.h"
#include "../controls/infobar.h"

namespace NickvisionTubeConverter::Views
{
    class MainWindow : public Gtk::ApplicationWindow
    {
    public:
        MainWindow();
        ~MainWindow();

    private:
        bool m_opened;
        NickvisionTubeConverter::Models::Update::Updater m_updater;
        //==UI==//
        NickvisionTubeConverter::Controls::HeaderBar m_headerBar;
        Gtk::Box m_mainBox;
        NickvisionTubeConverter::Controls::InfoBar m_infoBar;
        Gtk::Grid m_gridProperties;
        Gtk::Label m_lblVideoUrl;
        Gtk::Entry m_txtVideoUrl;
        Gtk::Label m_lblSaveFolder;
        Gtk::Entry m_txtSaveFolder;
        Gtk::Label m_lblFileFormat;
        Gtk::ComboBoxText m_cmbFileFormat;
        Gtk::Label m_lblNewFilename;
        Gtk::Entry m_txtNewFilename;
        Gtk::ScrolledWindow m_scrollDataDownloads;
        Gtk::TreeView m_dataDownloads;
        //==Slots==//
        void onShow();
        void selectSaveFolder();
        void downloadVideo();
        void clearCompletedDownloads();
        void settings();
        void checkForUpdates(const Glib::VariantBase& args);
        void gitHubRepo(const Glib::VariantBase& args);
        void reportABug(const Glib::VariantBase& args);
        void changelog(const Glib::VariantBase& args);
        void about(const Glib::VariantBase& args);
    };
}

#endif // MAINWINDOW_H
