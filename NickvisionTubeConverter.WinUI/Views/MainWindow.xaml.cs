using CommunityToolkit.WinUI.Notifications;
using Microsoft.UI;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using Nickvision.Aura.Events;
using Nickvision.Aura.Taskbar;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using NickvisionTubeConverter.WinUI.Controls;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using Vanara.PInvoke;
using Windows.Graphics;
using Windows.System;
using WinRT.Interop;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.WinUI.Views;

/// <summary>
/// The MainWindow
/// </summary>
public sealed partial class MainWindow : Window
{
    private readonly MainWindowController _controller;
    private readonly IntPtr _hwnd;
    private bool _isOpened;
    private bool _isContentDialogShowing;
    private RoutedEventHandler? _notificationButtonClickEvent;
    private Kernel32.SafePowerRequestObject? _powerRequest;
    private readonly Dictionary<Guid, DownloadRow> _downloadRows;

    private enum Monitor_DPI_Type : int
    {
        MDT_Effective_DPI = 0,
        MDT_Angular_DPI = 1,
        MDT_Raw_DPI = 2,
        MDT_Default = MDT_Effective_DPI
    }

    [DllImport("Shcore.dll", SetLastError = true)]
    private static extern int GetDpiForMonitor(IntPtr hmonitor, Monitor_DPI_Type dpiType, out uint dpiX, out uint dpiY);

    public MainWindow(MainWindowController controller)
    {
        InitializeComponent();
        _controller = controller;
        _hwnd = WindowNative.GetWindowHandle(this);
        _isOpened = false;
        _isContentDialogShowing = false;
        _powerRequest = null;
        _downloadRows = new Dictionary<Guid, DownloadRow>();
        //Register Events
        AppWindow.Closing += Window_Closing;
        _controller.NotificationSent += (sender, e) => DispatcherQueue?.TryEnqueue(() => NotificationSent(sender, e));
        _controller.ShellNotificationSent += (sender, e) => DispatcherQueue?.TryEnqueue(() => ShellNotificationSent(sender, e));
        _controller.PreventSuspendWhenDownloadingChanged += PreventSuspendWhenDownloadingChanged;
        _controller.DownloadManager.DownloadAdded += (sender, e) => DispatcherQueue.TryEnqueue(() => DownloadAdded(e));
        _controller.DownloadManager.DownloadProgressUpdated += (sender, e) => DispatcherQueue.TryEnqueue(() => DownloadProgressUpdated(e));
        _controller.DownloadManager.DownloadCompleted += (sender, e) => DispatcherQueue.TryEnqueue(() => DownloadCompleted(e));
        _controller.DownloadManager.DownloadStopped += (sender, e) => DispatcherQueue.TryEnqueue(() => DownloadStopped(e));
        _controller.DownloadManager.DownloadRetried += (sender, e) => DispatcherQueue.TryEnqueue(() => DownloadRetried(e));
        _controller.DownloadManager.DownloadStartedFromQueue += (sender, e) => DispatcherQueue.TryEnqueue(() => DownloadStartedFromQueue(e));
        //Set TitleBar
        TitleBarTitle.Text = _controller.AppInfo.ShortName;
        AppWindow.TitleBar.ExtendsContentIntoTitleBar = true;
        AppWindow.TitleBar.PreferredHeightOption = TitleBarHeightOption.Tall;
        AppWindow.TitleBar.ButtonBackgroundColor = Colors.Transparent;
        AppWindow.TitleBar.ButtonInactiveBackgroundColor = Colors.Transparent;
        TitlePreview.Text = _controller.AppInfo.IsDevVersion ? _("PREVIEW") : "";
        AppWindow.Title = TitleBarTitle.Text;
        AppWindow.SetIcon(@"Resources\org.nickvision.tubeconverter.ico");
        TitleBar.Loaded += (sender, e) => SetDragRegionForCustomTitleBar();
        TitleBar.SizeChanged += (sender, e) => SetDragRegionForCustomTitleBar();
        //Localize Strings
        MenuFile.Title = _("File");
        MenuAddDownload.Text = _("Add Download");
        MenuExit.Text = _("Exit");
        MenuEdit.Title = _("Edit");
        MenuKeyring.Text = _("Keyring");
        MenuHistory.Text = _("History");
        MenuSettings.Text = _("Settings");
        MenuDownloader.Title = _("Downloader");
        MenuStopAllDownloads.Text = _("Stop All Downloads");
        MenuClearQueuedDownloads.Text = _("Clear Queued Downloads");
        MenuRetryFailedDownloads.Text = _("Retry Failed Downloads");
        MenuClearCompletedDownloads.Text = _("Clear Completed Downloads");
        MenuHelp.Title = _("Help");
        MenuCheckForUpdates.Text = _("Check for Updates");
        MenuDocumentation.Text = _("Documentation");
        MenuGitHubRepo.Text = _("GitHub Repo");
        MenuReportABug.Text = _("Report a Bug");
        MenuDiscussions.Text = _("Discussions");
        MenuAbout.Text = _("About {0}", _controller.AppInfo.ShortName);
        StatusPageHome.Title = _("Download Media");
        StatusPageHome.Description = _("Add a video, audio, or playlist URL to start downloading");
        LblBtnHomeAddDownload.Text = _("Add Download");
        LblHomeHelp.Text = _("Parabolic's documentation and support channels are accessible via the Help menu.");
        LblBtnAddDownload.Text = _("Add Download");
        NavItemDownloading.Content = _("Downloading");
        LblDownloading.Text = _("Downloading");
        BtnStopAllDownloads.Label = _("Stop All Downloads");
        StatusPageDownloading.Title = _("No Downloads Running");
        LblBtnShowCompleted.Text = _("Show Completed");
        StatusPageQueued.Title = _("No Queued Downloads");
        StatusPageCompleted.Title = _("No Completed Downloads");
        NavItemQueued.Content = _("Queued");
        LblQueued.Text = _("Queued");
        BtnClearQueuedDownloads.Label = _("Clear Queued Downloads");
        NavItemCompleted.Content = _("Completed");
        LblCompleted.Text = _("Completed");
        BtnRetryFailedDownloads.Label = _("Retry Failed Downloads");
        BtnClearCompletedDownloads.Label = _("Clear Completed Downloads");
        StatusIcon.Glyph = "\uE118";
        StatusLabel.Text = _("Remaining Downloads: {0}", 0);
        TrayIcon.ToolTipText = _("Parabolic");
        TrayMenuAddDownload.Text = _("Add Download");
        TrayMenuShowWindow.Text = _("Show Window");
        TrayMenuSettings.Text = _("Settings");
        TrayMenuExit.Text = _("Exit");
    }

