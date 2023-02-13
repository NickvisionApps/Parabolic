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
    private readonly Gtk.Box _mainBox;
    private readonly Adw.HeaderBar _headerBar;
    private readonly Adw.PreferencesPage _page;
    private readonly Adw.PreferencesGroup _grpUserInterface;
    private readonly Adw.ComboRow _rowTheme;
    private readonly Adw.PreferencesGroup _grpConverter;
    private readonly Adw.ActionRow _rowEmbedMetadata;
    private readonly Gtk.Switch _switchEmbedMetadata;

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
        //Main Box
        _mainBox = Gtk.Box.New(Gtk.Orientation.Vertical, 0);
        //Header Bar
        _headerBar = Adw.HeaderBar.New();
        _headerBar.SetTitleWidget(Adw.WindowTitle.New(_controller.Localizer["Preferences"], ""));
        _mainBox.Append(_headerBar);
        //Preferences Page
        _page = Adw.PreferencesPage.New();
        _mainBox.Append(_page);
        //User Interface Group
        _grpUserInterface = Adw.PreferencesGroup.New();
        _grpUserInterface.SetTitle(_controller.Localizer["UserInterface"]);
        _grpUserInterface.SetDescription(_controller.Localizer["UserInterfaceDescription"]);
        _rowTheme = Adw.ComboRow.New();
        _rowTheme.SetTitle(_controller.Localizer["Theme"]);
        _rowTheme.SetModel(Gtk.StringList.New(new string[] { _controller.Localizer["ThemeLight"], _controller.Localizer["ThemeDark"], _controller.Localizer["ThemeSystem"] }));
        _rowTheme.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "selected-item")
            {
                OnThemeChanged();
            }
        };
        _grpUserInterface.Add(_rowTheme);
        _page.Add(_grpUserInterface);
        //Converter Group
        _grpConverter = Adw.PreferencesGroup.New();
        _grpConverter.SetTitle(_controller.Localizer["Converter"]);
        _grpConverter.SetDescription(_controller.Localizer["Converter", "Description"]);
        //Embed Metadata
        _rowEmbedMetadata = Adw.ActionRow.New();
        _switchEmbedMetadata = Gtk.Switch.New();
        _switchEmbedMetadata.SetValign(Gtk.Align.Center);
        _rowEmbedMetadata.SetTitle(_controller.Localizer["EmbedMetadata"]);
        _rowEmbedMetadata.SetSubtitle(_controller.Localizer["EmbedMetadata", "Description"]);
        _rowEmbedMetadata.AddSuffix(_switchEmbedMetadata);
        _rowEmbedMetadata.SetActivatableWidget(_switchEmbedMetadata);
        _grpConverter.Add(_rowEmbedMetadata);
        _page.Add(_grpConverter);
        //Layout
        SetContent(_mainBox);
        OnHide += Hide;
        //Load Config
        _rowTheme.SetSelected((uint)_controller.Theme);
        _switchEmbedMetadata.SetActive(_controller.EmbedMetadata);
    }

    /// <summary>
    /// Occurs when the dialog is hidden
    /// </summary>
    /// <param name="sender">Gtk.Widget</param>
    /// <param name="e">EventArgs</param>
    private void Hide(Gtk.Widget sender, EventArgs e)
    {
        _controller.EmbedMetadata = _switchEmbedMetadata.GetActive();
        _controller.SaveConfiguration();
        Destroy();
    }

    /// <summary>
    /// Occurs when the theme selection is changed
    /// </summary>
    private void OnThemeChanged()
    {
        _controller.Theme = (Theme)_rowTheme.GetSelected();
        _application.StyleManager!.ColorScheme = _controller.Theme switch
        {
            Theme.System => Adw.ColorScheme.PreferLight,
            Theme.Light => Adw.ColorScheme.ForceLight,
            Theme.Dark => Adw.ColorScheme.ForceDark,
            _ => Adw.ColorScheme.PreferLight
        };
    }
}
