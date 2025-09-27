#include "Views/MainWindow.xaml.h"
#if __has_include("Views/MainWindow.g.cpp")
#include "Views/MainWindow.g.cpp"
#endif
#include <cmath>
#include <stdexcept>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>
#include <libnick/notifications/appnotification.h>
#include "Controls/DownloadRow.xaml.h"
#include "Controls/SettingsRow.xaml.h"
#include "Helpers/WinUIHelpers.h"
#include "Views/AddDownloadDialog.xaml.h"
#include "Views/CredentialDialog.xaml.h"
#include "Views/KeyringPage.xaml.h"
#include "Views/SettingsPage.xaml.h"

using namespace ::Nickvision::Events;
using namespace ::Nickvision::Helpers;
using namespace ::Nickvision::Notifications;
using namespace ::Nickvision::TubeConverter::Shared::Controllers;
using namespace ::Nickvision::TubeConverter::Shared::Events;
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
using namespace winrt::Windows::ApplicationModel::DataTransfer;
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
    UpdateCenter,
    Custom
};

enum UpdateCenterPage
{
    NoUpdates = 0,
    UpdatesAvailable,
    DownloadingUpdate,
};

enum ListPage
{
    None = 0,
    Has
};

namespace winrt::Nickvision::TubeConverter::WinUI::Views::implementation
{
    MainWindow::MainWindow()
        : m_opened{ false },
        m_notificationClickToken{ 0 },
        m_updateClickToken{ 0 }
    {
        InitializeComponent();
        this->m_inner.as<::IWindowNative>()->get_WindowHandle(&m_hwnd);
        AppWindow().SetIcon(L"resources\\icon.ico");
        ExtendsContentIntoTitleBar(true);
        SetTitleBar(TitleBar());
        AppWindow().TitleBar().PreferredHeightOption(TitleBarHeightOption::Tall);
    }

