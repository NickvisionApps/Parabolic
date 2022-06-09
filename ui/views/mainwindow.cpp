#include "mainwindow.h"
#include <iostream>
#include <filesystem>
#include <regex>
#include "../../models/download.h"
#include "../controls/progressdialog.h"
#include "preferencesdialog.h"
#include "shortcutsdialog.h"

using namespace NickvisionTubeConverter::Models;
using namespace NickvisionTubeConverter::UI;
using namespace NickvisionTubeConverter::UI::Controls;
using namespace NickvisionTubeConverter::UI::Views;

MainWindow::MainWindow(Configuration& configuration) : Widget{"/org/nickvision/tubeconverter/ui/views/mainwindow.xml", "adw_winMain"}, m_configuration{configuration}, m_opened{false}
{
    //==Signals==//
    g_signal_connect(m_gobj, "show", G_CALLBACK((void (*)(GtkWidget*, gpointer*))[](GtkWidget* widget, gpointer* data) { reinterpret_cast<MainWindow*>(data)->onStartup(); }), this);
    //==App Actions==//
    //Select Save Folder
    m_gio_actSelectSaveFolder = g_simple_action_new("selectSaveFolder", nullptr);
    g_signal_connect(m_gio_actSelectSaveFolder, "activate", G_CALLBACK((void (*)(GSimpleAction*, GVariant*, gpointer*))[](GSimpleAction* action, GVariant* parameter, gpointer* data) { reinterpret_cast<MainWindow*>(data)->selectSaveFolder(); }), this);
    g_action_map_add_action(G_ACTION_MAP(m_gobj), G_ACTION(m_gio_actSelectSaveFolder));
    //Add To Queue
    m_gio_actAddToQueue = g_simple_action_new("addToQueue", nullptr);
    g_signal_connect(m_gio_actAddToQueue, "activate", G_CALLBACK((void (*)(GSimpleAction*, GVariant*, gpointer*))[](GSimpleAction* action, GVariant* parameter, gpointer* data) { reinterpret_cast<MainWindow*>(data)->addToQueue(); }), this);
    g_action_map_add_action(G_ACTION_MAP(m_gobj), G_ACTION(m_gio_actAddToQueue));
    //Remove From Queue
    m_gio_actRemoveFromQueue = g_simple_action_new("removeFromQueue", nullptr);
    g_signal_connect(m_gio_actRemoveFromQueue, "activate", G_CALLBACK((void (*)(GSimpleAction*, GVariant*, gpointer*))[](GSimpleAction* action, GVariant* parameter, gpointer* data) { reinterpret_cast<MainWindow*>(data)->removeFromQueue(); }), this);
    g_action_map_add_action(G_ACTION_MAP(m_gobj), G_ACTION(m_gio_actRemoveFromQueue));
    //Clear Queue
    m_gio_actClearQueue = g_simple_action_new("clearQueue", nullptr);
    g_signal_connect(m_gio_actClearQueue, "activate", G_CALLBACK((void (*)(GSimpleAction*, GVariant*, gpointer*))[](GSimpleAction* action, GVariant* parameter, gpointer* data) { reinterpret_cast<MainWindow*>(data)->clearQueue(); }), this);
    g_action_map_add_action(G_ACTION_MAP(m_gobj), G_ACTION(m_gio_actClearQueue));
    //Download Videos
    m_gio_actDownloadVideos = g_simple_action_new("downloadVideos", nullptr);
    g_signal_connect(m_gio_actDownloadVideos, "activate", G_CALLBACK((void (*)(GSimpleAction*, GVariant*, gpointer*))[](GSimpleAction* action, GVariant* parameter, gpointer* data) { reinterpret_cast<MainWindow*>(data)->downloadVideos(); }), this);
    g_action_map_add_action(G_ACTION_MAP(m_gobj), G_ACTION(m_gio_actDownloadVideos));
    //==Help Actions==//
    //Preferences
    m_gio_actPreferences = g_simple_action_new("preferences", nullptr);
    g_signal_connect(m_gio_actPreferences, "activate", G_CALLBACK((void (*)(GSimpleAction*, GVariant*, gpointer*))[](GSimpleAction* action, GVariant* parameter, gpointer* data) { reinterpret_cast<MainWindow*>(data)->preferences(); }), this);
    g_action_map_add_action(G_ACTION_MAP(m_gobj), G_ACTION(m_gio_actPreferences));
    //Keyboard Shortcuts
    m_gio_actKeyboardShortcuts = g_simple_action_new("keyboardShortcuts", nullptr);
    g_signal_connect(m_gio_actKeyboardShortcuts, "activate", G_CALLBACK((void (*)(GSimpleAction*, GVariant*, gpointer*))[](GSimpleAction* action, GVariant* parameter, gpointer* data) { reinterpret_cast<MainWindow*>(data)->keyboardShortcuts(); }), this);
    g_action_map_add_action(G_ACTION_MAP(m_gobj), G_ACTION(m_gio_actKeyboardShortcuts));
    //Changelog
    m_gio_actChangelog = g_simple_action_new("changelog", nullptr);
    g_signal_connect(m_gio_actChangelog, "activate", G_CALLBACK((void (*)(GSimpleAction*, GVariant*, gpointer*))[](GSimpleAction* action, GVariant* parameter, gpointer* data) { reinterpret_cast<MainWindow*>(data)->changelog(); }), this);
    g_action_map_add_action(G_ACTION_MAP(m_gobj), G_ACTION(m_gio_actChangelog));
    //About
    m_gio_actAbout = g_simple_action_new("about", nullptr);
    g_signal_connect(m_gio_actAbout, "activate", G_CALLBACK((void (*)(GSimpleAction*, GVariant*, gpointer*))[](GSimpleAction* action, GVariant* parameter, gpointer* data) { reinterpret_cast<MainWindow*>(data)->about(); }), this);
    g_action_map_add_action(G_ACTION_MAP(m_gobj), G_ACTION(m_gio_actAbout));
    //==Help Menu Button==//
    GtkBuilder* builderMenu{gtk_builder_new_from_resource("/org/nickvision/tubeconverter/ui/views/menuhelp.xml")};
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(gtk_builder_get_object(m_builder, "gtk_btnMenuHelp")), G_MENU_MODEL(gtk_builder_get_object(builderMenu, "gio_menuHelp")));
    g_object_unref(builderMenu);
    //==Cmb File Format==//
    g_signal_connect(gtk_builder_get_object(m_builder, "gtk_cmbFileFormat"), "changed", G_CALLBACK((void (*)(GtkComboBox*, gpointer*))[](GtkComboBox* comboBox, gpointer* data) { reinterpret_cast<MainWindow*>(data)->onCmbFileFormatSelectionChanged(); }), this);
    //==List Downloads==//
    g_signal_connect(gtk_builder_get_object(m_builder, "gtk_listDownloads"), "row-selected", G_CALLBACK((void (*)(GtkListBox*, GtkListBoxRow*, gpointer*))[](GtkListBox* listBox, GtkListBoxRow* row, gpointer* data) { reinterpret_cast<MainWindow*>(data)->onListDownloadsSelectionChanged(); }), this);
}

