using Nickvision.Desktop.GNOME.Controls;
using Nickvision.Desktop.GNOME.Helpers;
using Nickvision.Desktop.Notifications;
using Nickvision.Parabolic.GNOME.Controls;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Events;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.Linq;

namespace Nickvision.Parabolic.GNOME.Views;

public class MainWindow : Adw.ApplicationWindow
{
    private readonly MainWindowController _controller;
    private readonly Gtk.Builder _builder;
    private readonly Dictionary<int, DownloadRow> _downloadRows;
    private readonly List<DownloadRow> _addedDownloadRows;

    [Gtk.Connect("windowTitle")]
    private Adw.WindowTitle? _windowTitle;
    [Gtk.Connect("toastOverlay")]
    private Adw.ToastOverlay? _toastOverlay;
    [Gtk.Connect("viewStack")]
    private Adw.ViewStack? _viewStack;
    [Gtk.Connect("downloadsToggleGroup")]
    private Adw.ToggleGroup? _downloadsToggleGroup;
    [Gtk.Connect("listDownloads")]
    private Gtk.ListBox? _listDownloads;

    public MainWindow(MainWindowController controller, Adw.Application application) : this(controller, application, Gtk.Builder.NewFromBlueprint("MainWindow", controller.Translator))
    {

    }

