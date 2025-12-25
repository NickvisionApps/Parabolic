using Microsoft.UI.Xaml;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.WinUI.Views;

namespace Nickvision.Parabolic.WinUI;

public partial class App : Microsoft.UI.Xaml.Application
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
            _window = new MainWindow(new MainWindowController(args.Arguments.Split(' ')));
        }
        _window.Activate();
    }
}
