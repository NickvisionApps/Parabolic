using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using Microsoft.UI.Xaml;
using Nickvision.Parabolic.WinUI.Views;
using System;

namespace Nickvision.Parabolic.WinUI;

public partial class App : Application
{
    private readonly IServiceProvider _serviceProvider;
    private Window? _window;

    public App(IServiceProvider serviceProvider)
    {
        InitializeComponent();
        _serviceProvider = serviceProvider;
        UnhandledException += (_, e) =>
        {
            _serviceProvider.GetRequiredService<ILogger<App>>().LogError(e.Exception, $"An unhandled exception occurred: {e.Message}");
        };
    }

    protected override void OnLaunched(LaunchActivatedEventArgs args)
    {
        if (_window is null)
        {
            _window = _serviceProvider.GetRequiredService<MainWindow>();
        }
        _window.Activate();
    }
}
