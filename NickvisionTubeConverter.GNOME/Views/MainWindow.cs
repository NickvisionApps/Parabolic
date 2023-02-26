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

    [Gtk.Connect] private readonly Gtk.Overlay _root;
    [Gtk.Connect] private readonly Adw.Bin _spinnerContainer;
    [Gtk.Connect] private readonly Gtk.Spinner _spinner;
    [Gtk.Connect] private readonly Gtk.Box _mainBox;
    [Gtk.Connect] private readonly Adw.ToastOverlay _toastOverlay;
    [Gtk.Connect] private readonly Adw.ViewStack _viewStack;
    [Gtk.Connect] private readonly Gtk.Button _addDownloadButton;
    [Gtk.Connect] private readonly Gtk.Box _downloadsBox;

    /// <summary>
    /// Constructs a MainWindow
    /// </summary>
    /// <param name="controller">The MainWindowController</param>
    /// <param name="application">The Adw.Application</param>
    public MainWindow(MainWindowController controller, Adw.Application application)
    {
        //Window Settings
        _controller = controller;
        _application = application;
        SetDefaultSize(800, 600);
        SetSizeRequest(360, -1);
        SetTitle(_controller.AppInfo.ShortName);
        if (_controller.IsDevVersion)
        {
            AddCssClass("devel");
        }
        OnCloseRequest += OnCloseRequested;
        //Build UI
        var builder = Builder.FromFile("window.ui", _controller.Localizer);
        builder.Connect(this);
        SetContent(_root);
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
        _controller.StopDownloads();
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
        _addDownloadButton.SetVisible(true);
        var downloadRow = new DownloadRow(_controller.Localizer, download);
        _viewStack.SetVisibleChildName("pageDownloads");
        if (_downloadsBox.GetFirstChild() != null)
        {
            var separator = Gtk.Separator.New(Gtk.Orientation.Horizontal);
            _downloadsBox.Append(separator);
        }
        _downloadsBox.Append(downloadRow);
        return downloadRow;
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
        var shortcutsDialog = new ShortcutsDialog(_controller.Localizer, _controller.AppInfo.ShortName, this);
        shortcutsDialog.Show();
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
