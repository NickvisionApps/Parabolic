using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// The PreferencesDialog for the application
/// </summary>
public partial class PreferencesDialog : Adw.PreferencesWindow
{
    private readonly PreferencesViewController _controller;
    private readonly Adw.Application _application;

    [Gtk.Connect] private readonly Gtk.Box _mainBox;
    [Gtk.Connect] private readonly Adw.ComboRow _themeRow;
    [Gtk.Connect] private readonly Gtk.Switch _embedMetadataSwitch;

    /// <summary>
    /// Constructs a PreferencesDialog
    /// </summary>
    /// <param name="controller">PreferencesViewController</param>
    /// <param name="application">Adw.Application</param>
    /// <param name="parent">Gtk.Window</param>
    public PreferencesDialog(PreferencesViewController controller, Adw.Application application, Gtk.Window parent)
    {
        //Window Settings
        _controller = controller;
        _application = application;
        SetTransientFor(parent);
        SetDefaultSize(600, 400);
        SetModal(true);
        SetDestroyWithParent(false);
        SetHideOnClose(true);
        //Build UI
        var builder = Builder.FromFile("preferences.ui", _controller.Localizer);
        builder.Connect(this);
        SetContent(_mainBox);
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
        _embedMetadataSwitch.SetActive(_controller.EmbedMetadata);
    }

    /// <summary>
    /// Occurs when the dialog is hidden
    /// </summary>
    /// <param name="sender">Gtk.Widget</param>
    /// <param name="e">EventArgs</param>
    private void Hide(Gtk.Widget sender, EventArgs e)
    {
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
