#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include "includes.h"
#include <memory>
#include "Controls/SettingsRow.g.h"
#include "SettingsPage.g.h"
#include "controllers/preferencesviewcontroller.h"

namespace winrt::Nickvision::TubeConverter::WinUI::implementation 
{
    /**
     * @brief The settings page for the application. 
     */
    class SettingsPage : public SettingsPageT<SettingsPage>
    {
    public:
        /**
         * @brief Constructs a SettingsPage.
         */
        SettingsPage();
        /**
         * @brief Sets the controller for the page.
         * @param controller The PreferencesViewController 
         * @param hwnd The window handle
         */
        void SetController(const std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::PreferencesViewController>& controller, HWND hwnd);
        /**
         * @brief Handles when a combo preference is changed. 
         */
        void OnCmbChanged(const IInspectable& sender, const Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs& args);
        /**
         * @brief Handles when a switch preference is changed. 
         */
        void OnSwitchToggled(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Handles when a number preference is changed. 
         */
        void OnNumChanged(const Microsoft::UI::Xaml::Controls::NumberBox& sender, const Microsoft::UI::Xaml::Controls::NumberBoxValueChangedEventArgs& args);
        /**
         * @brief Handles when a text preference is changed. 
         */
        void OnTextChanged(const IInspectable& sender, const Microsoft::UI::Xaml::Controls::TextChangedEventArgs& args);
        /**
         * @brief Prompts the user to open a cookies file.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        Windows::Foundation::IAsyncAction SelectCookiesFile(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Clears the cookies file.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void ClearCookiesFile(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Opens the link to the cookies extension for chrome-based browsers.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        Windows::Foundation::IAsyncAction CookiesChrome(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Opens the link to the cookies extension for firefox.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        Windows::Foundation::IAsyncAction CookiesFirefox(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);

    private:
        /**
         * @brief Applies changes the application's configuration.
         */
        void ApplyChanges();
        std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::PreferencesViewController> m_controller;
        bool m_constructing;
        HWND m_hwnd;
    };
}

namespace winrt::Nickvision::TubeConverter::WinUI::factory_implementation 
{
    class SettingsPage : public SettingsPageT<SettingsPage, implementation::SettingsPage>
    {

    };
}

#endif //SETTINGSPAGE_H