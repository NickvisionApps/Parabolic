using NickvisionTubeConverter.GNOME.Views;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Threading;

namespace NickvisionTubeConverter.GNOME;

/// <summary>
/// The Program 
/// </summary>
public partial class Program
{
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
        _application = Adw.Application.New("org.nickvision.tubeconverter", Gio.ApplicationFlags.FlagsNone);
        _mainWindow = null;
        _mainWindowController = new MainWindowController();
        _mainWindowController.AppInfo.Changelog =
            @"* Added the option to prevent suspend while using Parabolic
              * Improved bitrate used for audio-only downloads with the best quality
              * Parabolic will now prefer videos with the h264 codec when downloading in the mp4 format. If space is a concern, users are recommended to download in the webm format which uses the vp* codec
              * Fixed an issue where downloads with specific timeframes would download incorrectly
              * Fixed an issue where Parabolic would be unusable on systems without NetworkManager installed
              * Fixed an issue where Parabolic would say no active internet connection even though there was a connection established
              * Updated translations (Thanks everyone on Weblate!)";
        _application.OnActivate += OnActivate;
        if (File.Exists(Path.GetFullPath(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)) + "/org.nickvision.tubeconverter.gresource"))
        {
            //Load file from program directory, required for `dotnet run`
            Gio.Functions.ResourcesRegister(Gio.Functions.ResourceLoad(Path.GetFullPath(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)) + "/org.nickvision.tubeconverter.gresource"));
        }
        else
        {
            var prefixes = new List<string> {
               Directory.GetParent(Directory.GetParent(Path.GetFullPath(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location))).FullName).FullName,
               Directory.GetParent(Path.GetFullPath(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location))).FullName,
               "/usr"
            };
            foreach (var prefix in prefixes)
            {
                if (File.Exists(prefix + "/share/org.nickvision.tubeconverter/org.nickvision.tubeconverter.gresource"))
                {
                    Gio.Functions.ResourcesRegister(Gio.Functions.ResourceLoad(Path.GetFullPath(prefix + "/share/org.nickvision.tubeconverter/org.nickvision.tubeconverter.gresource")));
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
            SynchronizationContext.SetSynchronizationContext(new GLib.Internal.MainLoopSynchronizationContext());
            var argc = args.Length + 1;
            var argv = new string[argc];
            argv[0] = "org.nickvision.tubeconverter";
            args.CopyTo(argv, 1);
            return _application.Run(argc, argv);
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
        }
        else
        {
            _mainWindow = new MainWindow(_mainWindowController, _application);
            await _mainWindow.StartAsync();
        }
    }
}