    void MainWindow::Controller(const std::shared_ptr<MainWindowController>& controller)
    {
        m_controller = controller;
        //Register Events
        AppWindow().Closing({ this, &MainWindow::OnClosing });
        m_controller->configurationSaved() += [this](const EventArgs& args){ DispatcherQueue().TryEnqueue([this, args](){ OnConfigurationSaved(args); }); };
        m_controller->notificationSent() += [this](const NotificationSentEventArgs& args){ DispatcherQueue().TryEnqueue([this, args](){ OnNotificationSent(args); }); };
        m_controller->appUpdateAvailable() += [this](const ParamEventArgs<Version>& args){ DispatcherQueue().TryEnqueue([this, args](){ OnAppUpdateAvailable(args); }); };
        m_controller->appUpdateProgressChanged() += [this](const ParamEventArgs<double>& args){ DispatcherQueue().TryEnqueue([this, args](){ OnAppUpdateProgressChanged(args); }); };
        m_controller->ytdlpUpdateAvailable() += [this](const ParamEventArgs<Version>& args){ DispatcherQueue().TryEnqueue([this, args](){ OnYtdlpUpdateAvailable(args); }); };
        m_controller->ytdlpUpdateProgressChanged() += [this](const ParamEventArgs<double>& args){ DispatcherQueue().TryEnqueue([this, args](){ OnYtdlpUpdateProgressChanged(args); }); };
        m_controller->historyChanged() += [this](const ParamEventArgs<std::vector<HistoricDownload>>& args){ DispatcherQueue().TryEnqueue([this, args](){ OnHistoryChanged(args); }); };
        m_controller->downloadCredentialNeeded() += [this](const DownloadCredentialNeededEventArgs& args){ OnDownloadCredentialNeeded(args); };
        m_controller->downloadAdded() += [this](const DownloadAddedEventArgs& args){ DispatcherQueue().TryEnqueue([this, args](){ OnDownloadAdded(args); }); };
        m_controller->downloadCompleted() += [this](const DownloadCompletedEventArgs& args){ DispatcherQueue().TryEnqueue([this, args](){ OnDownloadCompleted(args); }); };
        m_controller->downloadProgressChanged() += [this](const DownloadProgressChangedEventArgs& args){ DispatcherQueue().TryEnqueue([this, args](){ OnDownloadProgressChanged(args); }); };
        m_controller->downloadStopped() += [this](const ParamEventArgs<int>& args){ DispatcherQueue().TryEnqueue([this, args](){ OnDownloadStopped(args); }); };
        m_controller->downloadPaused() += [this](const ParamEventArgs<int>& args){ DispatcherQueue().TryEnqueue([this, args](){ OnDownloadPaused(args); }); };
        m_controller->downloadResumed() += [this](const ParamEventArgs<int>& args){ DispatcherQueue().TryEnqueue([this, args](){ OnDownloadResumed(args); }); };
        m_controller->downloadRetried() += [this](const ParamEventArgs<int>& args){ DispatcherQueue().TryEnqueue([this, args](){ OnDownloadRetried(args); }); };
        m_controller->downloadStartedFromQueue() += [this](const ParamEventArgs<int>& args){ DispatcherQueue().TryEnqueue([this, args](){ OnDownloadStartedFromQueue(args); }); };
        //Localize Strings
        AppWindow().Title(winrt::to_hstring(m_controller->getAppInfo().getShortName()));
        TitleBar().Title(winrt::to_hstring(m_controller->getAppInfo().getShortName()));
        TitleBar().Subtitle(m_controller->getAppInfo().getVersion().getVersionType() == VersionType::Preview ? winrt::to_hstring(_("Preview")) : L"");
        TitleBarSearch().PlaceholderText(winrt::to_hstring(_("Search")));
        NavViewHome().Content(winrt::box_value(winrt::to_hstring(_("Home"))));
        NavViewKeyring().Content(winrt::box_value(winrt::to_hstring(_("Keyring"))));
        NavViewHistory().Content(winrt::box_value(winrt::to_hstring(_("History"))));
        NavViewDownloads().Content(winrt::box_value(winrt::to_hstring(_("Downloads"))));
        NavViewDownloading().Content(winrt::box_value(winrt::to_hstring(_("Downloading"))));
        NavViewQueued().Content(winrt::box_value(winrt::to_hstring(_("Queued"))));
        NavViewCompleted().Content(winrt::box_value(winrt::to_hstring(_("Completed"))));
        NavViewUpdates().Content(winrt::box_value(winrt::to_hstring(_("Updates"))));
        NavViewSettings().Content(winrt::box_value(winrt::to_hstring(_("Settings"))));
        LblGreeting().Text(winrt::to_hstring(_("Download Media")));
        LblGettingStarted().Text(winrt::to_hstring(_("Add a video, audio, or playlist URL to start downloading")));
        LblHomeStart().Text(winrt::to_hstring(_("Start")));
        LblHomeAddDownload().Text(winrt::to_hstring(_("Add Download")));
        BtnHomeDocumentation().Label(winrt::to_hstring(_("Documentation")));
        BtnHomeGitHubRepo().Label(winrt::to_hstring(_("GitHub Repo")));
        BtnHomeReportABug().Label(winrt::to_hstring(_("Report a Bug")));
        BtnHomeDiscussions().Label(winrt::to_hstring(_("Discussions")));
        LblHistoryTitle().Text(winrt::to_hstring(_("History")));
        PageNoHistory().Title(winrt::to_hstring(_("No History Available")));
        LblHistoryAddDownload().Text(winrt::to_hstring(_("Add Download")));
        LblHistoryClearHistory().Text(winrt::to_hstring(_("Clear")));
        LblDownloadingTitle().Text(winrt::to_hstring(_("Downloading")));
        PageNoDownloading().Title(winrt::to_hstring(_("No Downloads Running")));
        LblDownloadingAddDownload().Text(winrt::to_hstring(_("Add Download")));
        ToolTipService::SetToolTip(BtnStopAllDownloads(), winrt::box_value(winrt::to_hstring(_("Stop All Downloads"))));
        LblStopAllDownloads().Text(winrt::to_hstring(_("Stop")));
        LblQueuedTitle().Text(winrt::to_hstring(_("Queued")));
        PageNoQueued().Title(winrt::to_hstring(_("No Downloads Queued")));
        LblQueuedAddDownload().Text(winrt::to_hstring(_("Add Download")));
        ToolTipService::SetToolTip(BtnClearQueuedDownloads(), winrt::box_value(winrt::to_hstring(_("Clear Queued Downloads"))));
        LblClearQueuedDownloads().Text(winrt::to_hstring(_("Clear")));
        LblCompletedTitle().Text(winrt::to_hstring(_("Completed")));
        PageNoCompleted().Title(winrt::to_hstring(_("No Downloads Completed")));
        LblCompletedAddDownload().Text(winrt::to_hstring(_("Add Download")));
        ToolTipService::SetToolTip(BtnRetryFailedDownloads(), winrt::box_value(winrt::to_hstring(_("Retry Failed Downloads"))));
        LblRetryFailedDownloads().Text(winrt::to_hstring(_("Retry")));
        ToolTipService::SetToolTip(BtnClearCompletedDownloads(), winrt::box_value(winrt::to_hstring(_("Clear Completed Downloads"))));
        LblClearCompletedDownloads().Text(winrt::to_hstring(_("Clear")));
        LblUpdateCenter().Text(winrt::to_hstring(_("Updates")));
        LblNoUpdates().Text(winrt::to_hstring(_("You're up to date")));
        LblNoUpdatesDetails().Text(winrt::to_hstring(_("We will let you know once app or yt-dlp updates are available to install")));
        LblUpdatesAvailable().Text(winrt::to_hstring(_("There is an update available")));
        LblDownloadUpdate().Text(winrt::to_hstring(_("Download")));
        LblUpdatesDownloading().Text(winrt::to_hstring(_("Downloading the update")));
        LblUpdatesOptions().Text(winrt::to_hstring(_("Options")));
        RowUpdatesBeta().Title(winrt::to_hstring(_("Receive Beta App Updates")));
        RowUpdatesBeta().Description(winrt::to_hstring(_f("Check for pre-release (beta) versions of {}", m_controller->getAppInfo().getShortName())));
        TglUpdatesBeta().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglUpdatesBeta().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        LblUpdatesChangelogTitle().Text(winrt::to_hstring(_("Changelog")));
        LblUpdatesChangelogVersion().Text(winrt::to_hstring(_f("Version {}", m_controller->getAppInfo().getVersion().str())));
        LblUpdatesChangelog().Text(winrt::to_hstring(m_controller->getAppInfo().getChangelog()));
        BtnCopyDebugInformation().Label(winrt::to_hstring(_("Copy Debug Information")));
        //Load
        TglUpdatesBeta().IsOn(m_controller->getPreferredUpdateType() == VersionType::Preview);
    }