    private MainWindow(MainWindowController controller, Adw.Application application, Gtk.Builder builder) : base(new Adw.Internal.ApplicationWindowHandle(builder.GetPointer("root"), false))
    {
        Application = application;
        _controller = controller;
        _builder = builder;
        _downloadRows = new Dictionary<int, DownloadRow>();
        _addedDownloadRows = new List<DownloadRow>();
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
        _controller.AppNotificationSent += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            Controller_AppNotificationSent(sender, e);
            return false;
        });
        _controller.DownloadAdded += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            Controller_DownloadAdded(sender, e);
            return false;
        });
        _controller.DownloadProgressChanged += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            Controller_DownloadProgressChanged(sender, e);
            return false;
        });
        _controller.DownloadCompleted += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            Controller_DownloadCompleted(sender, e);
            return false;
        });
        _controller.DownloadStopped += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            Controller_DownloadStopped(sender, e);
            return false;
        });
        _controller.DownloadStartedFromQueue += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            Controller_DownloadStartedFromQueue(sender, e);
            return false;
        });
        _controller.DownloadRetired += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            Controller_DownloadRetired(sender, e);
            return false;
        });
        // Quit action
        var actQuit = Gio.SimpleAction.New("quit", null);
        actQuit.OnActivate += Quit;
        AddAction(actQuit);
        Application!.SetAccelsForAction("win.quit", ["<Ctrl>q"]);
        // Preferences action
        var actPreferences = Gio.SimpleAction.New("preferences", null);
        actPreferences.OnActivate += Preferences;
        AddAction(actPreferences);
        Application!.SetAccelsForAction("win.preferences", ["<Ctrl>period"]);
        // Keyboard shortcuts action
        var actKeyboardShortcuts = Gio.SimpleAction.New("keyboardShortcuts", null);
        actKeyboardShortcuts.OnActivate += KeyboardShortcuts;
        AddAction(actKeyboardShortcuts);
        Application!.SetAccelsForAction("win.keyboardShortcuts", ["<Ctrl>question"]);
        // About action
        var actAbout = Gio.SimpleAction.New("about", null);
        actAbout.OnActivate += About;
        AddAction(actAbout);
        Application!.SetAccelsForAction("win.about", ["F1"]);
        // Add download action
        var actAddDownload = Gio.SimpleAction.New("addDownload", null);
        actAddDownload.OnActivate += AddDownload;
        AddAction(actAddDownload);
        Application!.SetAccelsForAction("win.addDownload", ["<Ctrl>n"]);
        // Keyring action
        var actKeyring = Gio.SimpleAction.New("keyring", null);
        actKeyring.OnActivate += Keyring;
        AddAction(actKeyring);
        Application!.SetAccelsForAction("win.keyring", ["<Ctrl>k"]);
        // History action
        var actHistory = Gio.SimpleAction.New("history", null);
        actHistory.OnActivate += History;
        AddAction(actHistory);
        Application!.SetAccelsForAction("win.history", ["<Ctrl>h"]);
        // Stop all remaining action
        var actStopAllRemaining = Gio.SimpleAction.New("stopAllRemaining", null);
        actStopAllRemaining.OnActivate += StopAllRemaining;
        AddAction(actStopAllRemaining);
        Application!.SetAccelsForAction("win.stopAllRemaining", ["<Ctrl><Shift>s"]);
        // Retry all failed action
        var actRetryAllFailed = Gio.SimpleAction.New("retryAllFailed", null);
        actRetryAllFailed.OnActivate += RetryAllFailed;
        AddAction(actRetryAllFailed);
        Application!.SetAccelsForAction("win.retryAllFailed", ["<Ctrl><Shift>r"]);
        // Clear all queued action
        var actClearAllQueued = Gio.SimpleAction.New("clearAllQueued", null);
        actClearAllQueued.OnActivate += ClearAllQueued;
        AddAction(actClearAllQueued);
        // Clear all completed action
        var actClearAllCompleted = Gio.SimpleAction.New("clearAllCompleted", null);
        actClearAllCompleted.OnActivate += ClearAllCompleted;
        AddAction(actClearAllCompleted);
    }

    public new void Present()
    {
        base.Present();
        this.WindowGeometry = _controller.WindowGeometry;
    }

    private bool Window_OnCloseRequest(Gtk.Window sender, EventArgs e)
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

    private void Controller_AppNotificationSent(object? sender, AppNotificationSentEventArgs e)
    {
        var toast = Adw.Toast.New(e.Notification.Message);
        _toastOverlay!.AddToast(toast);
    }

    private void Controller_DownloadAdded(object? sender, DownloadAddedEventArgs e)
    {
        var row = new DownloadRow(_controller.Translator, this);
        row.PauseRequested += DownloadRow_PauseRequested;
        row.ResumeRequested += DownloadRow_ResumeRequested;
        row.StopRequested += DownloadRow_StopRequested;
        row.RetryRequested += DownloadRow_RetryRequested;
        row.TriggerAddedState(e);
        _downloadRows[e.Id] = row;
        UpdateDownloadsList();
        _viewStack!.VisibleChildName = "Downloads";
    }

    private void Controller_DownloadCompleted(object? sender, DownloadCompletedEventArgs e)
    {
        if (_downloadRows.TryGetValue(e.Id, out var row))
        {
            row.TriggerCompletedState(e);
            UpdateDownloadsList();
        }
    }

    private void Controller_DownloadProgressChanged(object? sender, DownloadProgressChangedEventArgs e)
    {
        if (_downloadRows.TryGetValue(e.Id, out var row))
        {
            row.TriggerProgressState(e);
        }
    }

    private void Controller_DownloadStartedFromQueue(object? sender, DownloadEventArgs e)
    {
        if (_downloadRows.TryGetValue(e.Id, out var row))
        {
            row.TriggerStartedFromQueueState();
            UpdateDownloadsList();
        }
    }

    private void Controller_DownloadStopped(object? sender, DownloadEventArgs e)
    {
        if (_downloadRows.TryGetValue(e.Id, out var row))
        {
            row.TriggerStoppedState();
            UpdateDownloadsList();
        }
    }

    private void Controller_DownloadRetired(object? sender, DownloadEventArgs e)
    {
        if (_downloadRows.TryGetValue(e.Id, out var row))
        {
            _downloadRows.Remove(e.Id);
            UpdateDownloadsList();
        }
    }

    private void DownloadRow_PauseRequested(object? sender, int id)
    {
        if (_controller.PauseDownload(id) && _downloadRows.TryGetValue(id, out var row))
        {
            row.TriggerPausedState();
        }
    }

    private void DownloadRow_ResumeRequested(object? sender, int id)
    {
        if (_controller.ResumeDownload(id) && _downloadRows.TryGetValue(id, out var row))
        {
            row.TriggerResumedState();
        }
    }

    private async void DownloadRow_RetryRequested(object? sender, int id) => await _controller.RetryDownloadAsync(id);

    private async void DownloadRow_StopRequested(object? sender, int id) => await _controller.StopDownloadAsync(id);

    private void Quit(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e)
    {
        if (!Window_OnCloseRequest(this, new EventArgs()))
        {
            Application!.Quit();
        }
    }

    private void Preferences(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e)
    {
        var preferencesDialog = new PreferencesDialog(_controller.PreferencesViewController, this);
        preferencesDialog.Present(this);
    }

    private void KeyboardShortcuts(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e)
    {
        var shortcutsDialog = new ShortcutsDialog(_controller.Translator);
        shortcutsDialog.Present(this);
    }

    private async void About(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e)
    {
        var loadingDialog = new LoadingDialog(_controller.Translator);
        loadingDialog.Present(this);
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
        dialog.Copyright = "© Nickvision 2021-2026";
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
        loadingDialog.ForceClose();
        dialog.Present(this);
    }

    private async void AddDownload(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e)
    {
        var addDownloadDialog = new AddDownloadDialog(_controller.AddDownloadDialogController, this);
        await addDownloadDialog.PresentWithClipboardAsync();
    }

    private void Keyring(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e)
    {
        var keyringDialog = new KeyringDialog(_controller.KeyringViewController);
        keyringDialog.Present(this);
    }

    private async void History(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e)
    {
        var historyDialog = new HistoryDialog(_controller.HistoryViewController, this);
        await historyDialog.PresentAndLoadAsync();
    }

    private async void StopAllRemaining(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e) => await _controller.StopAllDownloadsAsync();

    private async void RetryAllFailed(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e) => await _controller.RetryFailedDownloadsAsync();

    private void ClearAllQueued(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e) => _controller.ClearQueuedDownloads();

    private void ClearAllCompleted(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e) => _controller.ClearCompletedDownloads();

    private void UpdateDownloadsList()
    {
        var downloads = _downloadRows.Values.Where(row => _downloadsToggleGroup!.Active switch
        {
            1 => row.Status == DownloadStatus.Running || row.Status == DownloadStatus.Paused,
            2 => row.Status == DownloadStatus.Queued,
            3 => row.Status == DownloadStatus.Success || row.Status == DownloadStatus.Error || row.Status == DownloadStatus.Stopped,
            _ => true
        }).Reverse();
        _listDownloads!.RemoveAll();
        foreach(var row in downloads)
        {
            _listDownloads!.Append(row);
        }
        _viewStack!.VisibleChildName = downloads.Any() ? "Downloads" : "NoDownloads";
    }
}
