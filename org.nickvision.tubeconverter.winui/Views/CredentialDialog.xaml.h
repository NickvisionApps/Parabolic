#ifndef CREDENTIALDIALOG_H
#define CREDENTIALDIALOG_H

#include "pch.h"
#include "Views/CredentialDialog.g.h"
#include <memory>
#include "controllers/credentialdialogcontroller.h"

namespace winrt::Nickvision::TubeConverter::WinUI::Views::implementation
{
    /**
     * @brief A dialog for selecting a credential.
     */
    struct CredentialDialog : public CredentialDialogT<CredentialDialog>
    {
    public:
        /**
         * @brief Constructs a CredentialDialog.
         */
        CredentialDialog();
        /**
         * @brief Sets the controller for the dialog.
         * @param controller The CredentialDialogController
         */
        void Controller(const std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::CredentialDialogController>& controller);
        /**
         * @brief Shows the dialog
         * @return Microsoft::UI::Xaml::Controls::ContentDialogResult
         */
        Windows::Foundation::IAsyncOperation<Microsoft::UI::Xaml::Controls::ContentDialogResult> ShowAsync();
        /**
         * @brief Handles when the credential combobox's selection is changed.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs
         */
        void OnCmbCredentialSelectionChanged(const IInspectable& sender, const Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs& args);

    private:
        std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::CredentialDialogController> m_controller;
    };
}

namespace winrt::Nickvision::TubeConverter::WinUI::Views::factory_implementation
{
    struct CredentialDialog : public CredentialDialogT<CredentialDialog, implementation::CredentialDialog> { };
}

#endif //CREDENTIALDIALOG_H
