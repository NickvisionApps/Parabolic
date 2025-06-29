#include "Views/MainWindow.xaml.h"
#if __has_include("Views/MainWindow.g.cpp")
#include "Views/MainWindow.g.cpp"
#endif
#include <stdexcept>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>
#include "Controls/AboutDialog.xaml.h"
#include "Controls/SettingsRow.xaml.h"
#include "Helpers/WinUIHelpers.h"
#include "Views/AddDownloadDialog.xaml.h"
#include "Views/KeyringPage.xaml.h"
#include "Views/SettingsPage.xaml.h"

using namespace ::Nickvision::Events;
using namespace ::Nickvision::Helpers;
using namespace ::Nickvision::Notifications;
using namespace ::Nickvision::TubeConverter::Shared::Controllers;
using namespace ::Nickvision::TubeConverter::Shared::Models;
using namespace ::Nickvision::TubeConverter::WinUI::Helpers;
using namespace ::Nickvision::Update;
using namespace winrt::Microsoft::UI::Dispatching;
using namespace winrt::Microsoft::UI::Windowing;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Controls::Primitives;
using namespace winrt::Microsoft::UI::Xaml::Input;
using namespace winrt::Microsoft::UI::Xaml::Media;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Graphics;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::System;

enum MainWindowPage
{
    Home = 0,
    History,
    Downloading,
    Queued,
    Completed,
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
        m_controller->configurationSaved() += [this](const EventArgs& args){ OnConfigurationSaved(args); };
        m_controller->notificationSent() += [this](const NotificationSentEventArgs& args){ DispatcherQueue().TryEnqueue([this, args](){ OnNotificationSent(args); }); };
        m_controller->getDownloadManager().historyChanged() += [this](const ParamEventArgs<std::vector<HistoricDownload>>& args){ DispatcherQueue().TryEnqueue([this, args](){ OnHistoryChanged(args); }); };
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
        LblHistoryTitle().Text(winrt::to_hstring(_("History")));
        PageNoHistory().Title(winrt::to_hstring(_("No History Available")));
        LblHistoryAddDownload().Text(winrt::to_hstring(_("Add Download")));
        LblHistoryClearHistory().Text(winrt::to_hstring(_("Clear")));
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

