using NickvisionTubeConverter.GNOME.Controls;
using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Events;
using NickvisionTubeConverter.Shared.Models;
using Python.Runtime;
using System;
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
    private delegate bool GSourceFunc(nint data);
    private delegate void GAsyncReadyCallback(nint source, nint res, nint user_data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void g_main_context_invoke(nint context, GSourceFunc function, nint data);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial uint g_timeout_add(uint interval, GSourceFunc function, nint data);
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
    private static partial nint g_file_new_for_path(string path);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint g_file_icon_new(nint gfile);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void g_notification_set_icon(nint notification, nint icon);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gdk_clipboard_read_text_async(nint clipboard, nint cancellable, GAsyncReadyCallback callback, nint user_data);
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial string gdk_clipboard_read_text_finish(nint clipboard, nint result, nint error);

    [LibraryImport("libunity.so.9", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint unity_launcher_entry_get_for_desktop_id(string desktop_id);
    [LibraryImport("libunity.so.9", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void unity_launcher_entry_set_progress_visible(nint launcher, [MarshalAs(UnmanagedType.I1)] bool visibility);
    [LibraryImport("libunity.so.9", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void unity_launcher_entry_set_progress(nint launcher, double progress);

    private readonly MainWindowController _controller;
    private readonly Adw.Application _application;
    private readonly nint _bus;
    private readonly GSourceFunc _backgroundSourceFunc;
    private readonly GSourceFunc _libUnitySourceFunc;
    private readonly GSourceFunc _downloadAddedFunc;
    private readonly GSourceFunc _downloadProgressUpdatedFunc;
    private readonly GSourceFunc _downloadCompletedFunc;
    private readonly GSourceFunc _downloadStoppedFunc;
    private readonly GSourceFunc _downloadRetriedFunc;
    private readonly GSourceFunc _downloadStartedFromQueueFunc;
    private readonly nint _unityLauncher;
    private bool _isBackgroundStatusReported;
    private GAsyncReadyCallback _clipboardCallback;

    [Gtk.Connect] private readonly Adw.Bin _spinnerContainer;
    [Gtk.Connect] private readonly Gtk.Spinner _spinner;
    [Gtk.Connect] private readonly Gtk.Box _mainBox;
    [Gtk.Connect] private readonly Adw.HeaderBar _headerBar;
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
        _bus = g_bus_get_sync(2, IntPtr.Zero, IntPtr.Zero); // 2 = session bus
        _backgroundSourceFunc = (x) =>
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
        };
        _libUnitySourceFunc = (x) =>
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
        };
        _downloadAddedFunc = (x) =>
        {
            var handle = GCHandle.FromIntPtr(x);
            var target = ((Guid Id, string Filename, string SaveFolder, bool IsDownloading)?)handle.Target;
            if (target != null)
            {
                var e = target.Value;
                var downloadRow = new DownloadRow(e.Id, e.Filename, e.SaveFolder, (e) => NotificationSent(null, e));
                downloadRow.StopRequested += (sender, e) => _controller.DownloadManager.RequestStop(e);
                downloadRow.RetryRequested += (sender, e) => _controller.DownloadManager.RequestRetry(e, _controller.DownloadOptions);
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
                _stopAllDownloadsButton.SetVisible(_controller.DownloadManager.RemainingDownloadsCount > 1);
                box.GetParent().SetVisible(true);
            }
            handle.Free();
            return false;
        };
        _downloadProgressUpdatedFunc = (x) =>
        {
            var handle = GCHandle.FromIntPtr(x);
            var target = ((Guid Id, DownloadProgressState State)?)handle.Target;
            if (target != null)
            {
                var e = target.Value;
                var i = _downloadingBox.GetFirstChild();
                DownloadRow? row = null;
                while (row == null && i != null)
                {
                    if (i is DownloadRow j)
                    {
                        if (j.Id == e.Id)
                        {
                            row = j;
                            break;
                        }
                    }
                    i = i.GetNextSibling();
                }
                if (row != null)
                {
                    row.SetProgressState(e.State);
                }
            }
            handle.Free();
            return false;
        };
        _downloadCompletedFunc = (x) =>
        {
            var handle = GCHandle.FromIntPtr(x);
            var target = ((Guid Id, bool Successful)?)handle.Target;
            if (target != null)
            {
                var e = target.Value;
                var i = _downloadingBox.GetFirstChild();
                DownloadRow? row = null;
                while (row == null && i != null)
                {
                    if (i is DownloadRow j)
                    {
                        if (j.Id == e.Id)
                        {
                            row = j;
                            break;
                        }
                    }
                    i = i.GetNextSibling();
                }
                if (row != null)
                {
                    row.SetCompletedState(e.Successful);
                    var oldSeparator = row.GetPrevSibling() ?? row.GetNextSibling();
                    if (oldSeparator is Gtk.Separator)
                    {
                        _downloadingBox.Remove(oldSeparator);
                    }
                    _downloadingBox.Remove(row);
                    if (_completedBox.GetFirstChild() != null)
                    {
                        var newSeparator = Gtk.Separator.New(Gtk.Orientation.Horizontal);
                        _completedBox.Append(newSeparator);
                    }
                    _completedBox.Append(row);
                    _downloadingBox.GetParent().SetVisible(_controller.DownloadManager.RemainingDownloadsCount > 0 ? true : false);
                    _completedBox.GetParent().SetVisible(true);
                    if (!GetFocus()!.GetHasFocus() || !GetVisible())
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
                if (!GetVisible() && _controller.DownloadManager.RemainingDownloadsCount == 0 && _controller.DownloadManager.ErrorsCount == 0)
                {
                    _application.Quit();
                }
            }
            handle.Free();
            return false;
        };
        _downloadStoppedFunc = (x) =>
        {
            var handle = GCHandle.FromIntPtr(x);
            var target = (Guid?)handle.Target;
            if (target != null)
            {
                var e = target.Value;
                var i = _downloadingBox.GetFirstChild();
                DownloadRow? row = null;
                while (row == null && i != null)
                {
                    if (i is DownloadRow j)
                    {
                        if (j.Id == e)
                        {
                            row = j;
                            break;
                        }
                    }
                    i = i.GetNextSibling();
                }
                if (row != null)
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
                        var newSeparator = Gtk.Separator.New(Gtk.Orientation.Horizontal);
                        _completedBox.Append(newSeparator);
                    }
                    _completedBox.Append(row);
                    _downloadingBox.GetParent().SetVisible(_controller.DownloadManager.AreDownloadsRunning);
                    _completedBox.GetParent().SetVisible(true);
                }
                else
                {
                    i = _queuedBox.GetFirstChild();
                    while (row == null && i != null)
                    {
                        if (i is DownloadRow j)
                        {
                            if (j.Id == e)
                            {
                                row = j;
                                break;
                            }
                        }
                        i = i.GetNextSibling();
                    }
                    if (row != null)
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
                }
            }
            handle.Free();
            return false;
        };
        _downloadRetriedFunc = (x) =>
        {
            var handle = GCHandle.FromIntPtr(x);
            var target = (Guid?)handle.Target;
            if (target != null)
            {
                var e = target.Value;
                var i = _completedBox.GetFirstChild();
                DownloadRow? row = null;
                while (row == null && i != null)
                {
                    if (i is DownloadRow j)
                    {
                        if (j.Id == e)
                        {
                            row = j;
                            break;
                        }
                    }
                    i = i.GetNextSibling();
                }
                if (row != null)
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
            }
            handle.Free();
            return false;
        };
        _downloadStartedFromQueueFunc = (x) =>
        {
            var handle = GCHandle.FromIntPtr(x);
            var target = (Guid?)handle.Target;
            if (target != null)
            {
                var e = target.Value;
                var i = _queuedBox.GetFirstChild();
                DownloadRow? row = null;
                while (row == null && i != null)
                {
                    if (i is DownloadRow j)
                    {
                        if (j.Id == e)
                        {
                            row = j;
                            break;
                        }
                    }
                    i = i.GetNextSibling();
                }
                if (row != null)
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
            }
            handle.Free();
            return false;
        };
        try
        {
            _unityLauncher = unity_launcher_entry_get_for_desktop_id($"{_controller.AppInfo.ID}.desktop");
            g_timeout_add(1000, _libUnitySourceFunc, IntPtr.Zero);
        }
        catch (DllNotFoundException e)
        {
            _unityLauncher = IntPtr.Zero;
        }
        //Build UI
        SetTitle(_controller.AppInfo.ShortName);
        SetIconName(_controller.AppInfo.ID);
        if (_controller.IsDevVersion)
        {
            AddCssClass("devel");
        }
        builder.Connect(this);
        //Register Events
        OnCloseRequest += OnCloseRequested;
        _controller.NotificationSent += NotificationSent;
        _controller.RunInBackgroundChanged += RunInBackgroundChanged;
        _controller.DownloadManager.DownloadAdded += (sender, e) => g_main_context_invoke(0, _downloadAddedFunc, (IntPtr)GCHandle.Alloc(e));
        _controller.DownloadManager.DownloadProgressUpdated += (sender, e) => g_main_context_invoke(0, _downloadProgressUpdatedFunc, (IntPtr)GCHandle.Alloc(e));
        _controller.DownloadManager.DownloadCompleted += (sender, e) => g_main_context_invoke(0, _downloadCompletedFunc, (IntPtr)GCHandle.Alloc(e));
        _controller.DownloadManager.DownloadStopped += (sender, e) => g_main_context_invoke(0, _downloadStoppedFunc, (IntPtr)GCHandle.Alloc(e));
        _controller.DownloadManager.DownloadRetried += (sender, e) => g_main_context_invoke(0, _downloadRetriedFunc, (IntPtr)GCHandle.Alloc(e));
        _controller.DownloadManager.DownloadStartedFromQueue += (sender, e) => g_main_context_invoke(0, _downloadStartedFromQueueFunc, (IntPtr)GCHandle.Alloc(e));
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
        //About Action
        var actAbout = Gio.SimpleAction.New("about", null);
        actAbout.OnActivate += About;
        AddAction(actAbout);
        application.SetAccelsForAction("win.about", new string[] { "F1" });
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
    public void Start()
    {
        _application.AddWindow(this);
        Present();
        _spinnerContainer.SetVisible(true);
        _mainBox.SetVisible(false);
        _spinner.Start();
        _controller.Startup();
        ValidateClipboard();
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
        if (e.Action == "clipboard")
        {
            toast.SetButtonLabel(_("Download"));
            toast.OnButtonClicked += async (s, ex) => await AddDownloadAsync(e);
        }
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
            var iconHandle = g_file_icon_new(g_file_new_for_path($"{Environment.GetEnvironmentVariable("SNAP")}/usr/share/icons/hicolor/symbolic/apps/{_controller.AppInfo.ID}-symbolic.svg"));
            g_notification_set_icon(notification.Handle, iconHandle);
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
            var closeDialog = new MessageDialog(this, _controller.AppInfo.ID, _("Close and Stop Downloads?"), _("Some downloads are still in progress.\nAre you sure you want to close Tube Converter and stop the running downloads?"), _("No"), _("Yes"));
            if (closeDialog.Run() == MessageDialogResponse.Cancel)
            {
                return true;
            }
        }
        _controller.DownloadManager.StopAllDownloads(false);
        _controller.Dispose();
        Environment.Exit(0);
        return false;
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
            dynamic yt_dlp = Py.Import("yt_dlp");
            debugInfo.AppendLine($"yt-dlp {yt_dlp.version.__version__.As<string>()}");
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
        dialog.SetCopyright($"© Nickvision 2021-2023\n\n{_("The authors of Nickvision Tube Converter are not responsible/liable for any misuse of this program that may violate local copyright/DMCA laws. Users use this application at their own risk.")}");
        dialog.SetWebsite(_controller.AppInfo.GitHubRepo.ToString());
        dialog.SetIssueUrl(_controller.AppInfo.IssueTracker.ToString());
        dialog.SetSupportUrl(_controller.AppInfo.SupportUrl.ToString());
        dialog.AddLink(_("List of supported sites"), "https://github.com/yt-dlp/yt-dlp/blob/master/supportedsites.md");
        dialog.AddLink(_("Matrix Chat"), "https://matrix.to/#/#nickvision:matrix.org");
        dialog.SetDevelopers(_("Nicholas Logozzo {0}\nContributors on GitHub ❤️ {1}", "https://github.com/nlogozzo", "https://github.com/NickvisionApps/TubeConverter/graphs/contributors").Split("\n"));
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
                g_timeout_add(1000, _backgroundSourceFunc, IntPtr.Zero);
            }
        }
        else
        {
            _isBackgroundStatusReported = false;
        }
    }

    /// <summary>
    /// Reads the clipboard's text and checks for a valid media url
    /// </summary>
    private void ValidateClipboard()
    {
        var clipboard = Gdk.Display.GetDefault()!.GetClipboard();
        _clipboardCallback = (source, res, data) =>
        {
            var clipboardText = gdk_clipboard_read_text_finish(clipboard.Handle, res, IntPtr.Zero);
            if(!string.IsNullOrEmpty(clipboardText))
            {
                _controller.ValidateClipboard(clipboardText);
            }
        };
        gdk_clipboard_read_text_async(clipboard.Handle, IntPtr.Zero, _clipboardCallback, IntPtr.Zero);
    }
}
