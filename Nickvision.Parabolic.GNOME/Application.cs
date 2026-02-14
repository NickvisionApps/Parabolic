using Nickvision.Parabolic.GNOME.Views;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.IO;
using System.Runtime.InteropServices;

namespace Nickvision.Parabolic.GNOME;

public partial class Application
{
    private readonly string[] _args;
    private readonly MainWindowController _controller;
    private readonly Adw.Application _application;
    private readonly Gio.Resource _resource;
    private readonly OpenCallback _openCallback;
    private MainWindow? _mainWindow;

    public delegate void OpenCallback(nint application, nint[] files, int n_files, nint hint, nint data);
#if WINDOWS
    [LibraryImport("libgobject-2.0-0.dll", StringMarshalling = StringMarshalling.Utf8)]
#else
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
#endif
    private static partial ulong g_signal_connect_data(nint instance, string signal, OpenCallback callback, nint data, nint destroy_data, int flags);
#if WINDOWS
    [LibraryImport("libgio-2.0-0.dll", StringMarshalling = StringMarshalling.Utf8)]
#else
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
#endif
    private static partial string g_file_get_uri(nint file);

    public Application(string[] args)
    {
        _args = new string[args.Length + 1];
        _args[0] = "org.nickvision.tubeconverter";
        args.CopyTo(_args, 1);
        _controller = new MainWindowController(_args);
        _application = Adw.Application.New(_controller.AppInfo.Id, Gio.ApplicationFlags.HandlesOpen);
        var resourceFilePath = Path.Combine(Desktop.System.Environment.ExecutingDirectory, $"{_controller.AppInfo.Id}.gresource");
        try
        {
            _resource = Gio.Resource.Load(resourceFilePath);
            _resource.Register();
        }
        catch (Exception ex)
        {
            throw new Exception($"Failed to load GResource file: {resourceFilePath}", ex);
        }
        _openCallback = Application_OnOpen;
        _application.OnStartup += Application_OnStartup;
        _application.OnActivate += Application_OnActivate;
        g_signal_connect_data(_application.Handle.DangerousGetHandle(), "open", _openCallback, IntPtr.Zero, IntPtr.Zero, 0);
    }

    public int Run() => _application.RunWithSynchronizationContext(_args);

    private void Application_OnStartup(Gio.Application sender, EventArgs args)
    {
        if (_mainWindow is null)
        {
            _mainWindow = new MainWindow(_controller, _application);
            _application.AddWindow(_mainWindow);
        }
        Adw.StyleManager.GetDefault().ColorScheme = _controller.Theme switch
        {
            Theme.Light => Adw.ColorScheme.ForceLight,
            Theme.Dark => Adw.ColorScheme.ForceDark,
            _ => Adw.ColorScheme.Default
        };
    }

    private async void Application_OnActivate(Gio.Application sender, EventArgs args)
    {
        if (_mainWindow is not null)
        {
            await _mainWindow.PresentAsync();
        }
    }

    private void Application_OnOpen(nint application, nint[] files, int n_files, nint hint, nint data)
    {
        if (n_files < 1)
        {
            return;
        }
        try
        {
            if (Uri.TryCreate(g_file_get_uri(files[0]), UriKind.Absolute, out var uri))
            {
                _controller.UrlFromArgs = uri;
            }
            else
            {
                Console.WriteLine($"Invalid URI: {g_file_get_uri(files[0])}");
            }
        }
        catch (Exception e)
        {
            Console.Error.WriteLine(e);
        }
        _application.Activate();
    }
}
