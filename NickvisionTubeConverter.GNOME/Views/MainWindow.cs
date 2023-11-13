using Nickvision.Aura;
using Nickvision.Aura.Taskbar;
using NickvisionTubeConverter.GNOME.Controls;
using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Events;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using Python.Runtime;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Text;
using System.Threading.Tasks;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// The MainWindow for the application
/// </summary>
public partial class MainWindow : Adw.ApplicationWindow
{
    private readonly MainWindowController _controller;
    private readonly Adw.Application _application;
    private readonly Gio.DBusConnection _bus;
    private bool _isBackgroundStatusReported;
    private readonly Gio.SimpleAction _actDownload;
    private Dictionary<Guid, DownloadRow> _downloadRows;
    private uint? _inhibitCookie;

    [Gtk.Connect] private readonly Adw.Bin _spinnerContainer;
    [Gtk.Connect] private readonly Gtk.Spinner _spinner;
    [Gtk.Connect] private readonly Gtk.Box _mainBox;
    [Gtk.Connect] private readonly Adw.HeaderBar _headerBar;
    [Gtk.Connect] private readonly Adw.WindowTitle _title;
    [Gtk.Connect] private readonly Adw.ToastOverlay _toastOverlay;
    [Gtk.Connect] private readonly Adw.ViewStack _viewStack;
    [Gtk.Connect] private readonly Adw.Banner _banner;
    [Gtk.Connect] private readonly Gtk.Button _addDownloadButton;
    [Gtk.Connect] private readonly Gtk.Box _downloadingBox;
    [Gtk.Connect] private readonly Gtk.Button _stopAllDownloadsButton;
    [Gtk.Connect] private readonly Gtk.Box _completedBox;
    [Gtk.Connect] private readonly Gtk.Box _queuedBox;

