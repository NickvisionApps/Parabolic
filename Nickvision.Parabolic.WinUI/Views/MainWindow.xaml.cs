using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Network;
using Nickvision.Desktop.Notifications;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.WinUI.Controls;
using System;
using System.Threading.Tasks;
using Vanara.PInvoke;
using Windows.Graphics;
using Windows.System;
using WinRT.Interop;

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
    private readonly nint _hwnd;
    private RoutedEventHandler? _notificationClickHandler;

    public MainWindow(MainWindowController controller)
    {
        InitializeComponent();
        _controller = controller;
        _hwnd = WindowNative.GetWindowHandle(this);
        _notificationClickHandler = null;
        // Theme
        MainGrid.RequestedTheme = _controller.Theme switch
        {
            Theme.Light => ElementTheme.Light,
            Theme.Dark => ElementTheme.Dark,
            _ => ElementTheme.Default
        };
        // Size
        var windowGeometry = _controller.WindowGeometry;
        if (windowGeometry.IsMaximized)
        {
            AppWindow.Resize(new SizeInt32
            {
                Width = 900,
                Height = 700
            });
            User32.ShowWindow(_hwnd, ShowWindowCommand.SW_SHOWMAXIMIZED);
        }
        else
        {
            AppWindow.MoveAndResize(new RectInt32
            {
                X = windowGeometry.X,
                Y = windowGeometry.Y,
                Width = windowGeometry.Width,
                Height = windowGeometry.Height
            });
        }
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
        MenuFile.Title = _controller.Translator._("File");
        MenuAddDownload.Text = _controller.Translator._("Add Download");
        MenuExit.Text = _controller.Translator._("Exit");
        MenuEdit.Title = _controller.Translator._("Edit");
        MenuHistory.Text = _controller.Translator._("History");
        MenuSettings.Text = _controller.Translator._("Settings");
        MenuHelp.Title = _controller.Translator._("Help");
        MenuCheckForUpdates.Text = _controller.Translator._("Check for Updates");
        MenuGitHubRepo.Text = _controller.Translator._("GitHub Repo");
        MenuReportABug.Text = _controller.Translator._("Report a Bug");
        MenuDiscussions.Text = _controller.Translator._("Discussions");
        MenuAbout.Text = _controller.Translator._("About {0}", _controller.AppInfo.ShortName!);
        NavItemHome.Content = _controller.Translator._("Home");
        NavItemDownloads.Content = _controller.Translator._("Downloads");
        NavItemHistory.Content = _controller.Translator._("History");
        NavItemKeyring.Content = _controller.Translator._("Keyring");
        NavItemSettings.Content = _controller.Translator._("Settings");
        StatusHome.Title = _controller.Translator._("Download Media");
        StatusHome.Description = _controller.Translator._("Add a video, audio, or playlist URL to start downloading");
        LblHomeAddDownload.Text = _controller.Translator._("Add Download");
        BtnDownloadsAddDownload.Label = _controller.Translator._("Add");
    }

    private async void Window_Loaded(object? sender, RoutedEventArgs e)
    {
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
                RequestedTheme = MainGrid.RequestedTheme,
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
        _controller.WindowGeometry = new WindowGeometry(AppWindow.Size.Width, AppWindow.Size.Height, User32.IsZoomed(_hwnd), AppWindow.Position.X, AppWindow.Position.Y);
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
        else if (e.Notification.Action == "error")
        {
            BtnInfoBar.Content = _controller.Translator._("Details");
            _notificationClickHandler = async (_, _) =>
            {
                var errorDialog = new ContentDialog()
                {
                    Title = _controller.Translator._("Error"),
                    Content = new ScrollViewer()
                    {
                        HorizontalScrollBarVisibility = ScrollBarVisibility.Disabled,
                        VerticalScrollBarVisibility = ScrollBarVisibility.Visible,
                        Content = new TextBlock()
                        {
                            Text = e.Notification.ActionParam ?? string.Empty,
                            TextWrapping = TextWrapping.Wrap
                        }
                    },
                    CloseButtonText = _controller.Translator._("Close"),
                    DefaultButton = ContentDialogButton.Close,
                    RequestedTheme = MainGrid.RequestedTheme,
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

    private void Exit(object? sender, RoutedEventArgs e) => Close();

    private void History(object? sender, RoutedEventArgs e) => NavItemHistory.IsSelected = true;

    private void Settings(object? sender, RoutedEventArgs e) => NavItemSettings.IsSelected = true;

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
            RequestedTheme = MainGrid.RequestedTheme,
            XamlRoot = MainGrid.XamlRoot
        };
        DispatcherQueue.TryEnqueue(async () => await progressDialog.ShowAsync());
        var aboutDialog = new AboutDialog(_controller.AppInfo, await _controller.GetDebugInformationAsync(), _controller.Translator)
        {
            RequestedTheme = MainGrid.RequestedTheme,
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
                BtnProgress.Visibility = Visibility.Collapsed;
                ToolTipService.SetToolTip(BtnProgress, string.Empty);
                return;
            }
            var message = _controller.Translator._("Downloading update: {0}%", Math.Round(e.Percentage * 100));
            BtnProgress.Visibility = Visibility.Visible;
            ToolTipService.SetToolTip(BtnProgress, message);
            IconProgress.Glyph = "\uE896";
            StsProgress.Description = message;
            BarProgress.Value = e.Percentage * 100;
        });
    }

    private async Task AddDownloadAsync(Uri? uri)
    {
        var addDownloadDialog = new AddDownloadDialog(_controller.AddDownloadDialogController, AppWindow.Id)
        {
            RequestedTheme = MainGrid.RequestedTheme,
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