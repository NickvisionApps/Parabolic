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
        GtkStringList* credentialNamesList{ gtk_string_list_new(nullptr) };
        gtk_string_list_append(credentialNamesList, _("Use manual credential"));
        for(const std::string& name : m_controller->getKeyringCredentialNames())
        {
            gtk_string_list_append(credentialNamesList, name.c_str());
        }
        adw_combo_row_set_model(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "credentialRow")), G_LIST_MODEL(credentialNamesList));
        adw_combo_row_set_selected(ADW_COMBO_ROW(gtk_builder_get_object(m_builder, "credentialRow")), 0);
        //Signals
        m_closed += [&](const EventArgs&) { onClosed(); };
        g_signal_connect(gtk_builder_get_object(m_builder, "urlRow"), "changed", G_CALLBACK(+[](GtkEditable*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->onTxtUrlChanged(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "credentialRow"), "notify::selected-item", G_CALLBACK(+[](GObject*, GParamSpec* pspec, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->onCmbCredentialChanged(); }), this);
        g_signal_connect(gtk_builder_get_object(m_builder, "validateUrlButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<AddDownloadDialog*>(data)->onValidateUrl(); }), this);
        m_controller->urlValidated() += [&](const ParamEventArgs<std::vector<Media>>& args){ onUrlValidated(args); };
    }

    void AddDownloadDialog::onClosed()
    {
        
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

    void AddDownloadDialog::onValidateUrl()
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

    void AddDownloadDialog::onUrlValidated(const ParamEventArgs<std::vector<Media>>& args)
    {
        if(args.getParam().empty())
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
        adw_view_stack_set_visible_child_name(ADW_VIEW_STACK(gtk_builder_get_object(m_builder, "viewStack")), "download");
    }
}