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
         */
        void OnSwitchToggled(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);

    private:
        /**
         * @brief Applies changes the application's configuration.
         */
        void ApplyChanges();
        std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::PreferencesViewController> m_controller;
        bool m_constructing;
    };
}

namespace winrt::Nickvision::TubeConverter::WinUI::Views::factory_implementation
{
    struct SettingsPage : public SettingsPageT<SettingsPage, implementation::SettingsPage> { };
}

#endif //SETTINGSPAGE_H