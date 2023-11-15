using Nickvision.Aura;
using NickvisionTubeConverter.GNOME.Views;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;

namespace NickvisionTubeConverter.GNOME;

/// <summary>
/// The Program 
/// </summary>
public partial class Program
{
    public delegate void OpenCallback(nint application, nint[] files, int n_files, nint hint, nint data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial ulong g_signal_connect_data(nint instance, string signal, OpenCallback callback, nint data, nint destroy_data, int flags);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial string g_file_get_uri(nint file);

    private readonly Adw.Application _application;
    private MainWindow? _mainWindow;
    private MainWindowController _mainWindowController;

    /// <summary>
    /// Main method
    /// </summary>
    /// <param name="args">string[]</param>
    /// <returns>Return code from Adw.Application.Run()</returns>
    public static int Main(string[] args) => new Program(args).Run(args);

    /// <summary>
    /// Constructs a Program
    /// </summary>
    /// <param name="args">Command-line arguments</param>
    public Program(string[] args)
    {
        _application = Adw.Application.New("org.nickvision.tubeconverter", Gio.ApplicationFlags.HandlesOpen);
        _mainWindow = null;
        _mainWindowController = new MainWindowController(args);
        _mainWindowController.AppInfo.Changelog =
            @"* Added support for auto-generated subtitles from English
              * Added the ability to turn off downloading auto-generated subtitles
              * Added the advanced option to prefer the adv1 codec for video downloads
              * Added the ""Best"" resolution when downloading videos to allow Parabolic to pick the highest resolution for each video download
              * A URL can now be passed to Parabolic via the command-line or the freedesktop application open protocol to trigger its validation of startup
              * Improved the design of the Preferences dialog to allow for better searching of options
              * Fixed an issue where aria's max connections per server preference was allowed to be greater than 16
              * Fixed an issue where enabling the ""Download Specific Timeframe"" advanced option would cause a crash for certain media downloads
              * Fixed an issue where stopping all downloads would cause the app to crash
              * Updated to GNOME 45 runtime with latest libadwaita design
              * Updated translations (Thanks everyone on Weblate!)";
        _application.OnActivate += OnActivate;
        g_signal_connect_data(_application.Handle, "open", OnOpen, IntPtr.Zero, IntPtr.Zero, 0);
        if (File.Exists(Path.GetFullPath(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)!) + "/org.nickvision.tubeconverter.gresource"))
        {
            //Load file from program directory, required for `dotnet run`
            Gio.Functions.ResourcesRegister(Gio.Functions.ResourceLoad(Path.GetFullPath(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)!) + "/org.nickvision.tubeconverter.gresource"));
        }
        else
        {
            foreach (var dir in SystemDirectories.Data)
            {
                if (File.Exists($"{dir}/org.nickvision.tubeconverter/org.nickvision.tubeconverter.gresource"))
                {
                    Gio.Functions.ResourcesRegister(Gio.Functions.ResourceLoad(Path.GetFullPath($"{dir}/org.nickvision.tubeconverter/org.nickvision.tubeconverter.gresource")));
                    break;
                }
            }
        }
    }

    /// <summary>
    /// Runs the program
    /// </summary>
    /// <param name="args">Command-line arguments</param>
    /// <returns>Return code from Adw.Application.Run()</returns>
    public int Run(string[] args)
    {
        try
        {
            var argv = new string[args.Length + 1];
            argv[0] = "org.nickvision.tubeconverter";
            args.CopyTo(argv, 1);
            return _application.RunWithSynchronizationContext(argv);
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex.Message);
            Console.WriteLine($"\n\n{ex.StackTrace}");
            return -1;
        }
    }

    /// <summary>
    /// Occurs when the application is activated
    /// </summary>
    /// <param name="sender">Gio.Application</param>
    /// <param name="e">EventArgs</param>
    private async void OnActivate(Gio.Application sender, EventArgs e)
    {
        //Set Adw Theme
        _application.StyleManager!.ColorScheme = _mainWindowController.Theme switch
        {
            Theme.System => Adw.ColorScheme.PreferLight,
            Theme.Light => Adw.ColorScheme.ForceLight,
            Theme.Dark => Adw.ColorScheme.ForceDark,
            _ => Adw.ColorScheme.PreferLight
        };
        //Main Window
        if (_mainWindow != null)
        {
            _mainWindow!.SetVisible(true);
            _mainWindow!.Present();
            if (!string.IsNullOrEmpty(_mainWindowController.UrlToLaunch))
            {
                await _mainWindow.AddDownloadAsync(_mainWindowController.UrlToLaunch);
                _mainWindowController.UrlToLaunch = null;
            }
        }
        else
        {
            _mainWindow = new MainWindow(_mainWindowController, _application);
            await _mainWindow.StartAsync();
        }
    }

    /// <summary>
    /// Occurs when the application is opened
    /// </summary>
    /// <param name="sender">Gio.Application</param>
    /// <param name="e">Gio.Application.OpenSignalArgs</param>
    private void OnOpen(nint application, nint[] files, int n_files, nint hint, nint data)
    {
        if(n_files > 0)
        {
            try
            {
                _mainWindowController.UrlToLaunch = g_file_get_uri(files[0]);
            }
            catch { }
        }
        _application.Activate();
    }
}
