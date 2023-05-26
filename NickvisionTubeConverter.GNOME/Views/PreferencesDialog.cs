using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.IO;
using System.Runtime.InteropServices;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// The PreferencesDialog for the application
/// </summary>
public partial class PreferencesDialog : Adw.PreferencesWindow
{
    private delegate void GAsyncReadyCallback(nint source, nint res, nint user_data);
    
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial string g_file_get_path(nint file);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint gtk_file_dialog_new();
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_dialog_set_title(nint dialog, string title);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_dialog_set_filters(nint dialog, nint filters);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_dialog_open(nint dialog, nint parent, nint cancellable, GAsyncReadyCallback callback, nint user_data);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint gtk_file_dialog_open_finish(nint dialog, nint result, nint error);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint gtk_uri_launcher_new(string uri);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_uri_launcher_launch(nint uriLauncher, nint parent, nint cancellable, GAsyncReadyCallback callback, nint data);
    
    private readonly PreferencesViewController _controller;
    private readonly Adw.Application _application;

    [Gtk.Connect] private readonly Adw.ComboRow _themeRow;
    [Gtk.Connect] private readonly Adw.ActionRow _backgroundRow;
    [Gtk.Connect] private readonly Gtk.Switch _backgroundSwitch;
    [Gtk.Connect] private readonly Gtk.SpinButton _maxNumberOfActiveDownloadsSpin;
    [Gtk.Connect] private readonly Gtk.SpinButton _speedLimitSpin;
    [Gtk.Connect] private readonly Adw.ExpanderRow _useAriaRow;
    [Gtk.Connect] private readonly Adw.ViewStack _cookiesViewStack;
    [Gtk.Connect] private readonly Gtk.Button _selectCookiesFileButton;
    [Gtk.Connect] private readonly Gtk.Button _cookiesFileButton;
    [Gtk.Connect] private readonly Gtk.Label _cookiesFileLabel;
    [Gtk.Connect] private readonly Gtk.Button _unsetCookiesFileButton;
    [Gtk.Connect] private readonly Gtk.Button _chromeCookiesButton;
    [Gtk.Connect] private readonly Gtk.Button _firefoxCookiesButton;
    [Gtk.Connect] private readonly Gtk.Switch _embedMetadataSwitch;
    
    private GAsyncReadyCallback _fileDialogCallback;

