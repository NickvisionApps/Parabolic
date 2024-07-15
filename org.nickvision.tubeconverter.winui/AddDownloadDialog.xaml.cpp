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
        CloseButtonText(winrt::to_hstring(_("Cancel")));
        DefaultButton(ContentDialogButton::Primary);
        ViewStack().CurrentPage(L"Validate");
    }

    void AddDownloadDialog::SetController(const std::shared_ptr<AddDownloadDialogController>& controller, HWND hwnd)
    {
        m_controller = controller;
        m_hwnd = hwnd;
        //Load
    }
}
