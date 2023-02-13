using NickvisionTubeConverter.GNOME.Controls;
﻿using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Controls;
using NickvisionTubeConverter.Shared.Events;
using NickvisionTubeConverter.Shared.Models;
using System;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// The MainWindow for the application
/// </summary>
public partial class MainWindow
{
    private readonly MainWindowController _controller;
    private readonly Adw.Application _application;
    private readonly Gtk.Box _mainBox;
    private readonly Adw.HeaderBar _headerBar;
    private readonly Gtk.Button _btnAddDownload;
    private readonly Adw.ButtonContent _btnAddDownloadContent;
    private readonly Adw.WindowTitle _windowTitle;
    private readonly Gtk.MenuButton _btnMenuHelp;
    private readonly Adw.ToastOverlay _toastOverlay;
    private readonly Gtk.ScrolledWindow _scrollStartPage;
    private readonly Adw.Clamp _clampStartPage;
    private readonly Adw.Clamp _clampDownloadsPage;
    private readonly Gtk.Box _boxStartPage;
    private readonly Adw.ButtonContent _greetingStartPage;
    private readonly Gtk.Button _btnAddDownloadStartPage;
    private readonly Gtk.Label _lblStart;
    private readonly Gtk.ScrolledWindow _scrollDownloadsPage;
    private readonly Gtk.Box _boxMainContent;
    private readonly Gtk.Label _lblDownloads;
    private readonly Gtk.Box _boxDownloads;
    private readonly Adw.ViewStack _viewStack;

