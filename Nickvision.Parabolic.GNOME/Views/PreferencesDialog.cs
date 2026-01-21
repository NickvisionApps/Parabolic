using Nickvision.Desktop.Application;
using Nickvision.Desktop.GNOME.Helpers;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;

namespace Nickvision.Parabolic.GNOME.Views;

public class PreferencesDialog : Adw.PreferencesDialog
{
    private readonly PreferencesViewController _controller;
    private readonly Gtk.Builder _builder;
    private IReadOnlyList<SelectionItem<Theme>> _themes;
    private IReadOnlyList<SelectionItem<string>> _languages;

    [Gtk.Connect("themeRow")]
    private Adw.ComboRow? _themeRow;
    [Gtk.Connect("languageRow")]
    private Adw.ComboRow? _languageRow;

    public PreferencesDialog(PreferencesViewController controller) : this(controller, Gtk.Builder.NewFromBlueprint("PreferencesDialog", controller.Translator))
    {

    }

    private PreferencesDialog(PreferencesViewController controller, Gtk.Builder builder) : base(new Adw.Internal.PreferencesDialogHandle(builder.GetPointer("root"), false))
    {
        _controller = controller;
        _builder = builder;
        _themes = controller.Themes;
        _languages = controller.AvailableTranslationLanguages;
        builder.Connect(this);
        // Load
        _themeRow!.SetModel(_themes);
        _languageRow!.SetModel(_languages);
        // Events
        OnClosed += Dialog_OnClosed;
        _themeRow!.OnNotify += ThemeRow_OnNotify;
    }

    private async void Dialog_OnClosed(Adw.Dialog sender, EventArgs args)
    {
        _controller.TranslationLanguage = _languages[(int)_languageRow!.Selected];
        await _controller.SaveConfigurationAsync();
    }

    private void ThemeRow_OnNotify(GObject.Object sender, NotifySignalArgs args)
    {
        if (args.Pspec.GetName() == "selected-item")
        {
            _controller.Theme = _themes[(int)_themeRow!.Selected];
            Adw.StyleManager.GetDefault().ColorScheme = _themes[(int)_themeRow!.Selected].Value switch
            {
                Theme.Light => Adw.ColorScheme.ForceLight,
                Theme.Dark => Adw.ColorScheme.ForceDark,
                _ => Adw.ColorScheme.Default
            };
        }
    }
}
