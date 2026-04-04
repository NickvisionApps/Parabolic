using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Input;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Network;
using Nickvision.Desktop.Notifications;
using Nickvision.Desktop.WinUI.Helpers;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Events;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.Shared.Services;
using Nickvision.Parabolic.WinUI.Controls;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Windows.System;

namespace Nickvision.Parabolic.WinUI.Views;

public sealed partial class MainWindow : Window
{
    private enum Pages
    {
        Home = 0,
        Downloads,
        Custom
    }

    private readonly IServiceProvider _serviceProvider;
    private readonly MainWindowController _controller;
    private readonly AppInfo _appInfo;
    private readonly ITranslationService _translationService;
    private readonly Dictionary<int, DownloadRow> _downloadRows;
    private RoutedEventHandler? _notificationClickHandler;

    public MainWindow(IServiceProvider serviceProvider, MainWindowController controller, AppInfo appInfo, IEventsService eventsService, ITranslationService translationService)
    {
        InitializeComponent();
        _serviceProvider = serviceProvider;
        _controller = controller;
        _appInfo = appInfo;
        _translationService = translationService;
        _downloadRows = new Dictionary<int, DownloadRow>();
        _notificationClickHandler = null;
        // Config
        AppWindow.TitleBar.PreferredTheme = _controller.Theme switch
        {
            Theme.Light => TitleBarTheme.Light,
            Theme.Dark => TitleBarTheme.Dark,
            _ => TitleBarTheme.UseDefaultAppMode
        };
        MainGrid.RequestedTheme = _controller.Theme switch
        {
            Theme.Light => ElementTheme.Light,
            Theme.Dark => ElementTheme.Dark,
            _ => ElementTheme.Default
        };
        this.Geometry = _controller.WindowGeometry;
        // TitleBar
        AppWindow.SetIcon(_appInfo.Version!.IsPreview ? "./Assets/org.nickvision.tubeconverter-devel.ico" : "./Assets/org.nickvision.tubeconverter.ico");
        ExtendsContentIntoTitleBar = true;
        SetTitleBar(TitleBar);
        AppWindow.TitleBar.PreferredHeightOption = TitleBarHeightOption.Tall;
        // Events
        AppWindow.Closing += Window_Closing;
        eventsService.AppNotificationSent += (sender, e) => DispatcherQueue.TryEnqueue(() => App_AppNotificationSent(sender, e));
        eventsService.ConfigurationSaved += App_ConfigurationSaved;
        eventsService.DatabasePasswordRequired += App_DatabasePasswordRequired;
        eventsService.DownloadAdded += (sender, e) => DispatcherQueue.TryEnqueue(() => Controller_DownloadAdded(sender, e));
        eventsService.DownloadCredentialRequired += Controller_DownloadCredentialRequired;
        eventsService.DownloadProgressChanged += (sender, e) => DispatcherQueue.TryEnqueue(() => Controller_DownloadProgressChanged(sender, e));
        eventsService.DownloadCompleted += (sender, e) => DispatcherQueue.TryEnqueue(() => Controller_DownloadCompleted(sender, e));
        eventsService.DownloadStopped += (sender, e) => DispatcherQueue.TryEnqueue(() => Controller_DownloadStopped(sender, e));
        eventsService.DownloadStartedFromQueue += (sender, e) => DispatcherQueue.TryEnqueue(() => Controller_DownloadStartedFromQueue(sender, e));
        eventsService.DownloadRetired += (sender, e) => DispatcherQueue.TryEnqueue(() => Controller_DownloadRetired(sender, e));
        eventsService.DownloadRequested += async (s, args) => await AddDownloadAsync(args.Url);
        // Translations
        AppWindow.Title = _appInfo.ShortName;
        TitleBar.Title = _appInfo.ShortName;
        TitleBar.Subtitle = _appInfo.Version!.IsPreview ? _translationService._("Preview") : string.Empty;
        NavItemHome.Content = _translationService._("Home");
        NavItemDownloads.Content = _translationService._("Downloads");
        NavItemHistory.Content = _translationService._("History");
        NavItemKeyring.Content = _translationService._("Keyring");
        NavItemUpdates.Content = _translationService._("Updating");
        NavItemHelp.Content = _translationService._("Help");
        MenuCheckForUpdates.Text = _translationService._("Check for Updates");
        MenuGitHubRepo.Text = _translationService._("GitHub Repo");
        MenuReportABug.Text = _translationService._("Report a Bug");
        MenuDiscussions.Text = _translationService._("Discussions");
        MenuAbout.Text = _translationService._("About {0}", _appInfo.ShortName!);
        NavItemSettings.Content = _translationService._("Settings");
        StatusHome.Title = _translationService._("Download Media");
        StatusHome.Description = _translationService._("Add a video, audio, or playlist URL to start downloading");
        LblHomeAddDownload.Text = _translationService._("Add Download");
        LblDownloads.Text = _translationService._("Downloads");
        LblDownloadsAddDownload.Text = _translationService._("Add");
        FilterAll.Content = _translationService._("All");
        FilterRunning.Content = _translationService._("Running");
        FilterQueued.Content = _translationService._("Queued");
        FilterCompleted.Content = _translationService._("Completed");
        FilterFailed.Content = _translationService._("Failed");
        BtnStopAllRemaining.Label = _translationService._("Stop All Remaining");
        BtnRetryAllFailed.Label = _translationService._("Retry All Failed");
        BtnClearAllQueued.Label = _translationService._("Clear All Queued");
        BtnClearAllCompleted.Label = _translationService._("Clear All Completed");
        StatusNoneDownloads.Title = _translationService._("No Downloads");
        StatusNoneDownloads.Description = _translationService._("There are no downloads of this type");
        DlgCredential.Title = _translationService._("Credential Required");
        TxtCredentialUsername.PlaceholderText = _translationService._("Enter username here");
        TxtCredentialPassword.PlaceholderText = _translationService._("Enter password here");
        DlgCredential.PrimaryButtonText = _translationService._("Submit");
        DlgCredential.CloseButtonText = _translationService._("Cancel");
    }

