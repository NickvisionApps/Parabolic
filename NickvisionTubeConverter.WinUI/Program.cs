using Microsoft.UI.Dispatching;
using Microsoft.Windows.AppLifecycle;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.WinUI;

/// <summary>
/// The program's main entry
/// </summary>
public class Program
{
    /// <summary>
    /// The main method
    /// </summary>
    /// <param name="args">string[]</param>
    [STAThread]
    static async Task Main(string[] args)
    {
        WinRT.ComWrappersSupport.InitializeComWrappers();
        var isFirstInstance = false;
        var keyInstance = AppInstance.FindOrRegisterForKey("PARABOLIC_INSTANCE");
        if (keyInstance.IsCurrent)
        {
            keyInstance.Activated += OnActivated;
            isFirstInstance = true;
        }
        else
        {
            await keyInstance.RedirectActivationToAsync(AppInstance.GetCurrent().GetActivatedEventArgs());
        }
        if (isFirstInstance)
        {
            Microsoft.UI.Xaml.Application.Start((p) =>
            {
                var context = new DispatcherQueueSynchronizationContext(DispatcherQueue.GetForCurrentThread());
                SynchronizationContext.SetSynchronizationContext(context);
                new App();
            });
        }
    }

    /// <summary>
    /// Occurs when the first instance of the app is activated
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="args">AppActivationArguments</param>
    private static void OnActivated(object? sender, AppActivationArguments args) => (App.Current as App)!.ShowMainWindow();
}
