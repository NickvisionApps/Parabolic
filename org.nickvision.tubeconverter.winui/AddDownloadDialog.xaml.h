#ifndef ADDDOWNLOADDIALOG_H
#define ADDDOWNLOADDIALOG_H

#include "includes.h"
#include <memory>
#include "Controls/SettingsRow.g.h"
#include "Controls/ViewStack.g.h"
#include "Controls/ViewStackPage.g.h"
#include "AddDownloadDialog.g.h"
#include "controllers/adddownloaddialogcontroller.h"

namespace winrt::Nickvision::TubeConverter::WinUI::implementation 
{
    /**
     * @brief The settings page for the application. 
     */
    class AddDownloadDialog : public AddDownloadDialogT<AddDownloadDialog>
    {
    public:
        /**
         * @brief Constructs a AddDownloadDialog.
         */
        AddDownloadDialog();
        /**
         * @brief Sets the controller for the dialog.
         * @param controller The AddDownloadDialogController 
         * @param hwnd The window handle
         */
        void SetController(const std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::AddDownloadDialogController>& controller, HWND hwnd);
        /**
         * @brief Shows the dialog.
         * @return ContentDialogResult
         */
        Windows::Foundation::IAsyncOperation<Microsoft::UI::Xaml::Controls::ContentDialogResult> ShowAsync();
        /**
         * @brief Handles when the dialog is closing.
         * @param sender ContentDialog
         * @param args ContentDialogClosingEventArgs
         */
        void OnClosing(const Microsoft::UI::Xaml::Controls::ContentDialog& sender, const Microsoft::UI::Xaml::Controls::ContentDialogClosingEventArgs& args);
        
    private:
        std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::AddDownloadDialogController> m_controller;
        HWND m_hwnd;
    };
}

namespace winrt::Nickvision::TubeConverter::WinUI::factory_implementation 
{
    class AddDownloadDialog : public AddDownloadDialogT<AddDownloadDialog, implementation::AddDownloadDialog>
    {

    };
}

#endif //ADDDOWNLOADDIALOG_H