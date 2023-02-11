using Microsoft.UI.Xaml;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using NickvisionTubeConverter.WinUI.Views;
using System;
using System.IO;

namespace NickvisionTubeConverter.WinUI;


/// <summary>
/// The App
/// </summary>
public partial class App : Application
{
    private Window? _mainWindow;
    private MainWindowController _mainWindowController;

    /// <summary>
    /// Constructs an App
    /// </summary>
    public App()
    {
        InitializeComponent();
        _mainWindowController = new MainWindowController();
        //AppInfo
        _mainWindowController.AppInfo.ID = "org.nickvision.tubeconverter";
        _mainWindowController.AppInfo.Name = "NickvisionTubeConverter";
        _mainWindowController.AppInfo.ShortName = "Tube Converter";
        _mainWindowController.AppInfo.Description = $"{_mainWindowController.Localizer["Description"]}.";
        _mainWindowController.AppInfo.Version = "2023.2.1-next";
        _mainWindowController.AppInfo.Changelog = "- C# Rewrite";
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
    /// Finalizes an App
    /// </summary>
    ~App() => _mainWindowController.Dispose();

    /// <summary>
    /// Occurs when the app is launched
    /// </summary>
    /// <param name="args">LaunchActivatedEventArgs</param>
    protected override async void OnLaunched(LaunchActivatedEventArgs args)
    {
        //Python Setup
        Python.Runtime.Runtime.PythonDLL = $"{Python.Included.Installer.EmbeddedPythonHome}{Path.DirectorySeparatorChar}python311.dll";
        await Python.Included.Installer.SetupPython();
        Python.Runtime.PythonEngine.Initialize();
        //Main Window
        _mainWindow = new MainWindow(_mainWindowController);
        _mainWindow.Activate();
    }
}
