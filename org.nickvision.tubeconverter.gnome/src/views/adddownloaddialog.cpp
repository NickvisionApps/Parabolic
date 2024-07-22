#include "views/adddownloaddialog.h"
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
        g_signal_connect(gtk_builder_get_object(m_builder, "fileTypeSingleRow"), "notify::selected-item", G_CALLBACK(+[](GObject*, GParamSpec* pspec, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->onFileTypeSingleChanged(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "backDownloadSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->backSingle(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "advancedOptionsSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->advancedOptionsSingle(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "selectSaveFolderSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->selectSaveFolderSingle(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "revertFilenameSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->revertFilenameSingle(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "revertStartTimeSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->revertStartTimeSingle(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "revertEndTimeSingleButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->revertEndTimeSingle(); }), this);
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
        }
    }

    void AddDownloadDialog::onFileTypeSingleChanged()
    {
        setComboRowModel(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "qualitySingleRow")), m_controller->getQualityStrings(static_cast<size_t>(adw_combo_row_get_selected(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "fileTypeSingleRow"))))));
    }

    void AddDownloadDialog::backSingle()
    {
        adw_view_stack_set_visible_child_name(ADW_VIEW_STACK(gtk_builder_get_object(m_builder, "viewStack")), "download-single");
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(m_builder, "backDownloadSingleButton")), false);
    }

    void AddDownloadDialog::advancedOptionsSingle()
    {
        adw_view_stack_set_visible_child_name(ADW_VIEW_STACK(gtk_builder_get_object(m_builder, "viewStack")), "download-single-advanced");
        gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(m_builder, "backDownloadSingleButton")), true);
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
}