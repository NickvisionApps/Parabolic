using Microsoft.UI.Xaml;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using NickvisionTubeConverter.WinUI.Views;
using System;

namespace NickvisionTubeConverter.WinUI;


/// <summary>
/// The App
/// </summary>
public partial class App : Application
{
    public static Window? MainWindow { get; private set; } = null;
    private readonly MainWindowController _mainWindowController;

    /// <summary>
    /// Constructs an App
    /// </summary>
    public App()
    {
        InitializeComponent();
        _mainWindowController = new MainWindowController();
        //AppInfo
        _mainWindowController.AppInfo.ID = "org.nickvision.tubeconverter";
        _mainWindowController.AppInfo.Name = "Nickvision Tube Converter";
        _mainWindowController.AppInfo.ShortName = "Tube Converter";
        _mainWindowController.AppInfo.Description = $"{_mainWindowController.Localizer["Description"]}.";
        _mainWindowController.AppInfo.Version = "2023.4.1-next";
        _mainWindowController.AppInfo.Changelog = "- Added the ability to open a download file directly instead of just the save folder\n- Fixed an issue where the system tray icon would cause the application to crash\n- Fixed an issue where embedding metadata would cause some downloads to fail\n- Fixed an issue where some file formats and qualities could not be downloaded\n- Updated translations (Thanks everyone on Weblate!)";
        _mainWindowController.AppInfo.GitHubRepo = new Uri("https://github.com/nlogozzo/NickvisionTubeConverter");
        _mainWindowController.AppInfo.IssueTracker = new Uri("https://github.com/nlogozzo/NickvisionTubeConverter/issues/new");
        _mainWindowController.AppInfo.SupportUrl = new Uri("https://github.com/nlogozzo/NickvisionTubeConverter/discussions");
        //Theme
        if (_mainWindowController.Theme == Theme.Light)
        {
            RequestedTheme = ApplicationTheme.Light;
        }
        else if (_mainWindowController.Theme == Theme.Dark)
        {
            RequestedTheme = ApplicationTheme.Dark;
        }
    }

    /// <summary>
    /// Occurs when the app is launched
    /// </summary>
    /// <param name="args">LaunchActivatedEventArgs</param>
    protected override void OnLaunched(LaunchActivatedEventArgs args)
    {
        //Main Window
        MainWindow = new MainWindow(_mainWindowController);
        MainWindow.Activate();
    }
}