MainWindow::~MainWindow()
{
    m_configuration.save();
    gtk_window_destroy(GTK_WINDOW(m_gobj));
}

void MainWindow::showMaximized()
{
    gtk_widget_show(m_gobj);
    gtk_window_maximize(GTK_WINDOW(m_gobj));
}

void MainWindow::onStartup()
{
    if(!m_opened)
    {
        //==Set Action Shortcuts==//
        //Select Save Folder
        gtk_application_set_accels_for_action(gtk_window_get_application(GTK_WINDOW(m_gobj)), "win.selectSaveFolder", new const char*[2]{ "<Ctrl>o", nullptr });
        //Add To Queue
        gtk_application_set_accels_for_action(gtk_window_get_application(GTK_WINDOW(m_gobj)), "win.addToQueue", new const char*[2]{ "Insert", nullptr });
        //Remove From Queue
        gtk_application_set_accels_for_action(gtk_window_get_application(GTK_WINDOW(m_gobj)), "win.removeFromQueue", new const char*[2]{ "Delete", nullptr });
        //Clear Queue
        gtk_application_set_accels_for_action(gtk_window_get_application(GTK_WINDOW(m_gobj)), "win.clearQueue", new const char*[2]{ "<Ctrl>Delete", nullptr });
        //Download Videos
        gtk_application_set_accels_for_action(gtk_window_get_application(GTK_WINDOW(m_gobj)), "win.downloadVideos", new const char*[2]{ "<Ctrl>d", nullptr });
        //About
        gtk_application_set_accels_for_action(gtk_window_get_application(GTK_WINDOW(m_gobj)), "win.about", new const char*[2]{ "F1", nullptr });
        //==Load Configuration==//
        m_downloadManager.setMaxNumOfDownloads(m_configuration.getMaxNumberOfActiveDownloads());
        if(std::filesystem::exists(m_configuration.getPreviousSaveFolder()))
        {
            gtk_editable_set_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "gtk_txtSaveFolder")), m_configuration.getPreviousSaveFolder().c_str());
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(m_builder, "gtk_cmbFileFormat")), static_cast<int>(m_configuration.getPreviousFileFormat()));
        m_opened = true;
    }
}