    private MainWindow(Gtk.Builder builder, MainWindowController controller, Adw.Application application) : base(builder.GetPointer("_root"), false)
    {
        //Window Settings
        _controller = controller;
        _application = application;
        _bus = Gio.DBusConnection.Get(Gio.BusType.Session);
        _isBackgroundStatusReported = false;
        _downloadRows = new Dictionary<Guid, DownloadRow>();
        _inhibitCookie = null;
        //Build UI
        builder.Connect(this);
        SetTitle(_controller.AppInfo.ShortName);
        SetIconName(_controller.AppInfo.ID);
        if (_controller.AppInfo.IsDevVersion)
        {
            AddCssClass("devel");
        }
        _title.SetTitle(_controller.AppInfo.ShortName);
        //Register Events
        OnCloseRequest += OnCloseRequested;
        _controller.NotificationSent += (sender, e) => GLib.Functions.IdleAdd(0, () => NotificationSent(sender, e));
        _controller.PreventSuspendWhenDownloadingChanged += (sender, e) => GLib.Functions.IdleAdd(0, PreventSuspendWhenDownloadingChanged);
        _controller.RunInBackgroundChanged += (sender, e) => GLib.Functions.IdleAdd(0, RunInBackgroundChanged);
        _controller.KeyringLoginAsync = KeyringLoginAsync;
        _controller.DownloadManager.DownloadAdded += (sender, e) => GLib.Functions.IdleAdd(0, () => DownloadAdded(e));
        _controller.DownloadManager.DownloadProgressUpdated += (sender, e) => GLib.Functions.IdleAdd(0, () => DownloadProgressUpdated(e));
        _controller.DownloadManager.DownloadCompleted += (sender, e) => GLib.Functions.IdleAdd(0, () => DownloadCompleted(e));
        _controller.DownloadManager.DownloadStopped += (sender, e) => GLib.Functions.IdleAdd(0, () => DownloadStopped(e));
        _controller.DownloadManager.DownloadRetried += (sender, e) => GLib.Functions.IdleAdd(0, () => DownloadRetried(e));
        _controller.DownloadManager.DownloadStartedFromQueue += (sender, e) => GLib.Functions.IdleAdd(0, () => DownloadStartedFromQueue(e));
        //Add Download Action
        _actDownload = Gio.SimpleAction.New("addDownload", null);
        _actDownload.OnActivate += async (sender, e) => await AddDownloadAsync(null);
        AddAction(_actDownload);
        application.SetAccelsForAction("win.addDownload", new string[] { "<Ctrl>n" });
        //Stop All Downloads Action
        var actStopAllDownloads = Gio.SimpleAction.New("stopAllDownloads", null);
        actStopAllDownloads.OnActivate += (sender, e) => _controller.DownloadManager.StopAllDownloads(true);
        AddAction(actStopAllDownloads);
        application.SetAccelsForAction("win.stopAllDownloads", new string[] { "<Ctrl><Shift>c" });
        //Retry Failed Downloads Action
        var actRetryFailedDownloads = Gio.SimpleAction.New("retryFailedDownloads", null);
        actRetryFailedDownloads.OnActivate += (sender, e) => _controller.DownloadManager.RetryFailedDownloads(DownloadOptions.Current);
        AddAction(actRetryFailedDownloads);
        application.SetAccelsForAction("win.retryFailedDownloads", new string[] { "<Ctrl><Shift>r" });
        //Clear Queued Downloads Action
        var actClearQueuedDownloads = Gio.SimpleAction.New("clearQueuedDownloads", null);
        actClearQueuedDownloads.OnActivate += (sender, e) =>
        {
            _controller.DownloadManager.ClearQueuedDownloads();
            while (_queuedBox.GetFirstChild() != null)
            {
                _queuedBox.Remove(_queuedBox.GetFirstChild());
            }
            _queuedBox.GetParent().SetVisible(false);
            if (!_controller.DownloadManager.AreDownloadsQueued && !_controller.DownloadManager.AreDownloadsRunning && !_controller.DownloadManager.AreDownloadsCompleted)
            {
                _headerBar.AddCssClass("flat");
                _addDownloadButton.SetVisible(false);
                _viewStack.SetVisibleChildName("pageNoDownloads");
            }
        };
        AddAction(actClearQueuedDownloads);
        application.SetAccelsForAction("win.clearQueuedDownloads", new string[] { "<Ctrl>Delete" });
        //Clear Completed Downloads Action
        var actClearCompletedDownloads = Gio.SimpleAction.New("clearCompletedDownloads", null);
        actClearCompletedDownloads.OnActivate += (sender, e) =>
        {
            _controller.DownloadManager.ClearCompletedDownloads();
            while (_completedBox.GetFirstChild() != null)
            {
                _completedBox.Remove(_completedBox.GetFirstChild());
            }
            _completedBox.GetParent().SetVisible(false);
            if (!_controller.DownloadManager.AreDownloadsQueued && !_controller.DownloadManager.AreDownloadsRunning && !_controller.DownloadManager.AreDownloadsCompleted)
            {
                _headerBar.AddCssClass("flat");
                _addDownloadButton.SetVisible(false);
                _viewStack.SetVisibleChildName("pageNoDownloads");
            }
        };
        AddAction(actClearCompletedDownloads);
        //Keyring Action
        var actKeyring = Gio.SimpleAction.New("keyring", null);
        actKeyring.OnActivate += Keyring;
        AddAction(actKeyring);
        application.SetAccelsForAction("win.keyring", new string[] { "<Ctrl>k" });
        //History Action
        var actHistory = Gio.SimpleAction.New("history", null);
        actHistory.OnActivate += History;
        AddAction(actHistory);
        application.SetAccelsForAction("win.history", new string[] { "<Ctrl>h" });
        //Preferences Action
        var actPreferences = Gio.SimpleAction.New("preferences", null);
        actPreferences.OnActivate += Preferences;
        AddAction(actPreferences);
        application.SetAccelsForAction("win.preferences", new string[] { "<Ctrl>comma" });
        //Keyboard Shortcuts Action
        var actKeyboardShortcuts = Gio.SimpleAction.New("keyboardShortcuts", null);
        actKeyboardShortcuts.OnActivate += KeyboardShortcuts;
        AddAction(actKeyboardShortcuts);
        application.SetAccelsForAction("win.keyboardShortcuts", new string[] { "<Ctrl>question" });
        //Quit Action
        var actQuit = Gio.SimpleAction.New("quit", null);
        actQuit.OnActivate += Quit;
        AddAction(actQuit);
        application.SetAccelsForAction("win.quit", new string[] { "<Ctrl>q", "<Ctrl>w" });
        //Help Action
        var actHelp = Gio.SimpleAction.New("help", null);
        actHelp.OnActivate += (sender, e) => Gtk.Functions.ShowUri(this, DocumentationHelpers.GetHelpURL("index"), 0);
        AddAction(actHelp);
        application.SetAccelsForAction("win.help", new string[] { "F1" });
        //About Action
        var actAbout = Gio.SimpleAction.New("about", null);
        actAbout.OnActivate += About;
        AddAction(actAbout);
    }

