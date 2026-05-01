using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.GNOME.Controls;
using Nickvision.Desktop.GNOME.Helpers;
using Nickvision.Desktop.Network;
using Nickvision.Desktop.Notifications;
using Nickvision.Parabolic.GNOME.Controls;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Events;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.Linq;

namespace Nickvision.Parabolic.GNOME.Views;

public class MainWindow : Adw.ApplicationWindow
{
    private readonly IServiceProvider _serviceProvider;
    private readonly MainWindowController _controller;
    private readonly AppInfo _appInfo;
    private readonly ITranslationService _translationService;
    private readonly Gtk.Builder _builder;
    private bool _shown;
    private readonly Dictionary<int, DownloadRow> _downloadRows;

    [Gtk.Connect("windowTitle")]
    private Adw.WindowTitle? _windowTitle;
    [Gtk.Connect("updateButton")]
    private Gtk.MenuButton? _updateButton;
    [Gtk.Connect("updatePopover")]
    private Gtk.Popover? _updatePopover;
    [Gtk.Connect("updateProgressLabel")]
    private Gtk.Label? _updateProgressLabel;
    [Gtk.Connect("updateProgressBar")]
    private Gtk.ProgressBar? _updateProgressBar;
    [Gtk.Connect("toastOverlay")]
    private Adw.ToastOverlay? _toastOverlay;
    [Gtk.Connect("viewStack")]
    private Adw.ViewStack? _viewStack;
    [Gtk.Connect("downloadsToggleGroup")]
    private Adw.ToggleGroup? _downloadsToggleGroup;
    [Gtk.Connect("viewStackDownloads")]
    private Adw.ViewStack? _viewStackDownloads;
    [Gtk.Connect("listDownloads")]
    private Gtk.ListBox? _listDownloads;

    public MainWindow(IServiceProvider serviceProvider, Adw.Application application, MainWindowController controller, AppInfo appInfo, IEventsService eventsService, ITranslationService translationService, IGtkBuilderFactory builderFactory) : this(serviceProvider, application, controller, appInfo, eventsService, translationService, builderFactory.Create("MainWindow"))
    {

    }

