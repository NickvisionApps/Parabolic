using Microsoft.UI.Xaml;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.WinUI.Views;
using System;

namespace Nickvision.Parabolic.WinUI;

public partial class App : Application
{
    private Window? _window;

    public App()
    {
        InitializeComponent();
    }

    protected override void OnLaunched(LaunchActivatedEventArgs args)
    {
        if (_window is null)
        {
            _window = new MainWindow(new MainWindowController(Environment.GetCommandLineArgs()));
        }
        _window.Activate();
    }
}