    /// <summary>
    /// Constructs a MainWindow
    /// </summary>
    /// <param name="controller">The MainWindowController</param>
    /// <param name="application">The Adw.Application</param>
    public MainWindow(MainWindowController controller, Adw.Application application) : this(Builder.FromFile("window.ui"), controller, application)
    {
    }

    /// <summary>
    /// Starts the MainWindow
    /// </summary>
    public async Task StartAsync()
    {
        _application.AddWindow(this);
        Present();
        _spinnerContainer.SetVisible(true);
        _mainBox.SetVisible(false);
        _spinner.Start();
        var urlToLaunch = await _controller.StartupAsync();
        _controller.TaskbarItem = await TaskbarItem.ConnectLinuxAsync(string.IsNullOrEmpty(Environment.GetEnvironmentVariable("SNAP")) ? $"{_controller.AppInfo.ID}.desktop" : "tube-converter_tube-converter.desktop");
        _spinner.Stop();
        _spinnerContainer.SetVisible(false);
        _mainBox.SetVisible(true);
        PreventSuspendWhenDownloadingChanged();
        RunInBackgroundChanged();
        if (!string.IsNullOrEmpty(urlToLaunch))
        {
            await AddDownloadAsync(urlToLaunch);
            _controller.UrlToLaunch = null;
        }
    }

    /// <summary>
    /// Occurs when a notification is sent from the controller
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">NotificationSentEventArgs</param>
    private bool NotificationSent(object? sender, NotificationSentEventArgs e)
    {
        if (e.Action == "no-network")
        {
            _banner.SetTitle(e.Message);
            _headerBar.RemoveCssClass("flat");
            _actDownload.SetEnabled(false);
            _banner.SetRevealed(true);
            return false;
        }
        if (e.Action == "network-restored")
        {
            if (_downloadRows.Count == 0)
            {
                _headerBar.AddCssClass("flat");
            }
            _actDownload.SetEnabled(true);
            _banner.SetRevealed(false);
            return false;
        }
        var toast = Adw.Toast.New(e.Message);
        _toastOverlay.AddToast(toast);
        return false;
    }

    /// <summary>
    /// Sends a shell notification
    /// </summary>
    /// <param name="e">ShellNotificationSentEventArgs</param>
    private void SendShellNotification(ShellNotificationSentEventArgs e)
    {
        var notification = Gio.Notification.New(e.Title);
        notification.SetBody(e.Message);
        notification.SetPriority(e.Severity switch
        {
            NotificationSeverity.Success => Gio.NotificationPriority.High,
            NotificationSeverity.Warning => Gio.NotificationPriority.Urgent,
            NotificationSeverity.Error => Gio.NotificationPriority.Urgent,
            _ => Gio.NotificationPriority.Normal
        });
        if (string.IsNullOrEmpty(Environment.GetEnvironmentVariable("SNAP")))
        {
            notification.SetIcon(Gio.ThemedIcon.New($"{_controller.AppInfo.ID}-symbolic"));
        }
        else
        {
            var fileIcon = Gio.FileIcon.New(Gio.FileHelper.NewForPath($"{Environment.GetEnvironmentVariable("SNAP")}/usr/share/icons/hicolor/symbolic/apps/{_controller.AppInfo.ID}-symbolic.svg"));
            notification.SetIcon(fileIcon);
        }
        _application.SendNotification(_controller.AppInfo.ID, notification);
    }

