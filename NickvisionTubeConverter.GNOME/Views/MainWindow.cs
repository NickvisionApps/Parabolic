using NickvisionTubeConverter.GNOME.Controls;
using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Events;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// The MainWindow for the application
/// </summary>
public partial class MainWindow : Adw.ApplicationWindow
{
    private delegate bool GSourceFunc(nint data);

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

    private readonly MainWindowController _controller;
    private readonly Adw.Application _application;
    private readonly nint _bus;
    private readonly GSourceFunc[] _rowCallbacks;
    private readonly GSourceFunc _backgroundSourceFunc;

    private bool _isBackgroundStatusReported { get; set; }

    [Gtk.Connect] private readonly Adw.Bin _spinnerContainer;
    [Gtk.Connect] private readonly Gtk.Spinner _spinner;
    [Gtk.Connect] private readonly Gtk.Box _mainBox;
    [Gtk.Connect] private readonly Adw.ToastOverlay _toastOverlay;
    [Gtk.Connect] private readonly Adw.ViewStack _viewStack;
    [Gtk.Connect] private readonly Gtk.Button _addDownloadButton;
    [Gtk.Connect] private readonly Gtk.Box _downloadingBox;
    [Gtk.Connect] private readonly Gtk.Box _completedBox;
    [Gtk.Connect] private readonly Gtk.Box _queuedBox;

