#ifndef ADDDOWNLOADDIALOG_H
#define ADDDOWNLOADDIALOG_H

#include "pch.h"
#include "Controls/SettingsRow.xaml.h"
#include "Controls/ViewStack.g.h"
#include "Views/AddDownloadDialog.g.h"
#include <memory>
#include "controllers/mainwindowcontroller.h"

namespace winrt::Nickvision::TubeConverter::WinUI::Views::implementation
{
    /**
     * @brief A dialog for showing application information.
     */
    struct AddDownloadDialog : public AddDownloadDialogT<AddDownloadDialog>
    {
    public:
        /**
         * @brief Constructs a AddDownloadDialog.
         */
        AddDownloadDialog();
        /**
         * @brief Sets the controller for the dialog.
         * @param controller The AddDownloadDialogController
         * @param url A url to fill in the dialog with
         */
        void Controller(const std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::AddDownloadDialogController>& controller, const winrt::hstring& url);
        /**
         * @brief Shows the dialog
         * @return Microsoft::UI::Xaml::Controls::ContentDialogResult
         */
        Windows::Foundation::IAsyncOperation<Microsoft::UI::Xaml::Controls::ContentDialogResult> ShowAsync();
        /**
         * @brief Handles when the validate page's navigation view selection changes.
         * @param sender SelectorBar
         * @param args SelectorItemInvokedEventArgs
         */
        void OnNavViewValidateSelectionChanged(const Microsoft::UI::Xaml::Controls::SelectorBar& sender, const Microsoft::UI::Xaml::Controls::SelectorBarSelectionChangedEventArgs& args);

    private:
        std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::AddDownloadDialogController> m_controller;
    };
}

namespace winrt::Nickvision::TubeConverter::WinUI::Views::factory_implementation
{
    struct AddDownloadDialog : public AddDownloadDialogT<AddDownloadDialog, implementation::AddDownloadDialog> { };
}

#endif //ADDDOWNLOADDIALOG_H
