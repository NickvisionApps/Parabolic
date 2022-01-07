#include "mainwindow.h"
#include <stdexcept>
#include <filesystem>
#include "../helpers/filetypehelpers.h"
#include "../models/configuration.h"
#include "../models/download.h"
#include "../controls/progressdialog.h"
#include "settingsdialog.h"

namespace NickvisionTubeConverter::Views
{
    using namespace NickvisionTubeConverter::Helpers;
    using namespace NickvisionTubeConverter::Models;
    using namespace NickvisionTubeConverter::Controls;

    MainWindow::MainWindow() : m_opened(false), m_updater("https://raw.githubusercontent.com/nlogozzo/NickvisionTubeConverter/main/UpdateConfig.json", { "2022.1.2" })
    {
        //==Settings==//
        set_default_size(800, 600);
        set_title("Nickvision Tube Converter");
        set_titlebar(m_headerBar);
        signal_show().connect(sigc::mem_fun(*this, &MainWindow::onShow));
        //==HeaderBar==//
        m_headerBar.getBtnSelectSaveFolder().signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::selectSaveFolder));
        m_headerBar.getBtnDownloadVideos().signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::downloadVideos));
        m_headerBar.getBtnAddDownloadToQueue().signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::addDownloadToQueue));
        m_headerBar.getBtnRemoveSelectedDownloadFromQueue().signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::removeSelectedDownloadFromQueue));
        m_headerBar.getBtnConfirmRemoveAllQueuedDownloads().signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::removeAllQueuedDownloads));
        m_headerBar.getBtnSettings().signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::settings));
        m_headerBar.getActionCheckForUpdates()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::checkForUpdates));
        m_headerBar.getActionGitHubRepo()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::gitHubRepo));
        m_headerBar.getActionReportABug()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::reportABug));
        m_headerBar.getActionChangelog()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::changelog));
        m_headerBar.getActionAbout()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::about));
        m_headerBar.getBtnDownloadVideos().set_sensitive(false);
        m_headerBar.getBtnRemoveSelectedDownloadFromQueue().set_sensitive(false);
        m_headerBar.getBtnRemoveAllQueuedDownloads().set_sensitive(false);
        //==Grid==//
        m_gridProperties.set_margin(6);
        m_gridProperties.insert_column(0);
        m_gridProperties.insert_column(1);
        m_gridProperties.insert_row(0);
        m_gridProperties.insert_row(1);
        m_gridProperties.insert_row(2);
        m_gridProperties.insert_row(3);
        m_gridProperties.set_column_spacing(6);
        m_gridProperties.set_row_spacing(6);
        //Video URL
        m_lblVideoUrl.set_text("Video Url");
        m_lblVideoUrl.set_halign(Gtk::Align::START);
        m_txtVideoUrl.set_placeholder_text("Enter video url here");
        m_txtVideoUrl.set_size_request(340, -1);
        m_gridProperties.attach(m_lblVideoUrl, 0, 0);
        m_gridProperties.attach(m_txtVideoUrl, 0, 1);
        //Save Folder
        m_lblSaveFolder.set_text("Save Folder");
        m_lblSaveFolder.set_halign(Gtk::Align::START);
        m_txtSaveFolder.set_placeholder_text("Select save folder");
        m_txtSaveFolder.set_size_request(340, -1);
        m_txtSaveFolder.set_editable(false);
        m_gridProperties.attach(m_lblSaveFolder, 1, 0);
        m_gridProperties.attach(m_txtSaveFolder, 1, 1);
        //File Format
        m_lblFileFormat.set_text("File Format");
        m_lblFileFormat.set_halign(Gtk::Align::START);
        m_cmbFileFormat.append("MP4 - Video");
        m_cmbFileFormat.append("WEBM - Video");
        m_cmbFileFormat.append("--------------");
        m_cmbFileFormat.append("MP3 - Audio");
        m_cmbFileFormat.append("WAV - Audio");
        m_cmbFileFormat.append("FLAC - Audio");
        m_cmbFileFormat.set_active(0);
        m_cmbFileFormat.signal_changed().connect([&]()
        {
            if(m_cmbFileFormat.get_active_row_number() == 2)
            {
                m_cmbFileFormat.set_active(1);
            }
        });
        m_cmbFileFormat.set_size_request(340, -1);
        m_gridProperties.attach(m_lblFileFormat, 0, 2);
        m_gridProperties.attach(m_cmbFileFormat, 0, 3);
        //New Filename
        m_lblNewFilename.set_text("New Filename");
        m_lblNewFilename.set_halign(Gtk::Align::START);
        m_txtNewFilename.set_placeholder_text("Enter new filename here");
        m_txtNewFilename.set_size_request(340, -1);
        m_gridProperties.attach(m_lblNewFilename, 1, 2);
        m_gridProperties.attach(m_txtNewFilename, 1, 3);
        //==Data Downloads==//
        m_dataDownloadsModel = Gtk::ListStore::create(m_dataDownloadsColumns);
        m_dataDownloads.append_column("ID", m_dataDownloadsColumns.getColID());
        m_dataDownloads.append_column("Path", m_dataDownloadsColumns.getColPath());
        m_dataDownloads.append_column("File Type", m_dataDownloadsColumns.getColFileType());
        m_dataDownloads.append_column("Url", m_dataDownloadsColumns.getColUrl());
        m_dataDownloads.set_model(m_dataDownloadsModel);
        m_dataDownloads.get_selection()->set_mode(Gtk::SelectionMode::SINGLE);
        m_dataDownloads.get_selection()->signal_changed().connect([&]() { m_headerBar.getBtnRemoveSelectedDownloadFromQueue().set_sensitive(m_dataDownloads.get_selection()->get_selected_rows().size() == 1); });
        //ScrolledWindow
        m_scrollDataDownloads.set_child(m_dataDownloads);
        m_scrollDataDownloads.set_margin(6);
        m_scrollDataDownloads.set_expand(true);
        //==Layout==//
        m_mainBox.set_orientation(Gtk::Orientation::VERTICAL);
        m_mainBox.append(m_infoBar);
        m_mainBox.append(m_gridProperties);
        m_mainBox.append(m_scrollDataDownloads);
        set_child(m_mainBox);
        maximize();
    }

    MainWindow::~MainWindow()
    {
        //==Save Config==//
        Configuration configuration;
        configuration.setPreviousSaveFolder(m_txtSaveFolder.get_text());
        configuration.setPreviousFileFormat(m_cmbFileFormat.get_active_row_number());
        configuration.save();
    }

    void MainWindow::onShow()
    {
        if(!m_opened)
        {
            m_opened = true;
            //==Load Config==//
            Configuration configuration;
            m_downloadManager.setMaxNumOfDownloads(configuration.getMaxNumberOfActiveDownloads());
            if(std::filesystem::exists(configuration.getPreviousSaveFolder()))
            {
                m_txtSaveFolder.set_text(configuration.getPreviousSaveFolder());
            }
            m_cmbFileFormat.set_active(configuration.getPreviousFileFormat());
        }
    }

    void MainWindow::selectSaveFolder()
    {
        Gtk::FileChooserDialog* folderDialog = new Gtk::FileChooserDialog(*this, "Select Save Folder", Gtk::FileChooserDialog::Action::SELECT_FOLDER, true);
        folderDialog->set_modal(true);
        folderDialog->add_button("_Select", Gtk::ResponseType::OK);
        folderDialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
        folderDialog->signal_response().connect(sigc::bind([&](int response, Gtk::FileChooserDialog* dialog)
        {
            if(response == Gtk::ResponseType::OK)
            {
                m_txtSaveFolder.set_text(dialog->get_file()->get_path());
            }
            delete dialog;
        }, folderDialog));
        folderDialog->show();
    }

    void MainWindow::downloadVideos()
    {
        size_t* numOfDownloads = new size_t(m_downloadManager.getQueueCount());
        int* success = new int(0);
        ProgressDialog* downloadingDialog = new ProgressDialog(*this, "Downloading videos (this may take some time)...", [&]()
        {
            *success = m_downloadManager.downloadAll();
        });
        downloadingDialog->signal_hide().connect(sigc::bind([&](ProgressDialog* dialog, size_t* numOfDownloads, int* success)
        {
            delete dialog;
            m_infoBar.showMessage("Downloads Complete", "Downloaded " + std::to_string(*success) + " out of " + std::to_string(*numOfDownloads) + " videos successfully.");
            delete numOfDownloads;
            delete success;
            removeAllQueuedDownloads();
        }, downloadingDialog, numOfDownloads, success));
        downloadingDialog->show();
    }

    void MainWindow::addDownloadToQueue()
    {
        if(m_txtVideoUrl.get_text().empty())
        {
            m_infoBar.showMessage("Error", "Video url can not be empty.");
        }
        else if((m_txtVideoUrl.get_text().rfind("https://www.youtube.com/watch?v=", 0) == std::string::npos) && (m_txtVideoUrl.get_text().rfind("http://www.youtube.com/watch?v=", 0) == std::string::npos))
        {
            m_infoBar.showMessage("Error", "Video url must be a valid YouTube link.");
        }
        else if(m_txtSaveFolder.get_text().empty())
        {
            m_infoBar.showMessage("Error", "Please select a save folder.");
        }
        else if(!std::filesystem::exists(std::string(m_txtSaveFolder.get_text())))
        {
            m_infoBar.showMessage("Error", "Please select an existing save folder.");
        }
        else if(m_txtNewFilename.get_text().empty())
        {
            m_infoBar.showMessage("Error", "New filename can not be empty.");
        }
        else if(m_downloadManager.isQueueFull())
        {
            m_infoBar.showMessage("Error", "Download queue is full. Please download the videos in queue or change the max number of downloads allowed in settings.");
        }
        else
        {
            Download download(m_txtVideoUrl.get_text(), FileTypeHelpers::parse(m_cmbFileFormat.get_active_text()), m_txtSaveFolder.get_text(), m_txtNewFilename.get_text());
            m_downloadManager.addToQueue(download);
            Gtk::TreeRow newRow = *(m_dataDownloadsModel->append());
            newRow[m_dataDownloadsColumns.getColID()] = m_downloadManager.getQueueCount();
            newRow[m_dataDownloadsColumns.getColPath()] = download.getPath();
            newRow[m_dataDownloadsColumns.getColFileType()] = FileTypeHelpers::toString(download.getFileType());
            newRow[m_dataDownloadsColumns.getColUrl()] = download.getVideoUrl();
            m_dataDownloads.columns_autosize();
            m_txtVideoUrl.set_text("");
            m_txtNewFilename.set_text("");
            m_headerBar.getBtnDownloadVideos().set_sensitive(true);
            m_headerBar.getBtnRemoveAllQueuedDownloads().set_sensitive(true);
        }
    }

    void MainWindow::removeSelectedDownloadFromQueue()
    {
        Gtk::TreeModel::Path selectedRow = m_dataDownloads.get_selection()->get_selected_rows()[0];
        m_downloadManager.removeFromQueue(selectedRow[0]);
        m_dataDownloadsModel->erase(m_dataDownloads.get_selection()->get_selected());
        m_dataDownloads.columns_autosize();
        if(m_downloadManager.getQueueCount() == 0)
        {
            m_headerBar.getBtnDownloadVideos().set_sensitive(false);
            m_headerBar.getBtnRemoveAllQueuedDownloads().set_sensitive(false);
        }
    }

    void MainWindow::removeAllQueuedDownloads()
    {
        m_headerBar.getPopRemoveAllQueuedDownloads().popdown();
        m_downloadManager.removeAllFromQueue();
        m_dataDownloadsModel->clear();
        m_headerBar.getBtnDownloadVideos().set_sensitive(false);
        m_headerBar.getBtnRemoveAllQueuedDownloads().set_sensitive(false);
        m_dataDownloads.columns_autosize();
    }

    void MainWindow::settings()
    {
        SettingsDialog* settingsDialog = new SettingsDialog(*this);
        settingsDialog->signal_hide().connect(sigc::bind([&](SettingsDialog* dialog)
        {
            delete dialog;
            Configuration configuration;
            m_downloadManager.setMaxNumOfDownloads(configuration.getMaxNumberOfActiveDownloads());
        }, settingsDialog));
        settingsDialog->show();
    }

    void MainWindow::checkForUpdates(const Glib::VariantBase& args)
    {
        ProgressDialog* checkingDialog = new ProgressDialog(*this, "Checking for updates...", [&]() { m_updater.checkForUpdates(); });
        checkingDialog->signal_hide().connect(sigc::bind([&](ProgressDialog* dialog)
        {
            delete dialog;
            if(m_updater.updateAvailable())
            {
                Gtk::MessageDialog* updateDialog = new Gtk::MessageDialog(*this, "Update Available", false, Gtk::MessageType::INFO, Gtk::ButtonsType::YES_NO, true);
                updateDialog->set_secondary_text("\n===V" + m_updater.getLatestVersion()->toString() + " Changelog===\n" + m_updater.getChangelog() + "\n\nNickvision Tube Converter can automatically download the latest executable to your Downloads directory. Would you like to continue?");
                updateDialog->signal_response().connect(sigc::bind([&](int response, Gtk::MessageDialog* dialog)
                {
                    delete dialog;
                    if(response == Gtk::ResponseType::YES)
                    {
                        bool* success = new bool(false);
                        ProgressDialog* downloadingDialog = new ProgressDialog(*this, "Downloading the update...", [&]() { *success = m_updater.update(); });
                        downloadingDialog->signal_hide().connect(sigc::bind([&](ProgressDialog* dialog, bool* success)
                        {
                            delete dialog;
                            if(*success)
                            {
                                m_infoBar.showMessage("Download Successful", "We recommend moving the new version out of your Downloads directory and running it from elsewhere to allow future updates to download smoothly.");
                            }
                            else
                            {
                                m_infoBar.showMessage("Error", "Unable to download the executable. Please try again. If the issue continues, file a bug report.");
                            }
                            delete success;
                        }, downloadingDialog, success));
                        downloadingDialog->show();
                    }
                }, updateDialog));
                updateDialog->show();
            }
            else
            {
                m_infoBar.showMessage("No Update Available", "There is no update at this time. Please check again later.");
            }
        }, checkingDialog));
        checkingDialog->show();
    }

    void MainWindow::gitHubRepo(const Glib::VariantBase& args)
    {
        Gio::AppInfo::launch_default_for_uri("https://github.com/nlogozzo/NickvisionTubeConverter");
    }

    void MainWindow::reportABug(const Glib::VariantBase& args)
    {
        Gio::AppInfo::launch_default_for_uri("https://github.com/nlogozzo/NickvisionTubeConverter/issues/new");
    }

    void MainWindow::changelog(const Glib::VariantBase& args)
    {
        Gtk::MessageDialog* changelogDialog = new Gtk::MessageDialog(*this, "What's New?", false, Gtk::MessageType::INFO, Gtk::ButtonsType::OK, true);
        changelogDialog->set_secondary_text("\n- UX improvements");
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
        aboutDialog->set_version("2022.1.2");
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