void MainWindow::selectSaveFolder()
{
    GtkFileChooserNative* openFolderDialog{gtk_file_chooser_native_new("Select Save Folder", GTK_WINDOW(m_gobj), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "_Open", "_Cancel")};
    gtk_native_dialog_set_modal(GTK_NATIVE_DIALOG(openFolderDialog), true);
    g_signal_connect(openFolderDialog, "response", G_CALLBACK((void (*)(GtkNativeDialog*, gint, gpointer*))([](GtkNativeDialog* dialog, gint response_id, gpointer* data)
    {
        if(response_id == GTK_RESPONSE_ACCEPT)
        {
            MainWindow* mainWindow{reinterpret_cast<MainWindow*>(data)};
            GtkFileChooser* chooser{GTK_FILE_CHOOSER(dialog)};
            GFile* file{gtk_file_chooser_get_file(chooser)};
            std::string path{g_file_get_path(file)};
            g_object_unref(file);
            gtk_editable_set_text(GTK_EDITABLE(gtk_builder_get_object(GTK_BUILDER(mainWindow->m_builder), "gtk_txtSaveFolder")), path.c_str());
            mainWindow->m_configuration.setPreviousSaveFolder(path);
            mainWindow->m_configuration.save();
        }
        g_object_unref(dialog);
    })), this);
    gtk_native_dialog_show(GTK_NATIVE_DIALOG(openFolderDialog));
}

void MainWindow::addToQueue()
{
    std::string videoUrl{gtk_editable_get_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "gtk_txtVideoUrl")))};
    std::string saveFolder{gtk_editable_get_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "gtk_txtSaveFolder")))};
    MediaFileType fileFormat{static_cast<MediaFileType::Value>(gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(m_builder, "gtk_cmbFileFormat"))))};
    std::string newFilename{gtk_editable_get_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "gtk_txtNewFilename")))};
    if(videoUrl.empty())
    {
        sendToast("Error: Video url can't be empty.");
    }
    else if(videoUrl.find("https://www.youtube.com/watch?v=") == std::string::npos && videoUrl.find("http://www.youtube.com/watch?v=") == std::string::npos)
    {
        sendToast("Error: Video url must be a valid YouTube link.");
    }
    else if(saveFolder.empty())
    {
        sendToast("Error: Save folder can't be empty.");
    }
    else if(!std::filesystem::exists(saveFolder))
    {
        sendToast("Error: Save folder must be a valid folder.");
    }
    else if(newFilename.empty())
    {
        sendToast("Error: New filename can't be empty.");
    }
    else if(m_downloadManager.isQueueFull())
    {
        sendToast("Error: Download queue is full.");
    }
    else
    {
        std::shared_ptr<Download> download{std::make_shared<Download>(videoUrl, fileFormat, saveFolder, newFilename)};
        m_downloadManager.addToQueue(download);
        GtkWidget* row{adw_action_row_new()};
        adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), download->getPath().c_str());
        adw_action_row_set_subtitle(ADW_ACTION_ROW(row), std::regex_replace(download->getVideoUrl(), std::regex("\\&"), "&amp;").c_str());
        gtk_list_box_append(GTK_LIST_BOX(gtk_builder_get_object(m_builder, "gtk_listDownloads")), row);
        m_listDownloadsRows.push_back(row);
        gtk_editable_set_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "gtk_txtVideoUrl")), "");
        gtk_editable_set_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "gtk_txtNewFilename")), "");
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(m_builder, "gtk_btnClearQueue")), true);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(m_builder, "gtk_btnDownloadVideos")), true);
    }
}

