using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Input;
using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Network;
using Nickvision.Desktop.Notifications;
using Nickvision.Desktop.WinUI.Helpers;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.WinUI.Controls;
using System;
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

    private readonly MainWindowController _controller;
    private RoutedEventHandler? _notificationClickHandler;

    public MainWindow(MainWindowController controller)
    {
        InitializeComponent();
        _controller = controller;
        _notificationClickHandler = null;
        // Config
        MainGrid.RequestedTheme = _controller.Theme switch
        {
            Theme.Light => ElementTheme.Light,
            Theme.Dark => ElementTheme.Dark,
            _ => ElementTheme.Default
        };
        this.Geometry = _controller.WindowGeometry;
        // TitleBar
        AppWindow.SetIcon("./Assets/org.nickvision.tubeconverter-devel.ico");
        ExtendsContentIntoTitleBar = true;
        SetTitleBar(TitleBar);
        AppWindow.TitleBar.PreferredHeightOption = TitleBarHeightOption.Tall;
        // Events
        AppWindow.Closing += Window_Closing;
        _controller.AppNotificationSent += (sender, e) => DispatcherQueue.TryEnqueue(() => Controller_AppNotificationSent(sender, e));
        _controller.DownloadRequested += async (s, args) => await AddDownloadAsync(args.Url);
        _controller.JsonFileSaved += Controller_JsonFileSaved;
        // Translations
        AppWindow.Title = _controller.AppInfo.ShortName;
        TitleBar.Title = _controller.AppInfo.ShortName;
        TitleBar.Subtitle = _controller.AppInfo.Version!.IsPreview ? _controller.Translator._("Preview") : string.Empty;
        NavItemHome.Content = _controller.Translator._("Home");
        NavItemDownloads.Content = _controller.Translator._("Downloads");
        NavItemHistory.Content = _controller.Translator._("History");
        NavItemKeyring.Content = _controller.Translator._("Keyring");
        NavItemUpdates.Content = _controller.Translator._("Updating");
        NavItemHelp.Content = _controller.Translator._("Help");
        MenuCheckForUpdates.Text = _controller.Translator._("Check for Updates");
        MenuGitHubRepo.Text = _controller.Translator._("GitHub Repo");
        MenuReportABug.Text = _controller.Translator._("Report a Bug");
        MenuDiscussions.Text = _controller.Translator._("Discussions");
        MenuAbout.Text = _controller.Translator._("About {0}", _controller.AppInfo.ShortName!);
        NavItemSettings.Content = _controller.Translator._("Settings");
        StatusHome.Title = _controller.Translator._("Download Media");
        StatusHome.Description = _controller.Translator._("Add a video, audio, or playlist URL to start downloading");
        LblHomeAddDownload.Text = _controller.Translator._("Add Download");
        BtnDownloadsAddDownload.Label = _controller.Translator._("Add");
        TokenDownloadsAll.Content = _controller.Translator._("All");
        TokenDownloadsDownloading.Content = _controller.Translator._("Downloading");
        TokenDownloadsQueued.Content = _controller.Translator._("Queued");
        TokenDownloadsCompleted.Content = _controller.Translator._("Completed");
        StatusNoneDownloads.Title = _controller.Translator._("No Downloads");
        StatusNoneDownloads.Description = _controller.Translator._("There are no downloads added");
    }

    private async void Window_Loaded(object? sender, RoutedEventArgs e)
    {
        ViewStackDownloads.SelectedIndex = 0;
        MenuCheckForUpdates.IsEnabled = false;
        var updatesTask = _controller.CheckForUpdatesAsync(false);
        if (_controller.ShowDislcaimerOnStartup)
        {
            var checkBox = new CheckBox()
            {
                Content = _controller.Translator._("Don't show this message again")
            };
            var disclaimerDialog = new ContentDialog()
            {
                Title = _controller.Translator._("Legal Copyright Disclaimer"),
                Content = new StackPanel()
                {
                    Orientation = Orientation.Vertical,
                    Spacing = 6,
                    Children =
                    {
                        new TextBlock()
                        {
                            Text = _controller.Translator._("Videos on YouTube and other sites may be subject to DMCA protection. The authors of Parabolic do not endorse, and are not responsible for, the use of this application in means that will violate these laws."),
                            TextWrapping = TextWrapping.WrapWholeWords
                        },
                        checkBox
                    }
                },
                CloseButtonText = _controller.Translator._("I understand"),
                DefaultButton = ContentDialogButton.Close,
                RequestedTheme = MainGrid.ActualTheme,
                XamlRoot = MainGrid.XamlRoot
            };
            await disclaimerDialog.ShowAsync();
            if (checkBox.IsChecked ?? false)
            {
                _controller.ShowDislcaimerOnStartup = false;
            }
        }
        await updatesTask;
        MenuCheckForUpdates.IsEnabled = true;
    }

    private void Window_Closing(AppWindow sender, AppWindowClosingEventArgs e)
    {
        if (!_controller.CanShutdown)
        {
            e.Cancel = true;
            return;
        }
        _controller.WindowGeometry = this.Geometry;
        _controller.Dispose();
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
                "History" => new HistoryPage(_controller.HistoryPageController),
                "Keyring" => new KeyringPage(_controller.KeyringPageController),
                "Settings" => new SettingsPage(_controller.PreferencesViewController, AppWindow.Id),
                _ => null
            };
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

    private void Controller_AppNotificationSent(object? sender, AppNotificationSentEventArgs e)
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
            BtnInfoBar.Content = _controller.Translator._("Update");
            _notificationClickHandler = WindowsUpdate;
            BtnInfoBar.Click += _notificationClickHandler;
        }
        else if (e.Notification.Action == "update-ytdlp")
        {
            BtnInfoBar.Content = _controller.Translator._("Update");
            _notificationClickHandler = YtdlpUpdate;
            BtnInfoBar.Click += _notificationClickHandler;
        }
        else if (e.Notification.Action == "error" && !string.IsNullOrEmpty(e.Notification.ActionParam))
        {
            BtnInfoBar.Content = _controller.Translator._("Details");
            _notificationClickHandler = async (_, _) =>
            {
                InfoBar.IsOpen = false;
                var errorDialog = new ContentDialog()
                {
                    Title = _controller.Translator._("Error"),
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
                    CloseButtonText = _controller.Translator._("Close"),
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

    private void Controller_JsonFileSaved(object? sender, JsonFileSavedEventArgs e)
    {
        if (e.Name == Configuration.Key)
        {
            MainGrid.RequestedTheme = _controller.Theme switch
            {
                Theme.Light => ElementTheme.Light,
                Theme.Dark => ElementTheme.Dark,
                _ => ElementTheme.Default
            };
        }
    }

    private async void AddDownload(object? sender, RoutedEventArgs e) => await AddDownloadAsync(null);

    private async void CheckForUpdates(object? sender, RoutedEventArgs e)
    {
        MenuCheckForUpdates.IsEnabled = false;
        await _controller.CheckForUpdatesAsync(true);
        MenuCheckForUpdates.IsEnabled = true;
    }

    private async void GitHubRepo(object? sender, RoutedEventArgs e) => await LaunchUriAsync(_controller.AppInfo.SourceRepository);

    private async void ReportABug(object? sender, RoutedEventArgs e) => await LaunchUriAsync(_controller.AppInfo.IssueTracker);

    private async void Discussions(object? sender, RoutedEventArgs e) => await LaunchUriAsync(_controller.AppInfo.DiscussionsForum);

    private async void About(object? sender, RoutedEventArgs e)
    {
        var progressDialog = new ContentDialog()
        {
            Title = _controller.Translator._("About {0}", _controller.AppInfo.ShortName!),
            Content = new ProgressRing()
            {
                IsActive = true,
            },
            RequestedTheme = MainGrid.ActualTheme,
            XamlRoot = MainGrid.XamlRoot
        };
        DispatcherQueue.TryEnqueue(async () => await progressDialog.ShowAsync());
        var aboutDialog = new AboutDialog(_controller.AppInfo, await _controller.GetDebugInformationAsync(), _controller.Translator)
        {
            RequestedTheme = MainGrid.ActualTheme,
            XamlRoot = MainGrid.XamlRoot
        };
        progressDialog.Hide();
        await aboutDialog.ShowAsync();
    }

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
            var message = _controller.Translator._("Downloading update: {0}%", Math.Round(e.Percentage * 100));
            NavItemUpdates.Visibility = Visibility.Visible;
            StsProgress.Description = message;
            BarProgress.Value = e.Percentage * 100;
        });
    }

    private async Task AddDownloadAsync(Uri? uri)
    {
        var addDownloadDialog = new AddDownloadDialog(_controller.AddDownloadDialogController, AppWindow.Id)
        {
            RequestedTheme = MainGrid.ActualTheme,
            XamlRoot = MainGrid.XamlRoot
        };
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
}