    /// <summary>
    /// Calls InitializeWithWindow.Initialize on the target object with the MainWindow's hwnd
    /// </summary>
    /// <param name="target">The target object to initialize</param>
    public void InitializeWithWindow(object target) => WinRT.Interop.InitializeWithWindow.Initialize(target, _hwnd);

    /// <summary>
    /// Occurs when the window is activated
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">WindowActivatedEventArgs</param>
    private void Window_Activated(object sender, WindowActivatedEventArgs e)
    {
        _controller.IsWindowActive = e.WindowActivationState != WindowActivationState.Deactivated;
        //Update TitleBar
        TitleBarTitle.Foreground = (SolidColorBrush)Application.Current.Resources[_controller.IsWindowActive ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"];
        AppWindow.TitleBar.ButtonForegroundColor = ((SolidColorBrush)Application.Current.Resources[_controller.IsWindowActive ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"]).Color;
    }

    /// <summary>
    /// Occurs when the window is loaded
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void Window_Loaded(object sender, RoutedEventArgs e)
    {
        if (!_isOpened)
        {
            ViewStack.CurrentPageName = "Startup";
            var accent = (SolidColorBrush)Application.Current.Resources["AccentFillColorDefaultBrush"];
            _controller.TaskbarItem = TaskbarItem.ConnectWindows(_hwnd, new System.Drawing.SolidBrush(System.Drawing.Color.FromArgb(accent.Color.A, accent.Color.R, accent.Color.G, accent.Color.B)), MainGrid.ActualTheme == ElementTheme.Dark ? System.Drawing.Brushes.Black : System.Drawing.Brushes.White);
            await _controller.StartupAsync();
            MainMenu.IsEnabled = true;
            ViewStack.CurrentPageName = "Home";
            PreventSuspendWhenDownloadingChanged(null, EventArgs.Empty);
            if (_controller.ShowDisclaimerOnStartup)
            {
                var chkShow = new CheckBox()
                {
                    Content = _("Don't show this message again")
                };
                var disclaimerDialog = new ContentDialog()
                {
                    Title = _("Disclaimer"),
                    Content = new StackPanel()
                    {
                        Orientation = Orientation.Vertical,
                        Spacing = 6,
                        Children =
                        {
                            new TextBlock()
                            {
                                MaxWidth = 400,
                                TextWrapping = TextWrapping.WrapWholeWords,
                                Text = _("The authors of Nickvision Parabolic are not responsible/liable for any misuse of this program that may violate local copyright/DMCA laws. Users use this application at their own risk."),
                            },
                            chkShow
                        }
                    },
                    CloseButtonText = _("OK"),
                    DefaultButton = ContentDialogButton.Close,
                    XamlRoot = MainGrid.XamlRoot
                };
                _isContentDialogShowing = true;
                await disclaimerDialog.ShowAsync();
                _isContentDialogShowing = false;
                _controller.ShowDisclaimerOnStartup = !chkShow.IsChecked ?? true;
                _controller.SaveConfiguration();
            }
            _isOpened = true;
        }
    }

    /// <summary>
    /// Occurs when the window is closing
    /// </summary>
    /// <param name="sender">AppWindow</param>
    /// <param name="e">AppWindowClosingEventArgs</param>
    private async void Window_Closing(AppWindow sender, AppWindowClosingEventArgs e)
    {
        if (_controller.RunInBackground)
        {
            e.Cancel = true;
            User32.ShowWindow(_hwnd, ShowWindowCommand.SW_HIDE);
        }
        else if (_controller.DownloadManager.AreDownloadsRunning)
        {
            e.Cancel = true;
            var dialog = new ContentDialog()
            {
                Title = _("Close and Stop Downloads?"),
                Content = _("Some downloads are still in progress.\nAre you sure you want to close Parabolic and stop the running downloads?"),
                PrimaryButtonText = _("Yes"),
                CloseButtonText = _("No"),
                DefaultButton = ContentDialogButton.Primary,
                XamlRoot = MainGrid.XamlRoot
            };
            var res = await dialog.ShowAsync();
            if (res == ContentDialogResult.Primary)
            {
                ForceExit(sender, new RoutedEventArgs());
            }
        }
        else
        {
            _powerRequest?.Close();
            _powerRequest?.Dispose();
            TrayIcon.ContextFlyout.Hide();
            TrayIcon.Dispose();
            _controller.Dispose();
        }
    }

    /// <summary>
    /// Occurs when the window's theme is changed
    /// </summary>
    /// <param name="sender">FrameworkElement</param>
    /// <param name="e">object</param>
    private void Window_ActualThemeChanged(FrameworkElement sender, object e)
    {
        //Update TitleBar
        TitleBarTitle.Foreground = (SolidColorBrush)Application.Current.Resources[_controller.IsWindowActive ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"];
        MenuFile.Foreground = (SolidColorBrush)Application.Current.Resources[_controller.IsWindowActive ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"];
        MenuEdit.Foreground = (SolidColorBrush)Application.Current.Resources[_controller.IsWindowActive ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"];
        MenuHelp.Foreground = (SolidColorBrush)Application.Current.Resources[_controller.IsWindowActive ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"];
        AppWindow.TitleBar.ButtonForegroundColor = ((SolidColorBrush)Application.Current.Resources[_controller.IsWindowActive ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"]).Color;
    }

    /// <summary>
    /// Sets the drag region for the TitleBar
    /// </summary>
    /// <exception cref="Exception"></exception>
    private void SetDragRegionForCustomTitleBar()
    {
        var hMonitor = Win32Interop.GetMonitorFromDisplayId(DisplayArea.GetFromWindowId(Win32Interop.GetWindowIdFromWindow(_hwnd), DisplayAreaFallback.Primary).DisplayId);
        var result = GetDpiForMonitor(hMonitor, Monitor_DPI_Type.MDT_Default, out uint dpiX, out uint _);
        if (result != 0)
        {
            throw new Exception("Could not get DPI for monitor.");
        }
        var scaleFactorPercent = (uint)(((long)dpiX * 100 + (96 >> 1)) / 96);
        var scaleAdjustment = scaleFactorPercent / 100.0;
        RightPaddingColumn.Width = new GridLength(AppWindow.TitleBar.RightInset / scaleAdjustment);
        LeftPaddingColumn.Width = new GridLength(AppWindow.TitleBar.LeftInset / scaleAdjustment);
        var dragRectsList = new List<RectInt32>();
        RectInt32 dragRectL;
        dragRectL.X = (int)((LeftPaddingColumn.ActualWidth) * scaleAdjustment);
        dragRectL.Y = 0;
        dragRectL.Height = (int)(TitleBar.ActualHeight * scaleAdjustment);
        dragRectL.Width = (int)((IconColumn.ActualWidth
                                + TitleColumn.ActualWidth
                                + LeftDragColumn.ActualWidth) * scaleAdjustment);
        dragRectsList.Add(dragRectL);
        RectInt32 dragRectR;
        dragRectR.X = (int)((LeftPaddingColumn.ActualWidth
                            + IconColumn.ActualWidth
                            + TitleBarTitle.ActualWidth
                            + LeftDragColumn.ActualWidth
                            + MainMenu.ActualWidth) * scaleAdjustment);
        dragRectR.Y = 0;
        dragRectR.Height = (int)(TitleBar.ActualHeight * scaleAdjustment);
        dragRectR.Width = (int)(RightDragColumn.ActualWidth * scaleAdjustment);
        dragRectsList.Add(dragRectR);
        RectInt32[] dragRects = dragRectsList.ToArray();
        AppWindow.TitleBar.SetDragRectangles(dragRects);
    }

    /// <summary>
    /// Occurs when a notification is sent from the controller
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">NotificationSentEventArgs</param>
    private void NotificationSent(object? sender, NotificationSentEventArgs e)
    {
        if (e.Action == "network-restored")
        {
            MenuAddDownload.IsEnabled = true;
            BtnHomeAddDownload.IsEnabled = true;
            BtnAddDownload.IsEnabled = true;
            return;
        }
        //InfoBar
        InfoBar.Message = e.Message;
        InfoBar.Severity = e.Severity switch
        {
            NotificationSeverity.Informational => InfoBarSeverity.Informational,
            NotificationSeverity.Success => InfoBarSeverity.Success,
            NotificationSeverity.Warning => InfoBarSeverity.Warning,
            NotificationSeverity.Error => InfoBarSeverity.Error,
            _ => InfoBarSeverity.Informational
        };
        InfoBar.IsClosable = true;
        if (_notificationButtonClickEvent != null)
        {
            BtnInfoBar.Click -= _notificationButtonClickEvent;
        }
        if (e.Action == "update")
        {
            _notificationButtonClickEvent = WindowsUpdate;
            BtnInfoBar.Content = _("Update");
            BtnInfoBar.Click += _notificationButtonClickEvent;
        }
        else if (e.Action == "no-network")
        {
            MenuAddDownload.IsEnabled = false;
            BtnHomeAddDownload.IsEnabled = false;
            BtnAddDownload.IsEnabled = false;
            InfoBar.IsClosable = false;
            BtnInfoBar.Visibility = Visibility.Collapsed;
            return;
        }
        BtnInfoBar.Visibility = !string.IsNullOrEmpty(e.Action) ? Visibility.Visible : Visibility.Collapsed;
        InfoBar.IsOpen = true;
    }

    /// <summary>
    /// Occurs when a shell notification is sent from the controller
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">ShellNotificationSentEventArgs</param>
    private void ShellNotificationSent(object? sender, ShellNotificationSentEventArgs e)
    {
        var toast = new ToastContentBuilder().AddText(e.Title).AddText(e.Message);
        if (e.Action == "open-file")
        {
            toast.SetProtocolActivation(new Uri($"file:///{e.ActionParam}"));
        }
        toast.Show();
    }

    /// <summary>
    /// Occurs when the show window tray menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    public void ShowWindow(object sender, RoutedEventArgs e) => User32.ShowWindow(_hwnd, ShowWindowCommand.SW_SHOW);

    /// <summary>
    /// Occurs when the add download menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void AddDownload(object sender, RoutedEventArgs e)
    {
        ShowWindow(sender, e);
        await AddDownloadAsync(null);
    }

    /// <summary>
    /// Occurs when the exit menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Exit(object sender, RoutedEventArgs e) => Close();

    /// <summary>
    /// Occurs when the exit tray menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void ForceExit(object sender, RoutedEventArgs e)
    {
        _powerRequest?.Close();
        _powerRequest?.Dispose();
        TrayIcon.Dispose();
        _controller.Dispose();
        Environment.Exit(0);
    }

    /// <summary>
    /// Occurs when the keyring menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void Keyring(object sender, RoutedEventArgs e)
    {
        var keyringDialogController = _controller.CreateKeyringDialogController();
        var keyringDialog = new KeyringDialog(keyringDialogController)
        {
            XamlRoot = MainGrid.XamlRoot
        };
        if (!_isContentDialogShowing)
        {
            _isContentDialogShowing = true;
            await keyringDialog.ShowAsync();
            _isContentDialogShowing = false;
            _controller.UpdateKeyring(keyringDialogController);
        }
    }

    /// <summary>
    /// Occurs when the history menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void History(object sender, RoutedEventArgs e)
    {
        var historyDialog = new HistoryDialog(_controller.DownloadManager.History)
        {
            XamlRoot = MainGrid.XamlRoot
        };
        historyDialog.DownloadAgainRequested += async (s, ea) => await AddDownloadAsync(ea);
        if (!_isContentDialogShowing)
        {
            _isContentDialogShowing = true;
            await historyDialog.ShowAsync();
            _isContentDialogShowing = false;
        }
    }

    /// <summary>
    /// Occurs when the settings menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void Settings(object sender, RoutedEventArgs e)
    {
        ShowWindow(sender, e);
        var settingsDialog = new SettingsDialog(_controller.CreatePreferencesViewController(), InitializeWithWindow)
        {
            XamlRoot = MainGrid.XamlRoot
        };
        if (!_isContentDialogShowing)
        {
            _isContentDialogShowing = true;
            await settingsDialog.ShowAsync();
            _isContentDialogShowing = false;
        }
    }

    /// <summary>
    /// Occurs when the stop all downloads menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void StopAllDownloads(object sender, RoutedEventArgs e) => _controller.DownloadManager.StopAllDownloads(true);

    /// <summary>
    /// Occurs when the clear queued downloads menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void ClearQueuedDownloads(object sender, RoutedEventArgs e)
    {
        _controller.DownloadManager.ClearQueuedDownloads();
        ListQueued.Children.Clear();
        DownloadUIUpdate();
    }

    /// <summary>
    /// Occurs when the retry failed downloads menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void RetryFailedDownloads(object sender, RoutedEventArgs e) => _controller.DownloadManager.RetryFailedDownloads(DownloadOptions.Current);

    /// <summary>
    /// Occurs when the clear completed downloads menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void ClearCompletedDownloads(object sender, RoutedEventArgs e)
    {
        _controller.DownloadManager.ClearCompletedDownloads();
        ListCompleted.Children.Clear();
        DownloadUIUpdate();
    }

    /// <summary>
    /// Occurs when the check for updates menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void CheckForUpdates(object sender, RoutedEventArgs e) => await _controller.CheckForUpdatesAsync();

    /// <summary>
    /// Occurs when the windows update button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void WindowsUpdate(object sender, RoutedEventArgs e)
    {
        MainMenu.IsEnabled = false;
        InfoBar.IsOpen = false;
        var page = ViewStack.CurrentPageName;
        ViewStack.CurrentPageName = "Startup";
        if (!(await _controller.WindowsUpdateAsync()))
        {
            ViewStack.CurrentPageName = page;
        }
    }

    /// <summary>
    /// Occurs when the documentation menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void Documentation(object sender, RoutedEventArgs e) => await Launcher.LaunchUriAsync(new Uri(DocumentationHelpers.GetHelpURL("index")));

    /// <summary>
    /// Occurs when the github repo menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void GitHubRepo(object sender, RoutedEventArgs e) => await Launcher.LaunchUriAsync(_controller.AppInfo.SourceRepo);

    /// <summary>
    /// Occurs when the report a bug menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void ReportABug(object sender, RoutedEventArgs e) => await Launcher.LaunchUriAsync(_controller.AppInfo.IssueTracker);

    /// <summary>
    /// Occurs when the discussions menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void Discussions(object sender, RoutedEventArgs e) => await Launcher.LaunchUriAsync(_controller.AppInfo.SupportUrl);

    /// <summary>
    /// Occurs when the about menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void About(object sender, RoutedEventArgs e)
    {
        var aboutDialog = new AboutDialog(_controller.AppInfo)
        {
            XamlRoot = MainGrid.XamlRoot
        };
        if (!_isContentDialogShowing)
        {
            _isContentDialogShowing = true;
            await aboutDialog.ShowAsync();
            _isContentDialogShowing = false;
        }
    }

    /// <summary>
    /// Prompts the AddDownloadDialog
    /// </summary>
    /// <param name="url">A url to pass to the dialog</param>
    private async Task AddDownloadAsync(string? url)
    {
        var addController = _controller.CreateAddDownloadDialogController();
        var addDialog = new AddDownloadDialog(addController, InitializeWithWindow)
        {
            XamlRoot = MainGrid.XamlRoot
        };
        if (!_isContentDialogShowing || !string.IsNullOrEmpty(url))
        {
            _isContentDialogShowing = true;
            var res = await addDialog.ShowAsync(url);
            _isContentDialogShowing = false;
            if (res == ContentDialogResult.Primary)
            {
                _controller.AddDownloads(addController);
            }
        }
    }

    /// <summary>
    /// Occurs when the prevent suspend option is changed
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">EventArgs</param>
    private void PreventSuspendWhenDownloadingChanged(object? sender, EventArgs e)
    {
        if (_powerRequest == null && _controller.PreventSuspendWhenDownloading)
        {
            _powerRequest = Kernel32.PowerCreateRequest(new Kernel32.REASON_CONTEXT("Parabolic downloading"));
            Kernel32.PowerSetRequest(_powerRequest, Kernel32.POWER_REQUEST_TYPE.PowerRequestSystemRequired);
            Kernel32.PowerSetRequest(_powerRequest, Kernel32.POWER_REQUEST_TYPE.PowerRequestDisplayRequired);
        }
        else if (_powerRequest != null && !_controller.PreventSuspendWhenDownloading)
        {
            Kernel32.PowerClearRequest(_powerRequest, Kernel32.POWER_REQUEST_TYPE.PowerRequestSystemRequired);
            Kernel32.PowerClearRequest(_powerRequest, Kernel32.POWER_REQUEST_TYPE.PowerRequestDisplayRequired);
            _powerRequest.Close();
            _powerRequest.Dispose();
            _powerRequest = null;
        }
    }

    /// <summary>
    /// Occurs when the NavViewDownload's selection is changed
    /// </summary>
    /// <param name="sender">NavigationView</param>
    /// <param name="args">NavigationViewSelectionChangedEventArgs</param>
    private void NavViewDownloads_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs args)
    {
        var tag = (string)((NavViewDownloads.SelectedItem as NavigationViewItem)!.Tag);
        ViewStackDownloads.CurrentPageName = tag;
    }

    /// <summary>
    /// Occurs when BtnShowCompleted is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void ShowCompleted(object sender, RoutedEventArgs e) => NavItemCompleted.IsSelected = true;

    /// <summary>
    /// Updates the UI based on the current DownloadManager state
    /// </summary>
    private void DownloadUIUpdate()
    {
        ViewStackDownloading.CurrentPageName = ListDownloading.Children.Count > 0 ? "Has" : "No";
        ViewStackQueued.CurrentPageName = ListQueued.Children.Count > 0 ? "Has" : "No";
        ViewStackCompleted.CurrentPageName = ListCompleted.Children.Count > 0 ? "Has" : "No";
        StatusLabel.Text = _("Remaining Downloads: {0}", _controller.DownloadManager.RemainingDownloadsCount);
        TrayIcon.ToolTipText = _controller.DownloadManager.BackgroundActivityReport;
    }

    /// <summary>
    /// Occurs when a download is added
    /// </summary>
    /// <param name="e">(Guid Id, string Filename, string SaveFolder, bool IsDownloading)</param>
    private void DownloadAdded((Guid Id, string Filename, string SaveFolder, bool IsDownloading) e)
    {
        ViewStack.CurrentPageName = "Downloads";
        StatusBar.Visibility = Visibility.Visible;
        var downloadRow = new DownloadRow(e.Id, e.Filename, e.SaveFolder, (ea) => NotificationSent(null, ea), MainGrid.XamlRoot);
        downloadRow.StopRequested += (s, ea) => _controller.DownloadManager.RequestStop(ea);
        downloadRow.RetryRequested += (s, ea) => _controller.DownloadManager.RequestRetry(ea, DownloadOptions.Current);
        var list = e.IsDownloading ? ListDownloading : ListQueued;
        if (e.IsDownloading)
        {
            downloadRow.SetPreparingState();
            NavItemDownloading.IsSelected = true;
        }
        else
        {
            downloadRow.SetWaitingState();
            NavItemQueued.IsSelected = true;
        }
        list.Children.Add(downloadRow);
        _downloadRows[e.Id] = downloadRow;
        DownloadUIUpdate();
    }

    /// <summary>
    /// Occurs when a download's progress is updated
    /// </summary>
    /// <param name="e">(Guid Id, DownloadProgressState State)</param>
    private void DownloadProgressUpdated((Guid Id, DownloadProgressState State) e)
    {
        var row = _downloadRows[e.Id];
        row.SetProgressState(e.State);
        TrayIcon.ToolTipText = _controller.DownloadManager.BackgroundActivityReport;
    }

    /// <summary>
    /// Occurs when a download is completed
    /// </summary>
    /// <param name="e">(Guid Id, bool Successful, string Filename, bool ShowNotification)</param>
    private void DownloadCompleted((Guid Id, bool Successful, string Filename, bool ShowNotification) e)
    {
        var row = _downloadRows[e.Id];
        row.SetCompletedState(e.Successful, e.Filename);
        ListDownloading.Children.Remove(row);
        ListCompleted.Children.Add(row);
        DownloadUIUpdate();
    }

    /// <summary>
    /// Occurs when a download is stopped
    /// </summary>
    /// <param name="e">Guid</param>
    private void DownloadStopped(Guid e)
    {
        var row = _downloadRows[e];
        row.SetStopState();
        ListDownloading.Children.Remove(row);
        ListQueued.Children.Remove(row);
        ListCompleted.Children.Add(row);
        DownloadUIUpdate();
    }

    /// <summary>
    /// Occurs when a download is retried
    /// </summary>
    /// <param name="e">Guid</param>
    private void DownloadRetried(Guid e)
    {
        var row = _downloadRows[e];
        row.SetWaitingState();
        ListCompleted.Children.Remove(row);
        DownloadUIUpdate();
    }

    /// <summary>
    /// Occurs when a download is started from queue
    /// </summary>
    /// <param name="e">Guid</param>
    private void DownloadStartedFromQueue(Guid e)
    {
        var row = _downloadRows[e];
        row.SetPreparingState();
        ListQueued.Children.Remove(row);
        ListDownloading.Children.Add(row);
        DownloadUIUpdate();
    }
}