    public Adw.ApplicationWindow Handle { get; init; }

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
        Handle = Adw.ApplicationWindow.New(_application);
        Handle.SetDefaultSize(800, 600);
        Handle.SetSizeRequest(360, -1);
        Handle.SetTitle(_controller.AppInfo.ShortName);
        if (_controller.IsDevVersion)
        {
            Handle.AddCssClass("devel");
        }
        Handle.OnCloseRequest += OnCloseRequested;
        //Main Box
        _mainBox = Gtk.Box.New(Gtk.Orientation.Vertical, 0);
        //Header Bar
        _headerBar = Adw.HeaderBar.New();
        _windowTitle = Adw.WindowTitle.New(_controller.AppInfo.ShortName, null);
        _headerBar.SetTitleWidget(_windowTitle);
        _mainBox.Append(_headerBar);
        //Add Download Button
        _btnAddDownload = Gtk.Button.New();
        _btnAddDownloadContent = Adw.ButtonContent.New();
        _btnAddDownloadContent.SetIconName("list-add-symbolic");
        _btnAddDownloadContent.SetLabel(_controller.Localizer["Add"]);
        _btnAddDownload.SetChild(_btnAddDownloadContent);
        _btnAddDownload.SetTooltipText(_controller.Localizer["AddDownload", "Tooltip"]);
        _btnAddDownload.SetDetailedActionName("win.addDownload");
        _headerBar.PackStart(_btnAddDownload);
        //Menu Help Button
        _btnMenuHelp = Gtk.MenuButton.New();
        var menuHelp = Gio.Menu.New();
        menuHelp.Append(_controller.Localizer["Preferences"], "win.preferences");
        menuHelp.Append(_controller.Localizer["KeyboardShortcuts"], "win.keyboardShortcuts");
        menuHelp.Append(string.Format(_controller.Localizer["About"], _controller.AppInfo.ShortName), "win.about");
        _btnMenuHelp.SetDirection(Gtk.ArrowType.None);
        _btnMenuHelp.SetMenuModel(menuHelp);
        _btnMenuHelp.SetTooltipText(_controller.Localizer["MainMenu", "GTK"]);
        _headerBar.PackEnd(_btnMenuHelp);
        //Toast Overlay
        _toastOverlay = Adw.ToastOverlay.New();
        _toastOverlay.SetHexpand(true);
        _toastOverlay.SetVexpand(true);
        _mainBox.Append(_toastOverlay);
        //Greeting
        _greetingStartPage = Adw.ButtonContent.New();
        _greetingStartPage.SetIconName(_controller.ShowSun ? "sun-alt-symbolic" : "moon-symbolic");
        _greetingStartPage.SetLabel(_controller.Greeting);
        _greetingStartPage.AddCssClass("title-2");
        var image = (Gtk.Image)_greetingStartPage.GetFirstChild();
        image.SetIconSize(Gtk.IconSize.Large);
        _greetingStartPage.SetHalign(Gtk.Align.Center);
        _greetingStartPage.SetMarginBottom(32);
        //Add Download Button Start Page 
        _btnAddDownloadStartPage = Gtk.Button.NewWithLabel(_controller.Localizer["AddDownload"]);
        _btnAddDownloadStartPage.SetHalign(Gtk.Align.Center);
        _btnAddDownloadStartPage.SetSizeRequest(200, 50);
        _btnAddDownloadStartPage.AddCssClass("pill");
        _btnAddDownloadStartPage.AddCssClass("suggested-action");
        _btnAddDownloadStartPage.SetDetailedActionName("win.addDownload");
        //Start Label
        _lblStart = Gtk.Label.New(_controller.Localizer["NoDownloads", "Description"]);
        _lblStart.AddCssClass("dim-label");
        _lblStart.SetWrap(true);
        _lblStart.SetJustify(Gtk.Justification.Center);
        //Start Page
        _scrollStartPage = Gtk.ScrolledWindow.New();
        _clampStartPage = Adw.Clamp.New();
        _clampStartPage.SetMaximumSize(450);
        _clampStartPage.SetValign(Gtk.Align.Center);
        _clampStartPage.SetMarginStart(12);
        _clampStartPage.SetMarginEnd(12);
        _clampStartPage.SetMarginTop(12);
        _clampStartPage.SetMarginBottom(12);
        _scrollStartPage.SetChild(_clampStartPage);
        _boxStartPage = Gtk.Box.New(Gtk.Orientation.Vertical, 12);
        _boxStartPage.SetHexpand(true);
        _boxStartPage.SetHalign(Gtk.Align.Fill);
        _boxStartPage.Append(_greetingStartPage);
        _boxStartPage.Append(_btnAddDownloadStartPage);
        _boxStartPage.Append(_lblStart);
        _clampStartPage.SetChild(_boxStartPage);
        //Downloads Page
        _scrollDownloadsPage = Gtk.ScrolledWindow.New();
        _clampDownloadsPage = Adw.Clamp.New();
        _clampDownloadsPage.SetMaximumSize(800);
        _clampDownloadsPage.SetMarginStart(12);
        _clampDownloadsPage.SetMarginEnd(12);
        _clampDownloadsPage.SetMarginTop(12);
        _clampDownloadsPage.SetMarginBottom(12);
        _scrollDownloadsPage.SetChild(_clampDownloadsPage);
        _boxMainContent = Gtk.Box.New(Gtk.Orientation.Vertical, 10);
        _boxMainContent.SetHexpand(true);
        _boxMainContent.SetValign(Gtk.Align.Start);
        _clampDownloadsPage.SetChild(_boxMainContent);
        _lblDownloads = Gtk.Label.New(_controller.Localizer["Downloads"]);
        _lblDownloads.SetHalign(Gtk.Align.Start);
        _lblDownloads.SetMarginStart(5);
        _lblDownloads.AddCssClass("heading");
        _boxMainContent.Append(_lblDownloads);
        _boxDownloads = Gtk.Box.New(Gtk.Orientation.Vertical, 0);
        _boxDownloads.SetHexpand(true);
        _boxDownloads.SetValign(Gtk.Align.Start);
        _boxDownloads.AddCssClass("card");
        _boxMainContent.Append(_boxDownloads);
        //View Stack
        _viewStack = Adw.ViewStack.New();
        _viewStack.AddNamed(_scrollStartPage, "pageNoDownloads");
        _viewStack.AddNamed(_scrollDownloadsPage, "pageDownloads");
        _toastOverlay.SetChild(_viewStack);
        //Layout
        Handle.SetContent(_mainBox);
        //Register Events 
        _controller.NotificationSent += NotificationSent;
        _controller.UICreateDownloadRow = CreateDownloadRow;
        //Add Download Action
        var actDownload = Gio.SimpleAction.New("addDownload", null);
        actDownload.OnActivate += AddDownload;
        Handle.AddAction(actDownload);
        application.SetAccelsForAction("win.addDownload", new string[] { "<Ctrl>n" });
        //Preferences Action
        var actPreferences = Gio.SimpleAction.New("preferences", null);
        actPreferences.OnActivate += Preferences;
        Handle.AddAction(actPreferences);
        application.SetAccelsForAction("win.preferences", new string[] { "<Ctrl>comma" });
        //Keyboard Shortcuts Action
        var actKeyboardShortcuts = Gio.SimpleAction.New("keyboardShortcuts", null);
        actKeyboardShortcuts.OnActivate += KeyboardShortcuts;
        Handle.AddAction(actKeyboardShortcuts);
        application.SetAccelsForAction("win.keyboardShortcuts", new string[] { "<Ctrl>question" });
        //Quit Action
        var actQuit = Gio.SimpleAction.New("quit", null);
        actQuit.OnActivate += Quit;
        Handle.AddAction(actQuit);
        application.SetAccelsForAction("win.quit", new string[] { "<Ctrl>q", "<Ctrl>w" });
        //Primary Menu Action
        var actPrimaryMenu = Gio.SimpleAction.New("primaryMenu", null);
        actPrimaryMenu.OnActivate += (sender, e) => _btnMenuHelp.Popup();
        Handle.AddAction(actPrimaryMenu);
        application.SetAccelsForAction("win.primaryMenu", new string[] { "F10" });
        //About Action
        var actAbout = Gio.SimpleAction.New("about", null);
        actAbout.OnActivate += About;
        Handle.AddAction(actAbout);
        application.SetAccelsForAction("win.about", new string[] { "F1" });
    }

    /// <summary>
    /// Starts the MainWindow
    /// </summary>
    public void Start()
    {
        _application.AddWindow(Handle);
        Handle.Show();
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
        if(_controller.AreDownloadsRunning)
        {
            var closeDialog = new MessageDialog(Handle, _controller.Localizer["CloseAndStop", "Title"], _controller.Localizer["CloseAndStop", "Description"], _controller.Localizer["No"], _controller.Localizer["Yes"]);
            if(closeDialog.Run() == MessageDialogResponse.Cancel)
            {
                return true;
            }
        }
        _controller.StopDownloads();
        return false;
    }

    /// <summary>
    /// Creates a download row and adds it to the view
    /// </summary>
    /// <param name="download"></param>
    /// <returns></returns>
    private IDownloadRowControl CreateDownloadRow(Download download)
    {
        var downloadRow = new DownloadRow(_controller.Localizer, download);
        _viewStack.SetVisibleChildName("pageDownloads");
        if (_boxDownloads.GetFirstChild() != null)
        {
            var separator = Gtk.Separator.New(Gtk.Orientation.Horizontal);
            _boxDownloads.Append(separator);
        }
        _boxDownloads.Append(downloadRow);
        return downloadRow;
    }

    /// <summary>
    /// Occurs when the add download action is triggered
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void AddDownload(Gio.SimpleAction sender, EventArgs e)
    {
        var addController = _controller.CreateAddDownloadDialogController();
        var addDialog = new AddDownloadDialog(addController, Handle);
        addDialog.Show();
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
        var preferencesDialog = new PreferencesDialog(_controller.PreferencesViewController, _application, Handle);
        preferencesDialog.Show();
    }

    /// <summary>
    /// Occurs when the keyboard shortcuts action is triggered
    /// </summary>
    /// <param name="sender">Gio.SimpleAction</param>
    /// <param name="e">EventArgs</param>
    private void KeyboardShortcuts(Gio.SimpleAction sender, EventArgs e)
    {
        var shortcutsDialog = new ShortcutsDialog(_controller.Localizer, _controller.AppInfo.ShortName, Handle);
        shortcutsDialog.Show();
    }

    /// <summary>
    /// Occurs when quit action is triggered
    /// </summary>
    /// <param name="sender">Gio.SimpleAction</param>
    /// <param name="e">EventArgs</param>
    private void Quit(Gio.SimpleAction sender, EventArgs e) => _application.Quit();

    /// <summary>
    /// Occurs when the about action is triggered
    /// </summary>
    /// <param name="sender">Gio.SimpleAction</param>
    /// <param name="e">EventArgs</param>
    private void About(Gio.SimpleAction sender, EventArgs e)
    {
        var dialog = Adw.AboutWindow.New();
        dialog.SetTransientFor(Handle);
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