    winrt::fire_and_forget MainWindow::OnNotificationSent(const NotificationSentEventArgs& args)
    {
        if(args.getAction() == "error")
        {
            ContentDialog dialogErr;
            dialogErr.Title(winrt::box_value(winrt::to_hstring(_("Error"))));
            dialogErr.Content(winrt::box_value(winrt::to_hstring(args.getActionParam())));
            dialogErr.CloseButtonText(winrt::to_hstring(_("OK")));
            dialogErr.DefaultButton(ContentDialogButton::Close);
            dialogErr.RequestedTheme(MainGrid().RequestedTheme());
            dialogErr.XamlRoot(MainGrid().XamlRoot());
            co_await dialogErr.ShowAsync();
            co_return;
        }
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

    void MainWindow::OnHistoryChanged(const ParamEventArgs<std::vector<HistoricDownload>>& args)
    {
        ViewStackHistory().CurrentPageIndex(0);
        ListHistory().Children().Clear();
        for(const HistoricDownload& download : *args)
        {
            FontIcon icnPlay;
            icnPlay.FontFamily(WinUIHelpers::LookupAppResource<Microsoft::UI::Xaml::Media::FontFamily>(L"SymbolThemeFontFamily"));
            icnPlay.Glyph(L"\uE768");
            Button btnPlay;
            btnPlay.Content(icnPlay);
            ToolTipService::SetToolTip(btnPlay, winrt::box_value(winrt::to_hstring(_("Play"))));
            btnPlay.Click([this, download](const IInspectable&, const RoutedEventArgs&) -> Windows::Foundation::IAsyncAction
            {
                co_await Launcher::LaunchFileAsync(co_await StorageFile::GetFileFromPathAsync(winrt::to_hstring(download.getPath().string())));
            });
            FontIcon icnDownload;
            icnDownload.FontFamily(WinUIHelpers::LookupAppResource<Microsoft::UI::Xaml::Media::FontFamily>(L"SymbolThemeFontFamily"));
            icnDownload.Glyph(L"\uE896");
            Button btnDownload;
            btnDownload.Content(icnDownload);
            ToolTipService::SetToolTip(btnDownload, winrt::box_value(winrt::to_hstring(_("Download"))));
            btnDownload.Click([this, download](const IInspectable&, const RoutedEventArgs&) -> Windows::Foundation::IAsyncAction
            {
                co_await AddDownload(winrt::to_hstring(download.getUrl()));
            });
            FontIcon icnDelete;
            icnDelete.FontFamily(WinUIHelpers::LookupAppResource<Microsoft::UI::Xaml::Media::FontFamily>(L"SymbolThemeFontFamily"));
            icnDelete.Glyph(L"\uE74D");
            Button btnDelete;
            btnDelete.Content(icnDelete);
            ToolTipService::SetToolTip(btnDelete, winrt::box_value(winrt::to_hstring(_("Delete"))));
            btnDelete.Click([this, download](const IInspectable&, const RoutedEventArgs&)
            {
                m_controller->getDownloadManager().removeHistoricDownload(download);
            });
            StackPanel panel;
            panel.Orientation(Orientation::Horizontal);
            panel.Spacing(6);
            if(std::filesystem::exists(download.getPath()))
            {
                panel.Children().Append(btnPlay);
            }
            panel.Children().Append(btnDownload);
            panel.Children().Append(btnDelete);
            Controls::SettingsRow row{ winrt::make<Controls::implementation::SettingsRow>() };
            row.Title(winrt::to_hstring(download.getTitle()));
            row.Description(winrt::to_hstring(download.getUrl()));
            row.Child(panel);
            ViewStackHistory().CurrentPageIndex(1);
            ListHistory().Children().Append(row);
        }
    }

    void MainWindow::OnTitleBarSearchChanged(const Microsoft::UI::Xaml::Controls::AutoSuggestBox& sender, const Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs& args)
    {
        if(args.Reason() == AutoSuggestionBoxTextChangeReason::UserInput)
        {
            IObservableVector<IInspectable> items{ winrt::single_threaded_observable_vector<IInspectable>() };
            if(StringHelpers::isValidUrl(winrt::to_string(sender.Text())))
            {
                FontIcon icn;
                icn.FontFamily(WinUIHelpers::LookupAppResource<FontFamily>(L"SymbolThemeFontFamily"));
                icn.Glyph(L"\uE710");
                TextBlock txt;
                txt.Text(winrt::to_hstring(_("Add Download")));
                StackPanel panel;
                panel.Tag(winrt::box_value(sender.Text()));
                panel.Orientation(Orientation::Horizontal);
                panel.Spacing(6);
                panel.Children().Append(icn);
                panel.Children().Append(txt);
                items.Append(winrt::box_value(panel));
            }
            sender.ItemsSource(items);
        }
    }

    Windows::Foundation::IAsyncAction MainWindow::OnTitleBarSearchSelected(const Microsoft::UI::Xaml::Controls::AutoSuggestBox& sender, const Microsoft::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs& args)
    {
        co_await AddDownload(winrt::unbox_value<winrt::hstring>(args.SelectedItem().as<StackPanel>().Tag()));
    }

    void MainWindow::OnNavViewSelectionChanged(const NavigationView& sender, const NavigationViewSelectionChangedEventArgs& args)
    {
        winrt::hstring tag{ NavView().SelectedItem().as<NavigationViewItem>().Tag().as<winrt::hstring>() };
        if(tag == L"Home")
        {
            ViewStack().CurrentPageIndex(MainWindowPage::Home);
        }
        else if(tag == L"Keyring")
        {
            ViewStack().CurrentPageIndex(MainWindowPage::Custom);
            PageCustom().Content(winrt::make<implementation::KeyringPage>());
            PageCustom().Content().as<implementation::KeyringPage>()->Controller(m_controller->createKeyringDialogController());
        }
        else if(tag == L"History")
        {
            ViewStack().CurrentPageIndex(MainWindowPage::History);
        }
        else if(tag == L"Downloading")
        {
            ViewStack().CurrentPageIndex(MainWindowPage::Downloading);
        }
        else if(tag == L"Queued")
        {
            ViewStack().CurrentPageIndex(MainWindowPage::Queued);
        }
        else if(tag == L"Completed")
        {
            ViewStack().CurrentPageIndex(MainWindowPage::Completed);
        }
        else if(tag == L"Settings")
        {
            ViewStack().CurrentPageIndex(MainWindowPage::Custom);
            PageCustom().Content(winrt::make<implementation::SettingsPage>());
            PageCustom().Content().as<implementation::SettingsPage>()->Controller(m_controller->createPreferencesViewController());
            PageCustom().Content().as<implementation::SettingsPage>()->Hwnd(m_hwnd);
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

    Windows::Foundation::IAsyncAction MainWindow::AddDownload(const IInspectable& sender, const RoutedEventArgs& args)
    {
        co_await AddDownload();
    }

    void MainWindow::ClearHistory(const IInspectable& sender, const RoutedEventArgs& args)
    {
        m_controller->getDownloadManager().clearHistory();
    }

    Windows::Foundation::IAsyncAction MainWindow::AddDownload(const winrt::hstring& url)
    {
        ContentDialog dialog{ winrt::make<implementation::AddDownloadDialog>() };
        co_await dialog.as<implementation::AddDownloadDialog>()->Controller(m_controller->createAddDownloadDialogController(), url);
        dialog.as<implementation::AddDownloadDialog>()->Hwnd(m_hwnd);
        dialog.RequestedTheme(MainGrid().RequestedTheme());
        dialog.XamlRoot(MainGrid().XamlRoot());
        co_await dialog.as<implementation::AddDownloadDialog>()->ShowAsync();
    }
}
