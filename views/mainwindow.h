#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
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
        //==Shortcuts==//
        std::shared_ptr<Gtk::ShortcutController> m_shortcutController;
        //Select Save Folder
        std::shared_ptr<Gtk::Shortcut> m_shortcutSelectSaveFolder;
        std::shared_ptr<Gtk::KeyvalTrigger> m_shortcutSelectSaveFolderTrigger;
        std::shared_ptr<Gtk::CallbackAction> m_shortcutSelectSaveFolderAction;
        //Download Videos
        std::shared_ptr<Gtk::Shortcut> m_shortcutDownloadVideos;
        std::shared_ptr<Gtk::KeyvalTrigger> m_shortcutDownloadVideosTrigger;
        std::shared_ptr<Gtk::CallbackAction> m_shortcutDownloadVideosAction;
        //Add Download to Queue
        std::shared_ptr<Gtk::Shortcut> m_shortcutAddDownloadToQueue;
        std::shared_ptr<Gtk::KeyvalTrigger> m_shortcutAddDownloadToQueueTrigger;
        std::shared_ptr<Gtk::CallbackAction> m_shortcutAddDownloadToQueueAction;
        //Remove Download from Queue
        std::shared_ptr<Gtk::Shortcut> m_shortcutRemoveDownloadFromQueue;
        std::shared_ptr<Gtk::KeyvalTrigger> m_shortcutRemoveDownloadFromQueueTrigger;
        std::shared_ptr<Gtk::CallbackAction> m_shortcutRemoveDownloadFromQueueAction;
        //Clear Downloads Queue
        std::shared_ptr<Gtk::Shortcut> m_shortcutClearDownloadsQueue;
        std::shared_ptr<Gtk::KeyvalTrigger> m_shortcutClearDownloadsQueueTrigger;
        std::shared_ptr<Gtk::CallbackAction> m_shortcutClearDownloadsQueueAction;
        //About
        std::shared_ptr<Gtk::Shortcut> m_shortcutAbout;
        std::shared_ptr<Gtk::KeyvalTrigger> m_shortcutAboutTrigger;
        std::shared_ptr<Gtk::CallbackAction> m_shortcutAboutAction;
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
        void shortcuts(const Glib::VariantBase& args);
        void changelog(const Glib::VariantBase& args);
        void about(const Glib::VariantBase& args);
    };
}

#endif // MAINWINDOW_H
