#ifndef KEYRINGPAGE_H
#define KEYRINGPAGE_H

#include "pch.h"
#include "Controls/ViewStack.g.h"
#include "Views/KeyringPage.g.h"
#include <memory>
#include "controllers/keyringdialogcontroller.h"

namespace winrt::Nickvision::TubeConverter::WinUI::Views::implementation
{
    /**
     * @brief The keyring page for the application.
     */
    struct KeyringPage : KeyringPageT<KeyringPage>
    {
    public:
        /**
         * @brief Constructs a KeyringPage.
         */
        KeyringPage();
        /**
         * @brief Sets the controller for the keyring page.
         * @param controller The KeyringDialogController
         */
        void Controller(const std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::KeyringDialogController>& controller);
        /**
         * @brief Prompts the user to add a credential.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        Windows::Foundation::IAsyncAction AddCredential(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);

    private:
        /**
         * @brief Reloads the credentials shown on the page.
         */
        void ReloadCredentials();
        /**
         * @brief Prompts the user to add a credential.
         * @param credential Nickvision::Keyring::Credential
         */
        Windows::Foundation::IAsyncAction EditCredential(const ::Nickvision::Keyring::Credential& credential);
        /**
         * @brief Prompts the user to add a credential.
         * @param credential Nickvision::Keyring::Credential
         */
        Windows::Foundation::IAsyncAction DeleteCredential(const ::Nickvision::Keyring::Credential& credential);
        std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::KeyringDialogController> m_controller;
    };
}

namespace winrt::Nickvision::TubeConverter::WinUI::Views::factory_implementation
{
    struct KeyringPage : public KeyringPageT<KeyringPage, implementation::KeyringPage> { };
}

#endif //KEYRINGPAGE_H