    private MainWindow(Gtk.Builder builder, MainWindowController controller, Adw.Application application) : base(builder.GetPointer("_root"), false)
    {
        //Window Settings
        _controller = controller;
        _application = application;
        _isBackgroundStatusReported = false;
        _bus = g_bus_get_sync(2, IntPtr.Zero, IntPtr.Zero); // 2 = session bus
        _rowCallbacks = new GSourceFunc[3];
        _backgroundSourceFunc = (d) =>
        {
            try
            {
                if (_isBackgroundStatusReported)
                {
                    var builder = g_variant_builder_new(g_variant_type_new("a{sv}"));
                    g_variant_builder_add(builder, "{sv}", "message", g_variant_new_string(_controller.GetBackgroundActivityReport()));
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
        _controller.RunInBackgroundChanged += RunInBackgroundChanged;
        SetTitle(_controller.AppInfo.ShortName);
        SetIconName(_controller.AppInfo.ID);
        if (_controller.IsDevVersion)
        {
            AddCssClass("devel");
        }
        OnCloseRequest += OnCloseRequested;
        //Build UI
        builder.Connect(this);
        //Update Title
        var windowTitle = (Adw.WindowTitle)builder.GetObject("_title");
        windowTitle.SetTitle(_controller.AppInfo.ShortName);
        //Update Greeting
        var greeting = (Adw.ButtonContent)builder.GetObject("_greeting");
        greeting.SetIconName(_controller.ShowSun ? "sun-outline-symbolic" : "moon-outline-symbolic");
        greeting.SetLabel(_controller.Greeting);
        var greetingIcon = (Gtk.Image)greeting.GetFirstChild();
        greetingIcon.SetPixelSize(48);
        greetingIcon.SetMarginEnd(6);
        var greetingLabel = (Gtk.Label)greeting.GetLastChild();
        greetingLabel.AddCssClass("greeting-title");
        //Register Events 
        _controller.NotificationSent += NotificationSent;
        _controller.UICreateDownloadRow = CreateDownloadRow;
        _controller.UIMoveDownloadRow = MoveDownloadRow;
        _controller.UIDeleteDownloadRowFromQueue = (row) => DeleteDownloadRow(row, _queuedBox);
        //Add Download Action
        var actDownload = Gio.SimpleAction.New("addDownload", null);
        actDownload.OnActivate += AddDownload;
        AddAction(actDownload);
        application.SetAccelsForAction("win.addDownload", new string[] { "<Ctrl>n" });
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
    public MainWindow(MainWindowController controller, Adw.Application application) : this(Builder.FromFile("window.ui", controller.Localizer, (s) => s == "About" ? string.Format(controller.Localizer[s], controller.AppInfo.ShortName) : controller.Localizer[s]), controller, application)
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
    private void NotificationSent(object? sender, NotificationSentEventArgs e) => _toastOverlay.AddToast(Adw.Toast.New(e.Message));

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
        notification.SetIcon(Gio.ThemedIcon.New($"{_controller.AppInfo.ID}-symbolic"));
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
        if (_controller.AreDownloadsRunning)
        {
            if (_controller.RunInBackground && (File.Exists("/.flatpak-info") || !String.IsNullOrEmpty(Environment.GetEnvironmentVariable("SNAP"))))
            {
                SetVisible(false);
                return true;
            }
            var closeDialog = new MessageDialog(this, _controller.AppInfo.ID, _controller.Localizer["CloseAndStop", "Title"], _controller.Localizer["CloseAndStop", "Description"], _controller.Localizer["No"], _controller.Localizer["Yes"]);
            if (closeDialog.Run() == MessageDialogResponse.Cancel)
            {
                return true;
            }
        }
        _controller.StopAllDownloads();
        _controller.Dispose();
        Environment.Exit(0);
        return false;
    }

    /// <summary>
    /// Creates a download row and adds it to the view
    /// </summary>
    /// <param name="download">The download model</param>
    /// <returns>The new download row</returns>
    private IDownloadRowControl CreateDownloadRow(Download download)
    {
        var downloadRow = new DownloadRow(download, _controller.Localizer, (e) => NotificationSent(null, e));
        _rowCallbacks[0] = (x) =>
        {
            _addDownloadButton.SetVisible(true);
            _viewStack.SetVisibleChildName("pageDownloads");
            return false;
        };
        g_main_context_invoke(0, _rowCallbacks[0], 0);
        return downloadRow;
    }

    /// <summary>
    /// Moves the download row to a new section
    /// </summary>
    /// <param name="row">IDownloadRowControl</param>
    /// <param name="stage">DownloadStage</param>
    private void MoveDownloadRow(IDownloadRowControl row, DownloadStage stage)
    {
        _rowCallbacks[1] = (x) =>
        {
            var gtkRow = (DownloadRow)row;
            var parent = gtkRow.GetParent();
            var box = stage switch
            {
                DownloadStage.InQueue => _queuedBox,
                DownloadStage.Downloading => _downloadingBox,
                DownloadStage.Completed => _completedBox
            };
            if (parent == box)
            {
                return false;
            }
            else if (parent != null)
            {
                DeleteDownloadRow(row, (Gtk.Box)parent);
            }
            if (box.GetFirstChild() != null)
            {
                var separator = Gtk.Separator.New(Gtk.Orientation.Horizontal);
                box.Append(separator);
            }
            box.Append(gtkRow);
            box.GetParent().SetVisible(true);
            if (stage == DownloadStage.Completed && (!GetFocus()!.GetHasFocus() || !GetVisible()))
            {
                SendShellNotification(new ShellNotificationSentEventArgs(_controller.Localizer[row.FinishedWithError ? "DownloadFinishedWithError" : "DownloadFinished"], string.Format(_controller.Localizer[row.FinishedWithError ? "DownloadFinishedWithError" : "DownloadFinished", "Description"], $"\"{row.Filename}\""), row.FinishedWithError ? NotificationSeverity.Error : NotificationSeverity.Success));
                if (!GetVisible() && !_controller.AreDownloadsRunning && _controller.ErrorsCount == 0)
                {
                    _application.Quit();
                }
            }
            return false;
        };
        g_main_context_invoke(0, _rowCallbacks[1], 0);
    }

    /// <summary>
    /// Deletes a download row from a section
    /// </summary>
    /// <param name="row">IDownloadRowControl</param>
    /// <param name="box">Gtk.Box</param>
    private void DeleteDownloadRow(IDownloadRowControl row, Gtk.Box box)
    {
        _rowCallbacks[2] = (x) =>
        {
            var gtkRow = (DownloadRow)row;
            var separator = gtkRow.GetPrevSibling() ?? gtkRow.GetNextSibling();
            if (separator is Gtk.Separator)
            {
                box.Remove(separator);
            }
            box.Remove(gtkRow);
            box.GetParent().SetVisible(box.GetFirstChild() != null);
            return false;
        };
        g_main_context_invoke(0, _rowCallbacks[2], 0);
    }

    /// <summary>
    /// Occurs when the add download action is triggered
    /// </summary>
    /// <param name="sender">Gio.SimpleAction</param>
    /// <param name="e">EventArgs</param>
    private void AddDownload(Gio.SimpleAction sender, EventArgs e)
    {
        var addController = _controller.CreateAddDownloadDialogController();
        var addDialog = new AddDownloadDialog(addController, this);
        addDialog.Present();
        addDialog.OnDownload += (sender, e) =>
        {
            foreach (var download in addController.Downloads)
            {
                _controller.AddDownload(download);
            }
            addDialog.Close();
        };
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
        var builder = Builder.FromFile("shortcuts_dialog.ui", _controller.Localizer, (s) => s == "About" ? string.Format(_controller.Localizer[s], _controller.AppInfo.ShortName) : _controller.Localizer[s]);
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
        var dialog = Adw.AboutWindow.New();
        dialog.SetTransientFor(this);
        dialog.SetIconName(_controller.AppInfo.ID);
        dialog.SetApplicationName(_controller.AppInfo.ShortName);
        dialog.SetApplicationIcon(_controller.AppInfo.ID + (_controller.AppInfo.GetIsDevelVersion() ? "-devel" : ""));
        dialog.SetVersion(_controller.AppInfo.Version);
        dialog.SetComments(_controller.AppInfo.Description);
        dialog.SetDeveloperName("Nickvision");
        dialog.SetLicenseType(Gtk.License.MitX11);
        dialog.SetCopyright($"© Nickvision 2021-2023\n\n{_controller.Localizer["Disclaimer"]}");
        dialog.SetWebsite(_controller.AppInfo.GitHubRepo.ToString());
        dialog.SetIssueUrl(_controller.AppInfo.IssueTracker.ToString());
        dialog.SetSupportUrl(_controller.AppInfo.SupportUrl.ToString());
        dialog.AddLink(_controller.Localizer["SupportedSites"], "https://github.com/yt-dlp/yt-dlp/blob/master/supportedsites.md");
        dialog.AddLink(_controller.Localizer["MatrixChat"], "https://matrix.to/#/#nickvision:matrix.org");
        dialog.SetDevelopers(_controller.Localizer["Developers", "Credits"].Split(Environment.NewLine));
        dialog.SetDesigners(_controller.Localizer["Designers", "Credits"].Split(Environment.NewLine));
        dialog.SetArtists(_controller.Localizer["Artists", "Credits"].Split(Environment.NewLine));
        dialog.SetTranslatorCredits((string.IsNullOrEmpty(_controller.Localizer["Translators", "Credits"]) ? "" : _controller.Localizer["Translators", "Credits"]));
        dialog.SetReleaseNotes(_controller.AppInfo.Changelog);
        dialog.Present();
    }

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
}
