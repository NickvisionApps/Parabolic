#include "Views/CredentialDialog.xaml.h"
#if __has_include("Views/CredentialDialog.g.cpp")
#include "Views/CredentialDialog.g.cpp"
#endif
#include <libnick/localization/gettext.h>

using namespace ::Nickvision::TubeConverter::Shared::Controllers;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;


namespace winrt::Nickvision::TubeConverter::WinUI::Views::implementation
{
    CredentialDialog::CredentialDialog()
    {
        InitializeComponent();
    }

    void CredentialDialog::Controller(const std::shared_ptr<CredentialDialogController>& controller)
    {
        m_controller = controller;
        //Localize Strings
        Title(winrt::box_value(winrt::to_hstring(_("Credential Needed"))));
        PrimaryButtonText(winrt::to_hstring(_("Use")));
        CloseButtonText(winrt::to_hstring(_("Cancel")));
        DefaultButton(ContentDialogButton::Primary);
        LblDescription().Text(winrt::to_hstring(_f("{} needs a credential to download. Please select or enter one to use.", controller->getUrl())));
        CmbCredential().Header(winrt::box_value(winrt::to_hstring(_("Credential"))));
        TxtUsername().Header(winrt::box_value(winrt::to_hstring(_("Username"))));
        TxtUsername().PlaceholderText(winrt::to_hstring(_("Enter username here")));
        TxtPassword().Header(winrt::box_value(winrt::to_hstring(_("Password"))));
        TxtPassword().PlaceholderText(winrt::to_hstring(_("Enter password here")));
        //Load
        std::vector<std::string> credentialNames{ m_controller->getKeyringCredentialNames() };
        credentialNames.insert(credentialNames.begin(), _("Use manual credential"));
        for(const std::string& name : credentialNames)
        {
            CmbCredential().Items().Append(winrt::box_value(winrt::to_hstring(name)));
        }
        CmbCredential().SelectedIndex(0);
    }

    Windows::Foundation::IAsyncOperation<ContentDialogResult> CredentialDialog::ShowAsync()
    {
        while(true)
        {
            ContentDialogResult res{ co_await base_type::ShowAsync() };
            if(res == ContentDialogResult::Primary)
            {
                if(CmbCredential().SelectedIndex() == 0)
                {
                    if(TxtUsername().Text().empty() && TxtPassword().Text().empty())
                    {
                        ContentDialog dialog;
                        dialog.Title(winrt::box_value(winrt::to_hstring(_("Error"))));
                        dialog.Content(winrt::box_value(winrt::to_hstring(_("Both the username and password cannot be empty."))));
                        dialog.CloseButtonText(winrt::to_hstring(_("OK")));
                        dialog.DefaultButton(ContentDialogButton::Close);
                        dialog.RequestedTheme(RequestedTheme());
                        dialog.XamlRoot(XamlRoot());
                        co_await dialog.ShowAsync();
                    }
                    else
                    {
                        m_controller->use(winrt::to_string(TxtUsername().Text()), winrt::to_string(TxtPassword().Text()));
                        co_return res;
                    }
                }
                else
                {
                    m_controller->use(CmbCredential().SelectedIndex() - 1);
                    co_return res;
                }
            }
            else
            {
                co_return res;
            }
        }
    }

    void CredentialDialog::OnCmbCredentialSelectionChanged(const IInspectable& sender, const SelectionChangedEventArgs& args)
    {
        TxtUsername().Visibility(CmbCredential().SelectedIndex() == 0 ? Visibility::Visible : Visibility::Collapsed);
        TxtUsername().Text(L"");
        TxtPassword().Visibility(CmbCredential().SelectedIndex() == 0 ? Visibility::Visible : Visibility::Collapsed);
        TxtPassword().Text(L"");
    }
}
