#include "views/credentialdialog.h"
#include <libnick/localization/gettext.h>
#include "helpers/gtkhelpers.h"

using namespace Nickvision::TubeConverter::GNOME::Helpers;
using namespace Nickvision::TubeConverter::Shared::Controllers;

namespace Nickvision::TubeConverter::GNOME::Views
{
    CredentialDialog::CredentialDialog(const std::shared_ptr<CredentialDialogController>& controller, GtkWindow* parent)
        : DialogBase{ parent, "credential_dialog" },
        m_controller{ controller }
    {
        gtk_label_set_text(m_builder.get<GtkLabel>("messageLabel"), _f("{} needs a credential to download.\nPlease select or enter one to use.", m_controller->getUrl()).c_str());
        //Signals
        g_signal_connect(m_builder.get<GObject>("credentialRow"), "notify::selected-item", G_CALLBACK(+[](GObject*, GParamSpec*, gpointer data){ reinterpret_cast<CredentialDialog*>(data)->onCmbCredentialChanged(); }), this);
        g_signal_connect(m_builder.get<GObject>("useButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<CredentialDialog*>(data)->use(); }), this);
        //Load
        std::vector<std::string> credentialNames{ m_controller->getKeyringCredentialNames() };
        credentialNames.insert(credentialNames.begin(), _("Use manual credential"));
        GtkHelpers::setComboRowModel(m_builder.get<AdwComboRow>("credentialRow"), credentialNames);
    }

    void CredentialDialog::onCmbCredentialChanged()
    {
        bool visible{ adw_combo_row_get_selected(m_builder.get<AdwComboRow>("credentialRow")) == 0 };
        gtk_widget_set_visible(m_builder.get<GtkWidget>("usernameRow"), visible);
        gtk_widget_set_visible(m_builder.get<GtkWidget>("passwordRow"), visible);
    }

    void CredentialDialog::use()
    {
        if(adw_combo_row_get_selected(m_builder.get<AdwComboRow>("credentialRow")) == 0)
        {
            std::string username{ gtk_editable_get_text(m_builder.get<GtkEditable>("usernameRow")) };
            std::string password{ gtk_editable_get_text(m_builder.get<GtkEditable>("passwordRow")) };
            if(username.empty() && password.empty())
            {
                AdwAlertDialog* dialog{ ADW_ALERT_DIALOG(adw_alert_dialog_new(_("Error"), _("Both the username and password cannot be empty."))) };
                adw_alert_dialog_add_response(dialog, "close", _("Close"));
                adw_alert_dialog_set_default_response(dialog, "close");
                adw_alert_dialog_set_close_response(dialog, "close");
                adw_dialog_present(ADW_DIALOG(dialog), GTK_WIDGET(m_dialog));
                return;
            }
            m_controller->use(username, password);
        }
        else
        {
            m_controller->use(adw_combo_row_get_selected(m_builder.get<AdwComboRow>("credentialRow")) - 1);
        }
        adw_dialog_close(ADW_DIALOG(m_dialog));
    }
}
