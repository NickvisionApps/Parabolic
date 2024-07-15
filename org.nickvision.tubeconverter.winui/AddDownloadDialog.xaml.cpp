#include "AddDownloadDialog.xaml.h"
#if __has_include("AddDownloadDialog.g.cpp")
#include "AddDownloadDialog.g.cpp"
#endif
#include <libnick/localization/gettext.h>

using namespace ::Nickvision::TubeConverter::Shared::Controllers;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;

namespace winrt::Nickvision::TubeConverter::WinUI::implementation 
{
    AddDownloadDialog::AddDownloadDialog()
    {
        InitializeComponent();
        //Localize Strings
        Title(winrt::box_value(winrt::to_hstring(_("Add Download"))));
        PrimaryButtonText(winrt::to_hstring(_("Validate")));
        SecondaryButtonText(winrt::to_hstring(_("Cancel")));
        DefaultButton(ContentDialogButton::Primary);
        RowUrl().Title(winrt::to_hstring(_("Media URL")));
        ViewStack().CurrentPage(L"Validate");
    }

    void AddDownloadDialog::SetController(const std::shared_ptr<AddDownloadDialogController>& controller, HWND hwnd)
    {
        m_controller = controller;
        m_hwnd = hwnd;
        //Load
    }

    Windows::Foundation::IAsyncOperation<ContentDialogResult> AddDownloadDialog::ShowAsync()
    {
        ContentDialogResult dialogResult{ co_await base_type::ShowAsync() };
        if(dialogResult == ContentDialogResult::Primary)
        {
            if(PrimaryButtonText() == winrt::to_hstring(_("Validate")))
            {
                
            }
            else if(PrimaryButtonText() == winrt::to_hstring(_("Download")))
            {
                
            }
        }
        co_return dialogResult;
    }

    void AddDownloadDialog::OnClosing(const ContentDialog& sender, const ContentDialogClosingEventArgs& args)
    {
        if(ViewStack().CurrentPage() == L"Spinner" && args.Result() == ContentDialogResult::None)
        {
            args.Cancel(true);
        }
    }
}
