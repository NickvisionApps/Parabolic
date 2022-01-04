#include "headerbar.h"

namespace NickvisionTubeConverter::Controls
{
    HeaderBar::HeaderBar()
    {
        //==Select Save Folder==//
        m_btnSelectSaveFolder.set_icon_name("folder-open");
        m_btnSelectSaveFolder.set_tooltip_text("Select Save Folder");
        //==Download Video==//
        m_btnDownloadVideo.set_icon_name("document-save");
        m_btnDownloadVideo.set_tooltip_text("Download Video");
        //==Clear Completed Downloads==//
        m_btnClearCompletedDownloads.set_icon_name("edit-select-all");
        m_btnClearCompletedDownloads.set_tooltip_text("Clear Completed Downloads");
        //==Settings==//
        m_btnSettings.set_icon_name("settings");
        m_btnSettings.set_tooltip_text("Settings");
        //==Help==//
        m_actionHelp = Gio::SimpleActionGroup::create();
        m_actionCheckForUpdates = m_actionHelp->add_action("checkForUpdates");
        m_actionGitHubRepo = m_actionHelp->add_action("gitHubRepo");
        m_actionReportABug = m_actionHelp->add_action("reportABug");
        m_actionChangelog = m_actionHelp->add_action("changelog");
        m_actionAbout = m_actionHelp->add_action("about");
        insert_action_group("help", m_actionHelp);
        m_menuHelp = Gio::Menu::create();
        m_menuHelpUpdate = Gio::Menu::create();
        m_menuHelpUpdate->append("Check for Updates", "help.checkForUpdates");
        m_menuHelpLinks = Gio::Menu::create();
        m_menuHelpLinks->append("GitHub Repo", "help.gitHubRepo");
        m_menuHelpLinks->append("Report a Bug", "help.reportABug");
        m_menuHelpActions = Gio::Menu::create();
        m_menuHelpActions->append("Changelog", "help.changelog");
        m_menuHelpActions->append("About", "help.about");
        m_menuHelp->append_section(m_menuHelpUpdate);
        m_menuHelp->append_section(m_menuHelpLinks);
        m_menuHelp->append_section(m_menuHelpActions);
        m_btnHelp.set_direction(Gtk::ArrowType::NONE);
        m_btnHelp.set_menu_model(m_menuHelp);
        m_btnHelp.set_tooltip_text("Help");
        //==Layout==//
        pack_start(m_btnSelectSaveFolder);
        pack_start(m_sep1);
        pack_start(m_btnDownloadVideo);
        pack_start(m_btnClearCompletedDownloads);
        pack_end(m_btnHelp);
        pack_end(m_btnSettings);
    }

    Gtk::Button& HeaderBar::getBtnSelectSaveFolder()
    {
        return m_btnSelectSaveFolder;
    }

    Gtk::Button& HeaderBar::getBtnDownloadVideo()
    {
        return m_btnDownloadVideo;
    }

    Gtk::Button& HeaderBar::getBtnClearCompletedDownloads()
    {
        return m_btnClearCompletedDownloads;
    }

    Gtk::Button& HeaderBar::getBtnSettings()
    {
        return m_btnSettings;
    }

    const std::shared_ptr<Gio::SimpleAction>& HeaderBar::getActionCheckForUpdates() const
    {
        return m_actionCheckForUpdates;
    }

    const std::shared_ptr<Gio::SimpleAction>& HeaderBar::getActionGitHubRepo() const
    {
        return m_actionGitHubRepo;
    }

    const std::shared_ptr<Gio::SimpleAction>& HeaderBar::getActionReportABug() const
    {
        return m_actionReportABug;
    }

    const std::shared_ptr<Gio::SimpleAction>& HeaderBar::getActionChangelog() const
    {
        return m_actionChangelog;
    }

    const std::shared_ptr<Gio::SimpleAction>& HeaderBar::getActionAbout() const
    {
        return m_actionAbout;
    }
}
