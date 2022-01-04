#include "mainwindow.h"
#include "../models/configuration.h"
#include "../models/update/updater.h"
#include "settingsdialog.h"

namespace NickvisionTubeConverter::Views
{
    using namespace NickvisionTubeConverter::Models;
    using namespace NickvisionTubeConverter::Models::Update;

    MainWindow::MainWindow()
    {
        //==Settings==//
        set_default_size(800, 600);
        set_title("Nickvision Tube Converter");
        set_titlebar(m_headerBar);
        //==HeaderBar==//
        m_headerBar.getBtnSelectSaveFolder().signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::selectSaveFolder));
        m_headerBar.getBtnDownloadVideo().signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::downloadVideo));
        m_headerBar.getBtnClearCompletedDownloads().signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::clearCompletedDownloads));
        m_headerBar.getBtnSettings().signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::settings));
        m_headerBar.getActionCheckForUpdates()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::checkForUpdates));
        m_headerBar.getActionGitHubRepo()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::gitHubRepo));
        m_headerBar.getActionReportABug()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::reportABug));
        m_headerBar.getActionChangelog()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::changelog));
        m_headerBar.getActionAbout()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::about));
        //==Name Field==//
        m_lblName.set_label("Name");
        m_lblName.set_halign(Gtk::Align::START);
        m_lblName.set_margin_start(6);
        m_lblName.set_margin_top(6);
        m_txtName.set_margin(6);
        m_txtName.set_placeholder_text("Enter name here");
        //==Layout==//
        m_mainBox.set_orientation(Gtk::Orientation::VERTICAL);
        m_mainBox.append(m_infoBar);
        m_mainBox.append(m_lblName);
        m_mainBox.append(m_txtName);
        set_child(m_mainBox);
        maximize();
        //==Load Config==//
        Configuration configuration;
        if(configuration.isFirstTimeOpen())
        {
            configuration.setIsFirstTimeOpen(false);
        }
        configuration.save();
    }

    MainWindow::~MainWindow()
    {
        //==Save Config==//
        Configuration configuration;
        configuration.save();
    }

    void MainWindow::selectSaveFolder()
    {
        Gtk::FileChooserDialog* folderDialog = new Gtk::FileChooserDialog(*this, "Select Folder", Gtk::FileChooserDialog::Action::SELECT_FOLDER, true);
        folderDialog->set_modal(true);
        folderDialog->add_button("_Select", Gtk::ResponseType::OK);
        folderDialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
        folderDialog->signal_response().connect(sigc::bind([&](int response, Gtk::FileChooserDialog* dialog)
        {
            if(response == Gtk::ResponseType::OK)
            {
                set_title("Nickvision Tube Converter (" + dialog->get_file()->get_path() + ")");
            }
            delete dialog;
        }, folderDialog));
        folderDialog->show();
    }

    void MainWindow::downloadVideo()
    {

    }

    void MainWindow::clearCompletedDownloads()
    {

    }

    void MainWindow::settings()
    {
        SettingsDialog* settingsDialog = new SettingsDialog(*this);
        settingsDialog->signal_hide().connect(sigc::bind([](SettingsDialog* dialog)
        {
            delete dialog;
        }, settingsDialog));
        settingsDialog->show();
    }

    void MainWindow::checkForUpdates(const Glib::VariantBase& args)
    {
        Updater updater("https://raw.githubusercontent.com/nlogozzo/NickvisionTubeConverter/main/UpdateConfig.json", { "2022.1.0" });
        m_infoBar.showMessage("Please Wait", "Checking for updates...", false);
        updater.checkForUpdates();
        m_infoBar.hide();
        if(updater.updateAvailable())
        {
            Gtk::MessageDialog* updateDialog = new Gtk::MessageDialog(*this, "Update Available", false, Gtk::MessageType::INFO, Gtk::ButtonsType::OK, true);
            updateDialog->set_secondary_text("\n===V" + updater.getLatestVersion()->toString() + " Changelog===\n" + updater.getChangelog() + "\n\nPlease visit the GitHub repo or update through your package manager to get the latest version.");
            updateDialog->signal_response().connect(sigc::bind([](int response, Gtk::MessageDialog* dialog)
            {
               delete dialog;
            }, updateDialog));
            updateDialog->show();
        }
        else
        {
            m_infoBar.showMessage("No Update Available", "There is no update at this time. Please check again later.");
        }
    }

    void MainWindow::gitHubRepo(const Glib::VariantBase& args)
    {
        system("xdg-open https://github.com/nlogozzo/NickvisionTubeConverter");
    }

    void MainWindow::reportABug(const Glib::VariantBase& args)
    {
        system("xdg-open https://github.com/nlogozzo/NickvisionTubeConverter/issues/new");
    }

    void MainWindow::changelog(const Glib::VariantBase& args)
    {
        Gtk::MessageDialog* changelogDialog = new Gtk::MessageDialog(*this, "What's New?", false, Gtk::MessageType::INFO, Gtk::ButtonsType::OK, true);
        changelogDialog->set_secondary_text("\n- Rewrote Tube Converter in C++ using gtkmm\n- Removed support for Windows OS. Only Linux is now supported");
        changelogDialog->signal_response().connect(sigc::bind([](int response, Gtk::MessageDialog* dialog)
        {
           delete dialog;
        }, changelogDialog));
        changelogDialog->show();
    }

    void MainWindow::about(const Glib::VariantBase& args)
    {
        Gtk::AboutDialog* aboutDialog = new Gtk::AboutDialog();
        aboutDialog->set_transient_for(*this);
        aboutDialog->set_modal(true);
        aboutDialog->set_hide_on_close(true);
        aboutDialog->set_program_name("Nickvision Tube Converter");
        aboutDialog->set_version("2022.1.0-alpha1");
        aboutDialog->set_comments("An easy to use YouTube video downloader.");
        aboutDialog->set_copyright("(C) Nickvision 2021-2022");
        aboutDialog->set_license_type(Gtk::License::GPL_3_0);
        aboutDialog->set_website("https://github.com/nlogozzo");
        aboutDialog->set_website_label("GitHub");
        aboutDialog->set_authors({ "Nicholas Logozzo" });
        aboutDialog->signal_hide().connect(sigc::bind([](Gtk::AboutDialog* dialog)
        {
           delete dialog;
        }, aboutDialog));
        aboutDialog->show();
    }
}
