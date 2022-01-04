#ifndef HEADERBAR_H
#define HEADERBAR_H

#include <memory>
#include <gtkmm.h>

namespace NickvisionTubeConverter::Controls
{
    class HeaderBar : public Gtk::HeaderBar
    {
    public:
        HeaderBar();
        Gtk::Button& getBtnSelectSaveFolder();
        Gtk::Button& getBtnDownloadVideo();
        Gtk::Button& getBtnClearCompletedDownloads();
        Gtk::Button& getBtnSettings();
        const std::shared_ptr<Gio::SimpleAction>& getActionCheckForUpdates() const;
        const std::shared_ptr<Gio::SimpleAction>& getActionGitHubRepo() const;
        const std::shared_ptr<Gio::SimpleAction>& getActionReportABug() const;
        const std::shared_ptr<Gio::SimpleAction>& getActionChangelog() const;
        const std::shared_ptr<Gio::SimpleAction>& getActionAbout() const;

    private:
        //==Select Save Folder==//
        Gtk::Button m_btnSelectSaveFolder;
        //==Download Video==//
        Gtk::Button m_btnDownloadVideo;
        //==Clear Completed Downloads==//
        Gtk::Button m_btnClearCompletedDownloads;
        //==Settings==//
        Gtk::Button m_btnSettings;
        //==Help==//
        std::shared_ptr<Gio::SimpleActionGroup> m_actionHelp;
        std::shared_ptr<Gio::SimpleAction> m_actionCheckForUpdates;
        std::shared_ptr<Gio::SimpleAction> m_actionGitHubRepo;
        std::shared_ptr<Gio::SimpleAction> m_actionReportABug;
        std::shared_ptr<Gio::SimpleAction> m_actionChangelog;
        std::shared_ptr<Gio::SimpleAction> m_actionAbout;
        std::shared_ptr<Gio::Menu> m_menuHelp;
        std::shared_ptr<Gio::Menu> m_menuHelpUpdate;
        std::shared_ptr<Gio::Menu> m_menuHelpLinks;
        std::shared_ptr<Gio::Menu> m_menuHelpActions;
        Gtk::MenuButton m_btnHelp;
        //==Separators==//
        Gtk::Separator m_sep1;
    };
}

#endif // HEADERBAR_H
