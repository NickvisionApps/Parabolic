using Microsoft.UI;
using Microsoft.UI.Composition;
using Microsoft.UI.Composition.SystemBackdrops;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Events;
using NickvisionTubeConverter.WinUI.Controls;
using System;
using Vanara.PInvoke;
using Windows.ApplicationModel.DataTransfer;
using Windows.Graphics;
using Windows.Storage;
using Windows.Storage.Pickers;
using WinRT;
using WinRT.Interop;

namespace NickvisionTubeConverter.WinUI.Views;

/// <summary>
/// The MainWindow for the application
/// </summary>
public sealed partial class MainWindow : Window
{
    private readonly MainWindowController _controller;
    private readonly IntPtr _hwnd;
    private readonly AppWindow _appWindow;
    private bool _isActived;
    private readonly SystemBackdropConfiguration _backdropConfiguration;
    private readonly MicaController? _micaController;

    /// <summary>
    /// Constructs a MainWindow
    /// </summary>
    /// <param name="controller">The MainWindowController</param>
    public MainWindow(MainWindowController controller)
    {
        InitializeComponent();
        //Initialize Vars
        _controller = controller;
        _hwnd = WindowNative.GetWindowHandle(this);
        _appWindow = AppWindow.GetFromWindowId(Win32Interop.GetWindowIdFromWindow(_hwnd));
        _isActived = true;
        //Register Events
        _appWindow.Closing += Window_Closing;
        _controller.NotificationSent += NotificationSent;
        //Set TitleBar
        TitleBarTitle.Text = _controller.AppInfo.ShortName;
        _appWindow.Title = TitleBarTitle.Text;
        TitlePreview.Text = _controller.IsDevVersion ? _controller.Localizer["Preview", "WinUI"] : "";
        if (AppWindowTitleBar.IsCustomizationSupported())
        {
            _appWindow.TitleBar.ExtendsContentIntoTitleBar = true;
            TitleBarLeftPaddingColumn.Width = new GridLength(_appWindow.TitleBar.LeftInset);
            TitleBarRightPaddingColumn.Width = new GridLength(_appWindow.TitleBar.RightInset);
            _appWindow.TitleBar.ButtonBackgroundColor = Colors.Transparent;
            _appWindow.TitleBar.ButtonInactiveBackgroundColor = Colors.Transparent;
        }
        else
        {
            TitleBar.Visibility = Visibility.Collapsed;
            NavView.Margin = new Thickness(0, 0, 0, 0);
        }
        //Setup Backdrop
        WindowsSystemDispatcherQueueHelper.EnsureWindowsSystemDispatcherQueueController();
        _backdropConfiguration = new SystemBackdropConfiguration()
        {
            IsInputActive = true,
            Theme = ((FrameworkElement)Content).ActualTheme switch
            {
                ElementTheme.Default => SystemBackdropTheme.Default,
                ElementTheme.Light => SystemBackdropTheme.Light,
                ElementTheme.Dark => SystemBackdropTheme.Dark,
                _ => SystemBackdropTheme.Default
            }
        };
        if (MicaController.IsSupported())
        {
            _micaController = new MicaController();
            _micaController.AddSystemBackdropTarget(this.As<ICompositionSupportsSystemBackdrop>());
            _micaController.SetSystemBackdropConfiguration(_backdropConfiguration);
        }
        //Window Sizing
        _appWindow.Resize(new SizeInt32(800, 600));
        User32.ShowWindow(_hwnd, ShowWindowCommand.SW_SHOWMAXIMIZED);
        //Localize Strings
        NavViewItemHome.Content = _controller.Localizer["Home"];
        NavViewItemSettings.Content = _controller.Localizer["Settings"];
        StatusPageHome.Glyph = _controller.ShowSun ? "\xE706" : "\xE708";
        StatusPageHome.Title = _controller.Greeting;
        StatusPageHome.Description = _controller.Localizer["NoFolderDescription"];
        //Page
        NavViewItemHome.IsSelected = true;
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
        _appWindow.TitleBar.ButtonForegroundColor = ((SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"]).Color;
        //Update Backdrop
        _backdropConfiguration.IsInputActive = _isActived;
    }

    /// <summary>
    /// Occurs when the window is closing
    /// </summary>
    /// <param name="sender">AppWindow</param>
    /// <param name="e">AppWindowClosingEventArgs</param>
    private void Window_Closing(AppWindow sender, AppWindowClosingEventArgs e) => _micaController?.Dispose();

    /// <summary>
    /// Occurs when the window's theme is changed
    /// </summary>
    /// <param name="sender">FrameworkElement</param>
    /// <param name="e">object</param>
    private void Window_ActualThemeChanged(FrameworkElement sender, object e)
    {
        //Update TitleBar
        TitleBarTitle.Foreground = (SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"];
        _appWindow.TitleBar.ButtonForegroundColor = ((SolidColorBrush)Application.Current.Resources[_isActived ? "WindowCaptionForeground" : "WindowCaptionForegroundDisabled"]).Color;
        //Update Backdrop
        _backdropConfiguration.Theme = sender.ActualTheme switch
        {
            ElementTheme.Default => SystemBackdropTheme.Default,
            ElementTheme.Light => SystemBackdropTheme.Light,
            ElementTheme.Dark => SystemBackdropTheme.Dark,
            _ => SystemBackdropTheme.Default
        };
    }

    /// <summary>
    /// Occurs when the NavigationView's item selection is changed
    /// </summary>
    /// <param name="sender">NavigationView</param>
    /// <param name="e">NavigationViewSelectionChangedEventArgs</param>
    private void NavView_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs e)
    {
        var pageName = (string)((NavigationViewItem)e.SelectedItem).Tag;
        if (pageName == "Settings")
        {
            PageSettings.Content = new PreferencesPage(_controller.PreferencesViewController);
        }
        ViewStack.ChangePage(pageName);
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
        InfoBar.IsOpen = true;
    }
}
