using H.NotifyIcon.Core;
using Microsoft.UI;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using Microsoft.Windows.AppNotifications;
using Microsoft.Windows.AppNotifications.Builder;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Events;
using NickvisionTubeConverter.Shared.Models;
using NickvisionTubeConverter.WinUI.Controls;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Reflection;
using System.Runtime.InteropServices;
using Vanara.PInvoke;
using Windows.Graphics;
using WinRT.Interop;

namespace NickvisionTubeConverter.WinUI.Views;

/// <summary>
/// The MainWindow for the application
/// </summary>
public sealed partial class MainWindow : Window
{
    private bool _isOpened;
    private readonly MainWindowController _controller;
    private readonly IntPtr _hwnd;
    private bool _isActived;
    private RoutedEventHandler? _notificationButtonClickEvent;
    private bool _closeAllowed;
    private TrayIconWithContextMenu? _taskbarIcon;
    private DispatcherTimer _timer;

    private enum Monitor_DPI_Type : int
    {
        MDT_Effective_DPI = 0,
        MDT_Angular_DPI = 1,
        MDT_Raw_DPI = 2,
        MDT_Default = MDT_Effective_DPI
    }

    [DllImport("Shcore.dll", SetLastError = true)]
    private static extern int GetDpiForMonitor(IntPtr hmonitor, Monitor_DPI_Type dpiType, out uint dpiX, out uint dpiY);

    /// <summary>
    /// Constructs a MainWindow
    /// </summary>
    /// <param name="controller">The MainWindowController</param>
    public MainWindow(MainWindowController controller)
    {
        InitializeComponent();
        //Initialize Vars
        _isOpened = false;
        _controller = controller;
        _hwnd = WindowNative.GetWindowHandle(this);
        _isActived = true;
        _closeAllowed = false;
        _taskbarIcon = null;
        _timer = new DispatcherTimer()
        {
            Interval = new TimeSpan(0, 0, 1)
        };
        //Register Events
        AppWindow.Closing += Window_Closing;
        _controller.NotificationSent += NotificationSent;
        _controller.UICreateDownloadRow = CreateDownloadRow;
        _controller.UIMoveDownloadRow = MoveDownloadRow;
        _controller.UIDeleteDownloadRowFromQueue = DeleteDownloadRowFromQueue;
        _controller.RunInBackgroundChanged += ToggleTaskbarIcon;
        _timer.Tick += Timer_Tick;
        //Set TitleBar
        TitleBarTitle.Text = _controller.AppInfo.ShortName;
        AppWindow.TitleBar.ExtendsContentIntoTitleBar = true;
        AppWindow.TitleBar.PreferredHeightOption = TitleBarHeightOption.Tall;
        AppWindow.TitleBar.ButtonBackgroundColor = Colors.Transparent;
        AppWindow.TitleBar.ButtonInactiveBackgroundColor = Colors.Transparent;
        TitlePreview.Text = _controller.IsDevVersion ? _controller.Localizer["Preview", "WinUI"] : "";
        AppWindow.Title = TitleBarTitle.Text;
        AppWindow.SetIcon(@"Assets\org.nickvision.tubeconverter.ico");
        SystemBackdrop = new MicaBackdrop();
        //Window Sizing
        AppWindow.Resize(new SizeInt32(900, 740));
        User32.ShowWindow(_hwnd, ShowWindowCommand.SW_SHOWMAXIMIZED);
        //Taskbar Icon
        ToggleTaskbarIcon(null, EventArgs.Empty);
        //Localize Strings
        MenuFile.Title = _controller.Localizer["File"];
        MenuAddDownload.Text = _controller.Localizer["AddDownload"];
        MenuExit.Text = _controller.Localizer["Exit"];
        MenuEdit.Title = _controller.Localizer["Edit"];
        MenuSettings.Text = _controller.Localizer["Settings"];
        MenuDownloader.Title = _controller.Localizer["Downloader"];
        MenuStopAllDownloads.Text = _controller.Localizer["StopDownloads"];
        MenuRetryFailedDownloads.Text = _controller.Localizer["RetryDownloads"];
        MenuClearQueuedDownloads.Text = _controller.Localizer["ClearQueue"];
        MenuAbout.Text = string.Format(_controller.Localizer["About"], _controller.AppInfo.ShortName);
        MenuHelp.Title = _controller.Localizer["Help"];
        LblStatus.Text = _controller.Localizer["StatusReady", "WinUI"];
        LblLoading.Text = _controller.Localizer["DependencyDownload"];
        StatusPageHome.Glyph = _controller.ShowSun ? "\xE706" : "\xE708";
        StatusPageHome.Title = _controller.Greeting;
        StatusPageHome.Description = _controller.Localizer["NoDownloads", "Description"];
        ToolTipService.SetToolTip(BtnHomeAddDownload, _controller.Localizer["AddDownload", "Tooltip"]);
        LblBtnHomeAddDownload.Text = _controller.Localizer["AddDownload"];
        LblDownloading.Text = _controller.Localizer["Downloading"];
        LblCompleted.Text = _controller.Localizer["Completed"];
        LblQueued.Text = _controller.Localizer["Queued"];
        BtnAddDownload.Label = _controller.Localizer["AddDownload"];
        ToolTipService.SetToolTip(BtnAddDownload, _controller.Localizer["AddDownload", "Tooltip"]);
        BtnStopAllDownloads.Label = _controller.Localizer["StopDownloads"];
        ToolTipService.SetToolTip(BtnStopAllDownloads, _controller.Localizer["StopDownloads", "Tooltip"]);
        BtnRetryFailedDownloads.Label = _controller.Localizer["RetryDownloads"];
        ToolTipService.SetToolTip(BtnRetryFailedDownloads, _controller.Localizer["RetryDownloads", "Tooltip"]);
        BtnClearQueuedDownloads.Label = _controller.Localizer["ClearQueue"];
        ToolTipService.SetToolTip(BtnClearQueuedDownloads, _controller.Localizer["ClearQueue", "Tooltip"]);
        //Page
        ViewStack.ChangePage("Home");
    }

