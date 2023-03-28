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
        _mainWindowController.AppInfo.Version = "2023.3.1";
        _mainWindowController.AppInfo.Changelog = "- Fixed an issue where the user could not download playlists with unavailable videos\n- Fixed an issue where the some videos could not be downloaded when embedding metadata\n- Fixed an issue where videos with invalid filename characters could not be downloaded\n- Updated translations (Thanks everyone on Weblate!)";
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
