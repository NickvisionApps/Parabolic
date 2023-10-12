using CommunityToolkit.WinUI.Notifications;
using Microsoft.UI;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using Nickvision.Aura.Taskbar;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Events;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.WinUI.Controls;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
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
    private bool _isActived;
    private RoutedEventHandler? _notificationButtonClickEvent;

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
        _isActived = true;
        //Register Events
        AppWindow.Closing += Window_Closing;
        _controller.NotificationSent += (sender, e) => DispatcherQueue.TryEnqueue(() => NotificationSent(sender, e));
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
        //Home
        HomeBanner.Background = new AcrylicBrush()
        {
            TintOpacity = 0.9,
            TintColor = MainGrid.ActualTheme == ElementTheme.Light ? ColorHelper.FromArgb(255, 255, 123, 99) : ColorHelper.FromArgb(255, 192, 28, 40)
        };
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
        MenuRetryFailedDownloads.Text = _("Retry Failed Downloads");
        MenuClearQueuedDownloads.Text = _("Clear Queued Downloads");
        MenuHelp.Title = _("Help");
        MenuCheckForUpdates.Text = _("Check for Updates");
        MenuDocumentation.Text = _("Documentation");
        MenuGitHubRepo.Text = _("GitHub Repo");
        MenuReportABug.Text = _("Report a Bug");
        MenuDiscussions.Text = _("Discussions");
        MenuAbout.Text = _("About {0}", _controller.AppInfo.ShortName);
        LblStartup.Text = _("Preparing required tools...");
        LblStartup2.Text = _("This may take a while");
        StatusLabel.Text = _("Ready");
        HomeBannerTitle.Text = _controller.Greeting;
        HomeBannerDescription.Text = _controller.AppInfo.Description;
        HomeGettingStartedTitle.Text = _("Getting Started");
        HomeGettingStartedDescription.Text = _("Add a video, audio, or playlist URL to start downloading.");
        HomeAddDownlaodButtonLabel.Text = _("Add Download");
        HomeDocumentationTitle.Text = _("Documentation");
        HomeDocumentationDescription.Text = _("Read more about Parabolic's inner workings.");
        HomeReportABugTitle.Text = _("Report a Bug");
        HomeReportABugDescription.Text = _("Let us fix whatever issue you are having.");
        HomeDiscussionsTitle.Text = _("Discussions");
        HomeDiscussionsDescription.Text = _("Start a conversation with us.");
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
        _isActived = e.WindowActivationState != WindowActivationState.Deactivated;
        //Update TitleBar
        TitleBarTitle.Foreground = (SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"];
        AppWindow.TitleBar.ButtonForegroundColor = ((SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"]).Color;
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
            _isOpened = true;
        }
    }

    /// <summary>
    /// Occurs when the window is closing
    /// </summary>
    /// <param name="sender">AppWindow</param>
    /// <param name="e">AppWindowClosingEventArgs</param>
    private void Window_Closing(AppWindow sender, AppWindowClosingEventArgs e)
    {
        _controller.Dispose();
    }

    /// <summary>
    /// Occurs when the window's theme is changed
    /// </summary>
    /// <param name="sender">FrameworkElement</param>
    /// <param name="e">object</param>
    private void Window_ActualThemeChanged(FrameworkElement sender, object e)
    {
        //Update TitleBar
        TitleBarTitle.Foreground = (SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"];
        MenuFile.Foreground = (SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"];
        MenuEdit.Foreground = (SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"];
        MenuHelp.Foreground = (SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"];
        AppWindow.TitleBar.ButtonForegroundColor = ((SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"]).Color;
        HomeBanner.Background = HomeBanner.Background = new AcrylicBrush()
        {
            TintOpacity = 0.9,
            TintColor = MainGrid.ActualTheme == ElementTheme.Light ? ColorHelper.FromArgb(255, 255, 123, 99) : ColorHelper.FromArgb(255, 192, 28, 40)
        };
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
            //TODO: Add Download Enabled
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
            //TODO: Add Download Disabled
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
    private void ShellNotificationSent(object? sender, ShellNotificationSentEventArgs e) => new ToastContentBuilder().AddText(e.Title).AddText(e.Message).Show();

    /// <summary>
    /// Occurs when the add download menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void AddDownload(object sender, RoutedEventArgs e)
    {

    }

    /// <summary>
    /// Occurs when the exit menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Exit(object sender, RoutedEventArgs e) => Close();

    /// <summary>
    /// Occurs when the keyring menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Keyring(object sender, RoutedEventArgs e)
    {

    }

    /// <summary>
    /// Occurs when the history menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void History(object sender, RoutedEventArgs e)
    {
        var historyDialog = new HistoryDialog(_controller.DownloadHistory)
        {
            XamlRoot = MainGrid.XamlRoot
        };
        historyDialog.DownloadAgainRequested += (s, ea) => AddDownload(sender, new RoutedEventArgs());
        await historyDialog.ShowAsync();
    }

    /// <summary>
    /// Occurs when the settings menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void Settings(object sender, RoutedEventArgs e)
    {
        var settingsDialog = new SettingsDialog(_controller.CreatePreferencesViewController(), InitializeWithWindow)
        {
            XamlRoot = MainGrid.XamlRoot
        };
        await settingsDialog.ShowAsync();
    }

    /// <summary>
    /// Occurs when the stop all downloads menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void StopAllDownloads(object sender, RoutedEventArgs e)
    {

    }

    /// <summary>
    /// Occurs when the retry failed downloads menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void RetryFailedDownloads(object sender, RoutedEventArgs e)
    {

    }

    /// <summary>
    /// Occurs when the clear queued downloads menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void ClearQueuedDownloads(object sender, RoutedEventArgs e)
    {

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
        await aboutDialog.ShowAsync();
    }
}