    private async void Window_Loaded(object? sender, RoutedEventArgs e)
    {
        ViewStackDownloads.SelectedIndex = 0;
        DownloadsFilter.SelectedIndex = 0;
        MenuCheckForUpdates.IsEnabled = false;
        var updatesTask = _controller.CheckForUpdatesAsync(false);
        if (_controller.ShowDisclaimerOnStartup)
        {
            var checkBox = new CheckBox()
            {
                Content = _translationService._("Don't show this message again")
            };
            var disclaimerDialog = new ContentDialog()
            {
                Title = _translationService._("Legal Copyright Disclaimer"),
                Content = new StackPanel()
                {
                    Orientation = Orientation.Vertical,
                    Spacing = 6,
                    Children =
                    {
                        new TextBlock()
                        {
                            Text = _translationService._("Videos on YouTube and other sites may be subject to DMCA protection. The authors of Parabolic do not endorse, and are not responsible for, the use of this application in means that will violate these laws."),
                            TextWrapping = TextWrapping.WrapWholeWords
                        },
                        checkBox
                    }
                },
                CloseButtonText = _translationService._("I understand"),
                DefaultButton = ContentDialogButton.Close,
                RequestedTheme = MainGrid.ActualTheme,
                XamlRoot = MainGrid.XamlRoot
            };
            await disclaimerDialog.ShowAsync();
            if (checkBox.IsChecked ?? false)
            {
                _controller.ShowDisclaimerOnStartup = false;
            }
        }
        if (_controller.RecoverableDownloadsCount > 0)
        {
            var recoverDialog = new ContentDialog()
            {
                Title = _translationService._("Recover Downloads?"),
                Content = _translationService._("There are downloads available to recover from when Parabolic crashed. Would you like to download them again?"),
                PrimaryButtonText = _translationService._("Yes"),
                CloseButtonText = _translationService._("No"),
                DefaultButton = ContentDialogButton.Primary,
                RequestedTheme = MainGrid.ActualTheme,
                XamlRoot = MainGrid.XamlRoot
            };
            if ((await recoverDialog.ShowAsync()) == ContentDialogResult.Primary)
            {
                await _controller.RecoverAllDownloadsAsync();
            }
            else
            {
                await _controller.ClearRecoverableDownloadsAsync();
            }
        }
        if (_controller.UrlFromArgs is not null)
        {
            await AddDownloadAsync(_controller.UrlFromArgs);
        }
        await updatesTask;
        MenuCheckForUpdates.IsEnabled = true;
    }

