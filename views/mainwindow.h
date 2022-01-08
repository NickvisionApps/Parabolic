#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm.h>
#include "../models/update/updater.h"
#include "../models/downloadmanager.h"
#include "../models/datadownloadscolumns.h"
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
        NickvisionTubeConverter::Models::DownloadManager m_downloadManager;
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
        NickvisionTubeConverter::Models::DataDownloadsColumns m_dataDownloadsColumns;
        std::shared_ptr<Gtk::ListStore> m_dataDownloadsModel;
        //==Slots==//
        void onShow();
        void selectSaveFolder();
        void downloadVideos();
        void addDownloadToQueue();
        void removeSelectedDownloadFromQueue();
        void removeAllQueuedDownloads();
        void checkForUpdates(const Glib::VariantBase& args);
        void gitHubRepo(const Glib::VariantBase& args);
        void reportABug(const Glib::VariantBase& args);
        void settings(const Glib::VariantBase& args);
        void changelog(const Glib::VariantBase& args);
        void about(const Glib::VariantBase& args);
    };
}

#endif // MAINWINDOW_H
