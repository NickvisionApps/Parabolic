#include "views/adddownloaddialog.h"
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>
#include "helpers/gtkhelpers.h"

using namespace Nickvision::Events;
using namespace Nickvision::Helpers;
using namespace Nickvision::Keyring;
using namespace Nickvision::TubeConverter::GNOME::Helpers;
using namespace Nickvision::TubeConverter::Shared::Controllers;
using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::GNOME::Views
{
    AddDownloadDialog::AddDownloadDialog(const std::shared_ptr<AddDownloadDialogController>& controller, const std::string& url, GtkWindow* parent)
        : DialogBase{ parent, "add_download_dialog" },
        m_controller{ controller }
    {
        //Load Validate Page
        gtk_widget_set_sensitive(m_builder.get<GtkWidget>("validateUrlButton"), false);
        adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("viewStack"), "validate");
        if(StringHelpers::isValidUrl(url))
        {
            gtk_editable_set_text(m_builder.get<GtkEditable>("urlRow"), url.c_str());
            gtk_widget_set_sensitive(m_builder.get<GtkWidget>("validateUrlButton"), true);
        }
        else
        {
            gdk_clipboard_read_text_async(gdk_display_get_clipboard(gdk_display_get_default()), nullptr, GAsyncReadyCallback(+[](GObject* self, GAsyncResult* res, gpointer data)
            {
                char* clipboardText{ gdk_clipboard_read_text_finish(GDK_CLIPBOARD(self), res, nullptr) };
                if(clipboardText)
                {
                    std::string url{ clipboardText };
                    if(StringHelpers::isValidUrl(url))
                    {
                        Builder* builder{ reinterpret_cast<Builder*>(data) };
                        gtk_editable_set_text(builder->get<GtkEditable>("urlRow"), url.c_str());
                        gtk_widget_set_sensitive(builder->get<GtkWidget>("validateUrlButton"), true);
                    }
                    g_free(clipboardText);
                }
            }), &m_builder);
        }
        std::vector<std::string> credentialNames{ m_controller->getKeyringCredentialNames() };
        credentialNames.insert(credentialNames.begin(), _("Use manual credential"));
        GtkHelpers::setComboRowModel(m_builder.get<AdwComboRow>("credentialRow"), credentialNames);
        //Signals
        g_signal_connect(m_builder.get<GObject>("urlRow"), "changed", G_CALLBACK(+[](GtkEditable*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->onTxtUrlChanged(); }), this);
        g_signal_connect(m_builder.get<GObject>("batchFileButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->useBatchFile(); }), this);
        g_signal_connect(m_builder.get<GObject>("credentialRow"), "notify::selected-item", G_CALLBACK(+[](GObject*, GParamSpec* pspec, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->onCmbCredentialChanged(); }), this);
        g_signal_connect(m_builder.get<GObject>("validateUrlButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->validateUrl(); }), this);
        g_signal_connect(m_builder.get<GObject>("backButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->back(); }), this);
        g_signal_connect(m_builder.get<GObject>("fileTypeSingleRow"), "notify::selected-item", G_CALLBACK(+[](GObject*, GParamSpec* pspec, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->onFileTypeSingleChanged(); }), this);
        g_signal_connect(m_builder.get<GObject>("subtitlesSingleRow"), "activated", G_CALLBACK(+[](AdwActionRow*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->subtitlesSingle(); }), this);
        g_signal_connect(m_builder.get<GObject>("advancedOptionsSingleRow"), "activated", G_CALLBACK(+[](AdwActionRow*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->advancedOptionsSingle(); }), this);
        g_signal_connect(m_builder.get<GObject>("selectSaveFolderSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->selectSaveFolderSingle(); }), this);
        g_signal_connect(m_builder.get<GObject>("revertFilenameSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->revertFilenameSingle(); }), this);
        g_signal_connect(m_builder.get<GObject>("selectAllSubtitlesSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->selectAllSubtitlesSingle(); }), this);
        g_signal_connect(m_builder.get<GObject>("deselectAllSubtitlesSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->deselectAllSubtitlesSingle(); }), this);
        g_signal_connect(m_builder.get<GObject>("revertStartTimeSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->revertStartTimeSingle(); }), this);
        g_signal_connect(m_builder.get<GObject>("revertEndTimeSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->revertEndTimeSingle(); }), this);
        g_signal_connect(m_builder.get<GObject>("downloadSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->downloadSingle(); }), this);
        g_signal_connect(m_builder.get<GObject>("fileTypePlaylistRow"), "notify::selected-item", G_CALLBACK(+[](GObject*, GParamSpec* pspec, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->onFileTypePlaylistChanged(); }), this);
        g_signal_connect(m_builder.get<GObject>("selectSaveFolderPlaylistButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->selectSaveFolderPlaylist(); }), this);
        g_signal_connect(m_builder.get<GObject>("itemsPlaylistRow"), "activated", G_CALLBACK(+[](AdwActionRow*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->itemsPlaylist(); }), this);
        g_signal_connect(m_builder.get<GObject>("numberTitlesPlaylistRow"), "notify::active", G_CALLBACK(+[](GObject*, GParamSpec* pspec, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->onNumberTitlesPlaylistChanged(); }), this);
        g_signal_connect(m_builder.get<GObject>("selectAllPlaylistButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->selectAllPlaylist(); }), this);
        g_signal_connect(m_builder.get<GObject>("deselectAllPlaylistButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->deselectAllPlaylist(); }), this);
        g_signal_connect(m_builder.get<GObject>("downloadPlaylistButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->downloadPlaylist(); }), this);
        m_controller->urlValidated() += [this](const EventArgs& args){ GtkHelpers::dispatchToMainThread([this]{ onUrlValidated(); }); };
    }

    void AddDownloadDialog::onTxtUrlChanged()
    {
        std::string url{ gtk_editable_get_text(m_builder.get<GtkEditable>("urlRow")) };
        gtk_widget_set_sensitive(m_builder.get<GtkWidget>("validateUrlButton"), StringHelpers::isValidUrl(url));
    }

    void AddDownloadDialog::useBatchFile()
    {
        GtkFileDialog* fileDialog{ gtk_file_dialog_new() };
        gtk_file_dialog_set_title(fileDialog, _("Select Batch File"));
        GtkFileFilter* filter{ gtk_file_filter_new() };
        gtk_file_filter_set_name(filter, _("TXT Files (*.txt)"));
        gtk_file_filter_add_pattern(filter, "*.txt");
        gtk_file_filter_add_pattern(filter, "*.TXT");
        GListStore* filters{ g_list_store_new(gtk_file_filter_get_type()) };
        g_list_store_append(filters, G_OBJECT(filter));
        gtk_file_dialog_set_filters(fileDialog, G_LIST_MODEL(filters));
        gtk_file_dialog_open(fileDialog, m_parent, nullptr, GAsyncReadyCallback(+[](GObject* self, GAsyncResult* res, gpointer data)
        {
            GFile* file{ gtk_file_dialog_open_finish(GTK_FILE_DIALOG(self), res, nullptr) };
            if(file)
            {
                AddDownloadDialog* dialog{ reinterpret_cast<AddDownloadDialog*>(data) };
                adw_dialog_set_can_close(dialog->m_dialog, false);
                adw_view_stack_set_visible_child_name(dialog->m_builder.get<AdwViewStack>("viewStack"), "spinner");
                std::optional<Credential> credential{ std::nullopt };
                if(adw_expander_row_get_enable_expansion(dialog->m_builder.get<AdwExpanderRow>("authenticateRow")) && adw_combo_row_get_selected(dialog->m_builder.get<AdwComboRow>("credentialRow")) == 0)
                {
                    credential = Credential{ "", "", gtk_editable_get_text(dialog->m_builder.get<GtkEditable>("usernameRow")), gtk_editable_get_text(dialog->m_builder.get<GtkEditable>("passwordRow")) };
                }
                if(adw_combo_row_get_selected(dialog->m_builder.get<AdwComboRow>("credentialRow")) == 0)
                {
                    dialog->m_controller->validateBatchFile(g_file_get_path(file), credential);
                }
                else
                {
                    dialog->m_controller->validateBatchFile(g_file_get_path(file), adw_combo_row_get_selected(dialog->m_builder.get<AdwComboRow>("credentialRow")) - 1);
                }
            }
        }), this);
    }

    void AddDownloadDialog::onCmbCredentialChanged()
    {
        bool visible{ adw_combo_row_get_selected(m_builder.get<AdwComboRow>("credentialRow")) == 0 };
        gtk_widget_set_visible(m_builder.get<GtkWidget>("usernameRow"), visible);
        gtk_widget_set_visible(m_builder.get<GtkWidget>("passwordRow"), visible);
    }

    void AddDownloadDialog::validateUrl()
    {
        adw_dialog_set_can_close(m_dialog, false);
        adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("viewStack"), "spinner");
        std::optional<Credential> credential{ std::nullopt };
        if(adw_expander_row_get_enable_expansion(m_builder.get<AdwExpanderRow>("authenticateRow")) && adw_combo_row_get_selected(m_builder.get<AdwComboRow>("credentialRow")) == 0)
        {
            credential = Credential{ "", "", gtk_editable_get_text(m_builder.get<GtkEditable>("usernameRow")), gtk_editable_get_text(m_builder.get<GtkEditable>("passwordRow")) };
        }
        if(adw_combo_row_get_selected(m_builder.get<AdwComboRow>("credentialRow")) == 0)
        {
            m_controller->validateUrl(gtk_editable_get_text(m_builder.get<GtkEditable>("urlRow")), credential);
        }
        else
        {
            m_controller->validateUrl(gtk_editable_get_text(m_builder.get<GtkEditable>("urlRow")), adw_combo_row_get_selected(m_builder.get<AdwComboRow>("credentialRow")) - 1);
        }
    }

    void AddDownloadDialog::onUrlValidated()
    {
        if(!m_controller->isUrlValid())
        {
            AdwAlertDialog* dialog{ ADW_ALERT_DIALOG(adw_alert_dialog_new(_("Error"), _("The url provided is invalid or unable to be reached. Check the url, the authentication used, and the selected browser for cookies in preferences."))) };
            adw_alert_dialog_add_responses(dialog, "close", _("Close"), nullptr);
            adw_alert_dialog_set_close_response(dialog, "close");
            adw_alert_dialog_set_default_response(dialog, "close");
            adw_dialog_present(ADW_DIALOG(dialog), GTK_WIDGET(m_parent));
            adw_dialog_set_can_close(m_dialog, true);
            adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("viewStack"), "validate");
            return;
        }
        adw_dialog_set_can_close(m_dialog, true);
        if(!m_controller->isUrlPlaylist()) //Single Download
        {
            size_t previous{ 0 };
            //Load Options
            adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("viewStack"), "download-single");
            GtkHelpers::setComboRowModel(m_builder.get<AdwComboRow>("fileTypeSingleRow"), m_controller->getFileTypeStrings());
            adw_combo_row_set_selected(m_builder.get<AdwComboRow>("fileTypeSingleRow"), static_cast<unsigned int>(m_controller->getPreviousDownloadOptions().getFileType()));
            GtkHelpers::setComboRowModel(m_builder.get<AdwComboRow>("videoFormatSingleRow"), m_controller->getVideoFormatStrings(&previous), "", false);
            adw_combo_row_set_selected(m_builder.get<AdwComboRow>("videoFormatSingleRow"), previous);
            GtkHelpers::setComboRowModel(m_builder.get<AdwComboRow>("audioFormatSingleRow"), m_controller->getAudioFormatStrings(&previous), "", false);
            adw_combo_row_set_selected(m_builder.get<AdwComboRow>("audioFormatSingleRow"), previous);
            adw_action_row_set_subtitle(m_builder.get<AdwActionRow>("saveFolderSingleRow"), m_controller->getPreviousDownloadOptions().getSaveFolder().string().c_str());
            gtk_editable_set_text(m_builder.get<GtkEditable>("filenameSingleRow"), m_controller->getMediaTitle(0).c_str());
            //Load Subtitles
            std::vector<std::string> subtitles{ m_controller->getSubtitleLanguageStrings() };
            std::vector<SubtitleLanguage> previousSubtitles{ m_controller->getPreviousDownloadOptions().getSubtitleLanguages() };
            for(const std::string& subtitle : subtitles)
            {
                bool wasPreviouslySelected{ false };
                for(const SubtitleLanguage& language : previousSubtitles)
                {
                    if(subtitle == language.str())
                    {
                        wasPreviouslySelected = true;
                        break;
                    }
                }
                GtkCheckButton* chk{ GTK_CHECK_BUTTON(gtk_check_button_new()) };
                gtk_widget_set_valign(GTK_WIDGET(chk), GTK_ALIGN_CENTER);
                gtk_widget_add_css_class(GTK_WIDGET(chk), "selection-mode");
                gtk_check_button_set_active(chk, wasPreviouslySelected);
                AdwActionRow* row{ ADW_ACTION_ROW(adw_action_row_new()) };
                adw_preferences_row_set_use_markup(ADW_PREFERENCES_ROW(row), false);
                adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), subtitle.c_str());
                adw_action_row_add_prefix(row, GTK_WIDGET(chk));
                adw_action_row_set_activatable_widget(row, GTK_WIDGET(chk));
                adw_preferences_group_add(m_builder.get<AdwPreferencesGroup>("subtitlesSingleGroup"), GTK_WIDGET(row));
                m_singleSubtitleRows.push_back(row);
                m_singleSubtitleCheckButtons.push_back(chk);
            }
            if(subtitles.empty())
            {
                AdwActionRow* row{ ADW_ACTION_ROW(adw_action_row_new()) };
                adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), _("No Subtitles Available"));
                adw_preferences_group_add(m_builder.get<AdwPreferencesGroup>("subtitlesSingleGroup"), GTK_WIDGET(row));
            }
            //Load Advanced Options
            adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("splitChaptersSingleRow"), m_controller->getPreviousDownloadOptions().getSplitChapters());
            adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("limitSpeedSingleRow"), m_controller->getPreviousDownloadOptions().getLimitSpeed());
            adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("exportDescriptionSingleRow"), m_controller->getPreviousDownloadOptions().getExportDescription());
            gtk_editable_set_text(m_builder.get<GtkEditable>("startTimeSingleRow"), m_controller->getMediaTimeFrame(0).startStr().c_str());
            gtk_editable_set_text(m_builder.get<GtkEditable>("endTimeSingleRow"), m_controller->getMediaTimeFrame(0).endStr().c_str());
        }
        else //Playlist Download
        {
            adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("viewStack"), "download-playlist");
            GtkHelpers::setComboRowModel(m_builder.get<AdwComboRow>("fileTypePlaylistRow"), m_controller->getFileTypeStrings());
            adw_combo_row_set_selected(m_builder.get<AdwComboRow>("fileTypePlaylistRow"), static_cast<unsigned int>(m_controller->getPreviousDownloadOptions().getFileType()));
            adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("splitChaptersPlaylistRow"), m_controller->getPreviousDownloadOptions().getSplitChapters());
            adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("limitSpeedPlaylistRow"), m_controller->getPreviousDownloadOptions().getLimitSpeed());
            adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("exportDescriptionPlaylistRow"), m_controller->getPreviousDownloadOptions().getExportDescription());
            adw_action_row_set_subtitle(m_builder.get<AdwActionRow>("saveFolderPlaylistRow"), m_controller->getPreviousDownloadOptions().getSaveFolder().string().c_str());
            adw_action_row_set_subtitle(m_builder.get<AdwActionRow>("itemsPlaylistRow"), _f("{} items", m_controller->getMediaCount()).c_str());
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
                    gtk_editable_set_text(GTK_EDITABLE(dialog->m_playlistItemRows[index]), dialog->m_controller->getMediaTitle(index, adw_switch_row_get_active(dialog->m_builder.get<AdwSwitchRow>("numberTitlesPlaylistRow"))).c_str());
                }), this);
                AdwEntryRow* row{ ADW_ENTRY_ROW(adw_entry_row_new()) };
                adw_preferences_row_set_use_markup(ADW_PREFERENCES_ROW(row), false);
                adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), m_controller->getMediaUrl(i).c_str());
                gtk_editable_set_text(GTK_EDITABLE(row), m_controller->getMediaTitle(i, adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("numberTitlesPlaylistRow"))).c_str());
                adw_entry_row_add_prefix(row, GTK_WIDGET(chk));
                adw_entry_row_add_suffix(row, GTK_WIDGET(undo));
                adw_preferences_group_add(m_builder.get<AdwPreferencesGroup>("itemsPlaylistGroup"), GTK_WIDGET(row));
                m_playlistItemRows.push_back(row);
                m_playlistItemCheckButtons.push_back(chk);
            }
            adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("numberTitlesPlaylistRow"), m_controller->getPreviousDownloadOptions().getNumberTitles());
        }
        if(m_controller->getDownloadImmediatelyAfterValidation())
        {
            if(m_controller->isUrlPlaylist())
            {
                downloadPlaylist();
            }
            else
            {
                downloadSingle();
            }
        }
    }

    void AddDownloadDialog::back()
    {
        adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("viewStack"), !m_controller->isUrlPlaylist() ? "download-single" : "download-playlist");
        gtk_widget_set_visible(m_builder.get<GtkWidget>("backButton"), false);
    }

    void AddDownloadDialog::onFileTypeSingleChanged()
    {
        int fileTypeIndex{ adw_combo_row_get_selected(m_builder.get<AdwComboRow>("fileTypeSingleRow")) };
        if(m_controller->getFileTypeStrings().size() == MediaFileType::getAudioFileTypeCount())
        {
            fileTypeIndex += MediaFileType::getVideoFileTypeCount();
        }
        MediaFileType type{ static_cast<MediaFileType::MediaFileTypeValue>(fileTypeIndex) };
        gtk_widget_set_sensitive(m_builder.get<GtkWidget>("videoFormatSingleRow"), !type.isAudio());
        if(type.isGeneric() && m_controller->getShowGenericDisclaimer())
        {
            AdwAlertDialog* dialog{ ADW_ALERT_DIALOG(adw_alert_dialog_new(_("Warning"), _("Generic file types do not support embedding thumbnails and subtitles. Please select a specific file type that supports embedding to prevent separate image and subtitle files from being written to disk."))) };
            adw_alert_dialog_set_extra_child(dialog, gtk_check_button_new_with_label(_("Don't show this message again")));
            adw_alert_dialog_add_responses(dialog, "close", _("Close"), nullptr);
            adw_alert_dialog_set_default_response(dialog, "close");
            adw_alert_dialog_set_close_response(dialog, "close");
            g_signal_connect(dialog, "response", G_CALLBACK(+[](AdwAlertDialog* self, const char*, gpointer data)
            {
                AddDownloadDialog* addDownloadDialog{ reinterpret_cast<AddDownloadDialog*>(data) };
                addDownloadDialog->m_controller->setShowGenericDisclaimer(!gtk_check_button_get_active(GTK_CHECK_BUTTON(adw_alert_dialog_get_extra_child(self))));
            }), this);
            adw_dialog_present(ADW_DIALOG(dialog), GTK_WIDGET(m_dialog));
        }
    }

    void AddDownloadDialog::subtitlesSingle()
    {
        adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("viewStack"), "download-single-subtitles");
        gtk_widget_set_visible(m_builder.get<GtkWidget>("backButton"), true);
    }

    void AddDownloadDialog::advancedOptionsSingle()
    {
        adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("viewStack"), "download-single-advanced");
        gtk_widget_set_visible(m_builder.get<GtkWidget>("backButton"), true);
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
                adw_action_row_set_subtitle(reinterpret_cast<Builder*>(data)->get<AdwActionRow>("saveFolderSingleRow"), g_file_get_path(folder));
            }
        }), &m_builder);
    }

    void AddDownloadDialog::revertFilenameSingle()
    {
        gtk_editable_set_text(m_builder.get<GtkEditable>("filenameSingleRow"), m_controller->getMediaTitle(0).c_str());
    }

    void AddDownloadDialog::selectAllSubtitlesSingle()
    {
        for(GtkCheckButton* chk : m_singleSubtitleCheckButtons)
        {
            gtk_check_button_set_active(chk, true);
        }
    }

    void AddDownloadDialog::deselectAllSubtitlesSingle()
    {
        for(GtkCheckButton* chk : m_singleSubtitleCheckButtons)
        {
            gtk_check_button_set_active(chk, false);
        }
    }

    void AddDownloadDialog::revertStartTimeSingle()
    {
        gtk_editable_set_text(m_builder.get<GtkEditable>("startTimeSingleRow"), m_controller->getMediaTimeFrame(0).startStr().c_str());
    }

    void AddDownloadDialog::revertEndTimeSingle()
    {
        gtk_editable_set_text(m_builder.get<GtkEditable>("endTimeSingleRow"), m_controller->getMediaTimeFrame(0).endStr().c_str());
    }

    void AddDownloadDialog::downloadSingle()
    {
        std::vector<std::string> subtitles;
        for(size_t i = 0; i < m_singleSubtitleRows.size(); i++)
        {
            if(gtk_check_button_get_active(m_singleSubtitleCheckButtons[i]))
            {
                subtitles.push_back(adw_preferences_row_get_title(ADW_PREFERENCES_ROW(m_singleSubtitleRows[i])));
            }
        }
        m_controller->addSingleDownload(adw_action_row_get_subtitle(m_builder.get<AdwActionRow>("saveFolderSingleRow")), gtk_editable_get_text(m_builder.get<GtkEditable>("filenameSingleRow")), adw_combo_row_get_selected(m_builder.get<AdwComboRow>("fileTypeSingleRow")), adw_combo_row_get_selected(m_builder.get<AdwComboRow>("videoFormatSingleRow")), adw_combo_row_get_selected(m_builder.get<AdwComboRow>("audioFormatSingleRow")), subtitles, adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("excludeHistorySingleRow")), adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("splitChaptersSingleRow")), adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("limitSpeedSingleRow")), adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("exportDescriptionSingleRow")), gtk_editable_get_text(m_builder.get<GtkEditable>("startTimeSingleRow")), gtk_editable_get_text(m_builder.get<GtkEditable>("endTimeSingleRow")));
        adw_dialog_close(m_dialog);
    }

    void AddDownloadDialog::onFileTypePlaylistChanged()
    {
        int fileTypeIndex{ adw_combo_row_get_selected(m_builder.get<AdwComboRow>("fileTypePlaylistRow")) };
        if(m_controller->getFileTypeStrings().size() == MediaFileType::getAudioFileTypeCount())
        {
            fileTypeIndex += MediaFileType::getVideoFileTypeCount();
        }
        MediaFileType type{ static_cast<MediaFileType::MediaFileTypeValue>(fileTypeIndex) };
        if(type.isGeneric() && m_controller->getShowGenericDisclaimer())
        {
            AdwAlertDialog* dialog{ ADW_ALERT_DIALOG(adw_alert_dialog_new(_("Warning"), _("Generic file types do not support embedding thumbnails and subtitles. Please select a specific file type that supports embedding to prevent separate image and subtitle files from being written to disk."))) };
            adw_alert_dialog_set_extra_child(dialog, gtk_check_button_new_with_label(_("Don't show this message again")));
            adw_alert_dialog_add_responses(dialog, "close", _("Close"), nullptr);
            adw_alert_dialog_set_default_response(dialog, "close");
            adw_alert_dialog_set_close_response(dialog, "close");
            g_signal_connect(dialog, "response", G_CALLBACK(+[](AdwAlertDialog* self, const char*, gpointer data)
            {
                AddDownloadDialog* addDownloadDialog{ reinterpret_cast<AddDownloadDialog*>(data) };
                addDownloadDialog->m_controller->setShowGenericDisclaimer(!gtk_check_button_get_active(GTK_CHECK_BUTTON(adw_alert_dialog_get_extra_child(self))));
            }), this);
            adw_dialog_present(ADW_DIALOG(dialog), GTK_WIDGET(m_dialog));
        }
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
                adw_action_row_set_subtitle(reinterpret_cast<Builder*>(data)->get<AdwActionRow>("saveFolderPlaylistRow"), g_file_get_path(folder));
            }
        }), &m_builder);
    }

    void AddDownloadDialog::itemsPlaylist()
    {
        adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("viewStack"), "download-playlist-items");
        gtk_widget_set_visible(m_builder.get<GtkWidget>("backButton"), true);
    }

    void AddDownloadDialog::onNumberTitlesPlaylistChanged()
    {
        int i{ 0 };
        for(AdwEntryRow* row : m_playlistItemRows)
        {
            gtk_editable_set_text(GTK_EDITABLE(row), m_controller->getMediaTitle(i, adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("numberTitlesPlaylistRow"))).c_str());
            i++;
        }
    }

    void AddDownloadDialog::selectAllPlaylist()
    {
        for(GtkCheckButton* chk : m_playlistItemCheckButtons)
        {
            gtk_check_button_set_active(chk, true);
        }
    }

    void AddDownloadDialog::deselectAllPlaylist()
    {
        for(GtkCheckButton* chk : m_playlistItemCheckButtons)
        {
            gtk_check_button_set_active(chk, false);
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
        m_controller->addPlaylistDownload(adw_action_row_get_subtitle(m_builder.get<AdwActionRow>("saveFolderPlaylistRow")), filenames, adw_combo_row_get_selected(m_builder.get<AdwComboRow>("fileTypePlaylistRow")), adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("excludeHistoryPlaylistRow")), adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("splitChaptersPlaylistRow")), adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("limitSpeedPlaylistRow")), adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("exportDescriptionPlaylistRow")));
        adw_dialog_close(m_dialog);
    }
}