    private async void Window_Closing(AppWindow sender, AppWindowClosingEventArgs e)
    {
        if (!_controller.CanShutdown)
        {
            e.Cancel = true;
            var confirmDialog = new ContentDialog()
            {
                Title = _appInfo.ShortName,
                Content = _translationService._("There are downloads still in progress. Would you like to stop them and exit?"),
                PrimaryButtonText = _translationService._("Yes"),
                CloseButtonText = _translationService._("No"),
                DefaultButton = ContentDialogButton.Close,
                RequestedTheme = MainGrid.ActualTheme,
                XamlRoot = MainGrid.XamlRoot
            };
            if ((await confirmDialog.ShowAsync()) == ContentDialogResult.Primary)
            {
                _controller.WindowGeometry = this.Geometry;
                await _controller.StopAllDownloadsAsync();
                Close();
                _serviceProvider.GetRequiredService<IHostApplicationLifetime>().StopApplication();
            }
            return;
        }
        _controller.WindowGeometry = this.Geometry;
        _serviceProvider.GetRequiredService<IHostApplicationLifetime>().StopApplication();
    }

    private void TitleBar_PaneToggleRequested(TitleBar sender, object e)
    {
        NavView.IsPaneOpen = !NavView.IsPaneOpen;
    }

    private void NavView_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs e)
    {
        if (e.SelectedItem is NavigationViewItem item)
        {
            var tag = item.Tag as string;
            FrameCustom.Content = tag switch
            {
                "History" => _serviceProvider.GetRequiredService<HistoryPage>(),
                "Keyring" => _serviceProvider.GetRequiredService<KeyringPage>(),
                "Settings" => _serviceProvider.GetRequiredService<SettingsPage>(),
                _ => null
            };
            if (tag == "Settings")
            {
                (FrameCustom.Content as SettingsPage)!.WindowId = AppWindow.Id;
            }
            ViewStack.SelectedIndex = tag switch
            {
                "Downloads" => (int)Pages.Downloads,
                "History" => (int)Pages.Custom,
                "Keyring" => (int)Pages.Custom,
                "Settings" => (int)Pages.Custom,
                _ => (int)Pages.Home
            };
        }
    }

    private void NavItem_Tapped(object sender, TappedRoutedEventArgs e) => FlyoutBase.ShowAttachedFlyout(sender as FrameworkElement);

    private void App_AppNotificationSent(object? sender, AppNotificationSentEventArgs e)
    {
        if (_notificationClickHandler is not null)
        {
            BtnInfoBar.Click -= _notificationClickHandler;
            _notificationClickHandler = null;
        }
        InfoBar.Message = e.Notification.Message;
        InfoBar.Severity = e.Notification.Severity switch
        {
            NotificationSeverity.Success => InfoBarSeverity.Success,
            NotificationSeverity.Warning => InfoBarSeverity.Warning,
            NotificationSeverity.Error => InfoBarSeverity.Error,
            _ => InfoBarSeverity.Informational
        };
        if (e.Notification.Action == "update")
        {
            BtnInfoBar.Content = _translationService._("Update");
            _notificationClickHandler = WindowsUpdate;
            BtnInfoBar.Click += _notificationClickHandler;
        }
        else if (e.Notification.Action == "update-ytdlp")
        {
            BtnInfoBar.Content = _translationService._("Update");
            _notificationClickHandler = YtdlpUpdate;
            BtnInfoBar.Click += _notificationClickHandler;
        }
        else if (e.Notification.Action == "update-deno")
        {
            BtnInfoBar.Content = _translationService._("Update");
            _notificationClickHandler = DenoUpdate;
            BtnInfoBar.Click += _notificationClickHandler;
        }
        else if (e.Notification.Action == "error" && !string.IsNullOrEmpty(e.Notification.ActionParam))
        {
            BtnInfoBar.Content = _translationService._("Details");
            _notificationClickHandler = async (_, _) =>
            {
                InfoBar.IsOpen = false;
                var errorDialog = new ContentDialog()
                {
                    Title = _translationService._("Error"),
                    Content = new ScrollViewer()
                    {
                        HorizontalScrollBarVisibility = ScrollBarVisibility.Disabled,
                        VerticalScrollBarVisibility = ScrollBarVisibility.Visible,
                        Content = new TextBlock()
                        {
                            Text = e.Notification.ActionParam,
                            TextWrapping = TextWrapping.Wrap
                        }
                    },
                    CloseButtonText = _translationService._("Close"),
                    DefaultButton = ContentDialogButton.Close,
                    RequestedTheme = MainGrid.ActualTheme,
                    XamlRoot = MainGrid.XamlRoot
                };
                await errorDialog.ShowAsync();
            };
            BtnInfoBar.Click += _notificationClickHandler;
        }
        BtnInfoBar.Visibility = _notificationClickHandler is not null ? Visibility.Visible : Visibility.Collapsed;
        InfoBar.IsOpen = true;
    }

    private void App_ConfigurationSaved(object? sender, ConfigurationSavedEventArgs args)
    {
        if (args.ChangedPropertyName == "Theme")
        {
            AppWindow.TitleBar.PreferredTheme = _controller.Theme switch
            {
                Theme.Light => TitleBarTheme.Light,
                Theme.Dark => TitleBarTheme.Dark,
                _ => TitleBarTheme.UseDefaultAppMode
            };
            MainGrid.RequestedTheme = _controller.Theme switch
            {
                Theme.Light => ElementTheme.Light,
                Theme.Dark => ElementTheme.Dark,
                _ => ElementTheme.Default
            };
        }
    }

    private async void App_DatabasePasswordRequired(object? sender, PasswordRequiredEventArgs args)
    {
        var passwordBox = new PasswordBox()
        {
            PlaceholderText = _translationService._("Enter password here")
        };
        var stackPanel = new StackPanel()
        {
            Orientation = Orientation.Vertical,
            Spacing = 12
        };
        stackPanel.Children.Add(new TextBlock()
        {
            Text = _translationService._("This app stores data in an encrypted database. As the system credential manager (secret service) is not available, please provide a password to use to encrypt the database.\n\nIf you've already provided a password, please provide it again to unlock the database."),
            TextWrapping = TextWrapping.WrapWholeWords
        });
        stackPanel.Children.Add(passwordBox);
        var contentDialog = new ContentDialog()
        {
            Title = _translationService._("Password Required"),
            Content = stackPanel,
            PrimaryButtonText = _translationService._("Submit"),
            DefaultButton = ContentDialogButton.Primary
        };
        while (string.IsNullOrEmpty(args.Password))
        {
            var res = await contentDialog.ShowAsync();
            if (res == ContentDialogResult.Primary)
            {
                args.Password = passwordBox.Password;
            }
        }
    }


    private async void Controller_DownloadAdded(object? sender, DownloadAddedEventArgs e)
    {
        var row = _serviceProvider.GetRequiredService<DownloadRow>();
        row.PauseRequested += DownloadRow_PauseRequested;
        row.ResumeRequested += DownloadRow_ResumeRequested;
        row.StopRequested += DownloadRow_StopRequested;
        row.RetryRequested += DownloadRow_RetryRequested;
        await row.TriggerAddedStateAsync(e);
        _downloadRows[e.Id] = row;
        UpdateDownloadsList();
        NavItemDownloads.IsSelected = true;
    }

    private void Controller_DownloadCompleted(object? sender, DownloadCompletedEventArgs e)
    {
        if (_downloadRows.TryGetValue(e.Id, out var row))
        {
            row.TriggerCompletedState(e);
            UpdateDownloadsList();
        }
    }

    private async void Controller_DownloadCredentialRequired(object? sender, DownloadCredentialRequiredEventArgs e)
    {
        LblCredentialRequired.Text = _translationService._("A credential is required to continue the download of \"{0}\".", e.Credential.Name);
        TxtCredentialUrl.Text = e.Credential.Url.ToString();
        TxtCredentialUsername.Text = string.Empty;
        TxtCredentialPassword.Password = string.Empty;
        DlgCredential.XamlRoot = MainGrid.XamlRoot;
        DlgCredential.RequestedTheme = MainGrid.ActualTheme;
        if ((await DlgCredential.ShowAsync()) == ContentDialogResult.Primary)
        {
            e.Credential.Username = TxtCredentialUsername.Text;
            e.Credential.Password = TxtCredentialPassword.Password;
        }
    }

    private void Controller_DownloadProgressChanged(object? sender, DownloadProgressChangedEventArgs e)
    {
        if (_downloadRows.TryGetValue(e.Id, out var row))
        {
            row.TriggerProgressState(e);
        }
    }

    private void Controller_DownloadStartedFromQueue(object? sender, DownloadEventArgs e)
    {
        if (_downloadRows.TryGetValue(e.Id, out var row))
        {
            row.TriggerStartedFromQueueState();
            UpdateDownloadsList();
        }
    }

    private void Controller_DownloadStopped(object? sender, DownloadEventArgs e)
    {
        if (_downloadRows.TryGetValue(e.Id, out var row))
        {
            row.TriggerStoppedState();
            UpdateDownloadsList();
        }
    }

    private void Controller_DownloadRetired(object? sender, DownloadEventArgs e)
    {
        if (_downloadRows.TryGetValue(e.Id, out var row))
        {
            _downloadRows.Remove(e.Id);
            UpdateDownloadsList();
        }
    }

    private void DownloadsFilter_SelectionChanged(object sender, SelectionChangedEventArgs e) => UpdateDownloadsList();

    private void DownloadRow_PauseRequested(object? sender, int id)
    {
        if (_controller.PauseDownload(id) && _downloadRows.TryGetValue(id, out var row))
        {
            row.TriggerPausedState();
        }
    }

    private void DownloadRow_ResumeRequested(object? sender, int id)
    {
        if (_controller.ResumeDownload(id) && _downloadRows.TryGetValue(id, out var row))
        {
            row.TriggerResumedState();
        }
    }

    private async void DownloadRow_RetryRequested(object? sender, int id) => await _controller.RetryDownloadAsync(id);

    private async void DownloadRow_StopRequested(object? sender, int id) => await _controller.StopDownloadAsync(id);

    private void UpdateProgress_Changed(object? sender, DownloadProgress e)
    {
        DispatcherQueue.TryEnqueue(() =>
        {
            if (e.Completed)
            {
                FlyoutProgress.Hide();
                NavItemUpdates.Visibility = Visibility.Collapsed;
                return;
            }
            var message = _translationService._("Downloading update: {0}%", Math.Round(e.Percentage * 100));
            NavItemUpdates.Visibility = Visibility.Visible;
            StsProgress.Description = message;
            BarProgress.Value = e.Percentage * 100;
        });
    }

    private async void About(object? sender, RoutedEventArgs e)
    {
        var progressDialog = new ContentDialog()
        {
            Title = _translationService._("About {0}", _appInfo.ShortName!),
            Content = new ProgressRing()
            {
                IsActive = true,
            },
            RequestedTheme = MainGrid.ActualTheme,
            XamlRoot = MainGrid.XamlRoot
        };
        DispatcherQueue.TryEnqueue(async () => await progressDialog.ShowAsync());
        var aboutDialog = _serviceProvider.GetRequiredService<AboutDialog>();
        aboutDialog.DebugInformation = await _controller.GetDebugInformationAsync();
        aboutDialog.RequestedTheme = MainGrid.ActualTheme;
        aboutDialog.XamlRoot = MainGrid.XamlRoot;
        progressDialog.Hide();
        await aboutDialog.ShowAsync();
    }

    private async void AddDownload(object? sender, RoutedEventArgs e) => await AddDownloadAsync(null);

    private async void CheckForUpdates(object? sender, RoutedEventArgs e)
    {
        MenuCheckForUpdates.IsEnabled = false;
        await _controller.CheckForUpdatesAsync(true);
        MenuCheckForUpdates.IsEnabled = true;
    }

    private void ClearAllCompleted(object? sender, RoutedEventArgs e)
    {
        foreach (var id in _controller.ClearCompletedDownloads())
        {
            _downloadRows.Remove(id);
        }
        UpdateDownloadsList();
    }

    private void ClearAllQueued(object? sender, RoutedEventArgs e)
    {
        foreach (var id in _controller.ClearQueuedDownloads())
        {
            _downloadRows.Remove(id);
        }
        UpdateDownloadsList();
    }

    private async void DenoUpdate(object? sender, RoutedEventArgs e)
    {
        var progress = new Progress<DownloadProgress>();
        progress.ProgressChanged += UpdateProgress_Changed;
        InfoBar.IsOpen = false;
        await _controller.DenoUpdateAsync(progress);
        progress.ProgressChanged -= UpdateProgress_Changed;
    }

    private async void Discussions(object? sender, RoutedEventArgs e) => await LaunchUriAsync(_appInfo.DiscussionsForum);

    private async void GitHubRepo(object? sender, RoutedEventArgs e) => await LaunchUriAsync(_appInfo.SourceRepository);

    private async void ReportABug(object? sender, RoutedEventArgs e) => await LaunchUriAsync(_appInfo.IssueTracker);

    private async void RetryAllFailed(object? sender, RoutedEventArgs e) => await _controller.RetryFailedDownloadsAsync();

    private async void StopAllRemaining(object? sender, RoutedEventArgs e) => await _controller.StopAllDownloadsAsync();

    private async void WindowsUpdate(object? sender, RoutedEventArgs e)
    {
        var progress = new Progress<DownloadProgress>();
        progress.ProgressChanged += UpdateProgress_Changed;
        InfoBar.IsOpen = false;
        await _controller.WindowsUpdateAsync(progress);
        progress.ProgressChanged -= UpdateProgress_Changed;
    }

    private async void YtdlpUpdate(object? sender, RoutedEventArgs e)
    {
        var progress = new Progress<DownloadProgress>();
        progress.ProgressChanged += UpdateProgress_Changed;
        InfoBar.IsOpen = false;
        await _controller.YtdlpUpdateAsync(progress);
        progress.ProgressChanged -= UpdateProgress_Changed;
    }

    private async Task AddDownloadAsync(Uri? uri)
    {
        var addDownloadDialog = _serviceProvider.GetRequiredService<AddDownloadDialog>();
        addDownloadDialog.WindowId = AppWindow.Id;
        addDownloadDialog.RequestedTheme = MainGrid.ActualTheme;
        addDownloadDialog.XamlRoot = MainGrid.XamlRoot;
        if (uri is not null)
        {
            await addDownloadDialog.ShowAsync(uri);
        }
        else
        {
            await addDownloadDialog.ShowAsync();
        }
    }

    private async Task LaunchUriAsync(Uri? uri)
    {
        if (uri is null)
        {
            return;
        }
        await Launcher.LaunchUriAsync(uri);
    }

    private void UpdateDownloadsList()
    {
        DownloadsList.ItemsSource = _downloadRows.Values.Where(row => DownloadsFilter.SelectedIndex switch
        {
            1 => row.Status == DownloadStatus.Running || row.Status == DownloadStatus.Paused,
            2 => row.Status == DownloadStatus.Queued,
            3 => row.Status == DownloadStatus.Success || row.Status == DownloadStatus.Error || row.Status == DownloadStatus.Stopped,
            4 => row.Status == DownloadStatus.Error,
            _ => true
        }).Reverse().ToList();
        ViewStackDownloads.SelectedIndex = (DownloadsList.ItemsSource as IEnumerable<DownloadRow>)!.Count() > 0 ? 1 : 0;
        BadgeDownloads.Value = _controller.RemainingDownloadsCount;
        BadgeDownloads.Visibility = _controller.RemainingDownloadsCount > 0 ? Visibility.Visible : Visibility.Collapsed;
    }
}