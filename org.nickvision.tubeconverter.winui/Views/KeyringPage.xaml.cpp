#include "Views/KeyringPage.xaml.h"
#if __has_include("Views/KeyringPage.g.cpp")
#include "Views/KeyringPage.g.cpp"
#endif
#include <libnick/localization/gettext.h>
#include <libnick/notifications/appnotification.h>
#include "Controls/SettingsRow.xaml.h"
#include "Helpers/WinUIHelpers.h"

using namespace ::Nickvision::Keyring;
using namespace ::Nickvision::Notifications;
using namespace ::Nickvision::TubeConverter::Shared::Controllers;
using namespace ::Nickvision::TubeConverter::Shared::Models;
using namespace ::Nickvision::TubeConverter::WinUI::Helpers;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;

namespace winrt::Nickvision::TubeConverter::WinUI::Views::implementation
{
    KeyringPage::KeyringPage()
    {
        InitializeComponent();
        //Localize Strings
        LblKeyringTitle().Text(winrt::to_hstring(_("Keyring")));
        PageNoCredentials().Title(winrt::to_hstring(_("No Credentials Found")));
        LblNoneAddCredential().Text(winrt::to_hstring(_("Add Credential")));
        LblAddCredential().Text(winrt::to_hstring(_("Add")));
    }

    void KeyringPage::Controller(const std::shared_ptr<KeyringDialogController>& controller)
    {
        m_controller = controller;
        //Load
        ReloadCredentials();
        if(!m_controller->isSavingToDisk())
        {
            AppNotification::send({ _("The keyring is not saving changes to disk. Any changes made will be lost"), NotificationSeverity::Warning });
        }
    }

    Windows::Foundation::IAsyncAction KeyringPage::AddCredential(const IInspectable& sender, const RoutedEventArgs& args)
    {
        TextBox txtName;
        txtName.Header(winrt::box_value(winrt::to_hstring(_("Name"))));
        txtName.PlaceholderText(winrt::to_hstring(_("Enter name here")));
        TextBox txtUrl;
        txtUrl.Header(winrt::box_value(winrt::to_hstring(_("URL"))));
        txtUrl.PlaceholderText(winrt::to_hstring(_("Enter url here")));
        TextBox txtUsername;
        txtUsername.Header(winrt::box_value(winrt::to_hstring(_("Username"))));
        txtUsername.PlaceholderText(winrt::to_hstring(_("Enter username here")));
        PasswordBox txtPassword;
        txtPassword.Header(winrt::box_value(winrt::to_hstring(_("Password"))));
        txtPassword.PlaceholderText(winrt::to_hstring(_("Enter password here")));
        StackPanel panel;
        panel.Orientation(Orientation::Vertical);
        panel.Spacing(12);
        panel.Children().Append(txtName);
        panel.Children().Append(txtUrl);
        panel.Children().Append(txtUsername);
        panel.Children().Append(txtPassword);
        ContentDialog dialog;
        dialog.Title(winrt::box_value(winrt::to_hstring(_("New Credential"))));
        dialog.Content(panel);
        dialog.PrimaryButtonText(winrt::to_hstring(_("Add")));
        dialog.CloseButtonText(winrt::to_hstring(_("Cancel")));
        dialog.DefaultButton(ContentDialogButton::Primary);
        dialog.RequestedTheme(RequestedTheme());
        dialog.XamlRoot(XamlRoot());
        while(true)
        {
            ContentDialogResult res{ co_await dialog.ShowAsync() };
            if(res == ContentDialogResult::Primary)
            {
                CredentialCheckStatus status{ m_controller->addCredential(winrt::to_string(txtName.Text()), winrt::to_string(txtUrl.Text()), winrt::to_string(txtUsername.Text()), winrt::to_string(txtPassword.Password())) };
                ContentDialog errorDialog;
                errorDialog.Title(winrt::box_value(winrt::to_hstring(_("Error"))));
                errorDialog.CloseButtonText(winrt::to_hstring(_("OK")));
                errorDialog.DefaultButton(ContentDialogButton::Close);
                errorDialog.RequestedTheme(RequestedTheme());
                errorDialog.XamlRoot(XamlRoot());
                switch(status)
                {
                case CredentialCheckStatus::EmptyName:
                    errorDialog.Content(winrt::box_value(winrt::to_hstring(_("The credential name cannot be empty."))));
                    co_await errorDialog.ShowAsync();
                    break;
                case CredentialCheckStatus::EmptyUsernamePassword:
                    errorDialog.Content(winrt::box_value(winrt::to_hstring(_("Both the username and password cannot be empty."))));
                    co_await errorDialog.ShowAsync();
                    break;
                case CredentialCheckStatus::InvalidUri:
                    errorDialog.Content(winrt::box_value(winrt::to_hstring(_("The provided url is invalid."))));
                    co_await errorDialog.ShowAsync();
                    break;
                case CredentialCheckStatus::ExistingName:
                    errorDialog.Content(winrt::box_value(winrt::to_hstring(_("A credential with this name already exists."))));
                    co_await errorDialog.ShowAsync();
                    break;
                case CredentialCheckStatus::DatabaseError:
                    errorDialog.Content(winrt::box_value(winrt::to_hstring(_("There was an unknown error adding the credential to the keyring."))));
                    co_await errorDialog.ShowAsync();
                    break;
                default:
                    ReloadCredentials();
                    co_return;
                }
            }
            else
            {
                co_return;
            }
        }
    }

