#include "Controls/SettingsRow.xaml.h"
#if __has_include("Controls/SettingsRow.g.cpp")
#include "Controls/SettingsRow.g.cpp"
#endif
#include "Helpers/WinUIHelpers.h"

using namespace ::Nickvision::TubeConverter::WinUI::Helpers;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Data;
using namespace winrt::Windows::Foundation::Collections;

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::implementation
{
    DependencyProperty SettingsRow::m_glyphProperty = DependencyProperty::Register(L"Glyph", winrt::xaml_typename<winrt::hstring>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::SettingsRow>(), PropertyMetadata{ winrt::box_value(L""), PropertyChangedCallback{ &SettingsRow::OnPropertyChanged } });
    DependencyProperty SettingsRow::m_titleProperty = DependencyProperty::Register(L"Title", winrt::xaml_typename<winrt::hstring>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::SettingsRow>(), PropertyMetadata{ winrt::box_value(L""), PropertyChangedCallback{ &SettingsRow::OnPropertyChanged } });
    DependencyProperty SettingsRow::m_descriptionProperty = DependencyProperty::Register(L"Description", winrt::xaml_typename<winrt::hstring>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::SettingsRow>(), PropertyMetadata{ winrt::box_value(L""), PropertyChangedCallback{ &SettingsRow::OnPropertyChanged } });
    DependencyProperty SettingsRow::m_childProperty = DependencyProperty::Register(L"Child", winrt::xaml_typename<IInspectable>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::SettingsRow>(), PropertyMetadata{ nullptr, PropertyChangedCallback{ &SettingsRow::OnPropertyChanged } });

    SettingsRow::SettingsRow()
    {
        InitializeComponent();
        Title(L"");
        Description(L"");
        Glyph(L"");
    }

    const DependencyProperty& SettingsRow::GlyphProperty()
    {
        return m_glyphProperty;
    }

    const DependencyProperty& SettingsRow::TitleProperty()
    {
        return m_titleProperty;
    }

    const DependencyProperty& SettingsRow::DescriptionProperty()
    {
        return m_descriptionProperty;
    }

    const DependencyProperty& SettingsRow::ChildProperty()
    {
        return m_childProperty;
    }

    void SettingsRow::OnPropertyChanged(const DependencyObject& d, const DependencyPropertyChangedEventArgs& args)
    {
        if(Nickvision::TubeConverter::WinUI::Controls::SettingsRow settingsRow{ d.try_as<Nickvision::TubeConverter::WinUI::Controls::SettingsRow>() })
        {
            SettingsRow* ptr{ winrt::get_self<SettingsRow>(settingsRow) };
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

    winrt::hstring SettingsRow::Glyph() const
    {
        return winrt::unbox_value<winrt::hstring>(GetValue(m_glyphProperty));
    }

    void SettingsRow::Glyph(const winrt::hstring& glyph)
    {
        SetValue(m_glyphProperty, winrt::box_value(glyph));
        m_propertyChangedEvent(*this, PropertyChangedEventArgs{ L"Glyph" });
        m_propertyChangedEvent(*this, PropertyChangedEventArgs{ L"IconVisibility" });
    }

    winrt::hstring SettingsRow::Title() const
    {
        return winrt::unbox_value<winrt::hstring>(GetValue(m_titleProperty));
    }

    void SettingsRow::Title(const winrt::hstring& title)
    {
        SetValue(m_titleProperty, winrt::box_value(title));
        m_propertyChangedEvent(*this, PropertyChangedEventArgs{ L"Title" });
        m_propertyChangedEvent(*this, PropertyChangedEventArgs{ L"TitleVisibility" });
    }

    winrt::hstring SettingsRow::Description() const
    {
        return winrt::unbox_value<winrt::hstring>(GetValue(m_descriptionProperty));
    }

    void SettingsRow::Description(const winrt::hstring& description)
    {
        SetValue(m_descriptionProperty, winrt::box_value(description));
        m_propertyChangedEvent(*this, PropertyChangedEventArgs{ L"Description" });
        m_propertyChangedEvent(*this, PropertyChangedEventArgs{ L"DescriptionVisibility" });
    }

    IInspectable SettingsRow::Child() const
    {
        return GetValue(m_childProperty);
    }

    void SettingsRow::Child(const IInspectable& child)
    {
        SetValue(m_childProperty, child);
        m_propertyChangedEvent(*this, PropertyChangedEventArgs{ L"Child" });
    }

    Visibility SettingsRow::IconVisibility() const
    {
        return Glyph().empty() ? Visibility::Collapsed : Visibility::Visible;
    }

    Visibility SettingsRow::TitleVisibility() const
    {
        return Title().empty() ? Visibility::Collapsed : Visibility::Visible;
    }

    Visibility SettingsRow::DescriptionVisibility() const
    {
        return Description().empty() ? Visibility::Collapsed : Visibility::Visible;
    }

    winrt::event_token SettingsRow::PropertyChanged(const PropertyChangedEventHandler& handler)
    {
        return m_propertyChangedEvent.add(handler);
    }

    void SettingsRow::PropertyChanged(const winrt::event_token& token)
    {
        m_propertyChangedEvent.remove(token);
    }
}