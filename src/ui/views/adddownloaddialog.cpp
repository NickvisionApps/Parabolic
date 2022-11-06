#include "adddownloaddialog.hpp"
#include "../controls/progressdialog.hpp"
#include "../../helpers/translation.hpp"

using namespace NickvisionTubeConverter::Controllers;
using namespace NickvisionTubeConverter::Models;
using namespace NickvisionTubeConverter::UI::Controls;
using namespace NickvisionTubeConverter::UI::Views;

AddDownloadDialog::AddDownloadDialog(GtkWindow* parent, AddDownloadDialogController& controller) : m_controller{ controller }, m_parent{ parent }, m_gobj{ adw_message_dialog_new(m_parent, _("Add Download"), nullptr) }
{
    //Dialog Settings
    gtk_window_set_hide_on_close(GTK_WINDOW(m_gobj), true);
    adw_message_dialog_add_responses(ADW_MESSAGE_DIALOG(m_gobj), "cancel", _("Cancel"), "ok", _("OK"), nullptr);
    adw_message_dialog_set_response_appearance(ADW_MESSAGE_DIALOG(m_gobj), "ok", ADW_RESPONSE_SUGGESTED);
    adw_message_dialog_set_default_response(ADW_MESSAGE_DIALOG(m_gobj), "cancel");
    adw_message_dialog_set_close_response(ADW_MESSAGE_DIALOG(m_gobj), "cancel");
    g_signal_connect(m_gobj, "response", G_CALLBACK((void (*)(AdwMessageDialog*, gchar*, gpointer))([](AdwMessageDialog*, gchar* response, gpointer data) { reinterpret_cast<AddDownloadDialog*>(data)->setResponse({ response }); })), this);
    //Preferences Group
    m_preferencesGroup = adw_preferences_group_new();
    //Video Url
    m_rowVideoUrl = adw_entry_row_new();
    gtk_widget_set_size_request(m_rowVideoUrl, 420, -1);
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(m_rowVideoUrl), _("Video Url"));
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(m_preferencesGroup), m_rowVideoUrl);
    //File Type
    m_rowFileType = adw_combo_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(m_rowFileType), _("File Type"));
    adw_combo_row_set_model(ADW_COMBO_ROW(m_rowFileType), G_LIST_MODEL(gtk_string_list_new(new const char*[7]{ "MP4", "WEBM", "MP3", "OPUS", "FLAC", "WAV", nullptr })));
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(m_preferencesGroup), m_rowFileType);
    g_signal_connect(m_rowFileType, "notify::selected-item", G_CALLBACK((void (*)(GObject*, GParamSpec*, gpointer))[](GObject*, GParamSpec*, gpointer data) { reinterpret_cast<AddDownloadDialog*>(data)->onFileTypeChanged(); }), this);
    //Quality
    m_rowQuality = adw_combo_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(m_rowQuality), _("Quality"));
    adw_combo_row_set_model(ADW_COMBO_ROW(m_rowQuality), G_LIST_MODEL(gtk_string_list_new(new const char*[4]{ _("Best"), _("Good"), _("Worst"), nullptr })));
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(m_preferencesGroup), m_rowQuality);
    //Subtitles
    m_rowSubtitles = adw_combo_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(m_rowSubtitles), _("Subtitles"));
    adw_combo_row_set_model(ADW_COMBO_ROW(m_rowSubtitles), G_LIST_MODEL(gtk_string_list_new(new const char*[4]{ _("None"), "VTT", "SRT", nullptr })));
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(m_preferencesGroup), m_rowSubtitles);
    //Save Folder
    m_btnSelectSaveFolder = gtk_button_new();
    gtk_widget_set_valign(m_btnSelectSaveFolder, GTK_ALIGN_CENTER);
    gtk_style_context_add_class(gtk_widget_get_style_context(m_btnSelectSaveFolder), "flat");
    gtk_button_set_icon_name(GTK_BUTTON(m_btnSelectSaveFolder), "folder-open-symbolic");
    gtk_widget_set_tooltip_text(m_btnSelectSaveFolder, _("Select Save Folder"));
    g_signal_connect(m_btnSelectSaveFolder, "clicked", G_CALLBACK((void (*)(GtkButton*, gpointer))[](GtkButton*, gpointer data) { reinterpret_cast<AddDownloadDialog*>(data)->onSelectSaveFolder(); }), this);
    m_rowSaveFolder = adw_entry_row_new();
    gtk_widget_set_size_request(m_rowSaveFolder, 420, -1);
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(m_rowSaveFolder), _("Save Folder"));
    adw_entry_row_add_suffix(ADW_ENTRY_ROW(m_rowSaveFolder), m_btnSelectSaveFolder);
    gtk_editable_set_editable(GTK_EDITABLE(m_rowSaveFolder), false);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(m_preferencesGroup), m_rowSaveFolder);
    //New Filename
    m_rowNewFilename = adw_entry_row_new();
    gtk_widget_set_size_request(m_rowNewFilename, 420, -1);
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(m_rowNewFilename), _("New Filename"));
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(m_preferencesGroup), m_rowNewFilename);
    //Layout
    adw_message_dialog_set_extra_child(ADW_MESSAGE_DIALOG(m_gobj), m_preferencesGroup);
    //Load Config
    adw_combo_row_set_selected(ADW_COMBO_ROW(m_rowFileType), m_controller.getPreviousFileTypeAsInt());
    gtk_editable_set_text(GTK_EDITABLE(m_rowSaveFolder), m_controller.getPreviousSaveFolder().c_str());
}

