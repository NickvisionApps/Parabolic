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
        void setTitle(const std::string& title);
        void setSubtitle(const std::string& subtitle);
        Gtk::Button& getBtnSelectSaveFolder();
        Gtk::Button& getBtnDownloadVideos();
        Gtk::Button& getBtnAddDownloadToQueue();
        Gtk::Button& getBtnRemoveSelectedDownloadFromQueue();
        Gtk::Popover& getPopRemoveAllQueuedDownloads();
        Gtk::Button& getBtnConfirmRemoveAllQueuedDownloads();
        Gtk::MenuButton& getBtnRemoveAllQueuedDownloads();
        const std::shared_ptr<Gio::SimpleAction>& getActionCheckForUpdates() const;
        const std::shared_ptr<Gio::SimpleAction>& getActionGitHubRepo() const;
        const std::shared_ptr<Gio::SimpleAction>& getActionReportABug() const;
        const std::shared_ptr<Gio::SimpleAction>& getActionSettings() const;
        const std::shared_ptr<Gio::SimpleAction>& getActionShortcuts() const;
        const std::shared_ptr<Gio::SimpleAction>& getActionChangelog() const;
        const std::shared_ptr<Gio::SimpleAction>& getActionAbout() const;

    private:
        //==Title Widget==//
        Gtk::Box m_boxTitle;
        Gtk::Label m_lblTitle;
        Gtk::Label m_lblSubtitle;
        //==Select Save Folder==//
        Gtk::Button m_btnSelectSaveFolder;
        //==Download Videos==//
        Gtk::Button m_btnDownloadVideos;
        //==Queue==//
        Gtk::Button m_btnAddDownloadToQueue;
        Gtk::Button m_btnRemoveSelectedDownloadFromQueue;
        Gtk::Popover m_popRemoveAllQueuedDownloads;
        Gtk::Box m_boxRemoveAllQueuedDownloads;
        Gtk::Label m_lblRemoveAllQueuedDownloads;
        Gtk::Box m_boxBtnsRemoveAllQueuedDownloads;
        Gtk::Button m_btnConfirmRemoveAllQueuedDownloads;
        Gtk::Button m_btnCancelRemoveAllQueuedDownloads;
        Gtk::MenuButton m_btnRemoveAllQueuedDownloads;
        //==Help==//
        std::shared_ptr<Gio::SimpleActionGroup> m_actionHelp;
        std::shared_ptr<Gio::SimpleAction> m_actionCheckForUpdates;
        std::shared_ptr<Gio::SimpleAction> m_actionGitHubRepo;
        std::shared_ptr<Gio::SimpleAction> m_actionReportABug;
        std::shared_ptr<Gio::SimpleAction> m_actionChangelog;
        std::shared_ptr<Gio::SimpleAction> m_actionSettings;
        std::shared_ptr<Gio::SimpleAction> m_actionShortcuts;
        std::shared_ptr<Gio::SimpleAction> m_actionAbout;
        std::shared_ptr<Gio::Menu> m_menuHelp;
        std::shared_ptr<Gio::Menu> m_menuHelpUpdate;
        std::shared_ptr<Gio::Menu> m_menuHelpLinks;
        std::shared_ptr<Gio::Menu> m_menuHelpActions;
        Gtk::MenuButton m_btnHelp;
        //==Separators==//
        Gtk::Separator m_sep1;
        Gtk::Separator m_sep2;
    };
}

#endif // HEADERBAR_H
