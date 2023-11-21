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
        _controller.AppInfo.Changelog = @"- Fixed an issue where some downloads' metadata was no longer downloaded correctly
- Fixed an issue where audio only downloads did not respect the selected format
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