    [DynamicDependency(DynamicallyAccessedMemberTypes.NonPublicFields, typeof(MainWindow))]
    private MainWindow(IServiceProvider serviceProvider, Adw.Application application, MainWindowController controller, AppInfo appInfo, IEventsService eventsService, ITranslationService translationService, Gtk.Builder builder) : base(new Adw.Internal.ApplicationWindowHandle(builder.GetPointer("root"), false))
    {
        _serviceProvider = serviceProvider;
        _controller = controller;
        _appInfo = appInfo;
        _translationService = translationService;
        _builder = builder;
        _downloadRows = new Dictionary<int, DownloadRow>();
        _shown = false;
        _builder.Connect(this);
        // Window
        Adw.StyleManager.GetDefault().ColorScheme = _controller.Theme switch
        {
            Theme.Light => Adw.ColorScheme.ForceLight,
            Theme.Dark => Adw.ColorScheme.ForceDark,
            _ => Adw.ColorScheme.Default
        };
        Title = _appInfo.ShortName;
        IconName = _appInfo.Id;
        if (_appInfo.Version!.IsPreview)
        {
            AddCssClass("devel");
        }
        _windowTitle!.Title = _appInfo.ShortName;
        // Events
        OnShow += Window_OnShow;
        OnCloseRequest += Window_OnCloseRequest;
        eventsService.AppNotificationSent += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            App_AppNotificationSent(sender, e);
            return false;
        });
        eventsService.DownloadAdded += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            Controller_DownloadAdded(sender, e);
            return false;
        });
        eventsService.DownloadProgressChanged += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            Controller_DownloadProgressChanged(sender, e);
            return false;
        });
        eventsService.DownloadCompleted += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            Controller_DownloadCompleted(sender, e);
            return false;
        });
        eventsService.DownloadStopped += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            Controller_DownloadStopped(sender, e);
            return false;
        });
        eventsService.DownloadStartedFromQueue += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            Controller_DownloadStartedFromQueue(sender, e);
            return false;
        });
        eventsService.DownloadRetired += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            Controller_DownloadRetired(sender, e);
            return false;
        });
        eventsService.DownloadRequested += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            _serviceProvider.GetRequiredService<AddDownloadDialog>().Present(e.Url, this).GetAwaiter().GetResult();
            return false;
        });
        _downloadsToggleGroup!.OnNotify += DownloadToggleGroup_OnNotify;
        // Quit action
        var actQuit = Gio.SimpleAction.New("quit", null);
        actQuit.OnActivate += Quit;
        AddAction(actQuit);
        application.SetAccelsForAction("win.quit", ["<Primary>q"]);
        // Preferences action
        var actPreferences = Gio.SimpleAction.New("preferences", null);
        actPreferences.OnActivate += Preferences;
        AddAction(actPreferences);
        application.SetAccelsForAction("win.preferences", ["<Primary>period"]);
        // Keyboard shortcuts action
        var actKeyboardShortcuts = Gio.SimpleAction.New("keyboardShortcuts", null);
        actKeyboardShortcuts.OnActivate += KeyboardShortcuts;
        AddAction(actKeyboardShortcuts);
        application.SetAccelsForAction("win.keyboardShortcuts", ["<Primary>question"]);
        // About action
        var actAbout = Gio.SimpleAction.New("about", null);
        actAbout.OnActivate += About;
        AddAction(actAbout);
        application.SetAccelsForAction("win.about", ["F1"]);
        // Add download action
        var actAddDownload = Gio.SimpleAction.New("addDownload", null);
        actAddDownload.OnActivate += AddDownload;
        AddAction(actAddDownload);
        application.SetAccelsForAction("win.addDownload", ["<Primary>n"]);
        // Keyring action
        var actKeyring = Gio.SimpleAction.New("keyring", null);
        actKeyring.OnActivate += Keyring;
        AddAction(actKeyring);
        application.SetAccelsForAction("win.keyring", ["<Primary>k"]);
        // History action
        var actHistory = Gio.SimpleAction.New("history", null);
        actHistory.OnActivate += History;
        AddAction(actHistory);
        application.SetAccelsForAction("win.history", ["<Primary>h"]);
        // Stop all remaining action
        var actStopAllRemaining = Gio.SimpleAction.New("stopAllRemaining", null);
        actStopAllRemaining.OnActivate += StopAllRemaining;
        AddAction(actStopAllRemaining);
        application.SetAccelsForAction("win.stopAllRemaining", ["<Primary><Shift>s"]);
        // Retry all failed action
        var actRetryAllFailed = Gio.SimpleAction.New("retryAllFailed", null);
        actRetryAllFailed.OnActivate += RetryAllFailed;
        AddAction(actRetryAllFailed);
        application.SetAccelsForAction("win.retryAllFailed", ["<Primary><Shift>r"]);
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

    public async void Window_OnShow(Gtk.Widget sender, EventArgs e)
    {
        if (_shown)
        {
            return;
        }
        var updatesTask = _controller.CheckForUpdatesAsync(false);
        if (_controller.ShowDisclaimerOnStartup)
        {
            var chkBox = Gtk.CheckButton.New();
            chkBox.Label = _translationService._("Don't show this message again");
            var disclaimerDialog = Adw.AlertDialog.New(_translationService._("Legal Copyright Disclaimer"), _translationService._("Videos on YouTube and other sites may be subject to DMCA protection. The authors of Parabolic do not endorse, and are not responsible for, the use of this application in means that will violate these laws."));
            disclaimerDialog.AddResponse("ok", _translationService._("I understand"));
            disclaimerDialog.SetDefaultResponse("ok");
            disclaimerDialog.SetCloseResponse("ok");
            disclaimerDialog.ExtraChild = chkBox;
            disclaimerDialog.OnResponse += async (_, _) =>
            {
                _controller.ShowDisclaimerOnStartup = !chkBox.Active;
            };
            disclaimerDialog.Present(this);
        }
        if (_controller.RecoverableDownloadsCount > 0)
        {
            var recoverDialog = Adw.AlertDialog.New(_translationService._("Recover Downloads?"), _translationService._("There are downloads available to recover from when Parabolic crashed. Would you like to download them again?"));
            recoverDialog.AddResponse("yes", _translationService._("Yes"));
            recoverDialog.AddResponse("no", _translationService._("No"));
            recoverDialog.SetResponseAppearance("yes", Adw.ResponseAppearance.Suggested);
            recoverDialog.SetDefaultResponse("no");
            recoverDialog.SetCloseResponse("no");
            recoverDialog.OnResponse += async (_, e) =>
            {
                if (e.Response == "yes")
                {
                    await _controller.RecoverAllDownloadsAsync();
                }
                else
                {
                    await _controller.ClearRecoverableDownloadsAsync();
                }
            };
            recoverDialog.Present(this);
        }
        if (_controller.UrlFromArgs is not null)
        {
            await _serviceProvider.GetRequiredService<AddDownloadDialog>().Present(_controller.UrlFromArgs, this);
        }
        await updatesTask;
        _shown = true;
    }

    private bool Window_OnCloseRequest(Gtk.Window sender, EventArgs e)
    {
        if (!_controller.CanShutdown)
        {
            var confirmDialog = Adw.AlertDialog.New(_appInfo.ShortName, _translationService._("There are downloads still in progress. Would you like to stop them and exit?"));
            confirmDialog.AddResponse("yes", _translationService._("Yes"));
            confirmDialog.AddResponse("no", _translationService._("No"));
            confirmDialog.SetResponseAppearance("yes", Adw.ResponseAppearance.Destructive);
            confirmDialog.SetDefaultResponse("no");
            confirmDialog.SetCloseResponse("no");
            confirmDialog.OnResponse += async (_, e) =>
            {
                if (e.Response == "yes")
                {
                    await _controller.StopAllDownloadsAsync();
                    Close();
                }
            };
            confirmDialog.Present(this);
            return true;
        }
        GetDefaultSize(out int width, out int height);
        _controller.WindowGeometry = this.WindowGeometry;
        Destroy();
        _serviceProvider.GetRequiredService<IHostApplicationLifetime>().StopApplication();
        return false;
    }

    private void App_AppNotificationSent(object? sender, AppNotificationSentEventArgs e)
    {
        var toast = Adw.Toast.New(e.Notification.Message);
        if (e.Notification.Action == "update")
        {
            toast.Timeout = 0;
            toast.ButtonLabel = _translationService._("View");
            toast.OnButtonClicked += async (_, _) =>
            {
                var launcher = Gtk.UriLauncher.New($"{_appInfo.SourceRepository}/releases/latest");
                await launcher.LaunchAsync(this);
            };
        }
        else if (e.Notification.Action == "update-ytdlp")
        {
            toast.Timeout = 0;
            toast.ButtonLabel = _translationService._("Update");
            toast.OnButtonClicked += async (_, _) =>
            {
                _updateButton!.Visible = true;
                _updatePopover!.Popup();
                _updateProgressLabel!.Label_ = _translationService._("Downloading update: {0}%", 0);
                var progress = new Progress<DownloadProgress>();
                progress.ProgressChanged += UpdateProgress_Changed;
                await _controller.YtdlpUpdateAsync(progress);
                progress.ProgressChanged -= UpdateProgress_Changed;
            };
        }
        else if (e.Notification.Action == "update-deno")
        {
            toast.Timeout = 0;
            toast.ButtonLabel = _translationService._("Update");
            toast.OnButtonClicked += async (_, _) =>
            {
                _updateButton!.Visible = true;
                _updatePopover!.Popup();
                _updateProgressLabel!.Label_ = _translationService._("Downloading update: {0}%", 0);
                var progress = new Progress<DownloadProgress>();
                progress.ProgressChanged += UpdateProgress_Changed;
                await _controller.DenoUpdateAsync(progress);
                progress.ProgressChanged -= UpdateProgress_Changed;
            };
        }
        else if (e.Notification.Action == "error" && !string.IsNullOrEmpty(e.Notification.ActionParam))
        {
            toast.Timeout = 0;
            toast.ButtonLabel = _translationService._("Details");
            toast.OnButtonClicked += (_, _) =>
            {
                var dialog = Adw.AlertDialog.New(_translationService._("Error"), e.Notification.ActionParam);
                dialog.BodyUseMarkup = false;
                dialog.AddResponse("close", _translationService._("Close"));
                dialog.SetDefaultResponse("close");
                dialog.SetCloseResponse("close");
                dialog.Present(this);
            };
        }
        _toastOverlay!.AddToast(toast);
    }

    private async void Controller_DownloadAdded(object? sender, DownloadAddedEventArgs e)
    {
        var row = _serviceProvider.GetRequiredService<DownloadRow>();
        row.PauseRequested += DownloadRow_PauseRequested;
        row.ResumeRequested += DownloadRow_ResumeRequested;
        row.StopRequested += DownloadRow_StopRequested;
        row.RetryRequested += DownloadRow_RetryRequested;
        await row.TriggerAddedStateAsync(e);
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

    private void DownloadToggleGroup_OnNotify(GObject.Object sender, NotifySignalArgs e)
    {
        if (e.Pspec.GetName() == "active")
        {
            UpdateDownloadsList();
        }
    }

    private void UpdateProgress_Changed(object? sender, DownloadProgress e)
    {
        GLib.Functions.IdleAdd(0, () =>
        {
            if (e.Completed)
            {
                _updatePopover!.Popdown();
                _updateButton!.Visible = false;
                return false;
            }
            var message = _translationService._("Downloading update: {0}%", Math.Round(e.Percentage * 100));
            _updateProgressLabel!.Label_ = message;
            _updateProgressBar!.Fraction = e.Percentage;
            return false;
        });
    }
    private void Quit(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e) => Window_OnCloseRequest(this, new EventArgs());

    private void Preferences(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e) => _serviceProvider.GetRequiredService<PreferencesDialog>().Present(this);

    private void KeyboardShortcuts(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e) => _serviceProvider.GetRequiredService<ShortcutsDialog>().Present(this);

    private async void About(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e)
    {
        var loadingDialog = _serviceProvider.GetRequiredService<LoadingDialog>();
        loadingDialog.Present(this);
        var extraInfo = string.Empty;
        extraInfo += $"GTK {Gtk.Functions.GetMajorVersion()}.{Gtk.Functions.GetMinorVersion()}.{Gtk.Functions.GetMicroVersion()}\n";
        extraInfo += $"libadwaita {Adw.Functions.GetMajorVersion()}.{Adw.Functions.GetMinorVersion()}.{Adw.Functions.GetMicroVersion()}";
        var dialog = Adw.AboutDialog.New();
        dialog.ApplicationName = _appInfo.ShortName;
        dialog.ApplicationIcon = _appInfo.Version!.IsPreview ? $"{_appInfo.Id}-devel" : _appInfo.Id;
        dialog.DeveloperName = "Nickvision";
        dialog.Version = _appInfo.Version.ToString();
        dialog.ReleaseNotes = _appInfo.HtmlChangelog;
        dialog.DebugInfo = await _controller.GetDebugInformationAsync(extraInfo);
        dialog.Comments = _appInfo.Description;
        dialog.LicenseType = Gtk.License.MitX11;
        dialog.Copyright = "© Nickvision 2021-2026";
        dialog.Website = "https://nickvision.org";
        dialog.IssueUrl = _appInfo.IssueTracker!.ToString();
        dialog.SupportUrl = _appInfo.DiscussionsForum!.ToString();
        dialog.AddLink(_translationService._("GitHub Repo"), _appInfo.SourceRepository!.ToString());
        foreach (var pair in _appInfo.ExtraLinks)
        {
            dialog.AddLink(pair.Key, pair.Value.ToString());
        }
        dialog.Developers = _appInfo.Developers.Select(x => $"{x.Key} {x.Value}").ToArray();
        dialog.Designers = _appInfo.Designers.Select(x => $"{x.Key} {x.Value}").ToArray();
        dialog.Artists = _appInfo.Artists.Select(x => $"{x.Key} {x.Value}").ToArray();
        if (!string.IsNullOrEmpty(_appInfo.TranslationCredits) && _appInfo.TranslationCredits != "translation-credits")
        {
            dialog.TranslatorCredits = _appInfo.TranslationCredits;
        }
        loadingDialog.ForceClose();
        dialog.Present(this);
    }

    private async void AddDownload(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e) => await _serviceProvider.GetRequiredService<AddDownloadDialog>().Present(this);

    private async void Keyring(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e) => await _serviceProvider.GetRequiredService<KeyringDialog>().Present(this);

    private async void History(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e) => await _serviceProvider.GetRequiredService<HistoryDialog>().Present(this);

    private async void StopAllRemaining(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e) => await _controller.StopAllDownloadsAsync();

    private async void RetryAllFailed(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e) => await _controller.RetryFailedDownloadsAsync();

    private void ClearAllQueued(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e)
    {
        foreach (var id in _controller.ClearQueuedDownloads())
        {
            _downloadRows.Remove(id);
        }
        UpdateDownloadsList();
    }

    private void ClearAllCompleted(Gio.SimpleAction sender, Gio.SimpleAction.ActivateSignalArgs e)
    {
        foreach (var id in _controller.ClearCompletedDownloads())
        {
            _downloadRows.Remove(id);
        }
        UpdateDownloadsList();
    }

    private void UpdateDownloadsList()
    {
        var downloads = new List<DownloadRow>(_downloadRows.Count);
        foreach (var row in _downloadRows.Values)
        {
            if (_downloadsToggleGroup!.Active switch
            {
                1 => row.Status == DownloadStatus.Running || row.Status == DownloadStatus.Paused,
                2 => row.Status == DownloadStatus.Queued,
                3 => row.Status == DownloadStatus.Success || row.Status == DownloadStatus.Error || row.Status == DownloadStatus.Stopped,
                4 => row.Status == DownloadStatus.Error,
                _ => true
            })
            {
                downloads.Add(row);
            }
        }
        downloads.Reverse();
        _listDownloads!.RemoveAll();
        foreach (var row in downloads)
        {
            _listDownloads!.Append(row);
        }
        _viewStackDownloads!.VisibleChildName = downloads.Count > 0 ? "Has" : "None";
    }
}
