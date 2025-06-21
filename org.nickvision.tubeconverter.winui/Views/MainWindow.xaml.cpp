#include "Views/MainWindow.xaml.h"
#if __has_include("Views/MainWindow.g.cpp")
#include "Views/MainWindow.g.cpp"
#endif
#include <stdexcept>
#include <libnick/localization/gettext.h>
#include "Controls/AboutDialog.xaml.h"
#include "Views/SettingsPage.xaml.h"

using namespace ::Nickvision::Events;
using namespace ::Nickvision::Notifications;
using namespace ::Nickvision::TubeConverter::Shared::Controllers;
using namespace ::Nickvision::TubeConverter::Shared::Models;
using namespace ::Nickvision::Update;
using namespace winrt::Microsoft::UI::Dispatching;
using namespace winrt::Microsoft::UI::Windowing;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Controls::Primitives;
using namespace winrt::Microsoft::UI::Xaml::Input;
using namespace winrt::Windows::Graphics;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Pickers;
using namespace winrt::Windows::System;

enum MainWindowPage
{
    Home = 0,
    Custom
};

namespace winrt::Nickvision::TubeConverter::WinUI::Views::implementation
{
    MainWindow::MainWindow()
        : m_opened{ false },
        m_notificationClickToken{ 0 }
    {
        InitializeComponent();
        this->m_inner.as<::IWindowNative>()->get_WindowHandle(&m_hwnd);
        TitleBar().AppWindow(AppWindow());
    }

    void MainWindow::Controller(const std::shared_ptr<MainWindowController>& controller)
    {
        m_controller = controller;
        //Register Events
        AppWindow().Closing({ this, &MainWindow::OnClosing });
        m_controller->configurationSaved() += [&](const EventArgs& args){ OnConfigurationSaved(args); };
        m_controller->notificationSent() += [&](const NotificationSentEventArgs& args){ DispatcherQueue().TryEnqueue([this, args](){ OnNotificationSent(args); }); };
        //Localize Strings
        TitleBar().Title(winrt::to_hstring(m_controller->getAppInfo().getShortName()));
        TitleBar().Subtitle(m_controller->getAppInfo().getVersion().getVersionType() == VersionType::Preview ? winrt::to_hstring(_("Preview")) : L"");
        NavViewHome().Content(winrt::box_value(winrt::to_hstring(_("Home"))));
        NavViewKeyring().Content(winrt::box_value(winrt::to_hstring(_("Keyring"))));
        NavViewHistory().Content(winrt::box_value(winrt::to_hstring(_("History"))));
        NavViewDownloads().Content(winrt::box_value(winrt::to_hstring(_("Downloads"))));
        NavViewDownloading().Content(winrt::box_value(winrt::to_hstring(_("Downloading"))));
        NavViewQueued().Content(winrt::box_value(winrt::to_hstring(_("Queued"))));
        NavViewCompleted().Content(winrt::box_value(winrt::to_hstring(_("Completed"))));
        NavViewHelp().Content(winrt::box_value(winrt::to_hstring(_("Help"))));
        NavViewSettings().Content(winrt::box_value(winrt::to_hstring(_("Settings"))));
        MenuCheckForUpdates().Text(winrt::to_hstring(_("Check for Updates")));
        MenuDocumentation().Text(winrt::to_hstring(_("Documentation")));
        MenuGitHubRepo().Text(winrt::to_hstring(_("GitHub Repo")));
        MenuReportABug().Text(winrt::to_hstring(_("Report a Bug")));
        MenuDiscussions().Text(winrt::to_hstring(_("Discussions")));
        MenuAbout().Text(winrt::to_hstring(_("About")));
        PageHome().Title(winrt::to_hstring(_("Download Media")));
        PageHome().Description(winrt::to_hstring(_("Add a video, audio, or playlist URL to start downloading")));
        LblHomeAddDownload().Text(winrt::to_hstring(_("Add Download")));
    }

    void MainWindow::SystemTheme(ElementTheme theme)
    {
        m_systemTheme = theme;
    }

    void MainWindow::OnLoaded(const IInspectable& sender, const RoutedEventArgs& args)
    {
        if (!m_controller)
        {
            throw std::logic_error("MainWindow::SetController() must be called before using the window.");
        }
        if(m_opened)
        {
            return;
        }
        //Startup
        const StartupInformation& info{ m_controller->startup(m_hwnd) };
        if(info.getWindowGeometry().isMaximized())
        {
            ShowWindow(m_hwnd, SW_MAXIMIZE);
        }
        else
        {
            RectInt32 size;
            size.Width = info.getWindowGeometry().getWidth();
            size.Height = info.getWindowGeometry().getHeight();
            size.X = info.getWindowGeometry().getX();
            size.Y = info.getWindowGeometry().getY();
            AppWindow().MoveAndResize(size);
        }
        NavViewHome().IsSelected(true);
        m_opened = true;
    }

    void MainWindow::OnClosing(const Microsoft::UI::Windowing::AppWindow& sender, const AppWindowClosingEventArgs& args)
    {
        if(!m_controller->canShutdown())
        {
            args.Cancel(true);
            return;
        }
        m_controller->shutdown({ AppWindow().Size().Width, AppWindow().Size().Height, static_cast<bool>(IsZoomed(m_hwnd)), AppWindow().Position().X, AppWindow().Position().Y });
    }

