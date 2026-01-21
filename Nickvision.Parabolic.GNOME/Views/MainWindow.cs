using Nickvision.Desktop.GNOME.Controls;
using Nickvision.Desktop.GNOME.Helpers;
using Nickvision.Desktop.Notifications;
using Nickvision.Parabolic.Shared.Controllers;
using System;
using System.Linq;

namespace Nickvision.Parabolic.GNOME.Views;

public class MainWindow : Adw.ApplicationWindow
{
    private readonly MainWindowController _controller;
    private readonly Gtk.Builder _builder;
    private readonly Gio.SimpleAction _actQuit;
    private readonly Gio.SimpleAction _actPreferences;
    private readonly Gio.SimpleAction _actKeyboardShortcuts;
    private readonly Gio.SimpleAction _actAbout;

    [Gtk.Connect("windowTitle")]
    private Adw.WindowTitle? _windowTitle;
    [Gtk.Connect("toastOverlay")]
    private Adw.ToastOverlay? _toastOverlay;
    [Gtk.Connect("viewStack")]
    private Adw.ViewStack? _viewStack;

    public MainWindow(MainWindowController controller, Adw.Application application) : this(controller, application, Gtk.Builder.NewFromBlueprint("MainWindow", controller.Translator))
    {

    }

    private MainWindow(MainWindowController controller, Adw.Application application, Gtk.Builder builder) : base(new Adw.Internal.ApplicationWindowHandle(builder.GetPointer("root"), false))
    {
        Application = application;
        _controller = controller;
        _builder = builder;
        _builder.Connect(this);
        // Window
        Title = _controller.AppInfo.ShortName;
        IconName = _controller.AppInfo.Id;
        if (_controller.AppInfo.Version!.IsPreview)
        {
            AddCssClass("devel");
        }
        _windowTitle!.Title = _controller.AppInfo.ShortName;
        // Events
        OnCloseRequest += Window_OnCloseRequest;
        _controller.AppNotificationSent += (sender, args) => GLib.Functions.IdleAdd(200, () =>
        {
            Controller_AppNotificationSent(sender, args);
            return false;
        });
        // Quit action
        _actQuit = Gio.SimpleAction.New("quit", null);
        _actQuit.OnActivate += Quit;
        AddAction(_actQuit);
        Application!.SetAccelsForAction("win.quit", ["<Ctrl>q"]);
        // Preferences action
        _actPreferences = Gio.SimpleAction.New("preferences", null);
        _actPreferences.OnActivate += Preferences;
        AddAction(_actPreferences);
        Application!.SetAccelsForAction("win.preferences", ["<Ctrl>period"]);
        // Keyboard shortcuts action
        _actKeyboardShortcuts = Gio.SimpleAction.New("keyboardShortcuts", null);
        _actKeyboardShortcuts.OnActivate += KeyboardShortcuts;
        AddAction(_actKeyboardShortcuts);
        Application!.SetAccelsForAction("win.keyboardShortcuts", ["<Ctrl>question"]);
        // About action
        _actAbout = Gio.SimpleAction.New("about", null);
        _actAbout.OnActivate += About;
        AddAction(_actAbout);
        Application!.SetAccelsForAction("win.about", ["F1"]);
    }

    public new void Present()
    {
        base.Present();
        this.WindowGeometry = _controller.WindowGeometry;
    }

    private bool Window_OnCloseRequest(Gtk.Window sender, EventArgs args)
    {
        if (!_controller.CanShutdown)
        {
            return true;
        }
        GetDefaultSize(out int width, out int height);
        _controller.WindowGeometry = this.WindowGeometry;
        _controller.Dispose();
        Destroy();
        return false;
    }

    private void Controller_AppNotificationSent(object? sender, AppNotificationSentEventArgs args)
    {
        var toast = Adw.Toast.New(args.Notification.Message);
        _toastOverlay!.AddToast(toast);
    }

    private void Quit(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs args)
    {
        if (!Window_OnCloseRequest(this, new EventArgs()))
        {
            Application!.Quit();
        }
    }

    private void Preferences(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs args)
    {
        var preferencesDialog = new PreferencesDialog(_controller.PreferencesViewController);
        preferencesDialog.Present(this);
    }

    private void KeyboardShortcuts(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs args)
    {
        var shortcutsDialog = new ShortcutsDialog(_controller.Translator);
        shortcutsDialog.Present(this);
    }

    private async void About(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs args)
    {
        var extraInfo = string.Empty;
        extraInfo += $"GTK {Gtk.Functions.GetMajorVersion()}.{Gtk.Functions.GetMinorVersion()}.{Gtk.Functions.GetMicroVersion()}\n";
        extraInfo += $"libadwaita {Adw.Functions.GetMajorVersion()}.{Adw.Functions.GetMinorVersion()}.{Adw.Functions.GetMicroVersion()}";
        var dialog = Adw.AboutDialog.New();
        dialog.ApplicationName = _controller.AppInfo.ShortName;
        dialog.ApplicationIcon = _controller.AppInfo.Version!.IsPreview ? $"{_controller.AppInfo.Id}-devel" : _controller.AppInfo.Id;
        dialog.DeveloperName = "Nickvision";
        dialog.Version = _controller.AppInfo.Version.ToString();
        dialog.ReleaseNotes = _controller.AppInfo.HtmlChangelog;
        dialog.DebugInfo = await _controller.GetDebugInformationAsync(extraInfo);
        dialog.Comments = _controller.AppInfo.Description;
        dialog.LicenseType = Gtk.License.MitX11;
        dialog.Copyright = "© Nickvision 2021-2025";
        dialog.Website = "https://nickvision.org";
        dialog.IssueUrl = _controller.AppInfo.IssueTracker!.ToString();
        dialog.SupportUrl = _controller.AppInfo.DiscussionsForum!.ToString();
        dialog.AddLink(_controller.Translator._("GitHub Repo"), _controller.AppInfo.SourceRepository!.ToString());
        foreach (var pair in _controller.AppInfo.ExtraLinks)
        {
            dialog.AddLink(pair.Key, pair.Value.ToString());
        }
        dialog.Developers = _controller.AppInfo.Developers.Select(x => $"{x.Key} {x.Value}").ToArray();
        dialog.Designers = _controller.AppInfo.Designers.Select(x => $"{x.Key} {x.Value}").ToArray();
        dialog.Artists = _controller.AppInfo.Artists.Select(x => $"{x.Key} {x.Value}").ToArray();
        if (!string.IsNullOrEmpty(_controller.AppInfo.TranslationCredits) && _controller.AppInfo.TranslationCredits != "translation-credits")
        {
            dialog.TranslatorCredits = _controller.AppInfo.TranslationCredits;
        }
        dialog.Present(this);
    }
}