    void MainWindow::SystemTheme(ElementTheme theme)
    {
        m_systemTheme = theme;
    }

    Windows::Foundation::IAsyncAction MainWindow::OnLoaded(const IInspectable& sender, const RoutedEventArgs& args)
    {
        if (!m_controller)
        {
            throw std::logic_error("MainWindow::SetController() must be called before using the window.");
        }
        if(m_opened)
        {
            co_return;
        }
        //Load UI
        ViewStackDownloading().CurrentPageIndex(0);
        ViewStackQueued().CurrentPageIndex(0);
        ViewStackCompleted().CurrentPageIndex(0);
        ViewStackUpdateCenter().CurrentPageIndex(UpdateCenterPage::NoUpdates);
        NavViewHome().IsSelected(true);
        //Startup
        const StartupInformation& info{ m_controller->startup() };
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
        if(info.showDisclaimer())
        {
            TextBlock txt;
            txt.Text(winrt::to_hstring(_("Videos on YouTube and other sites may be subject to DMCA protection. The authors of Parabolic do not endorse, and are not responsible for, the use of this application in means that will violate these laws.")));
            txt.TextWrapping(TextWrapping::WrapWholeWords);
            CheckBox chk;
            chk.Content(winrt::box_value(winrt::to_hstring(_("Don't show this message again"))));
            StackPanel panel;
            panel.Orientation(Orientation::Vertical);
            panel.Spacing(6);
            panel.Children().Append(txt);
            panel.Children().Append(chk);
            ContentDialog dialog;
            dialog.Title(winrt::box_value(winrt::to_hstring(_("Legal Copyright Disclaimer"))));
            dialog.Content(panel);
            dialog.CloseButtonText(winrt::to_hstring(_("I understand")));
            dialog.DefaultButton(ContentDialogButton::Close);
            dialog.RequestedTheme(MainGrid().RequestedTheme());
            dialog.XamlRoot(MainGrid().XamlRoot());
            co_await dialog.ShowAsync();
            m_controller->setShowDisclaimerOnStartup(!chk.IsChecked().Value());
        }
        if(info.hasRecoverableDownloads())
        {
            ContentDialog dialog;
            dialog.Title(winrt::box_value(winrt::to_hstring(_("Recover Crashed Downloads?"))));
            dialog.Content(winrt::box_value(winrt::to_hstring(_("There are downloads available to recover from when Parabolic crashed. Parabolic will try to download them again."))));
            dialog.PrimaryButtonText(winrt::to_hstring(_("Recover")));
            dialog.CloseButtonText(winrt::to_hstring(_("Cancel")));
            dialog.DefaultButton(ContentDialogButton::Primary);
            dialog.RequestedTheme(MainGrid().RequestedTheme());
            dialog.XamlRoot(MainGrid().XamlRoot());
            ContentDialogResult res{ co_await dialog.ShowAsync() };
            m_controller->recoverDownloads(res != ContentDialogResult::Primary);
        }
        if(!info.getUrlToValidate().empty())
        {
            co_await AddDownload(winrt::to_hstring(info.getUrlToValidate()));
        }
        m_opened = true;
    }

