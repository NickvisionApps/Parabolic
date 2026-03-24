using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Xaml;
using Microsoft.Windows.AppNotifications;
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
        AppNotificationManager.Default.NotificationInvoked += App_NotificationInvoked;
        AppNotificationManager.Default.Register();
        AppDomain.CurrentDomain.ProcessExit += async (_, _) =>
        {
            await AppNotificationManager.Default.RemoveAllAsync();
            AppNotificationManager.Default.UnregisterAll();
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

    private void App_NotificationInvoked(AppNotificationManager sender, AppNotificationActivatedEventArgs args)
    {

    }
}