    /// <summary>
    /// Occurs when the window tries to close
    /// </summary>
    /// <param name="sender">Gtk.Window</param>
    /// <param name="e">EventArgs</param>
    /// <returns>True to stop close, else false</returns>
    private bool OnCloseRequested(Gtk.Window sender, EventArgs e)
    {
        if (_controller.DownloadManager.AreDownloadsRunning)
        {
            if (_controller.RunInBackground && (File.Exists("/.flatpak-info") || !string.IsNullOrEmpty(Environment.GetEnvironmentVariable("SNAP"))))
            {
                SetVisible(false);
                return true;
            }
            var response = "";
            var closeDialog = Adw.MessageDialog.New(this, _("Close and Stop Downloads?"), _("Some downloads are still in progress.\nAre you sure you want to close Parabolic and stop the running downloads?"));
            closeDialog.SetIconName(_controller.AppInfo.ID);
            closeDialog.AddResponse("no", _("No"));
            closeDialog.SetDefaultResponse("no");
            closeDialog.SetCloseResponse("no");
            closeDialog.AddResponse("yes", _("Yes"));
            closeDialog.SetResponseAppearance("yes", Adw.ResponseAppearance.Destructive);
            closeDialog.OnResponse += (s, ex) => response = ex.Response;
            closeDialog.Present();
            while (closeDialog.GetVisible())
            {
                GLib.Internal.MainContext.Iteration(GLib.Internal.MainContext.Default(), false);
            }
            closeDialog.Destroy();
            if (response == "no")
            {
                return true;
            }
        }
        _controller.Dispose();
        return false;
    }

    /// <summary>
    /// Occurs when Keyring needs a login
    /// </summary>
    /// <param name="title">The title of the account</param>
    private async Task<(bool WasSkipped, string Password)> KeyringLoginAsync(string title)
    {
        var tcs = new TaskCompletionSource<(bool WasSkipped, string Password)>();
        var passwordDialog = new PasswordDialog(this, title, tcs);
        passwordDialog.SetIconName(_controller.AppInfo.ID);
        passwordDialog.Present();
        return await tcs.Task;
    }

    /// <summary>
    /// Prompts the AddDownloadDialog
    /// </summary>
    /// <param name="url">A url to pass to the dialog</param>
    public async Task AddDownloadAsync(string? url)
    {
        var addController = _controller.CreateAddDownloadDialogController();
        var addDialog = new AddDownloadDialog(addController, this);
        addDialog.OnDownload += (s, ex) =>
        {
            _headerBar.RemoveCssClass("flat");
            _addDownloadButton.SetVisible(true);
            _viewStack.SetVisibleChildName("pageDownloads");
            _controller.AddDownloads(addController);
            addDialog.Close();
        };
        await addDialog.PresentAsync(url);
    }

    /// <summary>
    /// Occurs when the keyring action is triggered
    /// </summary>
    /// <param name="sender">Gio.SimpleAction</param>
    /// <param name="e">EventArgs</param>
    private async void Keyring(Gio.SimpleAction sender, EventArgs e)
    {
        var keyringDialogController = _controller.CreateKeyringDialogController();
        var keyringDialog = new KeyringDialog(keyringDialogController, _controller.AppInfo.ID, this);
        keyringDialog.OnCloseRequest += (sender, e) =>
        {
            _controller.UpdateKeyring(keyringDialogController);
            return false;
        };
        await keyringDialog.PresentAsync();
    }

    /// <summary>
    /// Occurs when the history action is triggered
    /// </summary>
    /// <param name="sender">Gio.SimpleAction</param>
    /// <param name="e">EventArgs</param>
    private void History(Gio.SimpleAction sender, EventArgs e)
    {
        var historyDialog = new HistoryDialog(this, _controller.AppInfo.ID, DownloadHistory.Current);
        historyDialog.DownloadAgainRequested += async (s, ea) =>
        {
            await AddDownloadAsync(ea);
            historyDialog.Destroy();
        };
        historyDialog.Present();
    }

    /// <summary>
    /// Occurs when the preferences action is triggered
    /// </summary>
    /// <param name="sender">Gio.SimpleAction</param>
    /// <param name="e">EventArgs</param>
    private void Preferences(Gio.SimpleAction sender, EventArgs e)
    {
        var preferencesDialog = new PreferencesDialog(_controller.CreatePreferencesViewController(), _application, this);
        preferencesDialog.Present();
    }