    Windows::Foundation::IAsyncAction MainWindow::OnClosing(const Microsoft::UI::Windowing::AppWindow& sender, const AppWindowClosingEventArgs& args)
    {
        if(!m_controller->canShutdown())
        {
            args.Cancel(true);
            ContentDialog dialog;
            dialog.Title(winrt::box_value(winrt::to_hstring(m_controller->getAppInfo().getShortName())));
            dialog.Content(winrt::box_value(winrt::to_hstring(_("There are downloads in progress. Exiting will stop all downloads."))));
            dialog.PrimaryButtonText(winrt::to_hstring(_("Exit")));
            dialog.CloseButtonText(winrt::to_hstring(_("Cancel")));
            dialog.DefaultButton(ContentDialogButton::Primary);
            dialog.RequestedTheme(MainGrid().RequestedTheme());
            dialog.XamlRoot(MainGrid().XamlRoot());
            ContentDialogResult res{ co_await dialog.ShowAsync() };
            if(res == ContentDialogResult::Primary)
            {
                m_controller->stopAllDownloads();
                Close();
            }
            co_return;
        }
        m_controller->shutdown({ AppWindow().Size().Width, AppWindow().Size().Height, static_cast<bool>(IsZoomed(m_hwnd)), AppWindow().Position().X, AppWindow().Position().Y });
    }

    void MainWindow::OnActivated(const IInspectable& sender, const WindowActivatedEventArgs& args)
    {
        m_controller->setIsWindowActive(args.WindowActivationState() != WindowActivationState::Deactivated);
    }

    void MainWindow::OnPaneToggleRequested(const Microsoft::UI::Xaml::Controls::TitleBar& sender, const IInspectable& args)
    {
        NavView().IsPaneOpen(!NavView().IsPaneOpen());
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
        BtnInfoBar().Visibility(!args.getAction().empty() ? Visibility::Visible : Visibility::Collapsed);
        InfoBar().IsOpen(true);
    }