void MainWindow::removeFromQueue()
{
    GtkListBoxRow* selectedRow{gtk_list_box_get_selected_row(GTK_LIST_BOX(gtk_builder_get_object(m_builder, "gtk_listDownloads")))};
    m_downloadManager.removeFromQueue(gtk_list_box_row_get_index(selectedRow));
    gtk_list_box_remove(GTK_LIST_BOX(gtk_builder_get_object(m_builder, "gtk_listDownloads")), GTK_WIDGET(selectedRow));
    gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(m_builder, "gtk_btnClearQueue")), m_downloadManager.getQueueCount() > 0);
    gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(m_builder, "gtk_btnDownloadVideos")), m_downloadManager.getQueueCount() > 0);
}

void MainWindow::clearQueue()
{
    GtkWidget* removeDialog{gtk_message_dialog_new(GTK_WINDOW(m_gobj), GtkDialogFlags(GTK_DIALOG_MODAL),
        GTK_MESSAGE_INFO, GTK_BUTTONS_YES_NO, "Clear Queue?")};
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(removeDialog), "Are you sure you want to remove all downloads in queue?\nThis action is irreversible.");
    g_signal_connect(removeDialog, "response", G_CALLBACK((void (*)(GtkDialog*, gint, gpointer*))([](GtkDialog* dialog, gint response_id, gpointer* data)
    {
        gtk_window_destroy(GTK_WINDOW(dialog));
        if(response_id == GTK_RESPONSE_YES)
        {
            MainWindow* mainWindow{reinterpret_cast<MainWindow*>(data)};
            mainWindow->m_downloadManager.clearQueue();
            gtk_list_box_unselect_all(GTK_LIST_BOX(gtk_builder_get_object(mainWindow->m_builder, "gtk_listDownloads")));
            for(GtkWidget* row : mainWindow->m_listDownloadsRows)
            {
                gtk_list_box_remove(GTK_LIST_BOX(gtk_builder_get_object(mainWindow->m_builder, "gtk_listDownloads")), row);
            }
            mainWindow->m_listDownloadsRows.clear();
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(mainWindow->m_builder, "gtk_btnClearQueue")), false);
            gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(mainWindow->m_builder, "gtk_btnDownloadVideos")), false);
        }
    })), this);
    gtk_widget_show(removeDialog);
}

void MainWindow::downloadVideos()
{
    ProgressDialog* progDialogDownloading{new ProgressDialog(m_gobj, "Downloading videos...", [&]() { m_downloadManager.downloadAll(); }, [&]()
    {
        m_downloadManager.clearQueue();
        gtk_list_box_unselect_all(GTK_LIST_BOX(gtk_builder_get_object(m_builder, "gtk_listDownloads")));
        for(GtkWidget* row : m_listDownloadsRows)
        {
            gtk_list_box_remove(GTK_LIST_BOX(gtk_builder_get_object(m_builder, "gtk_listDownloads")), row);
        }
        m_listDownloadsRows.clear();
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(m_builder, "gtk_btnClearQueue")), false);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(m_builder, "gtk_btnDownloadVideos")), false);
        GtkTextBuffer* logBuffer{gtk_text_buffer_new(nullptr)};
        gtk_text_buffer_set_text(logBuffer, m_downloadManager.getLog().c_str(), -1);
        gtk_text_view_set_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(m_builder, "gtk_txtLogs")), logBuffer);
        g_object_unref(logBuffer);
        sendToast("Downloaded " + std::to_string(m_downloadManager.getSuccessfulDownloads()) + " video(s) successfully. View logs for details.");
    })};
    progDialogDownloading->show();
}