    /// <summary>
    /// Occurs when the keyboard shortcuts action is triggered
    /// </summary>
    /// <param name="sender">Gio.SimpleAction</param>
    /// <param name="e">EventArgs</param>
    private void KeyboardShortcuts(Gio.SimpleAction sender, EventArgs e)
    {
        var builder = Builder.FromFile("shortcuts_dialog.ui");
        var shortcutsWindow = (Gtk.ShortcutsWindow)builder.GetObject("_shortcuts")!;
        shortcutsWindow.SetTransientFor(this);
        shortcutsWindow.SetIconName(_controller.AppInfo.ID);
        shortcutsWindow.Present();
    }

    /// <summary>
    /// Occurs when quit action is triggered
    /// </summary>
    /// <param name="sender">Gio.SimpleAction</param>
    /// <param name="e">EventArgs</param>
    private void Quit(Gio.SimpleAction sender, EventArgs e)
    {
        if (!OnCloseRequested(this, EventArgs.Empty))
        {
            _application.Quit();
        }
    }

    /// <summary>
    /// Occurs when the about action is triggered
    /// </summary>
    /// <param name="sender">Gio.SimpleAction</param>
    /// <param name="e">EventArgs</param>
    private async void About(Gio.SimpleAction sender, EventArgs e)
    {
        var debugInfo = new StringBuilder();
        debugInfo.AppendLine(_controller.AppInfo.ID);
        debugInfo.AppendLine(_controller.AppInfo.Version);
        debugInfo.AppendLine($"GTK {Gtk.Functions.GetMajorVersion()}.{Gtk.Functions.GetMinorVersion()}.{Gtk.Functions.GetMicroVersion()}");
        debugInfo.AppendLine($"libadwaita {Adw.Functions.GetMajorVersion()}.{Adw.Functions.GetMinorVersion()}.{Adw.Functions.GetMicroVersion()}");
        if (File.Exists("/.flatpak-info"))
        {
            debugInfo.AppendLine("Flatpak");
        }
        else if (!string.IsNullOrEmpty(Environment.GetEnvironmentVariable("SNAP")))
        {
            debugInfo.AppendLine("Snap");
        }
        var py = Task.Run(() =>
        {
            using (Py.GIL())
            {
                try
                {
                    dynamic yt_dlp = Py.Import("yt_dlp");
                    debugInfo.AppendLine($"yt-dlp {yt_dlp.version.__version__.As<string>()}");
                }
                catch
                {
                    debugInfo.AppendLine("yt-dlp not found");
                }
                try
                {
                    dynamic psutil = Py.Import("psutil");
                    debugInfo.AppendLine($"psutil {psutil.__version__.As<string>()}");
                }
                catch
                {
                    debugInfo.AppendLine("psutil not found");
                }
            }
        });
        var ffmpeg = Task.Run(() =>
        {
            using var ffmpegProcess = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = DependencyLocator.Find("ffmpeg"),
                    Arguments = "-version",
                    UseShellExecute = false,
                    RedirectStandardOutput = true
                }
            };
            try
            {
                ffmpegProcess.Start();
                var ffmpegVersion = ffmpegProcess.StandardOutput.ReadToEnd();
                ffmpegProcess.WaitForExit();
                ffmpegVersion = ffmpegVersion.Remove(ffmpegVersion.IndexOf(Environment.NewLine))
                                             .Remove(ffmpegVersion.IndexOf("Copyright"))
                                             .Trim();
                debugInfo.AppendLine(ffmpegVersion);
            }
            catch
            {
                debugInfo.AppendLine("ffmpeg not found");
            }
        });
        var aria = Task.Run(() =>
        {
            using var ariaProcess = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = DependencyLocator.Find("aria2c"),
                    Arguments = "--version",
                    UseShellExecute = false,
                    RedirectStandardOutput = true
                }
            };
            try
            {
                ariaProcess.Start();
                var ariaVersion = ariaProcess.StandardOutput.ReadToEnd();
                ariaProcess.WaitForExit();
                ariaVersion = ariaVersion.Remove(ariaVersion.IndexOf(Environment.NewLine)).Trim();
                debugInfo.AppendLine(ariaVersion);
            }
            catch
            {
                debugInfo.AppendLine("aria2c not found");
            }
        });
        await py;
        await ffmpeg;
        await aria;
        debugInfo.AppendLine(CultureInfo.CurrentCulture.ToString());
        using var localeProcess = new Process
        {
            StartInfo = new ProcessStartInfo
            {
                FileName = "locale",
                UseShellExecute = false,
                RedirectStandardOutput = true
            }
        };
        try
        {
            localeProcess.Start();
            var localeString = localeProcess.StandardOutput.ReadToEnd().Trim();
            localeProcess.WaitForExit();
            debugInfo.AppendLine(localeString);
        }
        catch
        {
            debugInfo.AppendLine("Unknown locale");
        }
        var dialog = Adw.AboutWindow.New();
        dialog.SetTransientFor(this);
        dialog.SetIconName(_controller.AppInfo.ID);
        dialog.SetApplicationName(_controller.AppInfo.ShortName);
        dialog.SetApplicationIcon(_controller.AppInfo.ID + (_controller.AppInfo.IsDevVersion ? "-devel" : ""));
        dialog.SetVersion(_controller.AppInfo.Version);
        dialog.SetDebugInfo(debugInfo.ToString());
        dialog.SetComments(_controller.AppInfo.Description);
        dialog.SetDeveloperName("Nickvision");
        dialog.SetLicenseType(Gtk.License.MitX11);
        dialog.SetCopyright($"© Nickvision 2021-2023\n\n{_("The authors of Nickvision Parabolic are not responsible/liable for any misuse of this program that may violate local copyright/DMCA laws. Users use this application at their own risk.")}");
        dialog.SetWebsite("https://nickvision.org/");
        dialog.SetIssueUrl(_controller.AppInfo.IssueTracker.ToString());
        dialog.SetSupportUrl(_controller.AppInfo.SupportUrl.ToString());
        dialog.AddLink(_("GitHub Repo"), _controller.AppInfo.SourceRepo.ToString());
        foreach (var pair in _controller.AppInfo.ExtraLinks)
        {
            dialog.AddLink(pair.Key, pair.Value.ToString());
        }
        dialog.SetDevelopers(_controller.AppInfo.ConvertURLDictToArray(_controller.AppInfo.Developers));
        dialog.SetDesigners(_controller.AppInfo.ConvertURLDictToArray(_controller.AppInfo.Designers));
        dialog.SetArtists(_controller.AppInfo.ConvertURLDictToArray(_controller.AppInfo.Artists));
        dialog.SetTranslatorCredits(_controller.AppInfo.TranslatorCredits);
        dialog.SetReleaseNotes(_controller.AppInfo.HTMLChangelog);
        dialog.Present();
    }

    /// <summary>
    /// Occurs when the prevent suspend option is changed
    /// </summary>
    private bool PreventSuspendWhenDownloadingChanged()
    {
        if (_inhibitCookie == null && _controller.PreventSuspendWhenDownloading)
        {
            _inhibitCookie = _application.Inhibit(this, Gtk.ApplicationInhibitFlags.Suspend, "user request");
        }
        else if (_inhibitCookie != null && !_controller.PreventSuspendWhenDownloading)
        {
            _application.Uninhibit(_inhibitCookie.Value);
            _inhibitCookie = null;
        }
        return false;
    }

    /// <summary>
    /// Occurs when the run in background option is changed
    /// </summary>
    private bool RunInBackgroundChanged()
    {
        if (_controller.RunInBackground)
        {
            if (!_isBackgroundStatusReported)
            {
                _isBackgroundStatusReported = true;
                GLib.Functions.TimeoutAdd(0, 1000, UpdateBackgroundSource);
            }
        }
        else
        {
            _isBackgroundStatusReported = false;
        }
        return false;
    }

    /// <summary>
    /// Updates the background source
    /// </summary>
    private bool UpdateBackgroundSource()
    {
        try
        {
            if (_isBackgroundStatusReported)
            {
                using var typeDictEntry = GLib.VariantType.NewDictEntry(GLib.VariantType.String, GLib.VariantType.Variant);
                using var msg = GLib.Variant.NewString("message");
                using var dataString = GLib.Variant.NewString(_controller.DownloadManager.BackgroundActivityReport);
                using var data = GLib.Variant.NewVariant(dataString);
                using var dictEntry = GLib.Variant.NewDictEntry(msg, data);
                using var array = GLib.Variant.NewArray(typeDictEntry, new[] { dictEntry });
                using var tuple = GLib.Variant.NewTuple(new[] { array });
                Task.Run(async () => await _bus.CallAsync(
                    "org.freedesktop.portal.Desktop", // Bus name
                    "/org/freedesktop/portal/desktop", // Object path
                    "org.freedesktop.portal.Background", // Interface name
                    "SetStatus", // Method name
                    tuple)); // Parameters 
            }
            return _isBackgroundStatusReported;
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
            return false;
        }
    }

    /// <summary>
    /// Occurs when a download is added
    /// </summary>
    /// <param name="e">(Guid Id, string Filename, string SaveFolder, bool IsDownloading)</param>
    private bool DownloadAdded((Guid Id, string Filename, string SaveFolder, bool IsDownloading) e)
    {
        var downloadRow = new DownloadRow(this, e.Id, e.Filename, e.SaveFolder, (ex) => NotificationSent(null, ex));
        downloadRow.StopRequested += (s, ex) => _controller.DownloadManager.RequestStop(ex);
        downloadRow.RetryRequested += (s, ex) => _controller.DownloadManager.RequestRetry(ex, DownloadOptions.Current);
        var box = e.IsDownloading ? _downloadingBox : _queuedBox;
        if (e.IsDownloading)
        {
            downloadRow.SetPreparingState();
        }
        else
        {
            downloadRow.SetWaitingState();
        }
        if (box.GetFirstChild() != null)
        {
            var separator = Gtk.Separator.New(Gtk.Orientation.Horizontal);
            box.Append(separator);
        }
        box.Append(downloadRow);
        _downloadRows[e.Id] = downloadRow;
        _stopAllDownloadsButton.SetVisible(_controller.DownloadManager.RemainingDownloadsCount > 1);
        box.GetParent().SetVisible(true);
        return false;
    }

    /// <summary>
    /// Occurs when a download's progress is updated
    /// </summary>
    /// <param name="e">(Guid Id, DownloadProgressState State)</param>
    private bool DownloadProgressUpdated((Guid Id, DownloadProgressState State) e)
    {
        var row = _downloadRows[e.Id];
        if (row.GetParent() == _downloadingBox)
        {
            row.SetProgressState(e.State);
        }
        return false;
    }

    /// <summary>
    /// Occurs when a download is completed
    /// </summary>
    /// <param name="e">(Guid Id, bool Successful, string Filename, bool ShowNotification)</param>
    private bool DownloadCompleted((Guid Id, bool Successful, string Filename, bool ShowNotification) e)
    {
        var row = _downloadRows[e.Id];
        if (row.GetParent() == _downloadingBox)
        {
            row.SetCompletedState(e.Successful, e.Filename);
            var oldSeparator = row.GetPrevSibling() ?? row.GetNextSibling();
            if (oldSeparator is Gtk.Separator)
            {
                _downloadingBox.Remove(oldSeparator);
            }
            _downloadingBox.Remove(row);
            if (_completedBox.GetFirstChild() != null)
            {
                _completedBox.InsertChildAfter(row, null);
                _completedBox.InsertChildAfter(Gtk.Separator.New(Gtk.Orientation.Horizontal), row);
            }
            else
            {
                _completedBox.InsertChildAfter(_downloadRows[e.Id], null);
            }
            _downloadingBox.GetParent().SetVisible(_controller.DownloadManager.RemainingDownloadsCount > 0);
            _completedBox.GetParent().SetVisible(true);
            if (e.ShowNotification && ((GetFocus() != null && !GetFocus()!.GetHasFocus()) || !GetVisible()))
            {
                if (_controller.CompletedNotificationPreference == NotificationPreference.ForEach)
                {
                    SendShellNotification(new ShellNotificationSentEventArgs(!e.Successful ? _("Download Finished With Error") : _("Download Finished"), !e.Successful ? _("\"{0}\" has finished with an error!", row.Filename) : _("\"{0}\" has finished downloading.", row.Filename), !e.Successful ? NotificationSeverity.Error : NotificationSeverity.Success));
                }
                else if (_controller.CompletedNotificationPreference == NotificationPreference.AllCompleted && !_controller.DownloadManager.AreDownloadsRunning && !_controller.DownloadManager.AreDownloadsQueued)
                {
                    SendShellNotification(new ShellNotificationSentEventArgs(_("Downloads Finished"), _("All downloads have finished."), NotificationSeverity.Informational));
                }
            }
        }
        _stopAllDownloadsButton.SetVisible(_controller.DownloadManager.RemainingDownloadsCount > 1);
        if (!GetVisible() && _controller.DownloadManager.RemainingDownloadsCount == 0 && _controller.DownloadManager.ErrorsCount == 0)
        {
            _application.Quit();
        }
        return false;
    }

    /// <summary>
    /// Occurs when a download is stopped
    /// </summary>
    /// <param name="e">Guid</param>
    private bool DownloadStopped(Guid e)
    {
        var row = _downloadRows[e];
        if (row.GetParent() == _downloadingBox)
        {
            row.SetStopState();
            var oldSeparator = row.GetPrevSibling() ?? row.GetNextSibling();
            if (oldSeparator is Gtk.Separator)
            {
                _downloadingBox.Remove(oldSeparator);
            }
            _downloadingBox.Remove(row);
            if (_completedBox.GetFirstChild() != null)
            {
                _completedBox.InsertChildAfter(row, null);
                _completedBox.InsertChildAfter(Gtk.Separator.New(Gtk.Orientation.Horizontal), row);
            }
            else
            {
                _completedBox.InsertChildAfter(row, null);
            }
            _downloadingBox.GetParent().SetVisible(_controller.DownloadManager.AreDownloadsRunning);
            _completedBox.GetParent().SetVisible(true);
        }
        else if (row.GetParent() == _queuedBox)
        {
            row.SetStopState();
            var oldSeparator = row.GetPrevSibling() ?? row.GetNextSibling();
            if (oldSeparator is Gtk.Separator)
            {
                _queuedBox.Remove(oldSeparator);
            }
            _queuedBox.Remove(row);
            if (_completedBox.GetFirstChild() != null)
            {
                var newSeparator = Gtk.Separator.New(Gtk.Orientation.Horizontal);
                _completedBox.Append(newSeparator);
            }
            _completedBox.Append(row);
            _queuedBox.GetParent().SetVisible(_controller.DownloadManager.AreDownloadsQueued);
            _completedBox.GetParent().SetVisible(true);
        }
        _stopAllDownloadsButton.SetVisible(_controller.DownloadManager.RemainingDownloadsCount > 1);
        return false;
    }

    /// <summary>
    /// Occurs when a download is retried
    /// </summary>
    /// <param name="e">Guid</param>
    private bool DownloadRetried(Guid e)
    {
        var row = _downloadRows[e];
        if (row.GetParent() == _completedBox)
        {
            row.SetWaitingState();
            var oldSeparator = row.GetPrevSibling() ?? row.GetNextSibling();
            if (oldSeparator is Gtk.Separator)
            {
                _completedBox.Remove(oldSeparator);
            }
            _completedBox.Remove(row);
            _completedBox.GetParent().SetVisible(_controller.DownloadManager.AreDownloadsCompleted);
        }
        _stopAllDownloadsButton.SetVisible(_controller.DownloadManager.RemainingDownloadsCount > 1);
        return false;
    }

    /// <summary>
    /// Occurs when a download is started from queue
    /// </summary>
    /// <param name="e">Guid</param>
    private bool DownloadStartedFromQueue(Guid e)
    {
        var row = _downloadRows[e];
        if (row.GetParent() == _queuedBox)
        {
            row.SetPreparingState();
            var oldSeparator = row.GetPrevSibling() ?? row.GetNextSibling();
            if (oldSeparator is Gtk.Separator)
            {
                _queuedBox.Remove(oldSeparator);
            }
            _queuedBox.Remove(row);
            if (_downloadingBox.GetFirstChild() != null)
            {
                var newSeparator = Gtk.Separator.New(Gtk.Orientation.Horizontal);
                _downloadingBox.Append(newSeparator);
            }
            _downloadingBox.Append(row);
            _queuedBox.GetParent().SetVisible(_controller.DownloadManager.AreDownloadsQueued);
            _downloadingBox.GetParent().SetVisible(true);
        }
        _stopAllDownloadsButton.SetVisible(_controller.DownloadManager.RemainingDownloadsCount > 1);
        return false;
    }
}