    void MainWindow::OnAppUpdateAvailable(const ParamEventArgs<Version>& args)
    {
        if(m_updateClickToken)
        {
            BtnDownloadUpdate().Click(m_updateClickToken);
            m_updateClickToken = {};
        }
        InfoBadgeUpdates().Visibility(Visibility::Visible);
        ViewStackUpdateCenter().CurrentPageIndex(UpdateCenterPage::UpdatesAvailable);
        LblUpdatesAvailableDetails().Text(winrt::to_hstring(_f("{} version {} is available to download and install", m_controller->getAppInfo().getShortName(), args->str())));
        m_updateClickToken = BtnDownloadUpdate().Click([this](const IInspectable&, const RoutedEventArgs&)
        {
            m_controller->startWindowsUpdate();
        });
    }

    void MainWindow::OnAppUpdateProgressChanged(const ParamEventArgs<double>& args)
    {
        ViewStackUpdateCenter().CurrentPageIndex(UpdateCenterPage::DownloadingUpdate);
        LblUpdatesDownloadingDetails().Text(winrt::to_hstring(_("The installer will start once the download is complete")));
        LblUpdatesDownloadingProgress().Text(winrt::to_hstring(_f("Downloading {}%", static_cast<int>(std::round((*args) * 100.0)))));
        if(*args == 1.0)
        {
            InfoBadgeUpdates().Visibility(Visibility::Collapsed);
            ViewStackUpdateCenter().CurrentPageIndex(UpdateCenterPage::NoUpdates);
        }
    }

    void MainWindow::OnAppUpdateBetaToggled(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args)
    {
        m_controller->setPreferredUpdateType(TglUpdatesBeta().IsOn() ? VersionType::Preview : VersionType::Stable);
    }

    void MainWindow::OnYtdlpUpdateAvailable(const ParamEventArgs<Version>& args)
    {
        if(m_updateClickToken)
        {
            BtnDownloadUpdate().Click(m_updateClickToken);
            m_updateClickToken = {};
        }
        InfoBadgeUpdates().Visibility(Visibility::Visible);
        ViewStackUpdateCenter().CurrentPageIndex(UpdateCenterPage::UpdatesAvailable);
        LblUpdatesAvailableDetails().Text(winrt::to_hstring(_f("{} version {} is available to download and install", "yt-dlp", args->str())));
        m_updateClickToken = BtnDownloadUpdate().Click([this](const IInspectable&, const RoutedEventArgs&)
        {
            m_controller->startYtdlpUpdate();
        });
    }

    void MainWindow::OnYtdlpUpdateProgressChanged(const ParamEventArgs<double>& args)
    {
        ViewStackUpdateCenter().CurrentPageIndex(UpdateCenterPage::DownloadingUpdate);
        LblUpdatesDownloadingDetails().Text(L"");
        LblUpdatesDownloadingProgress().Text(winrt::to_hstring(_f("Downloading {}%", static_cast<int>(std::round((*args) * 100.0)))));
        if(*args == 1.0)
        {
            InfoBadgeUpdates().Visibility(Visibility::Collapsed);
            ViewStackUpdateCenter().CurrentPageIndex(UpdateCenterPage::NoUpdates);
        }
    }

