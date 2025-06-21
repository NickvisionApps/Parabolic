#include "Controls/ViewStack.xaml.h"
#if __has_include("Controls/ViewStack.g.cpp")
#include "Controls/ViewStack.g.cpp"
#endif

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Windows::Foundation::Collections;

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::implementation
{
    DependencyProperty ViewStack::m_pagesProperty = DependencyProperty::Register(L"Pages", winrt::xaml_typename<IObservableVector<IInspectable>>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::ViewStack>(), PropertyMetadata{ nullptr });

    ViewStack::ViewStack()
        : m_currentPageIndex{ -1 }
    {
        InitializeComponent();
        Pages(winrt::single_threaded_observable_vector<IInspectable>());
    }

    const DependencyProperty& ViewStack::PagesProperty()
    {
        return m_pagesProperty;
    }

    int ViewStack::CurrentPageIndex() const
    {
        return m_currentPageIndex;
    }

    void ViewStack::CurrentPageIndex(int index)
    {
        if(index < 0 || index >= Pages().Size())
        {
            return;
        }
        m_currentPageIndex = index;
        Content(Pages().GetAt(m_currentPageIndex).as<UIElement>());
    }

    IObservableVector<IInspectable> ViewStack::Pages() const
    {
        return winrt::unbox_value<IObservableVector<IInspectable>>(GetValue(m_pagesProperty));
    }

    void ViewStack::Pages(const IObservableVector<IInspectable>& value)
    {
        SetValue(m_pagesProperty, winrt::box_value(value));
    }
}
