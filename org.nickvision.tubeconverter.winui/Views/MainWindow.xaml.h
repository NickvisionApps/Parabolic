#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "pch.h"
#include "Controls/TitleBar.g.h"
#include "Controls/ViewStack.g.h"
#include "Views/MainWindow.g.h"
#include <memory>
#include "controllers/mainwindowcontroller.h"

namespace winrt::Nickvision::TubeConverter::WinUI::Views::implementation
{
    /**
     * @brief The main window for the application.
     */
    struct MainWindow : MainWindowT<MainWindow>
    {
    public:
        /**
         * @brief Constructs a MainWindow.
         */
        MainWindow();
        /**
         * @brief Sets the controller for the main window.
         * @param controller The MainWindowController
         */
        void Controller(const std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::MainWindowController>& controller);
        /**
         * @brief Sets the system theme for the main window.
         * @param theme The Microsoft::UI::Xaml::ElementTheme
         */
        void SystemTheme(Microsoft::UI::Xaml::ElementTheme theme);
        /**
         * @brief Handles when the main window is loaded.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         * @throw std::logic_error Thrown if the controller has not been set before loading the window
         */
        Windows::Foundation::IAsyncAction OnLoaded(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Handles when the main window is closing.
         * @param sender Microsoft::UI::Windowing::AppWindow
         * @param args Microsoft::UI::Windowing::AppWindowClosingEventArgs
         */
        Windows::Foundation::IAsyncAction OnClosing(const Microsoft::UI::Windowing::AppWindow& sender, const Microsoft::UI::Windowing::AppWindowClosingEventArgs& args);\
        /**
         * @brief Handles when the main window is activated.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::WindowActivatedEventArgs
         */
        void OnActivated(const IInspectable& sender, const Microsoft::UI::Xaml::WindowActivatedEventArgs& args);
        /**
         * @brief Handles when the app's configuration is saved.
         * @param args Nickvision::Events::EventArgs
         */
        void OnConfigurationSaved(const ::Nickvision::Events::EventArgs& args);
        /**
         * @brief Handles when a notification is sent.
         * @param args Nickvision::Notifications::NotificationSentEventArgs
         */
        winrt::fire_and_forget OnNotificationSent(const ::Nickvision::Notifications::NotificationSentEventArgs& args);
        /**
         * @brief Handles when the download history is changed.
         * @param args Nickvision::Events::ParamEventArgs<std::vector<Nickvision::TubeConverter::Shared::Models::HistoricDownload>>
         */
        void OnHistoryChanged(const ::Nickvision::Events::ParamEventArgs<std::vector<::Nickvision::TubeConverter::Shared::Models::HistoricDownload>>& args);
        /**
         * @brief Handles when a credential is needed for a download.
         * @param args Nickvision::TubeConverter::Shared::Events::DownloadCredentialNeededEventArgs
         */
        winrt::fire_and_forget OnDownloadCredentialNeeded(const ::Nickvision::TubeConverter::Shared::Events::DownloadCredentialNeededEventArgs& args);
        /**
         * @brief Handles when a download is addded.
         * @param args Nickvision::TubeConverter::Shared::Events::DownloadAddedEventArgs
         */
        void OnDownloadAdded(const ::Nickvision::TubeConverter::Shared::Events::DownloadAddedEventArgs& args);
        /**
         * @brief Handles when a download is completed.
         * @param args Nickvision::TubeConverter::Shared::Events::DownloadCompletedEventArgs
         */
        void OnDownloadCompleted(const ::Nickvision::TubeConverter::Shared::Events::DownloadCompletedEventArgs& args);
        /**
         * @brief Handles when a download's progress is changed.
         * @param args Nickvision::TubeConverter::Shared::Events::DownloadProgressChangedEventArgs
         */
        void OnDownloadProgressChanged(const ::Nickvision::TubeConverter::Shared::Events::DownloadProgressChangedEventArgs& args);
        /**
         * @brief Handles when a download is stopped.
         * @param args Nickvision::Events::ParamEventArgs<int>
         */
        void OnDownloadStopped(const ::Nickvision::Events::ParamEventArgs<int>& args);
        /**
         * @brief Handles when a download is paused.
         * @param args Nickvision::Events::ParamEventArgs<int>
         */
        void OnDownloadPaused(const ::Nickvision::Events::ParamEventArgs<int>& args);
        /**
         * @brief Handles when a download is resumed.
         * @param args Nickvision::Events::ParamEventArgs<int>
         */
        void OnDownloadResumed(const ::Nickvision::Events::ParamEventArgs<int>& args);
        /**
         * @brief Handles when a download is retried.
         * @param args Nickvision::Events::ParamEventArgs<int>
         */
        void OnDownloadRetried(const ::Nickvision::Events::ParamEventArgs<int>& args);
        /**
         * @brief Handles when a download is started from queue.
         * @param args Nickvision::Events::ParamEventArgs<int>
         */
        void OnDownloadStartedFromQueue(const ::Nickvision::Events::ParamEventArgs<int>& args);
        /**
         * Handles when the titlebar's search box text is changed
         * @param sender Microsoft::UI::Xaml::Controls::AutoSuggestBox
         * @param args Microsoft::UI::Xaml::Controls::AutoSuggestEventArgs
         */
        void OnTitleBarSearchChanged(const Microsoft::UI::Xaml::Controls::AutoSuggestBox& sender, const Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs& args);
        /**
         * Handles when the titlebar's search box selection is changed
         * @param sender Microsoft::UI::Xaml::Controls::AutoSuggestBox
         * @param args Microsoft::UI::Xaml::Controls::AutoSuggestEventArgs
         */
        Windows::Foundation::IAsyncAction OnTitleBarSearchSelected(const Microsoft::UI::Xaml::Controls::AutoSuggestBox& sender, const Microsoft::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs& args);
        /**
        * @brief Handles when a change in the window's navigation occurs.
        * @param sender Microsoft::UI::Xaml::Controls::NavigationView
        * @param args Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs
        */
        void OnNavViewSelectionChanged(const Microsoft::UI::Xaml::Controls::NavigationView& sender, const Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs& args);
        /**
         * @brief Handles when a navigation item is tapped (to display it's flyout).
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::Input::TappedRoutedEventArgs
         */
        void OnNavViewItemTapped(const IInspectable& sender, const Microsoft::UI::Xaml::Input::TappedRoutedEventArgs& args);
        /**
         * @brief Checks for an update to the application.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void CheckForUpdates(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Opens the application's documentation page.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        Windows::Foundation::IAsyncAction Documentation(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Opens the application's GitHub repo.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        Windows::Foundation::IAsyncAction GitHubRepo(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Opens the application's issue tracker.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        Windows::Foundation::IAsyncAction ReportABug(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Opens the application's support page.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        Windows::Foundation::IAsyncAction Discussions(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
        * @brief Opens the about dialog.
        * @param sender IInspectable
        * @param args Microsoft::UI::Xaml::RoutedEventArgs
        */
        Windows::Foundation::IAsyncAction About(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
        * @brief Opens the add download dialog.
        * @param sender IInspectable
        * @param args Microsoft::UI::Xaml::RoutedEventArgs
        */
        Windows::Foundation::IAsyncAction AddDownload(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Clears the download history.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void ClearHistory(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);

    private:
        /**
        * @brief Opens the add download dialog.
        * @param url A url to fill in the dialog with
        */
        Windows::Foundation::IAsyncAction AddDownload(const winrt::hstring& url = L"");
        HWND m_hwnd;
        std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::MainWindowController> m_controller;
        Microsoft::UI::Xaml::ElementTheme m_systemTheme;
        bool m_opened;
        winrt::event_token m_notificationClickToken;
    };
}

namespace winrt::Nickvision::TubeConverter::WinUI::Views::factory_implementation
{
    struct MainWindow : public MainWindowT<MainWindow, implementation::MainWindow> { };
}

#endif //MAINWINDOW_H
