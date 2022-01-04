#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm.h>
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
        //==UI==//
        NickvisionTubeConverter::Controls::HeaderBar m_headerBar;
        Gtk::Box m_mainBox;
        NickvisionTubeConverter::Controls::InfoBar m_infoBar;
        Gtk::Label m_lblName;
        Gtk::Entry m_txtName;
        //==Slots==//
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