    void KeyringPage::ReloadCredentials()
    {
        ListCredentials().Children().Clear();
        std::vector<Credential> credentials{ m_controller->getCredentials() };
        for(const Credential& credential : credentials)
        {
            FontIcon icnEdit;
            icnEdit.FontFamily(WinUIHelpers::LookupAppResource<Microsoft::UI::Xaml::Media::FontFamily>(L"SymbolThemeFontFamily"));
            icnEdit.Glyph(L"\uE70F");
            Button btnEdit;
            btnEdit.Content(icnEdit);
            ToolTipService::SetToolTip(btnEdit, winrt::box_value(winrt::to_hstring(_("Edit"))));
            btnEdit.Click([this, credential](const IInspectable&, const RoutedEventArgs&) -> Windows::Foundation::IAsyncAction
            {
                co_await EditCredential(credential);
            });
            FontIcon icnDelete;
            icnDelete.FontFamily(WinUIHelpers::LookupAppResource<Microsoft::UI::Xaml::Media::FontFamily>(L"SymbolThemeFontFamily"));
            icnDelete.Glyph(L"\uE74D");
            Button btnDelete;
            btnDelete.Content(icnDelete);
            ToolTipService::SetToolTip(btnDelete, winrt::box_value(winrt::to_hstring(_("Delete"))));
            btnDelete.Click([this, credential](const IInspectable&, const RoutedEventArgs&) -> Windows::Foundation::IAsyncAction
            {
                co_await DeleteCredential(credential);
            });
            StackPanel panel;
            panel.Orientation(Orientation::Horizontal);
            panel.Spacing(6);
            panel.Children().Append(btnEdit);
            panel.Children().Append(btnDelete);
            Controls::SettingsRow row{ winrt::make<Controls::implementation::SettingsRow>() };
            row.Title(winrt::to_hstring(credential.getName()));
            row.Description(winrt::to_hstring(credential.getUri()));
            row.Child(panel);
            ListCredentials().Children().Append(row);
        }
        ViewStack().CurrentPageIndex(credentials.size() > 0 ? 1 : 0);
    }

