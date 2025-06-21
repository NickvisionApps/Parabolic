#ifndef TITLEBAR_H
#define TITLEBAR_H

#include "pch.h"
#include "Controls/TitleBar.g.h"

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::implementation
{
    /**
     * @brief A modern titlebar control.
     */
    struct TitleBar : public TitleBarT<TitleBar>
    {
    public:
        /**
         * @brief Constructs a TitleBar.
         */
        TitleBar();
        /**
         * @brief Gets the titlebar's title dependency property.
         * @return The title dependency property
         */
        static const Microsoft::UI::Xaml::DependencyProperty& TitleProperty();
        /**
         * @brief Gets the titlebar's subtitle dependency property.
         * @return The subtitle dependency property
         */
        static const Microsoft::UI::Xaml::DependencyProperty& SubtitleProperty();
        /**
         * @brief Gets the titlebar's activated state dependency property.
         * @return The activated state dependency property
         */
        static const Microsoft::UI::Xaml::DependencyProperty& IsActivatedProperty();
        /**
         * @brief Gets the titlebar's search visibility dependency property.
         * @return The search visibility dependency property
         */
        static const Microsoft::UI::Xaml::DependencyProperty& SearchVisibilityProperty();
        /**
         * @brief Handles when a property of the row is changed.
         * @param d Microsoft::UI::Xaml::DependencyObject
         * @param args Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs
         */
        static void OnPropertyChanged(const Microsoft::UI::Xaml::DependencyObject& d, const Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs& args);
        /**
         * @brief Gets the title for the titlebar.
         * @return The titlebar title
         */
        winrt::hstring Title() const;
        /**
         * @brief Sets the title for the titlebar.
         * @param title The new title
         */
        void Title(const winrt::hstring& title);
        /**
         * @brief Gets the subtitle for the titlebar.
         * @return The titlebar subtitle
         */
        winrt::hstring Subtitle() const;
        /**
         * @brief Sets the subtitle for the titlebar.
         * @param title The new subtitle
         */
        void Subtitle(const winrt::hstring& subtitle);
        /**
         * @brief Gets the activated state for the titlebar.
         * @return The activated state
         */
        bool IsActivated() const;
        /**
         * @brief Sets the activated state for the titlebar.
         * @param activated The new activated state
         */
        void IsActivated(bool activated);
        /**
         * @brief Gets the visibility of the search box control.
         * @return The search box control visibility
         */
        Microsoft::UI::Xaml::Visibility SearchVisibility() const;
        /**
         * @brief Sets the visibility of the search box control.
         * @param visibility The new search box control visibility
         */
        void SearchVisibility(const Microsoft::UI::Xaml::Visibility& visibility);
        /**
         * @brief Gets the app window for the titlebar.
         * @return The app window
         */
        Microsoft::UI::Windowing::AppWindow AppWindow();
        /**
         * @brief Sets the app window for the titlebar.
         * @param appWindow The new app window
         */
        void AppWindow(const Microsoft::UI::Windowing::AppWindow& appWindow);
        /**
         * @brief Subscribes a handler to the search changed event.
         * @return The token for the newly subscribed handler.
         */
        winrt::event_token SearchChanged(const Windows::Foundation::TypedEventHandler<Microsoft::UI::Xaml::Controls::AutoSuggestBox, Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs>& handler);
        /**
         * @brief Unsubscribes a handler from the search changed event.
         * @param token The token of the handler to unsubscribe.
         */
        void SearchChanged(const winrt::event_token& token);
        /**
         * @brief Subscribes a handler to the search selected event.
         * @return The token for the newly subscribed handler.
         */
        winrt::event_token SearchSelected(const Windows::Foundation::TypedEventHandler<Microsoft::UI::Xaml::Controls::AutoSuggestBox, Microsoft::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs>& handler);
        /**
         * @brief Unsubscribes a handler from the search selected event.
         * @param token The token of the handler to unsubscribe.
         */
        void SearchSelected(const winrt::event_token& token);
        /**
         * @brief Subscribes a handler to the property changed event.
         * @return The token for the newly subscribed handler.
         */
        winrt::event_token PropertyChanged(const Microsoft::UI::Xaml::Data::PropertyChangedEventHandler& handler);
        /**
         * @brief Unsubscribes a handler from the property changed event.
         * @param token The token of the handler to unsubscribe.
         */
        void PropertyChanged(const winrt::event_token& token);
        /**
         * @brief Handles when the titlebar is loaded.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void OnLoaded(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Handles when the titlebar's size is changed.
         * @param sender IInspectable
         * @param args Windows::UI::Xaml::SizeChangedEventArgs
         */
        void OnSizeChanged(const IInspectable& sender, const Microsoft::UI::Xaml::SizeChangedEventArgs& args);
        /**
         * @brief Handles when the titlebar's theme is changed.
         * @param sender Microsoft::UI::Xaml::FrameworkElement
         * @param args IInspectable
         */
        void OnThemeChanged(const Microsoft::UI::Xaml::FrameworkElement& sender, const IInspectable& args);
        /**
         * @brief Handles when the titlebar's search text is changed.
         * @param sender Microsoft::UI::Xaml::Controls::AutoSuggestBox
         * @param args Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs
         */
        void OnSearchTextChanged(const Microsoft::UI::Xaml::Controls::AutoSuggestBox& sender, const Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs& args);
        /**
         * @brief Handles when the titlebar's search suggestion is chosen.
         * @param sender Microsoft::UI::Xaml::Controls::AutoSuggestBox
         * @param args Microsoft::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs
         */
        void OnSearchSuggestionChosen(const Microsoft::UI::Xaml::Controls::AutoSuggestBox& sender, const Microsoft::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs& args);

    private:
        /**
         * @brief Sets the drag region for the titlebar.
         */
        void SetDragRegion();
        static Microsoft::UI::Xaml::DependencyProperty m_titleProperty;
        static Microsoft::UI::Xaml::DependencyProperty m_subtitleProperty;
        static Microsoft::UI::Xaml::DependencyProperty m_isActivatedProperty;
        static Microsoft::UI::Xaml::DependencyProperty m_searchVisibilityProperty;
        Microsoft::UI::Windowing::AppWindow m_appWindow;
        bool m_loaded;
        bool m_isActivated;
        winrt::event<Windows::Foundation::TypedEventHandler<Microsoft::UI::Xaml::Controls::AutoSuggestBox, Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs>> m_searchChangedEvent;
        winrt::event<Windows::Foundation::TypedEventHandler<Microsoft::UI::Xaml::Controls::AutoSuggestBox, Microsoft::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs>> m_searchSelectedEvent;
        winrt::event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChangedEvent;
    };
}

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::factory_implementation
{
    struct TitleBar : public TitleBarT<TitleBar, implementation::TitleBar> { };
}

#endif //TITLEBAR_H