using Nickvision.Parabolic.GNOME.Views;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.IO;

namespace Nickvision.Parabolic.GNOME;

public class Application
{
    private string[] _args;
    private MainWindowController _controller;
    private Adw.Application _application;
    private Gio.Resource _resource;
    private MainWindow? _mainWindow;

    public Application(string[] args)
    {
        _args = new string[args.Length + 1];
        _args[0] = "org.nickvision.tubeconverter";
        args.CopyTo(_args, 1);
        _controller = new MainWindowController(_args);
        _application = Adw.Application.New(_controller.AppInfo.Id, Gio.ApplicationFlags.DefaultFlags);
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
        _application.OnStartup += Application_OnStartup;
        _application.OnActivate += Application_OnActivate;
        _application.OnOpen += Application_OnOpen;
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

    private void Application_OnActivate(Gio.Application sender, EventArgs args) => _mainWindow?.Present();

    private void Application_OnOpen(Gio.Application sender, Gio.Application.OpenSignalArgs args)
    {
        if (args.NFiles < 1)
        {
            return;
        }
        var url = args.Files[0].GetUri();
        if (!url.StartsWith("parabolic://"))
        {
            return;
        }
        if (Uri.TryCreate(url.Substring(12), UriKind.Absolute, out var uri))
        {
            _controller.UrlFromArgs = uri;
        }
        _mainWindow?.Present();
    }
}
