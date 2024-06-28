#include "Controls/ViewStackPage.xaml.h"
#if __has_include("Controls/ViewStackPage.g.cpp")
#include "Controls/ViewStackPage.g.cpp"
#endif

using namespace winrt::Microsoft::UI::Xaml;

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::implementation 
{
    DependencyProperty ViewStackPage::m_pageNameProperty = DependencyProperty::Register(L"PageName", winrt::xaml_typename<winrt::hstring>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::ViewStackPage>(), PropertyMetadata{ winrt::box_value(L"") });

    ViewStackPage::ViewStackPage()
    {
        InitializeComponent();
    }

    winrt::hstring ViewStackPage::PageName() const
    {
        return winrt::unbox_value<winrt::hstring>(GetValue(m_pageNameProperty));
    }

    void ViewStackPage::PageName(const winrt::hstring& pageName)
    {
        SetValue(m_pageNameProperty, winrt::box_value(pageName));
    }

    const DependencyProperty& ViewStackPage::PageNameProperty()
    {
        return m_pageNameProperty;
    }
}