    private PreferencesDialog(Gtk.Builder builder, PreferencesViewController controller, Adw.Application application, Gtk.Window parent) : base(builder.GetPointer("_root"), false)
    {
        //Window Settings
        _controller = controller;
        _application = application;
        SetTransientFor(parent);
        SetIconName(_controller.AppInfo.ID);
        //Build UI
        builder.Connect(this);
        _themeRow.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "selected-item")
            {
                OnThemeChanged();
            }
        };
        _selectCookiesFileButton.OnClicked += SelectCookiesFile;
        _cookiesFileButton.OnClicked += SelectCookiesFile;
        _unsetCookiesFileButton.OnClicked += UnsetCookiesFile;
        _chromeCookiesButton.OnClicked += LaunchChromeCookiesExtension;
        _firefoxCookiesButton.OnClicked += LaunchFirefoxCookiesExtension;
        OnHide += Hide;
        //Load Config
        _themeRow.SetSelected((uint)_controller.Theme);
        _backgroundRow.SetVisible(File.Exists("/.flatpak-info") || !String.IsNullOrEmpty(Environment.GetEnvironmentVariable("SNAP")));
        _backgroundSwitch.SetActive(_controller.RunInBackground);
        _maxNumberOfActiveDownloadsSpin.SetValue(_controller.MaxNumberOfActiveDownloads);
        _speedLimitSpin.SetValue(_controller.SpeedLimit);
        _useAriaRow.SetEnableExpansion(_controller.UseAria);
        if (File.Exists(_controller.CookiesPath))
        {
            _cookiesViewStack.SetVisibleChildName("file-selected");
            _cookiesFileLabel.SetText(_controller.CookiesPath);
        }
        _embedMetadataSwitch.SetActive(_controller.EmbedMetadata);
    }

    /// <summary>
    /// Constructs a PreferencesDialog
    /// </summary>
    /// <param name="controller">PreferencesViewController</param>
    /// <param name="application">Adw.Application</param>
    /// <param name="parent">Gtk.Window</param>
    public PreferencesDialog(PreferencesViewController controller, Adw.Application application, Gtk.Window parent) : this(Builder.FromFile("preferences_dialog.ui", controller.Localizer), controller, application, parent)
    {
    }

    /// <summary>
    /// Occurs when the dialog is hidden
    /// </summary>
    /// <param name="sender">Gtk.Widget</param>
    /// <param name="e">EventArgs</param>
    private void Hide(Gtk.Widget sender, EventArgs e)
    {
        _controller.RunInBackground = _backgroundSwitch.GetActive();
        _controller.MaxNumberOfActiveDownloads = (int)_maxNumberOfActiveDownloadsSpin.GetValue();
        _controller.SpeedLimit = (uint)_speedLimitSpin.GetValue();
        _controller.UseAria = _useAriaRow.GetEnableExpansion();
        _controller.EmbedMetadata = _embedMetadataSwitch.GetActive();
        _controller.SaveConfiguration();
        Destroy();
    }

    /// <summary>
    /// Occurs when the theme selection is changed
    /// </summary>
    private void OnThemeChanged()
    {
        _controller.Theme = (Theme)_themeRow.GetSelected();
        _application.StyleManager!.ColorScheme = _controller.Theme switch
        {
            Theme.System => Adw.ColorScheme.PreferLight,
            Theme.Light => Adw.ColorScheme.ForceLight,
            Theme.Dark => Adw.ColorScheme.ForceDark,
            _ => Adw.ColorScheme.PreferLight
        };
    }

    /// <summary>
    /// Occurs when a button to select cookies file is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private void SelectCookiesFile(Gtk.Button sender, EventArgs e)
    {
        var filterTxt = Gtk.FileFilter.New();
        filterTxt.SetName("TXT (*.txt)");
        filterTxt.AddPattern("*.txt");
        filterTxt.AddPattern("*.TXT");
        var fileDialog = gtk_file_dialog_new();
        gtk_file_dialog_set_title(fileDialog, _controller.Localizer["SelectCookiesFile"]);
        var filters = Gio.ListStore.New(Gtk.FileFilter.GetGType());
        filters.Append(filterTxt);
        gtk_file_dialog_set_filters(fileDialog, filters.Handle);
        _fileDialogCallback = async (source, res, data) =>
        {
            var fileHandle = gtk_file_dialog_open_finish(fileDialog, res, IntPtr.Zero);
            if (fileHandle != IntPtr.Zero)
            {
                var path = g_file_get_path(fileHandle);
                _controller.CookiesPath = path;
                _cookiesViewStack.SetVisibleChildName("file-selected");
                _cookiesFileLabel.SetText(path);
            }
        };
        gtk_file_dialog_open(fileDialog, Handle, IntPtr.Zero, _fileDialogCallback, IntPtr.Zero);
    }

    /// <summary>
    /// Occurs when a button to clear cookies file is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private void UnsetCookiesFile(Gtk.Button sender, EventArgs e)
    {
        _controller.CookiesPath = "";
        _cookiesViewStack.SetVisibleChildName("no-file");
    }

    /// <summary>
    /// Occurs when a button to open chrome's cookies extension is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private void LaunchChromeCookiesExtension(Gtk.Button sender, EventArgs e)
    {
        var uriLauncher = gtk_uri_launcher_new("https://chrome.google.com/webstore/detail/get-cookiestxt-locally/cclelndahbckbenkjhflpdbgdldlbecc");
        gtk_uri_launcher_launch(uriLauncher, 0, 0, (source, res, data) => { }, 0);
    }

    /// <summary>
    /// Occurs when a button to open firefox's cookies extension is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private void LaunchFirefoxCookiesExtension(Gtk.Button sender, EventArgs e)
    {
        var uriLauncher = gtk_uri_launcher_new("https://addons.mozilla.org/en-US/firefox/addon/cookies-txt/");
        gtk_uri_launcher_launch(uriLauncher, 0, 0, (source, res, data) => { }, 0);
        
    }
}
