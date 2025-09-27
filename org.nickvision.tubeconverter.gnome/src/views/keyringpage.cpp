#include "views/keyringpage.h"
#include <libnick/localization/gettext.h>

using namespace Nickvision::Keyring;
using namespace Nickvision::TubeConverter::Shared::Controllers;
using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::GNOME::Views
{
    KeyringPage::KeyringPage(const std::shared_ptr<KeyringDialogController>& controller, AdwToastOverlay* toastOverlay, GtkWindow* parent)
        : ControlBase{ parent, "keyring_page" },
        m_controller{ controller },
        m_editMode{ EditMode::None }
    {
        //Signals
        g_signal_connect(m_builder.get<GObject>("addCredentialNoneButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<KeyringPage*>(data)->addNewCredential(); }), this);
        g_signal_connect(m_builder.get<GObject>("addCredentialButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<KeyringPage*>(data)->addNewCredential(); }), this);
        g_signal_connect(m_builder.get<GObject>("editConfirmButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<KeyringPage*>(data)->editConfirm(); }), this);
        //Load
        if(!m_controller->isSavingToDisk())
        {
            adw_toast_overlay_add_toast(toastOverlay, adw_toast_new(_("Keyring is not saving to disk. Changes will be lost.")));
        }
        reloadCredentials();
    }

    void KeyringPage::reloadCredentials()
    {
        for(AdwActionRow* row : m_credentialRows)
        {
            adw_preferences_group_remove(m_builder.get<AdwPreferencesGroup>("credentialsGroup"), GTK_WIDGET(row));
        }
        m_credentialRows.clear();
        for(const Credential& credential : m_controller->getCredentials())
        {
            //Edit Button
            GtkButton* editButton{ GTK_BUTTON(gtk_button_new_from_icon_name("document-edit-symbolic")) };
            std::pair<KeyringPage*, std::string>* editPair{ new std::pair<KeyringPage*, std::string>(this, credential.getName()) };
            gtk_widget_set_valign(GTK_WIDGET(editButton), GTK_ALIGN_CENTER);
            gtk_widget_set_tooltip_text(GTK_WIDGET(editButton), _("Edit"));
            gtk_widget_add_css_class(GTK_WIDGET(editButton), "flat");
            g_signal_connect_data(editButton, "clicked", GCallback(+[](GtkButton*, gpointer data)
            {
                std::pair<KeyringPage*, std::string>* pair{ reinterpret_cast<std::pair<KeyringPage*, std::string>*>(data) };
                pair->first->editCredential(pair->second);
            }), editPair, GClosureNotify(+[](gpointer data, GClosure*)
            {
                delete reinterpret_cast<std::pair<KeyringPage*, std::string>*>(data);
            }), G_CONNECT_DEFAULT);
            //Delete Button
            GtkButton* deleteButton{ GTK_BUTTON(gtk_button_new_from_icon_name("user-trash-symbolic")) };
            std::pair<KeyringPage*, std::string>* deletePair{ new std::pair<KeyringPage*, std::string>(this, credential.getName()) };
            gtk_widget_set_valign(GTK_WIDGET(deleteButton), GTK_ALIGN_CENTER);
            gtk_widget_set_tooltip_text(GTK_WIDGET(deleteButton), _("Delete"));
            gtk_widget_add_css_class(GTK_WIDGET(deleteButton), "flat");
            g_signal_connect_data(deleteButton, "clicked", GCallback(+[](GtkButton*, gpointer data)
            {
                std::pair<KeyringPage*, std::string>* pair{ reinterpret_cast<std::pair<KeyringPage*, std::string>*>(data) };
                pair->first->deleteCredential(pair->second);
            }), deletePair, GClosureNotify(+[](gpointer data, GClosure*)
            {
                delete reinterpret_cast<std::pair<KeyringPage*, std::string>*>(data);
            }), G_CONNECT_DEFAULT);
            //Row
            AdwActionRow* row{ ADW_ACTION_ROW(adw_action_row_new()) };
            adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), credential.getName().c_str());
            adw_action_row_set_subtitle(row, credential.getUri().c_str());
            adw_action_row_add_suffix(row, GTK_WIDGET(editButton));
            adw_action_row_add_suffix(row, GTK_WIDGET(deleteButton));
            adw_action_row_set_activatable_widget(row, GTK_WIDGET(editButton));
            adw_preferences_group_add(m_builder.get<AdwPreferencesGroup>("credentialsGroup"), GTK_WIDGET(row));
            m_credentialRows.push_back(row);
        }
        adw_view_stack_set_visible_child_name(m_builder.get<AdwViewStack>("viewStack"), m_controller->getCredentials().empty() ? "no-credentials" : "credentials");
    }

    void KeyringPage::addNewCredential()
    {
        m_editMode = EditMode::Add;
        gtk_editable_set_text(m_builder.get<GtkEditable>("editNameRow"), "");
        gtk_editable_set_text(m_builder.get<GtkEditable>("editUrlRow"), "");
        gtk_editable_set_text(m_builder.get<GtkEditable>("editUsernameRow"), "");
        gtk_editable_set_text(m_builder.get<GtkEditable>("editPasswordRow"), "");
        gtk_button_set_label(m_builder.get<GtkButton>("editConfirmButton"), _("Add"));
        adw_dialog_present(m_builder.get<AdwDialog>("editDialog"), GTK_WIDGET(m_parent));
    }

    void KeyringPage::editCredential(const std::string& name)
    {
        std::optional<Credential> credential{ m_controller->getCredential(name) };
        if(!credential)
        {
            return;
        }
        m_editMode = EditMode::Modify;
        gtk_editable_set_text(m_builder.get<GtkEditable>("editNameRow"), credential->getName().c_str());
        gtk_editable_set_text(m_builder.get<GtkEditable>("editUrlRow"), credential->getUri().c_str());
        gtk_editable_set_text(m_builder.get<GtkEditable>("editUsernameRow"), credential->getUsername().c_str());
        gtk_editable_set_text(m_builder.get<GtkEditable>("editPasswordRow"), credential->getPassword().c_str());
        gtk_button_set_label(m_builder.get<GtkButton>("editConfirmButton"), _("Modify"));
        adw_dialog_present(m_builder.get<AdwDialog>("editDialog"), GTK_WIDGET(m_parent));
    }

    void KeyringPage::deleteCredential(const std::string& name)
    {
        AdwAlertDialog* dialog{ ADW_ALERT_DIALOG(adw_alert_dialog_new(_("Delete Credential?"), _("Are you sure you want to delete this credential?"))) };
        std::pair<KeyringPage*, std::string>* pair{ new std::pair<KeyringPage*, std::string>(this, name) };
        adw_alert_dialog_add_responses(dialog, "delete", _("Delete"), "cancel", _("Cancel"), nullptr);
        adw_alert_dialog_set_response_appearance(dialog, "delete", ADW_RESPONSE_DESTRUCTIVE);
        adw_alert_dialog_set_default_response(dialog, "cancel");
        adw_alert_dialog_set_close_response(dialog, "cancel");
        g_signal_connect(dialog, "response", GCallback(+[](AdwAlertDialog* self, const char* response, gpointer data)
        {
            std::pair<KeyringPage*, std::string>* pair{ reinterpret_cast<std::pair<KeyringPage*, std::string>*>(data) };
            if(std::string(response) == "delete")
            {
                pair->first->m_controller->deleteCredential(pair->second);
                pair->first->reloadCredentials();
            }
            delete pair;
        }), pair);
        adw_dialog_present(ADW_DIALOG(dialog), GTK_WIDGET(m_parent));
    }

    void KeyringPage::editConfirm()
    {
        CredentialCheckStatus status;
        adw_dialog_set_title(m_builder.get<AdwDialog>("editDialog"), _("Credential"));
        adw_preferences_row_set_title(m_builder.get<AdwPreferencesRow>("editNameRow"), _("Name"));
        gtk_widget_remove_css_class(m_builder.get<GtkWidget>("editNameRow"), "error");
        adw_preferences_row_set_title(m_builder.get<AdwPreferencesRow>("editUrlRow"), _("URL"));
        gtk_widget_remove_css_class(m_builder.get<GtkWidget>("editUrlRow"), "error");
        adw_preferences_row_set_title(m_builder.get<AdwPreferencesRow>("editUsernameRow"), _("Username"));
        gtk_widget_remove_css_class(m_builder.get<GtkWidget>("editUsernameRow"), "error");
        adw_preferences_row_set_title(m_builder.get<AdwPreferencesRow>("editPasswordRow"), _("Password"));
        gtk_widget_remove_css_class(m_builder.get<GtkWidget>("editPasswordRow"), "error");
        switch(m_editMode)
        {
        case EditMode::Add:
            status = m_controller->addCredential(gtk_editable_get_text(m_builder.get<GtkEditable>("editNameRow")), gtk_editable_get_text(m_builder.get<GtkEditable>("editUrlRow")), gtk_editable_get_text(m_builder.get<GtkEditable>("editUsernameRow")), gtk_editable_get_text(m_builder.get<GtkEditable>("editPasswordRow")));
            break;
        case EditMode::Modify:
            status = m_controller->updateCredential(gtk_editable_get_text(m_builder.get<GtkEditable>("editNameRow")), gtk_editable_get_text(m_builder.get<GtkEditable>("editUrlRow")), gtk_editable_get_text(m_builder.get<GtkEditable>("editUsernameRow")), gtk_editable_get_text(m_builder.get<GtkEditable>("editPasswordRow")));
            break;
        default:
            return;
        }
        switch(status)
        {
        case CredentialCheckStatus::EmptyName:
            adw_preferences_row_set_title(m_builder.get<AdwPreferencesRow>("editNameRow"), _("Name (Empty)"));
            gtk_widget_add_css_class(m_builder.get<GtkWidget>("editNameRow"), "error");
            break;
        case CredentialCheckStatus::EmptyUsernamePassword:
            adw_preferences_row_set_title(m_builder.get<AdwPreferencesRow>("editUsernameRow"), _("Username (Empty)"));
            gtk_widget_add_css_class(m_builder.get<GtkWidget>("editUsernameRow"), "error");
            adw_preferences_row_set_title(m_builder.get<AdwPreferencesRow>("editPasswordRow"), _("Password (Empty)"));
            gtk_widget_add_css_class(m_builder.get<GtkWidget>("editPasswordRow"), "error");
            break;
        case CredentialCheckStatus::InvalidUri:
            adw_preferences_row_set_title(m_builder.get<AdwPreferencesRow>("editUrlRow"), _("URL (Invalid)"));
            gtk_widget_add_css_class(m_builder.get<GtkWidget>("editUrlRow"), "error");
            break;
        case CredentialCheckStatus::ExistingName:
            adw_preferences_row_set_title(m_builder.get<AdwPreferencesRow>("editNameRow"), _("Name (Exists)"));
            gtk_widget_add_css_class(m_builder.get<GtkWidget>("editNameRow"), "error");
            break;
        case CredentialCheckStatus::DatabaseError:
            adw_dialog_set_title(m_builder.get<AdwDialog>("editDialog"), _("Credential (Database Error)"));
            break;
        default:
            adw_dialog_force_close(m_builder.get<AdwDialog>("editDialog"));
            reloadCredentials();
            break;
        }
    }
}