GtkWidget* AddDownloadDialog::gobj()
{
    return m_gobj;
}

bool AddDownloadDialog::run()
{
    gtk_widget_show(m_gobj);
    while(gtk_widget_is_visible(m_gobj))
    {
        g_main_context_iteration(g_main_context_default(), false);
    }
    if(m_controller.getResponse() == "ok")
    {
        gtk_widget_hide(m_gobj);
        DownloadCheckStatus downloadCheckStatus{ DownloadCheckStatus::InvalidVideoUrl };
        ProgressDialog progressDialog{ GTK_WINDOW(m_parent), _("Preparing download..."), [&]() { downloadCheckStatus = m_controller.setDownload(gtk_editable_get_text(GTK_EDITABLE(m_rowVideoUrl)), adw_combo_row_get_selected(ADW_COMBO_ROW(m_rowFileType)), gtk_editable_get_text(GTK_EDITABLE(m_rowSaveFolder)), gtk_editable_get_text(GTK_EDITABLE(m_rowNewFilename)), adw_combo_row_get_selected(ADW_COMBO_ROW(m_rowQuality)), adw_combo_row_get_selected(ADW_COMBO_ROW(m_rowSubtitles))); } };
        progressDialog.run();
        //Invalid Download
        if(downloadCheckStatus != DownloadCheckStatus::Valid)
        {
            //Reset UI
            gtk_style_context_remove_class(gtk_widget_get_style_context(m_rowVideoUrl), "error");
            adw_preferences_row_set_title(ADW_PREFERENCES_ROW(m_rowVideoUrl), _("Video Url"));
            gtk_style_context_remove_class(gtk_widget_get_style_context(m_rowSaveFolder), "error");
            adw_preferences_row_set_title(ADW_PREFERENCES_ROW(m_rowSaveFolder), _("Save Folder"));
            //Mark Error
            if(downloadCheckStatus == DownloadCheckStatus::EmptyVideoUrl)
            {
                gtk_style_context_add_class(gtk_widget_get_style_context(m_rowVideoUrl), "error");
                adw_preferences_row_set_title(ADW_PREFERENCES_ROW(m_rowVideoUrl), _("Video Url (Empty)"));
            }
            else if(downloadCheckStatus == DownloadCheckStatus::InvalidVideoUrl)
            {
                gtk_style_context_add_class(gtk_widget_get_style_context(m_rowVideoUrl), "error");
                adw_preferences_row_set_title(ADW_PREFERENCES_ROW(m_rowVideoUrl), _("Video Url (Invalid)"));
            }
            else if(downloadCheckStatus == DownloadCheckStatus::EmptySaveFolder)
            {
                gtk_style_context_add_class(gtk_widget_get_style_context(m_rowSaveFolder), "error");
                adw_preferences_row_set_title(ADW_PREFERENCES_ROW(m_rowSaveFolder), _("Save Folder (Empty)"));
            }
            else if(downloadCheckStatus == DownloadCheckStatus::InvalidSaveFolder)
            {
                gtk_style_context_add_class(gtk_widget_get_style_context(m_rowSaveFolder), "error");
                adw_preferences_row_set_title(ADW_PREFERENCES_ROW(m_rowSaveFolder), _("Save Folder (Invalid)"));
            }
            //Prompt User to Fix
            return run();
        }
    }
    gtk_window_destroy(GTK_WINDOW(m_gobj));
    return m_controller.getResponse() == "ok";
}

void AddDownloadDialog::setResponse(const std::string& response)
{
    m_controller.setResponse(response);
}

void AddDownloadDialog::onFileTypeChanged()
{
    MediaFileType fileType{ static_cast<MediaFileType::Value>(adw_combo_row_get_selected(ADW_COMBO_ROW(m_rowFileType))) };
    gtk_widget_set_sensitive(m_rowSubtitles, fileType.isVideo());
}

void AddDownloadDialog::onSelectSaveFolder()
{
    GtkFileChooserNative* openFolderDialog{ gtk_file_chooser_native_new(_("Select Save Folder"), GTK_WINDOW(m_gobj), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, _("_Open"), _("_Cancel")) };
    gtk_native_dialog_set_modal(GTK_NATIVE_DIALOG(openFolderDialog), true);
    g_signal_connect(openFolderDialog, "response", G_CALLBACK((void (*)(GtkNativeDialog*, gint, gpointer))([](GtkNativeDialog* dialog, gint response_id, gpointer data)
    {
        if(response_id == GTK_RESPONSE_ACCEPT)
        {
            AddDownloadDialog* addDownloadDialog{ reinterpret_cast<AddDownloadDialog*>(data) };
            GFile* file{ gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog)) };
            gtk_editable_set_text(GTK_EDITABLE(addDownloadDialog->m_rowSaveFolder), g_file_get_path(file));
            g_object_unref(file);
        }
        g_object_unref(dialog);
    })), this);
    gtk_native_dialog_show(GTK_NATIVE_DIALOG(openFolderDialog));
}