    void MainWindow::OnActivated(const IInspectable& sender, const WindowActivatedEventArgs& args)
    {
        TitleBar().IsActivated(args.WindowActivationState() != WindowActivationState::Deactivated);
    }

    void MainWindow::OnConfigurationSaved(const ::Nickvision::Events::EventArgs&)
    {
        switch (m_controller->getTheme())
        {
        case Theme::Light:
            MainGrid().RequestedTheme(ElementTheme::Light);
            break;
        case Theme::Dark:
            MainGrid().RequestedTheme(ElementTheme::Dark);
            break;
        default:
            MainGrid().RequestedTheme(m_systemTheme);
            break;
        }
    }

    void MainWindow::OnNotificationSent(const NotificationSentEventArgs& args)
    {
        InfoBar().Message(winrt::to_hstring(args.getMessage()));
        switch(args.getSeverity())
        {
        case NotificationSeverity::Success:
            InfoBar().Severity(InfoBarSeverity::Success);
            break;
        case NotificationSeverity::Warning:
            InfoBar().Severity(InfoBarSeverity::Warning);
            break;
        case NotificationSeverity::Error:
            InfoBar().Severity(InfoBarSeverity::Error);
            break;
        default:
            InfoBar().Severity(InfoBarSeverity::Informational);
            break;
        }
        if(m_notificationClickToken)
        {
            BtnInfoBar().Click(m_notificationClickToken);
        }
        if(args.getAction() == "update")
        {
            BtnInfoBar().Content(winrt::box_value(winrt::to_hstring(_("Update"))));
            m_notificationClickToken = BtnInfoBar().Click([this](const IInspectable&, const RoutedEventArgs&)
            {
                InfoBar().IsOpen(false);
                m_controller->windowsUpdate();
            });
        }
        BtnInfoBar().Visibility(!args.getAction().empty() ? Visibility::Visible : Visibility::Collapsed);
        InfoBar().IsOpen(true);
    }

    void MainWindow::OnTitleBarSearchChanged(const Microsoft::UI::Xaml::Controls::AutoSuggestBox& sender, const Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs& args)
    {

    }

    void MainWindow::OnNavViewSelectionChanged(const NavigationView& sender, const NavigationViewSelectionChangedEventArgs& args)
    {
        winrt::hstring tag{ NavView().SelectedItem().as<NavigationViewItem>().Tag().as<winrt::hstring>() };
        if(tag == L"Home")
        {
            ViewStack().CurrentPageIndex(MainWindowPage::Home);
        }
        else if(tag == L"Settings")
        {
            ViewStack().CurrentPageIndex(MainWindowPage::Custom);
            PageCustom().Content(winrt::make<implementation::SettingsPage>());
            PageCustom().Content().as<implementation::SettingsPage>()->Controller(m_controller->createPreferencesViewController());
        }
    }

    void MainWindow::OnNavViewItemTapped(const IInspectable& sender, const TappedRoutedEventArgs& args)
    {
        FlyoutBase::ShowAttachedFlyout(sender.as<FrameworkElement>());
    }

    void MainWindow::CheckForUpdates(const IInspectable& sender, const RoutedEventArgs& args)
    {
        m_controller->checkForUpdates(true);
    }

    Windows::Foundation::IAsyncAction MainWindow::Documentation(const IInspectable& sender, const RoutedEventArgs& args)
    {
        co_await Launcher::LaunchUriAsync(Windows::Foundation::Uri{ winrt::to_hstring(m_controller->getHelpUrl()) });
    }

    Windows::Foundation::IAsyncAction MainWindow::GitHubRepo(const IInspectable& sender, const RoutedEventArgs& args)
    {
        co_await Launcher::LaunchUriAsync(Windows::Foundation::Uri{ winrt::to_hstring(m_controller->getAppInfo().getSourceRepo()) });
    }

    Windows::Foundation::IAsyncAction MainWindow::ReportABug(const IInspectable& sender, const RoutedEventArgs& args)
    {
        co_await Launcher::LaunchUriAsync(Windows::Foundation::Uri{ winrt::to_hstring(m_controller->getAppInfo().getIssueTracker()) });
    }

    Windows::Foundation::IAsyncAction MainWindow::Discussions(const IInspectable& sender, const RoutedEventArgs& args)
    {
        co_await Launcher::LaunchUriAsync(Windows::Foundation::Uri{ winrt::to_hstring(m_controller->getAppInfo().getSupportUrl()) });
    }

    Windows::Foundation::IAsyncAction MainWindow::About(const IInspectable& sender, const RoutedEventArgs& args)
    {
        ContentDialog dialog{ winrt::make<Controls::implementation::AboutDialog>() };
        dialog.as<Controls::implementation::AboutDialog>()->Info(m_controller->getAppInfo(), m_controller->getDebugInformation());
        dialog.RequestedTheme(MainGrid().RequestedTheme());
        dialog.XamlRoot(MainGrid().XamlRoot());
        co_await dialog.ShowAsync();
    }
}