    void MainWindow::OnHistoryChanged(const ParamEventArgs<std::vector<HistoricDownload>>& args)
    {
        ViewStackHistory().CurrentPageIndex(ListPage::None);
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
                m_controller->removeHistoricDownload(download);
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
            ViewStackHistory().CurrentPageIndex(ListPage::Has);
            ListHistory().Children().Append(row);
        }
    }

    winrt::fire_and_forget MainWindow::OnDownloadCredentialNeeded(const DownloadCredentialNeededEventArgs& args)
    {
        ContentDialog dialog{ winrt::make<implementation::CredentialDialog>() };
        dialog.as<implementation::CredentialDialog>()->Controller(m_controller->createCredentialDialogController(args));
        dialog.RequestedTheme(MainGrid().RequestedTheme());
        dialog.XamlRoot(MainGrid().XamlRoot());
        co_await dialog.as<implementation::CredentialDialog>()->ShowAsync();
    }

    void MainWindow::OnDownloadAdded(const DownloadAddedEventArgs& args)
    {
        Controls::DownloadRow row{ winrt::make<Controls::implementation::DownloadRow>() };
        row.PauseRequested([this, args](const IInspectable&, const RoutedEventArgs&){ m_controller->pauseDownload(args.getId()); });
        row.ResumeRequested([this, args](const IInspectable&, const RoutedEventArgs&){ m_controller->resumeDownload(args.getId()); });
        row.StopRequested([this, args](const IInspectable&, const RoutedEventArgs&){ m_controller->stopDownload(args.getId()); });
        row.RetryRequested([this, args](const IInspectable&, const RoutedEventArgs&){ m_controller->retryDownload(args.getId()); });
        row.as<Controls::implementation::DownloadRow>()->TriggerAddedState(args);
        m_downloadRows[args.getId()] = row;
        if(args.getStatus() == DownloadStatus::Queued)
        {
            ListQueued().Children().Append(row);
            ViewStackQueued().CurrentPageIndex(ListPage::Has);
            if(NavViewHome().IsSelected())
            {
                NavViewQueued().IsSelected(true);
            }
            BadgeQueued().Value(static_cast<int>(m_controller->getQueuedCount()));
        }
        else
        {
            ListDownloading().Children().Append(row);
            ViewStackDownloading().CurrentPageIndex(ListPage::Has);
            if(NavViewHome().IsSelected())
            {
                NavViewDownloading().IsSelected(true);
            }
            BadgeDownloading().Value(static_cast<int>(m_controller->getDownloadingCount()));
        }
    }

    void MainWindow::OnDownloadCompleted(const DownloadCompletedEventArgs& args)
    {
        unsigned int index;
        m_downloadRows[args.getId()].as<Controls::implementation::DownloadRow>()->TriggerCompletedState(args);
        if(ListDownloading().Children().IndexOf(m_downloadRows[args.getId()], index))
        {
            ListDownloading().Children().RemoveAt(index);
        }
        ViewStackDownloading().CurrentPageIndex(m_controller->getDownloadingCount() > 0 ? ListPage::Has : ListPage::None);
        BadgeDownloading().Value(static_cast<int>(m_controller->getDownloadingCount()));
        ListCompleted().Children().Append(m_downloadRows[args.getId()]);
        ViewStackCompleted().CurrentPageIndex(ListPage::Has);
        BadgeCompleted().Value(static_cast<int>(m_controller->getCompletedCount()));
    }

    void MainWindow::OnDownloadProgressChanged(const DownloadProgressChangedEventArgs& args)
    {
        m_downloadRows[args.getId()].as<Controls::implementation::DownloadRow>()->TriggerProgressState(args);
    }

    void MainWindow::OnDownloadStopped(const ParamEventArgs<int>& args)
    {
        unsigned int index;
        m_downloadRows[*args].as<Controls::implementation::DownloadRow>()->TriggerStoppedState();
        if(ListDownloading().Children().IndexOf(m_downloadRows[*args], index))
        {
            ListDownloading().Children().RemoveAt(index);
        }
        ViewStackDownloading().CurrentPageIndex(m_controller->getDownloadingCount() > 0 ? ListPage::Has : ListPage::None);
        BadgeDownloading().Value(static_cast<int>(m_controller->getDownloadingCount()));
        if(ListQueued().Children().IndexOf(m_downloadRows[*args], index))
        {
            ListQueued().Children().RemoveAt(index);
        }
        ViewStackQueued().CurrentPageIndex(m_controller->getQueuedCount() > 0 ? ListPage::Has : ListPage::None);
        BadgeQueued().Value(static_cast<int>(m_controller->getQueuedCount()));
        ListCompleted().Children().Append(m_downloadRows[*args]);
        ViewStackCompleted().CurrentPageIndex(ListPage::Has);
        BadgeCompleted().Value(static_cast<int>(m_controller->getCompletedCount()));
    }

    void MainWindow::OnDownloadPaused(const ParamEventArgs<int>& args)
    {
        m_downloadRows[*args].as<Controls::implementation::DownloadRow>()->TriggerPausedState();
    }

    void MainWindow::OnDownloadResumed(const ParamEventArgs<int>& args)
    {
        m_downloadRows[*args].as<Controls::implementation::DownloadRow>()->TriggerResumedState();
    }

    void MainWindow::OnDownloadRetried(const ParamEventArgs<int>& args)
    {
        unsigned int index;
        if(ListCompleted().Children().IndexOf(m_downloadRows[*args], index))
        {
            ListCompleted().Children().RemoveAt(index);
        }
        ViewStackCompleted().CurrentPageIndex(m_controller->getCompletedCount() > 0 ? ListPage::Has : ListPage::None);
        BadgeCompleted().Value(static_cast<int>(m_controller->getCompletedCount()));
    }

    void MainWindow::OnDownloadStartedFromQueue(const ParamEventArgs<int>& args)
    {
        unsigned int index;
        m_downloadRows[*args].as<Controls::implementation::DownloadRow>()->TriggerStartedFromQueueState();
        if(ListQueued().Children().IndexOf(m_downloadRows[*args], index))
        {
            ListQueued().Children().RemoveAt(index);
        }
        ViewStackQueued().CurrentPageIndex(m_controller->getQueuedCount() > 0 ? ListPage::Has : ListPage::None);
        BadgeQueued().Value(static_cast<int>(m_controller->getQueuedCount()));
        ListDownloading().Children().Append(m_downloadRows[*args]);
        ViewStackDownloading().CurrentPageIndex(ListPage::Has);
        BadgeDownloading().Value(static_cast<int>(m_controller->getDownloadingCount()));
    }

    void MainWindow::OnTitleBarSearchChanged(const Microsoft::UI::Xaml::Controls::AutoSuggestBox& sender, const Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs& args)
    {
        if(args.Reason() == AutoSuggestionBoxTextChangeReason::UserInput)
        {
            IObservableVector<IInspectable> items{ winrt::single_threaded_observable_vector<IInspectable>() };
            if(StringHelpers::isValidUrl(StringHelpers::trim(winrt::to_string(sender.Text()))))
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
        else if(tag == L"Updates")
        {
            ViewStack().CurrentPageIndex(MainWindowPage::UpdateCenter);
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

    void MainWindow::CopyDebugInformation(const IInspectable& sender, const RoutedEventArgs& args)
    {
        DataPackage dataPackage;
        dataPackage.SetText(winrt::to_hstring(m_controller->getDebugInformation()));
        Clipboard::SetContent(dataPackage);
        AppNotification::send({ _("Debug information copied to clipboard"), NotificationSeverity::Success });
    }

    Windows::Foundation::IAsyncAction MainWindow::AddDownload(const IInspectable& sender, const RoutedEventArgs& args)
    {
        co_await AddDownload();
    }

    void MainWindow::ClearHistory(const IInspectable& sender, const RoutedEventArgs& args)
    {
        m_controller->clearHistory();
    }

    void MainWindow::StopAllDownloads(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args)
    {
        m_controller->stopAllDownloads();
    }

    void MainWindow::ClearQueuedDownloads(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args)
    {
        unsigned int index;
        for(int id : m_controller->clearQueuedDownloads())
        {
            if(ListQueued().Children().IndexOf(m_downloadRows[id], index))
            {
                ListQueued().Children().RemoveAt(index);
            }
            ViewStackQueued().CurrentPageIndex(ListPage::None);
            BadgeQueued().Value(0);
            m_downloadRows.erase(id);
        }
    }

    void MainWindow::RetryFailedDownloads(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args)
    {
        m_controller->retryFailedDownloads();
    }

    void MainWindow::ClearCompletedDownloads(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args)
    {
        unsigned int index;
        for(int id : m_controller->clearCompletedDownloads())
        {
            if(ListCompleted().Children().IndexOf(m_downloadRows[id], index))
            {
                ListCompleted().Children().RemoveAt(index);
            }
            ViewStackCompleted().CurrentPageIndex(ListPage::None);
            BadgeCompleted().Value(0);
            m_downloadRows.erase(id);
        }
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
