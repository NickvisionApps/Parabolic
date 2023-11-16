using Microsoft.UI.Xaml;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using NickvisionTubeConverter.WinUI.Views;
using System;

namespace NickvisionTubeConverter.WinUI;

/// <summary>
/// Provides application-specific behavior to supplement the default Application class.
/// </summary>
public partial class App : Application
{
    private MainWindow? _mainWindow;
    private MainWindowController _controller;

    /// <summary>
    /// Initializes the singleton application object.  This is the first line of authored code
    /// executed, and as such is the logical equivalent of main() or WinMain().
    /// </summary>
    public App()
    {
        InitializeComponent();
        _controller = new MainWindowController(Array.Empty<string>());
        _controller.AppInfo.Changelog = @"- Parabolic is now available for Windows using Windows App SDK and WinUI 3
- Added support for auto-generated subtitles from English
- Added the ability to turn off downloading auto-generated subtitles
- Added the advanced option to prefer the adv1 codec for video downloads
- Added the ""Best"" resolution when downloading videos to allow Parabolic to pick the highest resolution for each video download
- Fixed an issue where aria's max connections per server preference was allowed to be greater than 16
- Fixed an issue where enabling the ""Download Specific Timeframe"" advanced option would cause a crash for certain media downloads
- Fixed an issue where stopping all downloads would cause the app to crash
- Updated translations (Thanks everyone on Weblate!)";
        if (_controller.Theme != Theme.System)
        {
            RequestedTheme = _controller.Theme switch
            {
                Theme.Light => ApplicationTheme.Light,
                Theme.Dark => ApplicationTheme.Dark,
                _ => ApplicationTheme.Light
            };
        }
    }

    /// <summary>
    /// Shows the main window of the app (if there is one)
    /// </summary>
    public void ShowMainWindow()
    {
        if (_mainWindow != null)
        {
            _mainWindow.DispatcherQueue.TryEnqueue(() =>
            {
                _mainWindow.ShowWindow(null, new RoutedEventArgs());
            });
        }
    }

    /// <summary>
    /// Invoked when the application is launched.
    /// </summary>
    /// <param name="args">Details about the launch request and process.</param>
    protected override void OnLaunched(LaunchActivatedEventArgs args)
    {
        _mainWindow = new MainWindow(_controller);
        _mainWindow.Activate();
    }
}
