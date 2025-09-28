#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include "pch.h"
#include "Controls/ViewStack.g.h"
#include "Views/SettingsPage.g.h"
#include <memory>
#include "controllers/preferencesviewcontroller.h"

namespace winrt::Nickvision::TubeConverter::WinUI::Views::implementation
{
    /**
     * @brief The settings page for the application.
     */
    struct SettingsPage : SettingsPageT<SettingsPage>
    {
    public:
        /**
         * @brief Constructs a SettingsPage.
         */
        SettingsPage();
        /**
         * @brief Sets the controller for the settings page.
         * @param controller The PreferencesViewController
         */
        void Controller(const std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::PreferencesViewController>& controller);
        /**
         * @brief Sets the WindowId for the dialog.
         * @param id The WindowId
         */
        void WindowId(Microsoft::UI::WindowId id);
        /**
         * @brief Handles when the navigation view selection changes.
         * @param sender SelectorBar
         * @param args SelectorItemInvokedEventArgs
         */
        void OnNavViewSelectionChanged(const Microsoft::UI::Xaml::Controls::SelectorBar& sender, const Microsoft::UI::Xaml::Controls::SelectorBarSelectionChangedEventArgs& args);
        /**
         * @brief Handles when a combo preference is changed.
         */
        void OnCmbChanged(const IInspectable& sender, const Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs& args);
        /**
         * @brief Handles when a switch preference is changed.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
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
         * @brief Prompts the user to select a cookies file.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        Windows::Foundation::IAsyncAction SelectCookiesFile(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Clears the selected cookie file.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void ClearCookiesFile(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Prompts the user to add a postprocessing argument.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        Windows::Foundation::IAsyncAction AddPostprocessingArgument(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Prompts the user to edit a postprocessing argument.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        Windows::Foundation::IAsyncAction EditPostprocessingArgument(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Prompts the user to delete a postprocessing argument.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        Windows::Foundation::IAsyncAction DeletePostprocessingArgument(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);

    private:
        /**
         * @brief Reloads the postprocessing arguments shown.
         */
        void ReloadPostprocessingArguments();
        /**
         * @brief Applies changes the application's configuration.
         */
        void ApplyChanges();
        std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::PreferencesViewController> m_controller;
        Microsoft::UI::WindowId m_windowId;
        bool m_constructing;
        std::filesystem::path m_cookiesFilePath;
    };
}

namespace winrt::Nickvision::TubeConverter::WinUI::Views::factory_implementation
{
    struct SettingsPage : public SettingsPageT<SettingsPage, implementation::SettingsPage> { };
}

#endif //SETTINGSPAGE_H
