#include "views/adddownloaddialog.h"
#include <format>
#include <libnick/helpers/codehelpers.h>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>

using namespace Nickvision::Events;
using namespace Nickvision::Helpers;
using namespace Nickvision::Keyring;
using namespace Nickvision::TubeConverter::Shared::Controllers;
using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::GNOME::Views
{
    static void setComboRowModel(AdwComboRow* row, const std::vector<std::string>& strs)
    {
        GtkStringList* list{ gtk_string_list_new(nullptr) };
        for(const std::string& str : strs)
        {
            gtk_string_list_append(list, str.c_str());
        }
        adw_combo_row_set_model(row, G_LIST_MODEL(list));
        adw_combo_row_set_selected(row, 0);
    }

    AddDownloadDialog::AddDownloadDialog(const std::shared_ptr<AddDownloadDialogController>& controller, GtkWindow* parent)
        : DialogBase{ parent, "add_download_dialog" },
        m_controller{ controller }
    {
        //Load Validate Page
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_builder, "validateUrlButton")), false);
        adw_view_stack_set_visible_child_name(ADW_VIEW_STACK(gtk_builder_get_object(m_builder, "viewStack")), "validate");
        gdk_clipboard_read_text_async(gdk_display_get_clipboard(gdk_display_get_default()), nullptr, GAsyncReadyCallback(+[](GObject* self, GAsyncResult* res, gpointer data)
        {
            std::string clipboardText{ gdk_clipboard_read_text_finish(GDK_CLIPBOARD(self), res, nullptr) };
            if(StringHelpers::isValidUrl(clipboardText))
            {
                gtk_editable_set_text(GTK_EDITABLE(gtk_builder_get_object(GTK_BUILDER(data), "urlRow")), clipboardText.c_str());
                gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(GTK_BUILDER(data), "validateUrlButton")), true);
            }
        }), m_builder);
        std::vector<std::string> credentialNames{ m_controller->getKeyringCredentialNames() };
        credentialNames.insert(credentialNames.begin(), _("Use manual credential"));
        setComboRowModel(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "credentialRow")), credentialNames);
        //Signals
        g_signal_connect(gtk_builder_get_object(m_builder, "urlRow"), "changed", G_CALLBACK(+[](GtkEditable*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->onTxtUrlChanged(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "credentialRow"), "notify::selected-item", G_CALLBACK(+[](GObject*, GParamSpec* pspec, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->onCmbCredentialChanged(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "validateUrlButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->validateUrl(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "backButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->back(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "fileTypeSingleRow"), "notify::selected-item", G_CALLBACK(+[](GObject*, GParamSpec* pspec, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->onFileTypeSingleChanged(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "advancedOptionsSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->advancedOptionsSingle(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "selectSaveFolderSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->selectSaveFolderSingle(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "revertFilenameSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->revertFilenameSingle(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "revertStartTimeSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->revertStartTimeSingle(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "revertEndTimeSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->revertEndTimeSingle(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "downloadSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->downloadSingle(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "selectSaveFolderPlaylistButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->selectSaveFolderPlaylist(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "itemsPlaylistButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->itemsPlaylist(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "numberTitlesPlaylistRow"), "notify::active", G_CALLBACK(+[](GObject*, GParamSpec* pspec, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->onNumberTitlesPlaylistChanged(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "downloadPlaylistButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->downloadPlaylist(); }), this);
        m_controller->urlValidated() += [&](const EventArgs& args){ g_main_context_invoke(g_main_context_default(), G_SOURCE_FUNC(+[](gpointer data) -> bool { reinterpret_cast<AddDownloadDialog*>(data)->onUrlValidated(); return false; }), this); };
    }

    void AddDownloadDialog::onTxtUrlChanged()
    {
        std::string url{ gtk_editable_get_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "urlRow"))) };
        gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_builder, "validateUrlButton")), StringHelpers::isValidUrl(url));
    }

    void AddDownloadDialog::onCmbCredentialChanged()
    {
        bool visible{ adw_combo_row_get_selected(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "credentialRow"))) == 0 };
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(m_builder, "usernameRow")), visible);
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(m_builder, "passwordRow")), visible);
    }

    void AddDownloadDialog::validateUrl()
    {
        adw_dialog_set_can_close(m_dialog, false);
        adw_view_stack_set_visible_child_name(ADW_VIEW_STACK(gtk_builder_get_object(m_builder, "viewStack")), "spinner");
        std::optional<Credential> credential{ std::nullopt };
        if(adw_expander_row_get_enable_expansion(ADW_EXPANDER_ROW(gtk_builder_get_object(m_builder, "authenticateRow"))) && adw_combo_row_get_selected(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "credentialRow"))) == 0)
        {
            credential = Credential{ "", "", gtk_editable_get_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "usernameRow"))), gtk_editable_get_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "passwordRow"))) };
        }
        if(adw_combo_row_get_selected(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "credentialRow"))) == 0)
        {
            m_controller->validateUrl(gtk_editable_get_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "urlRow"))), credential);
        }
        else
        {
            m_controller->validateUrl(gtk_editable_get_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "urlRow"))), adw_combo_row_get_selected(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "credentialRow"))) - 1);
        }
    }

    void AddDownloadDialog::onUrlValidated()
    {
        if(!m_controller->isUrlValid())
        {
            AdwAlertDialog* dialog{ ADW_ALERT_DIALOG(adw_alert_dialog_new(_("Error"), _("The url provided is invalid or unable to be reached. Check both the url and authentication used."))) };
            adw_alert_dialog_add_responses(dialog, "close", _("Close"), nullptr);
            adw_alert_dialog_set_close_response(dialog, "close");
            adw_alert_dialog_set_default_response(dialog, "close");
            adw_dialog_present(ADW_DIALOG(dialog), GTK_WIDGET(m_parent));
            adw_dialog_set_can_close(m_dialog, true);
            adw_view_stack_set_visible_child_name(ADW_VIEW_STACK(gtk_builder_get_object(m_builder, "viewStack")), "validate");
            return;
        }
        adw_dialog_set_can_close(m_dialog, true);
        if(!m_controller->isUrlPlaylist())
        {
            adw_view_stack_set_visible_child_name(ADW_VIEW_STACK(gtk_builder_get_object(m_builder, "viewStack")), "download-single");
            setComboRowModel(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "fileTypeSingleRow")), m_controller->getFileTypeStrings());
            adw_combo_row_set_selected(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "fileTypeSingleRow")), static_cast<unsigned int>(m_controller->getPreviousDownloadOptions().getFileType()));
            setComboRowModel(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "qualitySingleRow")), m_controller->getQualityStrings(static_cast<size_t>(adw_combo_row_get_selected(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "fileTypeSingleRow"))))));
            setComboRowModel(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "audioLanguageSingleRow")), m_controller->getAudioLanguageStrings());
            adw_switch_row_set_active(ADW_SWITCH_ROW(gtk_builder_get_object(m_builder, "downloadSubtitlesSingleRow")), m_controller->getPreviousDownloadOptions().getDownloadSubtitles());
            adw_action_row_set_subtitle(ADW_ACTION_ROW(gtk_builder_get_object(m_builder, "saveFolderSingleRow")), m_controller->getPreviousDownloadOptions().getSaveFolder().string().c_str());
            gtk_editable_set_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "filenameSingleRow")), m_controller->getMediaTitle(0).c_str());
            adw_switch_row_set_active(ADW_SWITCH_ROW(gtk_builder_get_object(m_builder, "preferAV1SingleRow")), m_controller->getPreviousDownloadOptions().getPreferAV1());
            adw_switch_row_set_active(ADW_SWITCH_ROW(gtk_builder_get_object(m_builder, "splitChaptersSingleRow")), m_controller->getPreviousDownloadOptions().getSplitChapters());
            adw_switch_row_set_active(ADW_SWITCH_ROW(gtk_builder_get_object(m_builder, "limitSpeedSingleRow")), m_controller->getPreviousDownloadOptions().getLimitSpeed());
            gtk_editable_set_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "startTimeSingleRow")), m_controller->getMediaTimeFrame(0).startStr().c_str());
            gtk_editable_set_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "endTimeSingleRow")), m_controller->getMediaTimeFrame(0).endStr().c_str());
        }
        else
        {
            adw_view_stack_set_visible_child_name(ADW_VIEW_STACK(gtk_builder_get_object(m_builder, "viewStack")), "download-playlist");
            adw_action_row_set_subtitle(ADW_ACTION_ROW(gtk_builder_get_object(m_builder, "itemsPlaylistRow")), std::vformat(_("{} items"), std::make_format_args(CodeHelpers::unmove(m_controller->getMediaCount()))).c_str());
            setComboRowModel(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "fileTypePlaylistRow")), m_controller->getFileTypeStrings());
            adw_switch_row_set_active(ADW_SWITCH_ROW(gtk_builder_get_object(m_builder, "downloadSubtitlesPlaylistRow")), m_controller->getPreviousDownloadOptions().getDownloadSubtitles());
            adw_action_row_set_subtitle(ADW_ACTION_ROW(gtk_builder_get_object(m_builder, "saveFolderPlaylistRow")), m_controller->getPreviousDownloadOptions().getSaveFolder().string().c_str());
            adw_switch_row_set_active(ADW_SWITCH_ROW(gtk_builder_get_object(m_builder, "preferAV1PlaylistRow")), m_controller->getPreviousDownloadOptions().getPreferAV1());
            adw_switch_row_set_active(ADW_SWITCH_ROW(gtk_builder_get_object(m_builder, "splitChaptersPlaylistRow")), m_controller->getPreviousDownloadOptions().getSplitChapters());
            adw_switch_row_set_active(ADW_SWITCH_ROW(gtk_builder_get_object(m_builder, "limitSpeedPlaylistRow")), m_controller->getPreviousDownloadOptions().getLimitSpeed());
            for(size_t i = 0; i < m_controller->getMediaCount(); i++)
            {
                GtkCheckButton* chk{ GTK_CHECK_BUTTON(gtk_check_button_new()) };
                gtk_widget_set_valign(GTK_WIDGET(chk), GTK_ALIGN_CENTER);
                gtk_widget_add_css_class(GTK_WIDGET(chk), "selection-mode");
                gtk_check_button_set_active(chk, true);
                GtkButton* undo{ GTK_BUTTON(gtk_button_new()) };
                gtk_widget_set_valign(GTK_WIDGET(undo), GTK_ALIGN_CENTER);
                gtk_button_set_icon_name(undo, "edit-undo-symbolic");
                gtk_widget_set_tooltip_text(GTK_WIDGET(undo), _("Revert to Title"));
                gtk_widget_add_css_class(GTK_WIDGET(undo), "flat");
                gtk_widget_set_name(GTK_WIDGET(undo), std::to_string(i).c_str());
                g_signal_connect(undo, "clicked", G_CALLBACK(+[](GtkButton* btn, gpointer data)
                { 
                    AddDownloadDialog* dialog{ reinterpret_cast<AddDownloadDialog*>(data) };
                    size_t index{ std::stoul(gtk_widget_get_name(GTK_WIDGET(btn))) };
                    gtk_editable_set_text(GTK_EDITABLE(dialog->m_playlistItemRows[index]), dialog->m_controller->getMediaTitle(index, adw_switch_row_get_active(ADW_SWITCH_ROW(gtk_builder_get_object(dialog->m_builder, "numberTitlesPlaylistRow")))).c_str());
                }), this);
                AdwEntryRow* row{ ADW_ENTRY_ROW(adw_entry_row_new()) };
                adw_preferences_row_set_use_markup(ADW_PREFERENCES_ROW(row), false);
                adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), m_controller->getMediaUrl(i).c_str());
                gtk_editable_set_text(GTK_EDITABLE(row), m_controller->getMediaTitle(i, adw_switch_row_get_active(ADW_SWITCH_ROW(gtk_builder_get_object(m_builder, "numberTitlesPlaylistRow")))).c_str());
                adw_entry_row_add_prefix(row, GTK_WIDGET(chk));
                adw_entry_row_add_suffix(row, GTK_WIDGET(undo));
                adw_preferences_group_add(ADW_PREFERENCES_GROUP(gtk_builder_get_object(m_builder, "itemsPlaylistGroup")), GTK_WIDGET(row));
                m_playlistItemRows.push_back(row);
                m_playlistItemCheckButtons.push_back(chk);
            }
        }
    }

    void AddDownloadDialog::back()
    {
        adw_view_stack_set_visible_child_name(ADW_VIEW_STACK(gtk_builder_get_object(m_builder, "viewStack")), !m_controller->isUrlPlaylist() ? "download-single" : "download-playlist");
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(m_builder, "backButton")), false);
    }

    void AddDownloadDialog::onFileTypeSingleChanged()
    {
        setComboRowModel(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "qualitySingleRow")), m_controller->getQualityStrings(static_cast<size_t>(adw_combo_row_get_selected(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "fileTypeSingleRow"))))));
    }

    void AddDownloadDialog::advancedOptionsSingle()
    {
        adw_view_stack_set_visible_child_name(ADW_VIEW_STACK(gtk_builder_get_object(m_builder, "viewStack")), "download-single-advanced");
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(m_builder, "backButton")), true);
    }

    void AddDownloadDialog::selectSaveFolderSingle()
    {
        GtkFileDialog* folderDialog{ gtk_file_dialog_new() };
        gtk_file_dialog_set_title(folderDialog, _("Select Save Folder"));
        gtk_file_dialog_select_folder(folderDialog, m_parent, nullptr, GAsyncReadyCallback(+[](GObject* self, GAsyncResult* res, gpointer data)
        {
            GFile* folder{ gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(self), res, nullptr) };
            if(folder)
            {
                adw_action_row_set_subtitle(ADW_ACTION_ROW(gtk_builder_get_object(reinterpret_cast<AddDownloadDialog*>(data)->m_builder, "saveFolderSingleRow")), g_file_get_path(folder));
            }
        }), this);
    }

    void AddDownloadDialog::revertFilenameSingle()
    {
        gtk_editable_set_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "filenameSingleRow")), m_controller->getMediaTitle(0).c_str());
    }

    void AddDownloadDialog::revertStartTimeSingle()
    {
        gtk_editable_set_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "startTimeSingleRow")), m_controller->getMediaTimeFrame(0).startStr().c_str());
    }

    void AddDownloadDialog::revertEndTimeSingle()
    {
        gtk_editable_set_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "endTimeSingleRow")), m_controller->getMediaTimeFrame(0).endStr().c_str());
    }

    void AddDownloadDialog::downloadSingle()
    {
        m_controller->addSingleDownload(adw_action_row_get_subtitle(ADW_ACTION_ROW(gtk_builder_get_object(m_builder, "saveFolderSingleRow"))), gtk_editable_get_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "filenameSingleRow"))), adw_combo_row_get_selected(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "fileTypeSingleRow"))), adw_combo_row_get_selected(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "qualitySingleRow"))), adw_combo_row_get_selected(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "audioLanguageSingleRow"))), adw_switch_row_get_active(ADW_SWITCH_ROW(gtk_builder_get_object(m_builder, "downloadSubtitlesSingleRow"))), adw_switch_row_get_active(ADW_SWITCH_ROW(gtk_builder_get_object(m_builder, "preferAV1SingleRow"))), adw_switch_row_get_active(ADW_SWITCH_ROW(gtk_builder_get_object(m_builder, "splitChaptersSingleRow"))), adw_switch_row_get_active(ADW_SWITCH_ROW(gtk_builder_get_object(m_builder, "limitSpeedSingleRow"))), gtk_editable_get_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "startTimeSingleRow"))), gtk_editable_get_text(GTK_EDITABLE(gtk_builder_get_object(m_builder, "endTimeSingleRow"))));
        adw_dialog_close(m_dialog);
    }

    void AddDownloadDialog::selectSaveFolderPlaylist()
    {
        GtkFileDialog* folderDialog{ gtk_file_dialog_new() };
        gtk_file_dialog_set_title(folderDialog, _("Select Save Folder"));
        gtk_file_dialog_select_folder(folderDialog, m_parent, nullptr, GAsyncReadyCallback(+[](GObject* self, GAsyncResult* res, gpointer data)
        {
            GFile* folder{ gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(self), res, nullptr) };
            if(folder)
            {
                adw_action_row_set_subtitle(ADW_ACTION_ROW(gtk_builder_get_object(reinterpret_cast<AddDownloadDialog*>(data)->m_builder, "saveFolderPlaylistRow")), g_file_get_path(folder));
            }
        }), this);
    }

    void AddDownloadDialog::itemsPlaylist()
    {
        adw_view_stack_set_visible_child_name(ADW_VIEW_STACK(gtk_builder_get_object(m_builder, "viewStack")), "download-playlist-items");
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(m_builder, "backButton")), true);
    }

    void AddDownloadDialog::onNumberTitlesPlaylistChanged()
    {
        int i{ 0 };
        for(AdwEntryRow* row : m_playlistItemRows)
        {
            gtk_editable_set_text(GTK_EDITABLE(row), m_controller->getMediaTitle(i, adw_switch_row_get_active(ADW_SWITCH_ROW(gtk_builder_get_object(m_builder, "numberTitlesPlaylistRow")))).c_str());
            i++;
        }
    }

    void AddDownloadDialog::downloadPlaylist()
    {
        std::unordered_map<size_t, std::string> filenames;
        for(size_t i = 0; i < m_playlistItemRows.size(); i++)
        {
            if(gtk_check_button_get_active(m_playlistItemCheckButtons[i]))
            {
                filenames.emplace(i, gtk_editable_get_text(GTK_EDITABLE(m_playlistItemRows[i])));
            }
        }
        m_controller->addPlaylistDownload(adw_action_row_get_subtitle(ADW_ACTION_ROW(gtk_builder_get_object(m_builder, "saveFolderPlaylistRow"))), filenames, adw_combo_row_get_selected(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "fileTypePlaylistRow"))), adw_switch_row_get_active(ADW_SWITCH_ROW(gtk_builder_get_object(m_builder, "downloadSubtitlesPlaylistRow"))), adw_switch_row_get_active(ADW_SWITCH_ROW(gtk_builder_get_object(m_builder, "preferAV1PlaylistRow"))), adw_switch_row_get_active(ADW_SWITCH_ROW(gtk_builder_get_object(m_builder, "splitChaptersPlaylistRow"))), adw_switch_row_get_active(ADW_SWITCH_ROW(gtk_builder_get_object(m_builder, "limitSpeedPlaylistRow"))));
        adw_dialog_close(m_dialog);
    }
}