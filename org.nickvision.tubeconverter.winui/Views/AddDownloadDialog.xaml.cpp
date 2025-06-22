#include "Views/AddDownloadDialog.xaml.h"
#if __has_include("Views/AddDownloadDialog.g.cpp")
#include "Views/AddDownloadDialog.g.cpp"
#endif
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>

using namespace ::Nickvision::Helpers;
using namespace ::Nickvision::TubeConverter::Shared::Controllers;
using namespace winrt::Microsoft::UI::Xaml::Controls;

enum AddDownloadDialogPage
{
    Validate = 0,
    Loading,
    Single,
    Playlist
};

namespace winrt::Nickvision::TubeConverter::WinUI::Views::implementation
{
    AddDownloadDialog::AddDownloadDialog()
    {
        InitializeComponent();
    }

    void AddDownloadDialog::Controller(const std::shared_ptr<AddDownloadDialogController>& controller, const winrt::hstring& url)
    {
        m_controller = controller;
        //Register Events

        //Localize Strings
        Title(winrt::box_value(winrt::to_hstring(_("Add Download"))));
        CloseButtonText(winrt::to_hstring(_("Cancel")));
        NavViewValidateMedia().Text(winrt::to_hstring(_("Media")));
        NavViewValidateAuthentication().Text(winrt::to_hstring(_("Authentication")));
        RowMediaUrl().Title(winrt::to_hstring(_("Media URL")));
        TxtMediaUrl().PlaceholderText(winrt::to_hstring(_("Enter media url here")));
        ToolTipService::SetToolTip(BtnUseBatchFile(), winrt::box_value(winrt::to_hstring(_("Use Batch File"))));
        RowDownloadImmediately().Title(winrt::to_hstring(_("Download Immediately")));
        TglDownloadImmediately().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglDownloadImmediately().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowCredential().Title(winrt::to_hstring(_("Credential")));
        RowUsername().Title(winrt::to_hstring(_("Username")));
        TxtUsername().PlaceholderText(winrt::to_hstring(_("Enter username here")));
        RowPassword().Title(winrt::to_hstring(_("Password")));
        TxtPassword().PlaceholderText(winrt::to_hstring(_("Enter username here")));
        LblLoading().Text(winrt::to_hstring(_("This may take some time...")));
        //Load
        ViewStack().CurrentPageIndex(AddDownloadDialogPage::Validate);
        if(StringHelpers::isValidUrl(winrt::to_string(url)))
        {
            TxtMediaUrl().Text(url);
        }
        PrimaryButtonText(winrt::to_hstring(_("Validate")));
    }

    Windows::Foundation::IAsyncOperation<ContentDialogResult> AddDownloadDialog::ShowAsync()
    {
        ContentDialogResult res{ co_await base_type::ShowAsync() };
        if(res == ContentDialogResult::Primary)
        {
            CloseButtonText(L"");
            PrimaryButtonText(L"");
            ViewStack().CurrentPageIndex(AddDownloadDialogPage::Loading);
            res = co_await base_type::ShowAsync();
        }
        co_return res;
    }

    void AddDownloadDialog::OnNavViewValidateSelectionChanged(const SelectorBar& sender, const SelectorBarSelectionChangedEventArgs& args)
    {
        uint32_t index;
        if(sender.Items().IndexOf(sender.SelectedItem(), index))
        {
            ViewStackValidate().CurrentPageIndex(static_cast<int>(index));
        }
    }
}
