using NickvisionTubeConverter.GNOME.Controls;
using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Events;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// The MainWindow for the application
/// </summary>
public partial class MainWindow : Adw.ApplicationWindow
{
    private readonly MainWindowController _controller;
    private readonly Adw.Application _application;
    private Dictionary<IDownloadRowControl, Gtk.Separator> _downloadingSeparators;
    private Dictionary<IDownloadRowControl, Gtk.Separator> _completedSeparators;
    private Dictionary<IDownloadRowControl, Gtk.Separator> _queuedSeparators;

    [Gtk.Connect] private readonly Adw.Bin _spinnerContainer;
    [Gtk.Connect] private readonly Gtk.Spinner _spinner;
    [Gtk.Connect] private readonly Gtk.Box _mainBox;
    [Gtk.Connect] private readonly Adw.ToastOverlay _toastOverlay;
    [Gtk.Connect] private readonly Adw.ViewStack _viewStack;
    [Gtk.Connect] private readonly Gtk.Button _addDownloadButton;
    [Gtk.Connect] private readonly Gtk.Box _sectionDownloading;
    [Gtk.Connect] private readonly Gtk.Box _downloadingBox;
    [Gtk.Connect] private readonly Gtk.Box _sectionCompleted;
    [Gtk.Connect] private readonly Gtk.Box _completedBox;
    [Gtk.Connect] private readonly Gtk.Box _sectionQueued;
    [Gtk.Connect] private readonly Gtk.Box _queuedBox;

    private MainWindow(Gtk.Builder builder, MainWindowController controller, Adw.Application application) : base(builder.GetPointer("_root"), false)
    {
        //Window Settings
        _controller = controller;
        _application = application;
        _downloadingSeparators = new Dictionary<IDownloadRowControl, Gtk.Separator>();
        _completedSeparators = new Dictionary<IDownloadRowControl, Gtk.Separator>();
        _queuedSeparators = new Dictionary<IDownloadRowControl, Gtk.Separator>();
        SetTitle(_controller.AppInfo.ShortName);
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
        _controller.UIDeleteDownloadRowFromQueue = DeleteDownloadRowFromQueue;
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
        Show();
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
        var downloadRow = new DownloadRow(_controller.Localizer, download);
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
        _downloadingBox.Remove((DownloadRow)row);
        if (_downloadingSeparators.ContainsKey(row))
        {
            _downloadingBox.Remove(_downloadingSeparators[row]);
            _downloadingSeparators.Remove(row);
        }
        _completedBox.Remove((DownloadRow)row);
        if (_completedSeparators.ContainsKey(row))
        {
            _completedBox.Remove(_completedSeparators[row]);
            _completedSeparators.Remove(row);
        }
        _queuedBox.Remove((DownloadRow)row);
        if (_queuedSeparators.ContainsKey(row))
        {
            _queuedBox.Remove(_queuedSeparators[row]);
            _queuedSeparators.Remove(row);
        }
        if (stage == DownloadStage.InQueue)
        {
            if (_queuedBox.GetFirstChild() != null)
            {
                var separator = Gtk.Separator.New(Gtk.Orientation.Horizontal);
                _queuedBox.Append(separator);
                _queuedSeparators.Add(row, separator);
            }
            _queuedBox.Append((DownloadRow)row);
        }
        else if (stage == DownloadStage.Downloading)
        {
            if (_downloadingBox.GetFirstChild() != null)
            {
                var separator = Gtk.Separator.New(Gtk.Orientation.Horizontal);
                _downloadingBox.Append(separator);
                _downloadingSeparators.Add(row, separator);
            }
            _downloadingBox.Append((DownloadRow)row);
        }
        else if (stage == DownloadStage.Completed)
        {
            if (_completedBox.GetFirstChild() != null)
            {
                var separator = Gtk.Separator.New(Gtk.Orientation.Horizontal);
                _completedBox.Append(separator);
                _completedSeparators.Add(row, separator);
            }
            _completedBox.Append((DownloadRow)row);
        }
        _sectionDownloading.SetVisible(_downloadingBox.GetFirstChild() != null);
        _sectionCompleted.SetVisible(_completedBox.GetFirstChild() != null);
        _sectionQueued.SetVisible(_queuedBox.GetFirstChild() != null);
    }

    /// <summary>
    /// Deletes a download row from the queue section
    /// </summary>
    /// <param name="row">IDownloadRowControl</param>
    private void DeleteDownloadRowFromQueue(IDownloadRowControl row)
    {
        _queuedBox.Remove((DownloadRow)row);
        if (_queuedSeparators.ContainsKey(row))
        {
            _queuedBox.Remove(_queuedSeparators[row]);
            _queuedSeparators.Remove(row);
        }
        _sectionQueued.SetVisible(_queuedBox.GetFirstChild() != null);
    }

    /// <summary>
    /// Occurs when the add download action is triggered
    /// </summary>
    /// <param name="sender">Gio.SimpleAction</param>
    /// <param name="e">EventArgs</param>
    private async void AddDownload(Gio.SimpleAction sender, EventArgs e)
    {
        var addController = _controller.CreateAddDownloadDialogController();
        var addDialog = new AddDownloadDialog(addController, this);
        await addDialog.ShowAsync();
        addDialog.OnResponse += async (sender, e) =>
        {
            if (addController.Accepted)
            {
                await _controller.AddDownloadAsync(addController.Download!);
            }
            addDialog.Destroy();
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
        preferencesDialog.Show();
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
        shortcutsWindow.Show();
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
        dialog.Show();
    }
}
