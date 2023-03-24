using NickvisionTubeConverter.GNOME.Controls;
using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Events;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// The MainWindow for the application
/// </summary>
public partial class MainWindow : Adw.ApplicationWindow
{
    private readonly MainWindowController _controller;
    private readonly Adw.Application _application;

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
            var closeDialog = new MessageDialog(this, _controller.Localizer["CloseAndStop", "Title"], _controller.Localizer["CloseAndStop", "Description"], _controller.Localizer["No"], _controller.Localizer["Yes"]);
            if (closeDialog.Run() == MessageDialogResponse.Cancel)
            {
                return true;
            }
        }
        _controller.StopAllDownloads();
        _controller.Dispose();
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
        _addDownloadButton.SetVisible(true);
        _viewStack.SetVisibleChildName("pageDownloads");
        return downloadRow;
    }

    /// <summary>
    /// Moves the download row to a new section
    /// </summary>
    /// <param name="row">IDownloadRowControl</param>
    /// <param name="stage">DownloadStage</param>
    private void MoveDownloadRow(IDownloadRowControl row, DownloadStage stage)
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
            return;
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
        if (stage == DownloadStage.Completed && !GetFocus()!.GetHasFocus())
        {
            SendShellNotification(new ShellNotificationSentEventArgs(_controller.Localizer["DownloadFinished"], string.Format(_controller.Localizer["DownloadFinished", "Description"], $"\"{row.Filename}\""), NotificationSeverity.Success));
        }
    }

    /// <summary>
    /// Deletes a download row from a section
    /// </summary>
    /// <param name="row">IDownloadRowControl</param>
    /// <param name="box">Gtk.Box</param>
    private void DeleteDownloadRow(IDownloadRowControl row, Gtk.Box box)
    {
        var gtkRow = (DownloadRow)row;
        var separator = gtkRow.GetPrevSibling() ?? gtkRow.GetNextSibling();
        if (separator is Gtk.Separator)
        {
            box.Remove(separator);
        }
        box.Remove(gtkRow);
        box.GetParent().SetVisible(box.GetFirstChild() != null);
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
        var preferencesDialog = new PreferencesDialog(_controller.PreferencesViewController, _application, this);
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
        dialog.SetDevelopers(_controller.Localizer["Developers", "Credits"].Split(Environment.NewLine));
        dialog.SetDesigners(_controller.Localizer["Designers", "Credits"].Split(Environment.NewLine));
        dialog.SetArtists(_controller.Localizer["Artists", "Credits"].Split(Environment.NewLine));
        dialog.SetTranslatorCredits((string.IsNullOrEmpty(_controller.Localizer["Translators", "Credits"]) ? "" : _controller.Localizer["Translators", "Credits"]));
        dialog.SetReleaseNotes(_controller.AppInfo.Changelog);
        dialog.Present();
    }
}