    Windows::Foundation::IAsyncAction KeyringPage::EditCredential(const Credential& credential)
    {
        TextBox txtName;
        txtName.IsReadOnly(true);
        txtName.Header(winrt::box_value(winrt::to_hstring(_("Name"))));
        txtName.Text(winrt::to_hstring(credential.getName()));
        TextBox txtUrl;
        txtUrl.Header(winrt::box_value(winrt::to_hstring(_("URL"))));
        txtUrl.PlaceholderText(winrt::to_hstring(_("Enter url here")));
        txtUrl.Text(winrt::to_hstring(credential.getUri()));
        TextBox txtUsername;
        txtUsername.Header(winrt::box_value(winrt::to_hstring(_("Username"))));
        txtUsername.PlaceholderText(winrt::to_hstring(_("Enter username here")));
        txtUsername.Text(winrt::to_hstring(credential.getUsername()));
        PasswordBox txtPassword;
        txtPassword.Header(winrt::box_value(winrt::to_hstring(_("Password"))));
        txtPassword.PlaceholderText(winrt::to_hstring(_("Enter password here")));
        txtPassword.Password(winrt::to_hstring(credential.getPassword()));
        StackPanel panel;
        panel.Orientation(Orientation::Vertical);
        panel.Spacing(12);
        panel.Children().Append(txtName);
        panel.Children().Append(txtUrl);
        panel.Children().Append(txtUsername);
        panel.Children().Append(txtPassword);
        ContentDialog dialog;
        dialog.Title(winrt::box_value(winrt::to_hstring(_("Edit Credential"))));
        dialog.Content(panel);
        dialog.PrimaryButtonText(winrt::to_hstring(_("Save")));
        dialog.SecondaryButtonText(winrt::to_hstring(_("Delete")));
        dialog.CloseButtonText(winrt::to_hstring(_("Cancel")));
        dialog.DefaultButton(ContentDialogButton::Primary);
        dialog.RequestedTheme(RequestedTheme());
        dialog.XamlRoot(XamlRoot());
        while(true)
        {
            ContentDialogResult res{ co_await dialog.ShowAsync() };
            if(res == ContentDialogResult::Primary)
            {
                CredentialCheckStatus status{ m_controller->updateCredential(winrt::to_string(txtName.Text()), winrt::to_string(txtUrl.Text()), winrt::to_string(txtUsername.Text()), winrt::to_string(txtPassword.Password())) };
                ContentDialog errorDialog;
                errorDialog.Title(winrt::box_value(winrt::to_hstring(_("Error"))));
                errorDialog.CloseButtonText(winrt::to_hstring(_("OK")));
                errorDialog.DefaultButton(ContentDialogButton::Close);
                errorDialog.RequestedTheme(RequestedTheme());
                errorDialog.XamlRoot(XamlRoot());
                switch(status)
                {
                case CredentialCheckStatus::EmptyUsernamePassword:
                    errorDialog.Content(winrt::box_value(winrt::to_hstring(_("Both the username and password cannot be empty."))));
                    co_await errorDialog.ShowAsync();
                    break;
                case CredentialCheckStatus::InvalidUri:
                    errorDialog.Content(winrt::box_value(winrt::to_hstring(_("The provided url is invalid."))));
                    co_await errorDialog.ShowAsync();
                    break;
                case CredentialCheckStatus::DatabaseError:
                    errorDialog.Content(winrt::box_value(winrt::to_hstring(_("There was an unknown error adding the credential to the keyring."))));
                    co_await errorDialog.ShowAsync();
                    break;
                default:
                    ReloadCredentials();
                    co_return;
                }
            }
            else if(res == ContentDialogResult::Secondary)
            {
                co_await DeleteCredential(credential);
            }
            else
            {
                co_return;
            }
        }
    }

    Windows::Foundation::IAsyncAction KeyringPage::DeleteCredential(const Credential& credential)
    {
        ContentDialog dialog;
        dialog.Title(winrt::box_value(winrt::to_hstring(_("Delete Credential"))));
        dialog.Content(winrt::box_value(winrt::to_hstring(_("Are you sure you want to delete this credential?"))));
        dialog.CloseButtonText(winrt::to_hstring(_("Cancel")));
        dialog.PrimaryButtonText(winrt::to_hstring(_("Delete")));
        dialog.DefaultButton(ContentDialogButton::Primary);
        dialog.RequestedTheme(RequestedTheme());
        dialog.XamlRoot(XamlRoot());
        ContentDialogResult res{ co_await dialog.ShowAsync() };
        if(res == ContentDialogResult::Primary)
        {
            m_controller->deleteCredential(credential.getName());
            ReloadCredentials();
        }
    }
}
