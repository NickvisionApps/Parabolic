using NickvisionTubeConverter.GNOME.Controls;
using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Events;
using NickvisionTubeConverter.Shared.Models;
using Python.Runtime;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using static NickvisionTubeConverter.Shared.Helpers.Gettext;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// The MainWindow for the application
/// </summary>
public partial class MainWindow : Adw.ApplicationWindow
{
    private delegate void GAsyncReadyCallback(nint source, nint res, nint user_data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint g_bus_get_sync(uint bus_type, nint cancellable, nint error);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint g_variant_new(string format_string, nint data);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint g_variant_new_string(string data);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint g_variant_builder_new(nint type);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void g_variant_builder_add(nint builder, string format, string key, nint data);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void g_variant_builder_unref(nint builder);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint g_variant_type_new(string type);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void g_dbus_connection_call(nint connection, string bus_name, string object_path, string interface_name, string method_name, nint parameters, nint reply_type, uint flags, int timeout_msec, nint cancellable, nint callback, nint user_data);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void g_notification_set_icon(nint notification, nint icon);
    
    [LibraryImport("libunity.so.9", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint unity_launcher_entry_get_for_desktop_id(string desktop_id);
    [LibraryImport("libunity.so.9", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void unity_launcher_entry_set_progress_visible(nint launcher, [MarshalAs(UnmanagedType.I1)] bool visibility);
    [LibraryImport("libunity.so.9", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void unity_launcher_entry_set_progress(nint launcher, double progress);

    private readonly MainWindowController _controller;
    private readonly Adw.Application _application;
    private readonly nint _bus;
    private readonly nint _unityLauncher;
    private bool _isBackgroundStatusReported;
    private Dictionary<Guid, DownloadRow> _downloadRows;

    [Gtk.Connect] private readonly Adw.Bin _spinnerContainer;
    [Gtk.Connect] private readonly Gtk.Spinner _spinner;
    [Gtk.Connect] private readonly Gtk.Box _mainBox;
    [Gtk.Connect] private readonly Adw.HeaderBar _headerBar;
    [Gtk.Connect] private readonly Adw.WindowTitle _title;
    [Gtk.Connect] private readonly Adw.ToastOverlay _toastOverlay;
    [Gtk.Connect] private readonly Adw.ViewStack _viewStack;
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
        _isBackgroundStatusReported = false;
        _downloadRows = new Dictionary<Guid, DownloadRow>();
        _bus = g_bus_get_sync(2, IntPtr.Zero, IntPtr.Zero); // 2 = session bus
        try
        {
            _unityLauncher = unity_launcher_entry_get_for_desktop_id(string.IsNullOrEmpty(Environment.GetEnvironmentVariable("SNAP")) ? $"{_controller.AppInfo.ID}.desktop" : "tube-converter_tube-converter.desktop");
            GLib.Functions.TimeoutAdd(0, 1000, UpdateLibUnity);
        }
        catch (DllNotFoundException e)
        {
            _unityLauncher = IntPtr.Zero;
        }
        //Build UI
        builder.Connect(this);
        SetTitle(_controller.AppInfo.ShortName);
        SetIconName(_controller.AppInfo.ID);
        if (_controller.IsDevVersion)
        {
            AddCssClass("devel");
        }
        _title.SetTitle(_controller.AppInfo.ShortName);
        //Register Events
        OnCloseRequest += OnCloseRequested;
        _controller.NotificationSent += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            NotificationSent(sender, e);
            return false;
        });
        _controller.RunInBackgroundChanged += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            RunInBackgroundChanged(sender, e);
            return false;
        });
        _controller.KeyringLoginAsync = KeyringLoginAsync;
        _controller.DownloadManager.DownloadAdded += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            DownloadAdded(sender, e);
            return false;
        });
        _controller.DownloadManager.DownloadProgressUpdated += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            DownloadProgressUpdated(sender, e);
            return false;
        });
        _controller.DownloadManager.DownloadCompleted += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            DownloadCompleted(sender, e);
            return false;
        });
        _controller.DownloadManager.DownloadStopped += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            DownloadStopped(sender, e);
            return false;
        });
        _controller.DownloadManager.DownloadRetried += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            DownloadRetried(sender, e);
            return false;
        });
        _controller.DownloadManager.DownloadStartedFromQueue += (sender, e) => GLib.Functions.IdleAdd(0, () =>
        {
            DownloadStartedFromQueue(sender, e);
            return false;
        });
        //Add Download Action
        var actDownload = Gio.SimpleAction.New("addDownload", null);
        actDownload.OnActivate += async (sender, e) => await AddDownloadAsync(new NotificationSentEventArgs("", NotificationSeverity.Informational));;
        AddAction(actDownload);
        application.SetAccelsForAction("win.addDownload", new string[] { "<Ctrl>n" });
        //Stop All Downloads Action
        var actStopAllDownloads = Gio.SimpleAction.New("stopAllDownloads", null);
        actStopAllDownloads.OnActivate += (sender, e) => _controller.DownloadManager.StopAllDownloads(true);
        AddAction(actStopAllDownloads);
        application.SetAccelsForAction("win.stopAllDownloads", new string[] { "<Ctrl><Shift>c" });
        //Retry Failed Downloads Action
        var actRetryFailedDownloads = Gio.SimpleAction.New("retryFailedDownloads", null);
        actRetryFailedDownloads.OnActivate += (sender, e) => _controller.DownloadManager.RetryFailedDownloads(_controller.DownloadOptions);
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
            if(!_controller.DownloadManager.AreDownloadsQueued && !_controller.DownloadManager.AreDownloadsRunning && !_controller.DownloadManager.AreDownloadsCompleted)
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
            if(!_controller.DownloadManager.AreDownloadsQueued && !_controller.DownloadManager.AreDownloadsRunning && !_controller.DownloadManager.AreDownloadsCompleted)
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
        actHelp.OnActivate += (sender, e) => Gtk.Functions.ShowUri(this, "help:parabolic", 0);
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
        await _controller.StartupAsync();
        _spinner.Stop();
        _spinnerContainer.SetVisible(false);
        _mainBox.SetVisible(true);
        RunInBackgroundChanged(this, EventArgs.Empty);
    }

    /// <summary>
    /// Occurs when a notification is sent from the controller
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">NotificationSentEventArgs</param>
    private void NotificationSent(object? sender, NotificationSentEventArgs e)
    {
        var toast = Adw.Toast.New(e.Message);
        _toastOverlay.AddToast(toast);
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
            g_notification_set_icon(notification.Handle, fileIcon.Handle);
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
            var closeDialog = new MessageDialog(this, _controller.AppInfo.ID, _("Close and Stop Downloads?"), _("Some downloads are still in progress.\nAre you sure you want to close Parabolic and stop the running downloads?"), _("No"), _("Yes"));
            if (closeDialog.Run() == MessageDialogResponse.Cancel)
            {
                return true;
            }
        }
        _controller.DownloadManager.StopAllDownloads(false);
        _controller.Dispose();
        return false;
    }

    /// <summary>
    /// Occurs when Keyring needs a login
    /// </summary>
    /// <param name="title">The title of the account</param>
    public async Task<string?> KeyringLoginAsync(string title)
    {
        var tcs = new TaskCompletionSource<string?>();
        var passwordDialog = new PasswordDialog(this, title, tcs);
        passwordDialog.SetIconName(_controller.AppInfo.ID);
        passwordDialog.Present();
        return await tcs.Task;
    }

    /// <summary>
    /// Prompts the AddDownloadDialog
    /// </summary>
    /// <param name="e">NotificationSentEventArgs</param>
    private async Task AddDownloadAsync(NotificationSentEventArgs e)
    {
        var addController = _controller.CreateAddDownloadDialogController();
        var addDialog = new AddDownloadDialog(addController, this);
        addDialog.OnDownload += (s, ex) =>
        {
            _headerBar.RemoveCssClass("flat");
            _addDownloadButton.SetVisible(true);
            _viewStack.SetVisibleChildName("pageDownloads");
            foreach (var download in addController.Downloads)
            {
                _controller.DownloadManager.AddDownload(download, _controller.DownloadOptions);
            }
            addDialog.Close();
        };
        await addDialog.PresentAsync(e.ActionParam);
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
        var messageDialog = new MessageDialog(this, _controller.AppInfo.ID, "TODO", "This feature is not yet implemented", "OK");
        messageDialog.OnResponse += (sender, e) => messageDialog.Destroy();
        messageDialog.Present();
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
        var shortcutsWindow = (Gtk.ShortcutsWindow)builder.GetObject("_shortcuts");
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
    private void About(Gio.SimpleAction sender, EventArgs e)
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
        var ffmpegProcess = new Process
        {
            StartInfo = new ProcessStartInfo
            {
                FileName = "ffmpeg",
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
        var ariaProcess = new Process
        {
            StartInfo = new ProcessStartInfo
            {
                FileName = "aria2c",
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
        debugInfo.AppendLine(CultureInfo.CurrentCulture.ToString());
        var localeProcess = new Process
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
        dialog.SetApplicationIcon(_controller.AppInfo.ID + (_controller.AppInfo.GetIsDevelVersion() ? "-devel" : ""));
        dialog.SetVersion(_controller.AppInfo.Version);
        dialog.SetDebugInfo(debugInfo.ToString());
        dialog.SetComments(_controller.AppInfo.Description);
        dialog.SetDeveloperName("Nickvision");
        dialog.SetLicenseType(Gtk.License.MitX11);
        dialog.SetCopyright($"© Nickvision 2021-2023\n\n{_("The authors of Nickvision Parabolic are not responsible/liable for any misuse of this program that may violate local copyright/DMCA laws. Users use this application at their own risk.")}");
        dialog.SetWebsite("https://nickvision.org/");
        dialog.SetIssueUrl(_controller.AppInfo.IssueTracker.ToString());
        dialog.SetSupportUrl(_controller.AppInfo.SupportUrl.ToString());
        dialog.AddLink(_("GitHub Repo"), _controller.AppInfo.GitHubRepo.ToString());
        dialog.AddLink(_("List of supported sites"), "https://github.com/yt-dlp/yt-dlp/blob/master/supportedsites.md");
        dialog.AddLink(_("Matrix Chat"), "https://matrix.to/#/#nickvision:matrix.org");
        dialog.SetDevelopers(_("Nicholas Logozzo {0}\nContributors on GitHub ❤️ {1}", "https://github.com/nlogozzo", "https://github.com/NickvisionApps/Parabolic/graphs/contributors").Split("\n"));
        dialog.SetDesigners(_("Nicholas Logozzo {0}\nFyodor Sobolev {1}\nDaPigGuy {2}", "https://github.com/nlogozzo", "https://github.com/fsobolev", "https://github.com/DaPigGuy").Split("\n"));
        dialog.SetArtists(_("David Lapshin {0}\nBrage Fuglseth {1}\nmarcin {2}", "https://github.com/daudix-UFO", "https://github.com/bragefuglseth", "https://github.com/martin-desktops").Split("\n"));
        dialog.SetTranslatorCredits(_("translator-credits"));
        dialog.SetReleaseNotes(_controller.AppInfo.Changelog);
        dialog.Present();
    }

    /// <summary>
    /// Occurs when the run in background option is changed
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">EventArgs</param>
    private void RunInBackgroundChanged(object? sender, EventArgs e)
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
                var builder = g_variant_builder_new(g_variant_type_new("a{sv}"));
                g_variant_builder_add(builder, "{sv}", "message", g_variant_new_string(_controller.DownloadManager.BackgroundActivityReport));
                g_dbus_connection_call(_bus,
                    "org.freedesktop.portal.Desktop", // Bus name
                    "/org/freedesktop/portal/desktop", // Object path
                    "org.freedesktop.portal.Background", // Interface name
                    "SetStatus", // Method name
                    g_variant_new("(a{sv})", builder), // Parameters
                    IntPtr.Zero, // Reply type
                    0, // Flags
                    -1, // Timeout
                    IntPtr.Zero, // Cancellable
                    IntPtr.Zero, // Callback
                    IntPtr.Zero); // User data
                g_variant_builder_unref(builder);
            }
            return _isBackgroundStatusReported;
        }
        catch
        {
            return false;
        }
    }

    /// <summary>
    /// Updates libunity
    /// </summary>
    private bool UpdateLibUnity()
    {
        try
        {
            var progress = _controller.DownloadManager.TotalProgress;
            if (progress > 0 && progress < 1)
            {
                unity_launcher_entry_set_progress_visible(_unityLauncher, true);
                unity_launcher_entry_set_progress(_unityLauncher, progress);
            }
            else
            {
                unity_launcher_entry_set_progress_visible(_unityLauncher, false);
            }
            return true;
        }
        catch
        {
            return false;
        }
    }

    /// <summary>
    /// Occurs when a download is added
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">(Guid Id, string Filename, string SaveFolder, bool IsDownloading)</param>
    private void DownloadAdded(object? sender, (Guid Id, string Filename, string SaveFolder, bool IsDownloading) e)
    {
        var downloadRow = new DownloadRow(e.Id, e.Filename, e.SaveFolder, (ex) => NotificationSent(null, ex));
        downloadRow.StopRequested += (s, ex) => _controller.DownloadManager.RequestStop(ex);
        downloadRow.RetryRequested += (s, ex) => _controller.DownloadManager.RequestRetry(ex, _controller.DownloadOptions);
        var box = e.IsDownloading ? _downloadingBox : _queuedBox;
        if(e.IsDownloading)
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
        if (_downloadRows.ContainsKey(e.Id))
        {
            _downloadRows[e.Id] = downloadRow;
        }
        else
        {
            _downloadRows.Add(e.Id, downloadRow);
        }
        _stopAllDownloadsButton.SetVisible(_controller.DownloadManager.RemainingDownloadsCount > 1);
        box.GetParent().SetVisible(true);
    }

    /// <summary>
    /// Occurs when a download's progress is updated
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">(Guid Id, DownloadProgressState State)</param>
    private void DownloadProgressUpdated(object? sender, (Guid Id, DownloadProgressState State) e)
    {
        var row = _downloadRows[e.Id];
        if(row.GetParent() == _downloadingBox)
        {
            row.SetProgressState(e.State);
        }
    }

    /// <summary>
    /// Occurs when a download is completed
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">(Guid Id, bool Successful, string Filename, bool ShowNotification)</param>
    private void DownloadCompleted(object? sender, (Guid Id, bool Successful, string Filename, bool ShowNotification) e)
    {
        var row = _downloadRows[e.Id];
        if(row.GetParent() == _downloadingBox)
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
                if(_controller.CompletedNotificationPreference == NotificationPreference.ForEach)
                {
                    SendShellNotification(new ShellNotificationSentEventArgs(!e.Successful ? _("Download Finished With Error") : _("Download Finished"), !e.Successful ? _("\"{0}\" has finished with an error!", row.Filename) : _("\"{0}\" has finished downloading.", row.Filename), !e.Successful ? NotificationSeverity.Error : NotificationSeverity.Success));
                }
                else if(_controller.CompletedNotificationPreference == NotificationPreference.AllCompleted && !_controller.DownloadManager.AreDownloadsRunning && !_controller.DownloadManager.AreDownloadsQueued)
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
    }

    /// <summary>
    /// Occurs when a download is stopped
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">Guid</param>
    private void DownloadStopped(object? sender, Guid e)
    {
        var row = _downloadRows[e];
        if(row.GetParent() == _downloadingBox)
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
        else if(row.GetParent() == _queuedBox)
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
    }

    /// <summary>
    /// Occurs when a download is retried
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">Guid</param>
    private void DownloadRetried(object? sender, Guid e)
    {
        var row = _downloadRows[e];
        if(row.GetParent() == _completedBox)
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
    }

    /// <summary>
    /// Occurs when a download is started from queue
    /// </summary>
    /// <param name="sender">object?</param>
    /// <param name="e">Guid</param>
    private void DownloadStartedFromQueue(object? sender, Guid e)
    {
        var row = _downloadRows[e];
        if(row.GetParent() == _queuedBox)
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
    }
}