void MainWindow::preferences()
{
    PreferencesDialog* preferencesDialog{new PreferencesDialog(m_gobj, m_configuration)};
    std::pair<PreferencesDialog*, MainWindow*>* pointers{new std::pair<PreferencesDialog*, MainWindow*>(preferencesDialog, this)};
    g_signal_connect(preferencesDialog->gobj(), "hide", G_CALLBACK((void (*)(GtkWidget*, gpointer*))([](GtkWidget* widget, gpointer* data)
    {
        std::pair<PreferencesDialog*, MainWindow*>* pointers{reinterpret_cast<std::pair<PreferencesDialog*, MainWindow*>*>(data)};
        delete pointers->first;
        if(pointers->second->m_configuration.getTheme() == Theme::System)
        {
           adw_style_manager_set_color_scheme(adw_style_manager_get_default(), ADW_COLOR_SCHEME_PREFER_LIGHT);
        }
        else if(pointers->second->m_configuration.getTheme() == Theme::Light)
        {
           adw_style_manager_set_color_scheme(adw_style_manager_get_default(), ADW_COLOR_SCHEME_FORCE_LIGHT);
        }
        else if(pointers->second->m_configuration.getTheme() == Theme::Dark)
        {
           adw_style_manager_set_color_scheme(adw_style_manager_get_default(), ADW_COLOR_SCHEME_FORCE_DARK);
        }
        if(pointers->second->m_configuration.getMaxNumberOfActiveDownloads() != pointers->second->m_downloadManager.getMaxNumOfDownloads())
        {
            pointers->second->m_downloadManager.setMaxNumOfDownloads(pointers->second->m_configuration.getMaxNumberOfActiveDownloads());
        }
        delete pointers;
    })), pointers);
    preferencesDialog->show();
}

void MainWindow::keyboardShortcuts()
{
    ShortcutsDialog* shortcutsDialog{new ShortcutsDialog(m_gobj)};
    g_signal_connect(shortcutsDialog->gobj(), "hide", G_CALLBACK((void (*)(GtkWidget*, gpointer*))([](GtkWidget* widget, gpointer* data)
    {
        ShortcutsDialog* dialog{reinterpret_cast<ShortcutsDialog*>(data)};
        delete dialog;
    })), shortcutsDialog);
    shortcutsDialog->show();
}

void MainWindow::changelog()
{
    GtkWidget* changelogDialog{gtk_message_dialog_new(GTK_WINDOW(m_gobj), GtkDialogFlags(GTK_DIALOG_MODAL),
        GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "What's New?")};
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(changelogDialog), "- UI fixes");
    g_signal_connect(changelogDialog, "response", G_CALLBACK(gtk_window_destroy), nullptr);
    gtk_widget_show(changelogDialog);
}

void MainWindow::about()
{
    gtk_show_about_dialog(GTK_WINDOW(m_gobj), "program-name", "Nickvision Tube Converter", "version", "2022.6.1", "comments", "An easy-to-use YouTube video downloader.",
                          "copyright", "(C) Nickvision 2021-2022", "license-type", GTK_LICENSE_GPL_3_0, "website", "https://github.com/nlogozzo/NickvisionTubeConverter", "website-label", "GitHub",
                          "authors", new const char*[2]{ "Nicholas Logozzo", nullptr }, "artists", new const char*[3]{ "Nicholas Logozzo", "daudix-UFO (Icons)", nullptr }, "logo-icon-name", "org.nickvision.tubeconverter", nullptr);
}

void MainWindow::sendToast(const std::string& message)
{
    AdwToast* toast{adw_toast_new(message.c_str())};
    adw_toast_overlay_add_toast(ADW_TOAST_OVERLAY(gtk_builder_get_object(m_builder, "adw_toastOverlay")), toast);
}

void MainWindow::onCmbFileFormatSelectionChanged()
{
    m_configuration.setPreviousFileFormat(static_cast<MediaFileType::Value>(gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(m_builder, "gtk_cmbFileFormat")))));
    m_configuration.save();
}

void MainWindow::onListDownloadsSelectionChanged()
{
    GtkListBoxRow* selectedRow{gtk_list_box_get_selected_row(GTK_LIST_BOX(gtk_builder_get_object(m_builder, "gtk_listDownloads")))};
    gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(m_builder, "gtk_btnRemoveFromQueue")), selectedRow != nullptr);
}