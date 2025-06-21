#include "Controls/TitleBar.xaml.h"
#if __has_include("Controls/TitleBar.g.cpp")
#include "Controls/TitleBar.g.cpp"
#endif
#include <vector>
#include <libnick/localization/gettext.h>

using namespace winrt::Microsoft::UI;
using namespace winrt::Microsoft::UI::Input;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Data;
using namespace winrt::Microsoft::UI::Xaml::Media;
using namespace winrt::Microsoft::UI::Windowing;
using namespace winrt::Windows::Graphics;

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::implementation
{
    DependencyProperty TitleBar::m_titleProperty = DependencyProperty::Register(L"Title", winrt::xaml_typename<winrt::hstring>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::TitleBar>(), PropertyMetadata{ winrt::box_value(L""), PropertyChangedCallback{ &TitleBar::OnPropertyChanged } });
    DependencyProperty TitleBar::m_subtitleProperty = DependencyProperty::Register(L"Subtitle", winrt::xaml_typename<winrt::hstring>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::TitleBar>(), PropertyMetadata{ winrt::box_value(L""), PropertyChangedCallback{ &TitleBar::OnPropertyChanged } });
    DependencyProperty TitleBar::m_isActivatedProperty = DependencyProperty::Register(L"IsActivated", winrt::xaml_typename<bool>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::TitleBar>(), PropertyMetadata{ winrt::box_value(true), PropertyChangedCallback{ &TitleBar::OnPropertyChanged } });
    DependencyProperty TitleBar::m_searchVisibilityProperty = DependencyProperty::Register(L"SearchVisibility", winrt::xaml_typename<Microsoft::UI::Xaml::Visibility>(), winrt::xaml_typename<Nickvision::TubeConverter::WinUI::Controls::TitleBar>(), PropertyMetadata{ winrt::box_value(Visibility::Collapsed), PropertyChangedCallback{ &TitleBar::OnPropertyChanged } });

    TitleBar::TitleBar()
        : m_appWindow{ nullptr },
        m_loaded{ false },
        m_isActivated{ true }
    {
        InitializeComponent();
    }

    const DependencyProperty& TitleBar::TitleProperty()
    {
        return m_titleProperty;
    }

    const DependencyProperty& TitleBar::SubtitleProperty()
    {
        return m_subtitleProperty;
    }

    const DependencyProperty& TitleBar::IsActivatedProperty()
    {
        return m_isActivatedProperty;
    }

    const DependencyProperty& TitleBar::SearchVisibilityProperty()
    {
        return m_searchVisibilityProperty;
    }

    void TitleBar::OnPropertyChanged(const DependencyObject& d, const DependencyPropertyChangedEventArgs& args)
    {
        if(Nickvision::TubeConverter::WinUI::Controls::TitleBar titleBar{ d.try_as<Nickvision::TubeConverter::WinUI::Controls::TitleBar>() })
        {
            TitleBar* ptr{ winrt::get_self<TitleBar>(titleBar) };
            if(args.Property() == m_titleProperty)
            {
                ptr->m_propertyChangedEvent(*ptr, PropertyChangedEventArgs{ L"Title" });
            }
            else if(args.Property() == m_subtitleProperty)
            {
                ptr->m_propertyChangedEvent(*ptr, PropertyChangedEventArgs{ L"Subtitle" });
            }
            else if(args.Property() == m_isActivatedProperty)
            {
                ptr->m_propertyChangedEvent(*ptr, PropertyChangedEventArgs{ L"IsActivated" });
            }
            else if(args.Property() == m_searchVisibilityProperty)
            {
                ptr->m_propertyChangedEvent(*ptr, PropertyChangedEventArgs{ L"SearchVisibility" });
            }
        }
    }

    winrt::hstring TitleBar::Title() const
    {
        return winrt::unbox_value<winrt::hstring>(GetValue(m_titleProperty));
    }

    void TitleBar::Title(const winrt::hstring& title)
    {
        SetValue(m_titleProperty, winrt::box_value(title));
        m_propertyChangedEvent(*this, PropertyChangedEventArgs{ L"Title" });
        if(m_appWindow)
        {
            m_appWindow.Title(title);
        }
    }

    winrt::hstring TitleBar::Subtitle() const
    {
        return winrt::unbox_value<winrt::hstring>(GetValue(m_subtitleProperty));
    }

    void TitleBar::Subtitle(const winrt::hstring& subtitle)
    {
        SetValue(m_subtitleProperty, winrt::box_value(subtitle));
        m_propertyChangedEvent(*this, PropertyChangedEventArgs{ L"Subtitle" });
    }

    bool TitleBar::IsActivated() const
    {
        return winrt::unbox_value<bool>(GetValue(m_isActivatedProperty));
    }

    void TitleBar::IsActivated(bool activated)
    {
        SetValue(m_isActivatedProperty, winrt::box_value(activated));
        m_propertyChangedEvent(*this, PropertyChangedEventArgs{ L"IsActivated" });
        if(m_appWindow)
        {
            SolidColorBrush scb{ nullptr };
            if(activated)
            {
                switch(ActualTheme())
                {
                case ElementTheme::Light:
                    scb = SolidColorBrush(Colors::Black());
                    break;
                case ElementTheme::Dark:
                    scb = SolidColorBrush(Colors::White());
                    break;
                }
            }
            else
            {
                scb = SolidColorBrush(Colors::Gray());
            }
            LblTitle().Foreground(scb);
            m_appWindow.TitleBar().ButtonForegroundColor(scb.Color());
            m_appWindow.TitleBar().ButtonInactiveForegroundColor(scb.Color());
        }
    }

    Microsoft::UI::Xaml::Visibility TitleBar::SearchVisibility() const
    {
        return winrt::unbox_value<Microsoft::UI::Xaml::Visibility>(GetValue(m_searchVisibilityProperty));
    }

    void TitleBar::SearchVisibility(const Microsoft::UI::Xaml::Visibility& visibility)
    {
        SetValue(m_searchVisibilityProperty, winrt::box_value(visibility));
        m_propertyChangedEvent(*this, PropertyChangedEventArgs{ L"SearchVisibility" });
        SetDragRegion();
    }

    Microsoft::UI::Windowing::AppWindow TitleBar::AppWindow()
    {
        return m_appWindow;
    }

    void TitleBar::AppWindow(const Microsoft::UI::Windowing::AppWindow& appWindow)
    {
        m_appWindow = appWindow;
        m_appWindow.TitleBar().ExtendsContentIntoTitleBar(true);
        m_appWindow.TitleBar().PreferredHeightOption(TitleBarHeightOption::Tall);
        m_appWindow.TitleBar().ButtonBackgroundColor(Colors::Transparent());
        m_appWindow.TitleBar().ButtonInactiveBackgroundColor(Colors::Transparent());
        m_appWindow.Title(Title());
        m_appWindow.SetIcon(L"resources\\org.nickvision.application.ico");
        SetDragRegion();
    }

    winrt::event_token TitleBar::SearchChanged(const Windows::Foundation::TypedEventHandler<AutoSuggestBox, AutoSuggestBoxTextChangedEventArgs>& handler)
    {
        return m_searchChangedEvent.add(handler);
    }

    void TitleBar::SearchChanged(const winrt::event_token& token)
    {
        m_searchChangedEvent.remove(token);
    }

    winrt::event_token TitleBar::PropertyChanged(const PropertyChangedEventHandler& handler)
    {
        return m_propertyChangedEvent.add(handler);
    }

    void TitleBar::PropertyChanged(const winrt::event_token& token)
    {
        m_propertyChangedEvent.remove(token);
    }

    void TitleBar::OnLoaded(const IInspectable& sender, const RoutedEventArgs& args)
    {
        if(m_loaded)
        {
            return;
        }
        m_loaded = true;
        AsbSearch().PlaceholderText(winrt::to_hstring(_("Search")));
        SetDragRegion();
    }

    void TitleBar::OnSizeChanged(const IInspectable& sender, const SizeChangedEventArgs& args)
    {
        SetDragRegion();
    }

    void TitleBar::OnThemeChanged(const FrameworkElement& sender, const IInspectable& args)
    {
        switch(MainGrid().ActualTheme())
        {
        case ElementTheme::Light:
            LblTitle().Foreground(SolidColorBrush(Colors::Black()));
            m_appWindow.TitleBar().ButtonForegroundColor(Colors::Black());
            m_appWindow.TitleBar().ButtonInactiveForegroundColor(Colors::Black());
            break;
        case ElementTheme::Dark:
            LblTitle().Foreground(SolidColorBrush(Colors::White()));
            m_appWindow.TitleBar().ButtonForegroundColor(Colors::White());
            m_appWindow.TitleBar().ButtonInactiveForegroundColor(Colors::White());
            break;
        default:
            break;
        }
        IsActivated(IsActivated());
    }

    void TitleBar::OnSearchTextChanged(const AutoSuggestBox& sender, const AutoSuggestBoxTextChangedEventArgs& args)
    {
        m_searchChangedEvent(sender, args);
    }

    void TitleBar::SetDragRegion()
    {
        if(!m_appWindow || !m_loaded)
        {
            return;
        }
        double scaleAdjustment{ XamlRoot().RasterizationScale() };
        RightPaddingColumn().Width({ m_appWindow.TitleBar().RightInset() / scaleAdjustment });
        LeftPaddingColumn().Width({ m_appWindow.TitleBar().LeftInset() / scaleAdjustment });
        GeneralTransform transformSearch{ AsbSearch().TransformToVisual(nullptr) };
        Windows::Foundation::Rect boundsSearch{ transformSearch.TransformBounds({ 0, 0, static_cast<float>(AsbSearch().ActualWidth()), static_cast<float>(AsbSearch().ActualHeight()) }) };
        RectInt32 searchBoxRect{ static_cast<int>(std::round(boundsSearch.X * scaleAdjustment)), static_cast<int>(std::round(boundsSearch.Y * scaleAdjustment)), static_cast<int>(std::round(boundsSearch.Width * scaleAdjustment)), static_cast<int>(std::round(boundsSearch.Height * scaleAdjustment)) };
        std::vector<RectInt32> rects;
        if(SearchVisibility() == Visibility::Visible)
        {
            rects.push_back(searchBoxRect);
        }
        InputNonClientPointerSource nonClientInputSrc{ InputNonClientPointerSource::GetForWindowId(m_appWindow.Id()) };
        nonClientInputSrc.SetRegionRects(NonClientRegionKind::Passthrough, rects);
    }
}