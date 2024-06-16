#include "Controls/ClickableSettingsRow.xaml.h"
#if __has_include("Controls/ClickableSettingsRow.g.cpp")
#include "Controls/ClickableSettingsRow.g.cpp"
#endif
#include "helpers/WinUI.h"

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Data;
using namespace winrt::Windows::Foundation::Collections;

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::implementation 
{
    DependencyProperty ClickableSettingsRow::m_glyphProperty = DependencyProperty::Register(L"Glyph", winrt::xaml_typename<winrt::hstring>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::ClickableSettingsRow>(), PropertyMetadata{ winrt::box_value(L""), PropertyChangedCallback{ &ClickableSettingsRow::OnPropertyChanged } });
    DependencyProperty ClickableSettingsRow::m_titleProperty = DependencyProperty::Register(L"Title", winrt::xaml_typename<winrt::hstring>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::ClickableSettingsRow>(), PropertyMetadata{ winrt::box_value(L""), PropertyChangedCallback{ &ClickableSettingsRow::OnPropertyChanged } });
    DependencyProperty ClickableSettingsRow::m_descriptionProperty = DependencyProperty::Register(L"Description", winrt::xaml_typename<winrt::hstring>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::ClickableSettingsRow>(), PropertyMetadata{ winrt::box_value(L""), PropertyChangedCallback{ &ClickableSettingsRow::OnPropertyChanged } });
    DependencyProperty ClickableSettingsRow::m_childProperty = DependencyProperty::Register(L"Child", winrt::xaml_typename<IInspectable>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::ClickableSettingsRow>(), PropertyMetadata{ nullptr, PropertyChangedCallback{ &ClickableSettingsRow::OnPropertyChanged } });
   
    ClickableSettingsRow::ClickableSettingsRow()
    {
        InitializeComponent();
        Title(L"");
        Description(L"");
        Glyph(L"");
    }
    
    winrt::hstring ClickableSettingsRow::Glyph() const
    {
        return winrt::unbox_value<winrt::hstring>(GetValue(m_glyphProperty));
    }

    void ClickableSettingsRow::Glyph(const winrt::hstring& glyph)
    {
        SetValue(m_glyphProperty, winrt::box_value(glyph));
        m_propertyChangedEvent(*this, PropertyChangedEventArgs{ L"Glyph" });
        m_propertyChangedEvent(*this, PropertyChangedEventArgs{ L"IconVisibility" });
    }

    winrt::hstring ClickableSettingsRow::Title() const
    {
        return winrt::unbox_value<winrt::hstring>(GetValue(m_titleProperty));
    }

    void ClickableSettingsRow::Title(const winrt::hstring& title)
    {
        SetValue(m_titleProperty, winrt::box_value(title));
        m_propertyChangedEvent(*this, PropertyChangedEventArgs{ L"Title" });
        m_propertyChangedEvent(*this, PropertyChangedEventArgs{ L"TitleVisibility" });
    }

    winrt::hstring ClickableSettingsRow::Description() const
    {
        return winrt::unbox_value<winrt::hstring>(GetValue(m_descriptionProperty));
    }

    void ClickableSettingsRow::Description(const winrt::hstring& description)
    {
        SetValue(m_descriptionProperty, winrt::box_value(description));
        m_propertyChangedEvent(*this, PropertyChangedEventArgs{ L"Description" });
        m_propertyChangedEvent(*this, PropertyChangedEventArgs{ L"DescriptionVisibility" });
    }

    IInspectable ClickableSettingsRow::Child() const
    {
        return GetValue(m_childProperty);
    }

    void ClickableSettingsRow::Child(const IInspectable& child)
    {
        SetValue(m_childProperty, child);
        m_propertyChangedEvent(*this, PropertyChangedEventArgs{ L"Child" });
    }

    Visibility ClickableSettingsRow::IconVisibility() const
    {
        return Glyph().empty() ? Visibility::Collapsed : Visibility::Visible;
    }

    Visibility ClickableSettingsRow::TitleVisibility() const
    {
        return Title().empty() ? Visibility::Collapsed : Visibility::Visible;
    }

    Visibility ClickableSettingsRow::DescriptionVisibility() const
    {
        return Description().empty() ? Visibility::Collapsed : Visibility::Visible;
    }

    winrt::event_token ClickableSettingsRow::PropertyChanged(const PropertyChangedEventHandler& handler)
    {
        return m_propertyChangedEvent.add(handler);
    }

    void ClickableSettingsRow::PropertyChanged(const winrt::event_token& token)
    {
        m_propertyChangedEvent.remove(token);
    }

    winrt::event_token ClickableSettingsRow::Clicked(const RoutedEventHandler& handler)
    {
        return BtnMain().Click(handler);
    }

    void ClickableSettingsRow::Clicked(const winrt::event_token& token)
    {
        BtnMain().Click(token);
    }

    const DependencyProperty& ClickableSettingsRow::GlyphProperty()
    {
        return m_glyphProperty;
    }

    const DependencyProperty& ClickableSettingsRow::TitleProperty()
    {
        return m_titleProperty;
    }

    const DependencyProperty& ClickableSettingsRow::DescriptionProperty()
    {
        return m_descriptionProperty;
    }

    const DependencyProperty& ClickableSettingsRow::ChildProperty()
    {
        return m_childProperty;
    }

    void ClickableSettingsRow::OnPropertyChanged(const DependencyObject& d, const DependencyPropertyChangedEventArgs& args)
    {
        if(Nickvision::TubeConverter::WinUI::Controls::ClickableSettingsRow settingsRow{ d.try_as<Nickvision::TubeConverter::WinUI::Controls::ClickableSettingsRow>() })
        {
            ClickableSettingsRow* ptr{ winrt::get_self<ClickableSettingsRow>(settingsRow) };
            if(args.Property() == m_glyphProperty)
            {
                ptr->m_propertyChangedEvent(*ptr, PropertyChangedEventArgs{ L"Glyph" });
                ptr->m_propertyChangedEvent(*ptr, PropertyChangedEventArgs{ L"IconVisibility" });
            }
            else if(args.Property() == m_titleProperty)
            {
                ptr->m_propertyChangedEvent(*ptr, PropertyChangedEventArgs{ L"Title" });
                ptr->m_propertyChangedEvent(*ptr, PropertyChangedEventArgs{ L"TitleVisibility" });
            }
            else if(args.Property() == m_descriptionProperty)
            {
                ptr->m_propertyChangedEvent(*ptr, PropertyChangedEventArgs{ L"Description" });
                ptr->m_propertyChangedEvent(*ptr, PropertyChangedEventArgs{ L"DescriptionVisibility" });
            }
            else if(args.Property() == m_childProperty)
            {
                ptr->m_propertyChangedEvent(*ptr, PropertyChangedEventArgs{ L"Child" });
            }
        }
    }
}