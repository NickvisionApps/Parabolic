#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "includes.h"
#include <memory>
#include "Controls/StatusPage.g.h"
#include "Controls/TitleBar.g.h"
#include "Controls/ViewStack.g.h"
#include "Controls/ViewStackPage.g.h"
#include "MainWindow.g.h"
#include "controllers/mainwindowcontroller.h"

namespace winrt::Nickvision::TubeConverter::WinUI::implementation 
{
    /**
     * @brief The main window for the application. 
     */
    class MainWindow : public MainWindowT<MainWindow>
    {
    public:
        /**
         * @brief Constructs a MainWindow.
         */
        MainWindow();
        /**
         * @brief Sets the controller for the main window.
         * @param controller The MainWindowController 
         * @param systemTheme The ElementTheme of the system
         */
        void SetController(const std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::MainWindowController>& controller, Microsoft::UI::Xaml::ElementTheme systemTheme);
        /**
         * @brief Handles when the main window is loaded.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void OnLoaded(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Handles when the main window is being closed.
         * @param sender Microsoft::UI::Windowing::AppWindow
         * @param args Microsoft::UI::Windowing::AppWindowClosingEventArgs
         */
        void OnClosing(const Microsoft::UI::Windowing::AppWindow& sender, const Microsoft::UI::Windowing::AppWindowClosingEventArgs& args);
        /**
         * @brief Handles when the main window is activated.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::WindowActivatedEventArgs
         */
        void OnActivated(const IInspectable& sender, const Microsoft::UI::Xaml::WindowActivatedEventArgs& args);
        /**
         * @brief Handles when the application's configuration is saved to disk.
         * @param args Nickvision::Events::EventArgs 
         */
        void OnConfigurationSaved(const ::Nickvision::Events::EventArgs& args);
        /**
         * @brief Handles when a notification is sent to the window.
         * @param args Nickvision::Notifications::NotificationSentEventArgs 
         */
        void OnNotificationSent(const ::Nickvision::Notifications::NotificationSentEventArgs& args);
        /**
         * @brief Handles when a shell notification is sent to the window.
         * @param args Nickvision::Notifications::ShellNotificationSentEventArgs
         */
        void OnShellNotificationSent(const ::Nickvision::Notifications::ShellNotificationSentEventArgs& args);
        /**
         * @brief Handles when the disclaimer is triggered.
         * @param args Nickvision::Events::ParamEventArgs<std::string>
         */
        Windows::Foundation::IAsyncAction OnDisclaimerTriggered(const ::Nickvision::Events::ParamEventArgs<std::string>& args);
        /**
         * @brief Handles when a change in the window's navigation occurs.
         * @param sender Microsoft::UI::Xaml::Controls::NavigationView
         * @param args Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs
         */
        void OnNavSelectionChanged(const Microsoft::UI::Xaml::Controls::NavigationView& sender, const Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs& args);
        /**
         * @brief Handles when a navigation item is tapped (to display it's flyout).
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::Input::TappedRoutedEventArgs
         */
        void OnNavViewItemTapped(const IInspectable& sender, const Microsoft::UI::Xaml::Input::TappedRoutedEventArgs& args);
        /**
         * @brief Handles when the history is changed.
         * @param args Nickvision::Events::ParamEventArgs<std::vector<Models::HistoricDownload>>
         */
        void OnHistoryChanged(const ::Nickvision::Events::ParamEventArgs<std::vector<::Nickvision::TubeConverter::Shared::Models::HistoricDownload>>& args);
        /**
         * @brief Checks for an update to the application.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void CheckForUpdates(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Updates the application.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void WindowsUpdate(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Opens the application's credits dialog.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        Windows::Foundation::IAsyncAction Credits(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Copies debugging information about the application to the clipboard.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void CopyDebugInformation(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
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
         * @brief Clears the download history.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void ClearHistory(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);

    private:
        std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::MainWindowController> m_controller;
        bool m_opened;
        HWND m_hwnd;
        Microsoft::UI::Xaml::ElementTheme m_systemTheme;
        winrt::event_token m_notificationClickToken;
    };
}

namespace winrt::Nickvision::TubeConverter::WinUI::factory_implementation 
{
    class MainWindow : public MainWindowT<MainWindow, implementation::MainWindow>
    {

    };
}

#endif //MAINWINDOW_H