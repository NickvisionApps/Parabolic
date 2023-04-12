using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.IO;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// The PreferencesDialog for the application
/// </summary>
public partial class PreferencesDialog : Adw.PreferencesWindow
{
    private readonly PreferencesViewController _controller;
    private readonly Adw.Application _application;

    [Gtk.Connect] private readonly Adw.ComboRow _themeRow;
    [Gtk.Connect] private readonly Adw.ActionRow _backgroundRow;
    [Gtk.Connect] private readonly Gtk.Switch _backgroundSwitch;
    [Gtk.Connect] private readonly Adw.ComboRow _maxNumberOfActiveDownloadsRow;
    [Gtk.Connect] private readonly Gtk.SpinButton _speedLimitSpin;
    [Gtk.Connect] private readonly Gtk.Switch _useAriaSwitch;
    [Gtk.Connect] private readonly Gtk.Switch _embedMetadataSwitch;

    private PreferencesDialog(Gtk.Builder builder, PreferencesViewController controller, Adw.Application application, Gtk.Window parent) : base(builder.GetPointer("_root"), false)
    {
        //Window Settings
        _controller = controller;
        _application = application;
        SetTransientFor(parent);
        SetIconName(_controller.AppInfo.ID);
        //Build UI
        builder.Connect(this);
        //Theme
        _themeRow.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "selected-item")
            {
                OnThemeChanged();
            }
        };
        OnHide += Hide;
        //Load Config
        _themeRow.SetSelected((uint)_controller.Theme);
        _backgroundRow.SetVisible(File.Exists("/.flatpak-info"));
        _backgroundSwitch.SetActive(_controller.RunInBackground);
        _maxNumberOfActiveDownloadsRow.SetSelected((uint)(_controller.MaxNumberOfActiveDownloads - 1));
        _speedLimitSpin.SetValue((double)_controller.SpeedLimit);
        _useAriaSwitch.SetActive(_controller.UseAria);
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
        _controller.MaxNumberOfActiveDownloads = (int)_maxNumberOfActiveDownloadsRow.GetSelected() + 1;
        _controller.SpeedLimit = (uint)_speedLimitSpin.GetValue();
        _controller.UseAria = _useAriaSwitch.GetActive();
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
}
