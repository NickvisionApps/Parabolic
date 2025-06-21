#include "Controls/StatusPage.xaml.h"
#if __has_include("Controls/StatusPage.g.cpp")
#include "Controls/StatusPage.g.cpp"
#endif
#include "Helpers/WinUIHelpers.h"

using namespace ::Nickvision::TubeConverter::WinUI::Helpers;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Data;
using namespace winrt::Windows::Foundation::Collections;

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::implementation
{
    DependencyProperty StatusPage::m_glyphProperty = DependencyProperty::Register(L"Glyph", winrt::xaml_typename<winrt::hstring>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::StatusPage>(), PropertyMetadata{ winrt::box_value(L""), PropertyChangedCallback{ &StatusPage::OnPropertyChanged } });
    DependencyProperty StatusPage::m_useAppIconProperty = DependencyProperty::Register(L"UseAppIcon", winrt::xaml_typename<bool>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::StatusPage>(), PropertyMetadata{ winrt::box_value(false), PropertyChangedCallback{ &StatusPage::OnPropertyChanged } });
    DependencyProperty StatusPage::m_titleProperty = DependencyProperty::Register(L"Title", winrt::xaml_typename<winrt::hstring>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::StatusPage>(), PropertyMetadata{ winrt::box_value(L""), PropertyChangedCallback{ &StatusPage::OnPropertyChanged } });
    DependencyProperty StatusPage::m_descriptionProperty = DependencyProperty::Register(L"Description", winrt::xaml_typename<winrt::hstring>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::StatusPage>(), PropertyMetadata{ winrt::box_value(L""), PropertyChangedCallback{ &StatusPage::OnPropertyChanged } });
    DependencyProperty StatusPage::m_childProperty = DependencyProperty::Register(L"Child", winrt::xaml_typename<IInspectable>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::StatusPage>(), PropertyMetadata{ nullptr, PropertyChangedCallback{ &StatusPage::OnPropertyChanged } });
    DependencyProperty StatusPage::m_isCompactProperty = DependencyProperty::Register(L"IsCompact", winrt::xaml_typename<bool>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::StatusPage>(), PropertyMetadata{ winrt::box_value(false), PropertyChangedCallback{ &StatusPage::OnPropertyChanged } });

    StatusPage::StatusPage()
    {
        InitializeComponent();
        Title(L"");
        Description(L"");
        Child(nullptr);
        IsCompact(false);
    }

    const DependencyProperty& StatusPage::GlyphProperty()
    {
        return m_glyphProperty;
    }

    const DependencyProperty& StatusPage::UseAppIconProperty()
    {
        return m_useAppIconProperty;
    }

    const DependencyProperty& StatusPage::TitleProperty()
    {
        return m_titleProperty;
    }

    const DependencyProperty& StatusPage::DescriptionProperty()
    {
        return m_descriptionProperty;
    }

    const DependencyProperty& StatusPage::ChildProperty()
    {
        return m_childProperty;
    }

    const DependencyProperty& StatusPage::IsCompactProperty()
    {
        return m_isCompactProperty;
    }

    void StatusPage::OnPropertyChanged(const DependencyObject& d, const DependencyPropertyChangedEventArgs& args)
    {
        if(Nickvision::TubeConverter::WinUI::Controls::StatusPage statusPage{ d.try_as<Nickvision::TubeConverter::WinUI::Controls::StatusPage>() })
        {
            StatusPage* ptr{ winrt::get_self<StatusPage>(statusPage) };
            if(args.Property() == m_glyphProperty)
            {
                ptr->m_propertyChanged(*ptr, PropertyChangedEventArgs{ L"Glyph" });
                ptr->m_propertyChanged(*ptr, PropertyChangedEventArgs{ L"GlyphIconVisibility" });
            }
            else if(args.Property() == m_useAppIconProperty)
            {
                ptr->m_propertyChanged(*ptr, PropertyChangedEventArgs{ L"UseAppIcon" });
                ptr->m_propertyChanged(*ptr, PropertyChangedEventArgs{ L"AppIconVisibility" });
            }
            else if(args.Property() == m_titleProperty)
            {
                ptr->m_propertyChanged(*ptr, PropertyChangedEventArgs{ L"Title" });
                ptr->m_propertyChanged(*ptr, PropertyChangedEventArgs{ L"TitleVisibility" });
            }
            else if(args.Property() == m_descriptionProperty)
            {
                ptr->m_propertyChanged(*ptr, PropertyChangedEventArgs{ L"Description" });
                ptr->m_propertyChanged(*ptr, PropertyChangedEventArgs{ L"DescriptionVisibility" });
            }
            else if(args.Property() == m_childProperty)
            {
                ptr->m_propertyChanged(*ptr, PropertyChangedEventArgs{ L"Child" });
                ptr->m_propertyChanged(*ptr, PropertyChangedEventArgs{ L"ChildVisibility" });
            }
            else if(args.Property() == m_isCompactProperty)
            {
                ptr->m_propertyChanged(*ptr, PropertyChangedEventArgs{ L"IsCompact" });
                ptr->m_propertyChanged(*ptr, PropertyChangedEventArgs{ L"CompactSpacing" });
                ptr->m_propertyChanged(*ptr, PropertyChangedEventArgs{ L"IconSize" });
                ptr->m_propertyChanged(*ptr, PropertyChangedEventArgs{ L"IconWidth" });
                ptr->m_propertyChanged(*ptr, PropertyChangedEventArgs{ L"IconHeight" });
                ptr->m_propertyChanged(*ptr, PropertyChangedEventArgs{ L"TitleStyle" });
            }
        }
    }

    winrt::hstring StatusPage::Glyph() const
    {
        return winrt::unbox_value<winrt::hstring>(GetValue(m_glyphProperty));
    }

    void StatusPage::Glyph(const winrt::hstring& glyph)
    {
        SetValue(m_glyphProperty, winrt::box_value(glyph));
        m_propertyChanged(*this, PropertyChangedEventArgs{ L"Glyph" });
        m_propertyChanged(*this, PropertyChangedEventArgs{ L"GlyphIconVisibility" });
    }

    bool StatusPage::UseAppIcon() const
    {
        return winrt::unbox_value<bool>(GetValue(m_useAppIconProperty));
    }

    void StatusPage::UseAppIcon(bool useAppIcon)
    {
        SetValue(m_useAppIconProperty, winrt::box_value(useAppIcon));
        m_propertyChanged(*this, PropertyChangedEventArgs{ L"UseAppIcon" });
        m_propertyChanged(*this, PropertyChangedEventArgs{ L"AppIconVisibility" });
    }

    winrt::hstring StatusPage::Title() const
    {
        return winrt::unbox_value<winrt::hstring>(GetValue(m_titleProperty));
    }

    void StatusPage::Title(const winrt::hstring& title)
    {
        SetValue(m_titleProperty, winrt::box_value(title));
        m_propertyChanged(*this, PropertyChangedEventArgs{ L"Title" });
        m_propertyChanged(*this, PropertyChangedEventArgs{ L"TitleVisibility" });
    }

    winrt::hstring StatusPage::Description() const
    {
        return winrt::unbox_value<winrt::hstring>(GetValue(m_descriptionProperty));
    }

    void StatusPage::Description(const winrt::hstring& description)
    {
        SetValue(m_descriptionProperty, winrt::box_value(description));
        m_propertyChanged(*this, PropertyChangedEventArgs{ L"Description" });
        m_propertyChanged(*this, PropertyChangedEventArgs{ L"DescriptionVisibility" });
    }

    IInspectable StatusPage::Child() const
    {
        return GetValue(m_childProperty);
    }

    void StatusPage::Child(const IInspectable& child)
    {
        SetValue(m_childProperty, child);
        m_propertyChanged(*this, PropertyChangedEventArgs{ L"Child" });
        m_propertyChanged(*this, PropertyChangedEventArgs{ L"ChildVisibility" });
    }

    bool StatusPage::IsCompact() const
    {
        return winrt::unbox_value<bool>(GetValue(m_isCompactProperty));
    }

    void StatusPage::IsCompact(bool isCompact)
    {
        SetValue(m_isCompactProperty, winrt::box_value(isCompact));
        m_propertyChanged(*this, PropertyChangedEventArgs{ L"IsCompact" });
        m_propertyChanged(*this, PropertyChangedEventArgs{ L"CompactSpacing" });
        m_propertyChanged(*this, PropertyChangedEventArgs{ L"IconSize" });
        m_propertyChanged(*this, PropertyChangedEventArgs{ L"IconWidth" });
        m_propertyChanged(*this, PropertyChangedEventArgs{ L"IconHeight" });
        m_propertyChanged(*this, PropertyChangedEventArgs{ L"TitleStyle" });
    }

    Visibility StatusPage::GlyphIconVisibility() const
    {
        return UseAppIcon() ? Visibility::Collapsed : Visibility::Visible;
    }

    Visibility StatusPage::AppIconVisibility() const
    {
        return UseAppIcon() ? Visibility::Visible : Visibility::Collapsed;
    }

    Visibility StatusPage::TitleVisibility() const
    {
        return Title().empty() ? Visibility::Collapsed : Visibility::Visible;
    }

    Visibility StatusPage::DescriptionVisibility() const
    {
        return Description().empty() ? Visibility::Collapsed : Visibility::Visible;
    }

    Visibility StatusPage::ChildVisibility() const
    {
        return Child() ? Visibility::Visible : Visibility::Collapsed;
    }

    double StatusPage::CompactSpacing() const
    {
        return IsCompact() ? 6 : 12;
    }

    double StatusPage::IconSize() const
    {
        return IsCompact() ? 30 : 60;
    }

    double StatusPage::IconWidth() const
    {
        return IsCompact() ? 64 : 128;
    }

    double StatusPage::IconHeight() const
    {
        return IsCompact() ? 64 : 128;
    }

    Style StatusPage::TitleStyle() const
    {
        return WinUIHelpers::LookupAppResource<Microsoft::UI::Xaml::Style>(IsCompact() ? L"SubtitleTextBlockStyle" : L"TitleTextBlockStyle");
    }

    winrt::event_token StatusPage::PropertyChanged(const PropertyChangedEventHandler& handler)
    {
        return m_propertyChanged.add(handler);
    }

    void StatusPage::PropertyChanged(const winrt::event_token& token)
    {
        m_propertyChanged.remove(token);
    }
}