    /// <summary>
    /// Calls InitializeWithWindow.Initialize on the target object with the MainWindow's hwnd
    /// </summary>
    /// <param name="target">The target object to initialize</param>
    public void InitializeWithWindow(object target) => WinRT.Interop.InitializeWithWindow.Initialize(target, _hwnd);

    /// <summary>
    /// Occurs when the window is loaded
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void Window_Loaded(object sender, RoutedEventArgs e)
    {
        if (!_isOpened)
        {
            //Start Loading
            MenuAddDownload.IsEnabled = false;
            IconStatus.Glyph = "\uE12B";
            LblStatus.Text = _controller.Localizer["DependencyDownload", "Short"];
            Loading.IsLoading = true;
            BorderLoading.Visibility = Visibility.Visible;
            //Work
            await _controller.StartupAsync();
            _timer.Start();
            //Done Loading
            MenuAddDownload.IsEnabled = true;
            IconStatus.Glyph = "\uE73E";
            LblStatus.Text = _controller.Localizer["StatusReady", "WinUI"];
            Loading.IsLoading = false;
            BorderLoading.Visibility = Visibility.Collapsed;
            _isOpened = true;
        }
    }

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
        MenuFile.Foreground = (SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"];
        MenuEdit.Foreground = (SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"];
        MenuHelp.Foreground = (SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"];
        AppWindow.TitleBar.ButtonForegroundColor = ((SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"]).Color;
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
            AppWindow.Hide();
        }
        else
        {
            if (_controller.AreDownloadsRunning && !_closeAllowed)
            {
                e.Cancel = true;
                var closeDialog = new ContentDialog()
                {
                    Title = _controller.Localizer["CloseAndStop", "Title"],
                    Content = _controller.Localizer["CloseAndStop", "Description"],
                    CloseButtonText = _controller.Localizer["No"],
                    PrimaryButtonText = _controller.Localizer["Yes"],
                    DefaultButton = ContentDialogButton.Close,
                    XamlRoot = Content.XamlRoot
                };
                var result = await closeDialog.ShowAsync();
                if (result == ContentDialogResult.Primary)
                {
                    _closeAllowed = true;
                    e.Cancel = false;
                    Close();
                    Environment.Exit(0);
                }
            }
            else
            {
                _timer.Stop();
                _controller.StopAllDownloads();
                _controller.Dispose();
                _taskbarIcon?.Dispose();
            }
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
        TitleBarTitle.Foreground = (SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"];
        MenuFile.Foreground = (SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"];
        MenuEdit.Foreground = (SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"];
        MenuHelp.Foreground = (SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"];
        AppWindow.TitleBar.ButtonForegroundColor = ((SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"]).Color;
    }

    /// <summary>
    /// Occurs when the TitleBar is loaded
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void TitleBar_Loaded(object sender, RoutedEventArgs e) => SetDragRegionForCustomTitleBar();

    /// <summary>
    /// Occurs when the TitleBar's size is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void TitleBar_SizeChanged(object sender, SizeChangedEventArgs e) => SetDragRegionForCustomTitleBar();

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
    /// Occurs when the ScrollViewer's size is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SizeChangedEventArgs</param>
    private void ScrollViewer_SizeChanged(object sender, SizeChangedEventArgs e) => GridDownloads.Margin = new Thickness(0, 0, ScrollViewer.ComputedVerticalScrollBarVisibility == Visibility.Visible ? 14 : 0, 0);

    /// <summary>
    /// Occurs whhen the TaskbarMenuShowWindow item is clicked
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">EventArgs</param>
    private void ShowWindow(object? sender, EventArgs e) => DispatcherQueue.TryEnqueue(() => AppWindow.Show());

    /// <summary>
    /// Occurs when the TaskbarMenuQuit item is clicked
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">EventArgs</param>
    private void Quit(object? sender, EventArgs e)
    {
        DispatcherQueue.TryEnqueue(() =>
        {
            _controller.StopAllDownloads();
            _timer.Stop();
            _taskbarIcon!.Remove();
            AppWindow.Hide();
            _taskbarIcon!.Dispose();
            _controller.Dispose();
            Environment.Exit(0);
        });
    }

    /// <summary>
    /// Occurs when a notification is sent from the controller
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">NotificationSentEventArgs</param>
    private void NotificationSent(object? sender, NotificationSentEventArgs e)
    {
        InfoBar.Message = e.Message;
        InfoBar.Severity = e.Severity switch
        {
            NotificationSeverity.Informational => InfoBarSeverity.Informational,
            NotificationSeverity.Success => InfoBarSeverity.Success,
            NotificationSeverity.Warning => InfoBarSeverity.Warning,
            NotificationSeverity.Error => InfoBarSeverity.Error,
            _ => InfoBarSeverity.Informational
        };
        if (_notificationButtonClickEvent != null)
        {
            BtnInfoBar.Click -= _notificationButtonClickEvent;
        }
        if (e.Action == "error")
        {
            BtnInfoBar.Content = _controller.Localizer["Info"];
            _notificationButtonClickEvent = async (sender, ex) =>
            {
                var contentDialog = new ContentDialog()
                {
                    Title = "Error",
                    Content = new ScrollViewer()
                    {
                        VerticalScrollBarVisibility = ScrollBarVisibility.Auto,
                        Content = new TextBlock()
                        {
                            Text = e.ActionParam
                        }
                    },
                    CloseButtonText = _controller.Localizer["OK"],
                    DefaultButton = ContentDialogButton.Close,
                    XamlRoot = Content.XamlRoot
                };
                await contentDialog.ShowAsync();
            };
            BtnInfoBar.Click += _notificationButtonClickEvent;
        }
        BtnInfoBar.Visibility = !string.IsNullOrEmpty(e.Action) ? Visibility.Visible : Visibility.Collapsed;
        InfoBar.IsOpen = true;
    }

    /// <summary>
    /// Sends a shell notification
    /// </summary>
    /// <param name="e">ShellNotificationSentEventArgs</param>
    private void SendShellNotification(ShellNotificationSentEventArgs e)
    {
        var notificationBuilder = new AppNotificationBuilder().AddText(e.Title, new AppNotificationTextProperties().SetMaxLines(1)).AddText(e.Message);
        AppNotificationManager.Default.Show(notificationBuilder.BuildNotification());
    }

    /// <summary>
    /// Toggles a taskbar icon for the app
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">EventArgs</param>
    private void ToggleTaskbarIcon(object? sender, EventArgs e)
    {
        if (_controller.RunInBackground)
        {
            var taskbarMenuPopup = new PopupMenu();
            //var taskbarMenuFlyout = new MenuFlyout();
            //Show Window
            var taskbarMenuShowWindow = new PopupMenuItem()
            {
                Text = _controller.Localizer["Open"],
                Visible = true
            };
            taskbarMenuShowWindow.Click += ShowWindow;
            taskbarMenuPopup.Items.Add(taskbarMenuShowWindow);
            //Separator
            taskbarMenuPopup.Items.Add(new PopupMenuSeparator());
            //Quit
            var taskbarMenuQuit = new PopupMenuItem()
            {
                Text = _controller.Localizer["Quit"],
                Visible = true
            };
            taskbarMenuQuit.Click += Quit;
            taskbarMenuPopup.Items.Add(taskbarMenuQuit);
            //Icon
            _taskbarIcon = new TrayIconWithContextMenu()
            {
                Icon = new Icon(Assembly.GetExecutingAssembly().GetManifestResourceStream("NickvisionTubeConverter.WinUI.Assets.org.nickvision.tubeconverter.resource.ico")!).Handle,
                UseStandardTooltip = true,
                ToolTip = "Nickvision Tube Converter",
                ContextMenu = taskbarMenuPopup
            };
            _taskbarIcon.Create();
        }
        else
        {
            _taskbarIcon?.Remove();
            _taskbarIcon?.Dispose();
            _taskbarIcon = null;
        }
    }

    /// <summary>
    /// Creates a download row
    /// </summary>
    /// <param name="download">The download model</param>
    /// <returns>The new download row</returns>
    private IDownloadRowControl CreateDownloadRow(Download download)
    {
        var downloadRow = new DownloadRow(_controller.Localizer, download);
        return downloadRow;
    }

    /// <summary>
    /// Moves the download row to a new section
    /// </summary>
    /// <param name="row">IDownloadRowControl</param>
    /// <param name="stage">DownloadStage</param>
    private void MoveDownloadRow(IDownloadRowControl row, DownloadStage stage)
    {
        ListDownloading.Items.Remove(row);
        ListCompleted.Items.Remove(row);
        ListQueued.Items.Remove(row);
        if (stage == DownloadStage.InQueue)
        {
            ListQueued.Items.Add(row);
        }
        else if (stage == DownloadStage.Downloading)
        {
            ListDownloading.Items.Add(row);
        }
        else if (stage == DownloadStage.Completed)
        {
            ListCompleted.Items.Add(row);
            if (!_isActived)
            {
                SendShellNotification(new ShellNotificationSentEventArgs(_controller.Localizer[row.FinishedWithError ? "DownloadFinishedWithError" : "DownloadFinished"], string.Format(_controller.Localizer[row.FinishedWithError ? "DownloadFinishedWithError" : "DownloadFinished", "Description"], $"\"{row.Filename}\""), row.FinishedWithError ? NotificationSeverity.Error : NotificationSeverity.Success));
            }
        }
        SectionDownloading.Visibility = ListDownloading.Items.Count > 0 ? Visibility.Visible : Visibility.Collapsed;
        SectionCompleted.Visibility = ListCompleted.Items.Count > 0 ? Visibility.Visible : Visibility.Collapsed;
        SectionQueued.Visibility = ListQueued.Items.Count > 0 ? Visibility.Visible : Visibility.Collapsed;
        LblStatus.Text = string.Format(_controller.Localizer["RemainingDownloads"], _controller.RemainingDownloads);
    }

    /// <summary>
    /// Deletes a download row from the queue section
    /// </summary>
    /// <param name="row">IDownloadRowControl</param>
    private void DeleteDownloadRowFromQueue(IDownloadRowControl row)
    {
        ListQueued.Items.Remove(row);
        SectionQueued.Visibility = ListQueued.Items.Count > 0 ? Visibility.Visible : Visibility.Collapsed;
    }

    /// <summary>
    /// Occurs when the add download button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void AddDownload(object sender, RoutedEventArgs e)
    {
        var addController = _controller.CreateAddDownloadDialogController();
        var addDialog = new AddDownloadDialog(addController, InitializeWithWindow)
        {
            XamlRoot = Content.XamlRoot,
            RequestedTheme = MainMenu.RequestedTheme
        };
        if (await addDialog.ShowAsync())
        {
            MenuStopAllDownloads.IsEnabled = true;
            MenuRetryFailedDownloads.IsEnabled = true;
            MenuClearQueuedDownloads.IsEnabled = true;
            ViewStack.ChangePage("Downloads");
            IconStatus.Glyph = "\uE118";
            LblStatus.Text = string.Format(_controller.Localizer["RemainingDownloads"], _controller.RemainingDownloads);
            foreach (var download in addController.Downloads)
            {
                _controller.AddDownload(download);
            }
        }
    }

    /// <summary>
    /// Occurs when the exit menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Exit(object sender, RoutedEventArgs e) => Close();

    /// <summary>
    /// Occurs when the settings menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void Settings(object sender, RoutedEventArgs e)
    {
        var preferencesDialog = new SettingsDialog(_controller.CreatePreferencesViewController())
        {
            XamlRoot = Content.XamlRoot,
            RequestedTheme = MainMenu.RequestedTheme
        };
        await preferencesDialog.ShowAsync();
    }

    /// <summary>
    /// Occurs when the stop all downloads menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void StopAllDownloads(object sender, RoutedEventArgs e) => _controller.StopAllDownloads();

    /// <summary>
    /// Occurs when the retry failed downloads menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void RetryFailedDownloads(object sender, RoutedEventArgs e) => await _controller.RetryFailedDownloadsAsync();

    /// <summary>
    /// Occurs when the clear queued downloads menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void ClearQueuedDownloads(object sender, RoutedEventArgs e) => _controller.ClearQueuedDownloads();

    /// <summary>
    /// Occurs when the about menu item is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void About(object sender, RoutedEventArgs e)
    {
        var aboutDialog = new AboutDialog(_controller.AppInfo, _controller.Localizer)
        {
            XamlRoot = Content.XamlRoot,
            RequestedTheme = MainMenu.RequestedTheme
        };
        await aboutDialog.ShowAsync();
    }

    /// <summary>
    /// Occurs when the timer ticks
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">object</param>
    private void Timer_Tick(object? sender, object e)
    {
        if (_taskbarIcon != null)
        {
            _taskbarIcon.UpdateToolTip(_controller.BackgroundActivityReport);
        }
        if (_controller.AreDownloadsRunning)
        {
            LblSpeed.Text = string.Format(_controller.Localizer["TotalSpeed"], _controller.TotalSpeedString);
        }
    }
}
