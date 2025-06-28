#ifndef ADDDOWNLOADDIALOG_H
#define ADDDOWNLOADDIALOG_H

#include "pch.h"
#include "Controls/SettingsRow.xaml.h"
#include "Controls/StatusPage.g.h"
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
        Windows::Foundation::IAsyncAction Controller(const std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::AddDownloadDialogController>& controller, const winrt::hstring& url);
        /**
         * @brief Sets the hwnd for the dialog.
         * @param hwnd The hwnd
         */
        void Hwnd(HWND hwnd);
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
        /**
         * @brief Handles when the media url textbox's text is changed.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::Controls::TextChangedEventArgs
         */
        void OnTxtMediaUrlTextChanged(const IInspectable& sender, const Microsoft::UI::Xaml::Controls::TextChangedEventArgs& args);
        /**
         * @brief Handles when the credential combobox's selection is changed.
         * @param sender Iinspectable
         * @param args Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs
         */
        void OnCmbCredentialSelectionChanged(const IInspectable& sender, const Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs& args);
        /**
         * @brief Prompts the user to select a batch file.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        Windows::Foundation::IAsyncAction UseBatchFile(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Handles when the file type combobox is changed.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs
         */
        void OnCmbFileTypeChanged(const IInspectable& sender, const Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs& args);
        /**
         * @brief Handles when the single page's navigation view selection changes.
         * @param sender SelectorBar
         * @param args SelectorItemInvokedEventArgs
         */
        void OnNavViewSingleSelectionChanged(const Microsoft::UI::Xaml::Controls::SelectorBar& sender, const Microsoft::UI::Xaml::Controls::SelectorBarSelectionChangedEventArgs& args);
        /**
         * @brief Prompts the user to select a save folder for a single download.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        Windows::Foundation::IAsyncAction SelectSaveFolderSingle(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Reverts the file name of a single download.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void RevertFileNameSingle(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Selects all subtitles for a single download.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void SelectAllSubtitlesSingle(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Deselects all subtitles for a single download.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void DeselectAllSubtitlesSingle(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Handles when the playlist page's navigation view selection changes.
         * @param sender SelectorBar
         * @param args SelectorItemInvokedEventArgs
         */
        void OnNavViewPlaylistSelectionChanged(const Microsoft::UI::Xaml::Controls::SelectorBar& sender, const Microsoft::UI::Xaml::Controls::SelectorBarSelectionChangedEventArgs& args);
        /**
         * @brief Prompts the user to select a save folder for a playlist download.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        Windows::Foundation::IAsyncAction SelectSaveFolderPlaylist(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Handles when the playlist's number titles toggle is changed.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void OnTglNumberTitlesPlaylistToggled(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Selects all items for a playlist download.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void SelectAllItemsPlaylist(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Deselects all items for a playlist download.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void DeselectAllItemsPlaylist(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);

    private:
        /**
         * @brief Handles when a url is validated.
         * @param valid Whether or not the url is valid
         */
        winrt::fire_and_forget OnUrlValidated(bool valid);
        /**
         * @brief Triggers the download.
         */
        void Download();
        std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::AddDownloadDialogController> m_controller;
        HWND m_hwnd;
    };
}

namespace winrt::Nickvision::TubeConverter::WinUI::Views::factory_implementation
{
    struct AddDownloadDialog : public AddDownloadDialogT<AddDownloadDialog, implementation::AddDownloadDialog> { };
}

#endif //ADDDOWNLOADDIALOG_H
