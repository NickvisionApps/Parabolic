using Nickvision.Keyring.Controllers;
using Nickvision.Keyring.Models;
using NickvisionTubeConverter.GNOME.Controls;
using NickvisionTubeConverter.GNOME.Helpers;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using static NickvisionTubeConverter.Shared.Helpers.Gettext;

namespace NickvisionTubeConverter.GNOME.Views;

public partial class KeyringDialog : Adw.Window
{
    private readonly Gtk.Window _parent;
    private readonly KeyringDialogController _controller;
    private readonly Gtk.ShortcutController _shortcutController;
    private bool _handlingEnableToggle;
    private int? _editId;
    private readonly List<Gtk.Widget> _credentialRows;
    private readonly string _appID;
    
    [Gtk.Connect] private readonly Gtk.Button _backButton;
    [Gtk.Connect] private readonly Gtk.Label _titleLabel;
    [Gtk.Connect] private readonly Adw.ToastOverlay _toastOverlay;
    [Gtk.Connect] private readonly Adw.ViewStack _viewStack;
    [Gtk.Connect] private readonly Gtk.Button _enableKeyringButton;
    [Gtk.Connect] private readonly Adw.EntryRow _newPasswordEntry;
    [Gtk.Connect] private readonly Adw.EntryRow _confirmPasswordEntry;
    [Gtk.Connect] private readonly Gtk.Button _setPasswordButton;
    [Gtk.Connect] private readonly Gtk.Box _mainBox;
    [Gtk.Connect] private readonly Adw.PreferencesGroup _credentialsGroup;
    [Gtk.Connect] private readonly Gtk.Button _addCredentialButton;
    [Gtk.Connect] private readonly Gtk.Button _disableKeyringButton;
    [Gtk.Connect] private readonly Gtk.Button _confirmDisableKeyringButton;
    [Gtk.Connect] private readonly Adw.StatusPage _noCredentialsPage;
    [Gtk.Connect] private readonly Gtk.Spinner _loadingSpinner;
    [Gtk.Connect] private readonly Adw.EntryRow _nameRow;
    [Gtk.Connect] private readonly Adw.EntryRow _urlRow;
    [Gtk.Connect] private readonly Adw.EntryRow _usernameRow;
    [Gtk.Connect] private readonly Adw.PasswordEntryRow _passwordRow;
    [Gtk.Connect] private readonly Adw.ViewStack _buttonViewStack;
    [Gtk.Connect] private readonly Gtk.Button _credentialAddButton;
    [Gtk.Connect] private readonly Gtk.Button _credentialDeleteButton;
    [Gtk.Connect] private readonly Gtk.Button _credentialEditButton;

    /// <summary>
    /// Constructs a KeyringDialog
    /// </summary>
    /// <param name="builder">Gtk.Builder</param>
    /// <param name="controller">KeyringDialogController</param>
    /// <param name="parent">Gtk.Window</param>
    private KeyringDialog(Gtk.Builder builder, KeyringDialogController controller, string appID, Gtk.Window parent) : base(builder.GetPointer("_root"), false)
    {
        _parent = parent;
        _controller = controller;
        _handlingEnableToggle = false;
        _editId = null;
        _credentialRows = new List<Gtk.Widget>();
        _appID = appID;
        //Dialog Settings
        SetTransientFor(parent);
        SetIconName(_appID);
        //Build UI
        builder.Connect(this);
        _enableKeyringButton.OnClicked += (sender, e) =>
        {
            _titleLabel.SetVisible(true);
            _titleLabel.SetLabel(_("Set Password"));
            _viewStack.SetVisibleChildName("password");
        };
        _newPasswordEntry.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "text")
            {
                ValidateNewPassword();
            }
        };
        _confirmPasswordEntry.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "text")
            {
                ValidateNewPassword();
            }
        };
        _setPasswordButton.OnClicked += async (sender, e) =>
        {
            _controller.EnableKeyring(_newPasswordEntry.GetText());
            await LoadHomePageAsync();
        };
        _disableKeyringButton.OnClicked += (sender, e) =>
        {
            _backButton.SetVisible(true);
            _titleLabel.SetVisible(false);
            _viewStack.SetVisibleChildName("disable");
        };
        _confirmDisableKeyringButton.OnClicked += (sender, e) =>
        {
            _controller.DisableKeyring();
            Close();
        };
        _backButton.OnClicked += async (sender, e) => await LoadHomePageAsync();
        _addCredentialButton.OnClicked += (sender, e) => LoadAddCredentialPage();
        _credentialAddButton.OnClicked += AddCredential;
        _credentialDeleteButton.OnClicked += DeleteCredential;
        _credentialEditButton.OnClicked += EditCredential;
        //Shortcut Controller
        _shortcutController = Gtk.ShortcutController.New();
        _shortcutController.SetScope(Gtk.ShortcutScope.Managed);
        _shortcutController.AddShortcut(Gtk.Shortcut.New(Gtk.ShortcutTrigger.ParseString("Escape"), Gtk.CallbackAction.New(OnEscapeKey)));
        AddController(_shortcutController);
        //Load
        if(!_controller.IsValid)
        {
            _mainBox.SetSensitive(false);
            _toastOverlay.AddToast(Adw.Toast.New(_("Keyring has not been unlocked.")));
            _enableKeyringButton.SetVisible(false);
        }
    }
    
    /// <summary>
    /// Constructs a KeyringDialog
    /// </summary>
    /// <param name="controller">KeyringDialogController</param>
    /// <param name="parent">Gtk.Window</param>
    public KeyringDialog(KeyringDialogController controller, string appID, Gtk.Window parent) : this(Builder.FromFile("keyring_dialog.ui"), controller, appID, parent)
    {
    }
    
    public async Task PresentAsync()
    {
        base.Present();
        if (_controller.IsEnabled)
        {
            await LoadHomePageAsync();
        }
    }

    /// <summary>
    /// Occurs when the escape key is pressed on the window
    /// </summary>
    /// <param name="sender">Gtk.Widget</param>
    /// <param name="e">GLib.Variant</param>
    private bool OnEscapeKey(Gtk.Widget sender, GLib.Variant e)
    {
        Close();
        return true;
    }

    private void ValidateNewPassword()
    {
        _setPasswordButton.SetSensitive(false);
        if (_newPasswordEntry.GetText() == _confirmPasswordEntry.GetText() && !string.IsNullOrEmpty(_newPasswordEntry.GetText()))
        {
            _setPasswordButton.SetSensitive(true);
        }
    }

    /// <summary>
    /// Loads the Home page
    /// </summary>
    private async Task LoadHomePageAsync()
    {
        _editId = null;
        _viewStack.SetVisibleChildName("home");
        _backButton.SetVisible(false);
        _titleLabel.SetVisible(true);
        _titleLabel.SetLabel(_("Keyring"));
        SetDefaultWidget(null);
        //Update Rows
        foreach(var row in _credentialRows)
        {
            _credentialsGroup.Remove(row);
        }
        _credentialRows.Clear();
        _noCredentialsPage.SetVisible(false);
        _credentialsGroup.SetVisible(false);
        _loadingSpinner.SetVisible(true);
        var credentials = new List<Credential>();
        await Task.Run(async () => credentials = await _controller.GetAllCredentialsAsync());
        foreach(var credential in credentials)
        {
            var row = Adw.ActionRow.New();
            row.SetTitle(credential.Name);
            row.SetSubtitle(credential.Uri?.ToString() ?? "");
            var img = Gtk.Image.NewFromIconName("go-next-symbolic");
            img.SetValign(Gtk.Align.Center);
            row.AddSuffix(img);
            row.SetActivatableWidget(img);
            row.OnActivated += (sender, e) => LoadEditCredentialPage(credential);
            _credentialRows.Add(row);
        }
        if (_credentialRows.Count > 0)
        {
            foreach (var row in _credentialRows)
            {
                _credentialsGroup.Add(row);
            }
            _credentialsGroup.SetVisible(true);
        }
        _loadingSpinner.SetVisible(false);
        _noCredentialsPage.SetVisible(_credentialRows.Count == 0);
    }

    /// <summary>
    /// Loads the AddCredential page
    /// </summary>
    private void LoadAddCredentialPage()
    {
        _viewStack.SetVisibleChildName("credential");
        _backButton.SetVisible(true);
        _titleLabel.SetLabel(_("Credential"));
        _buttonViewStack.SetVisibleChildName("add");
        _nameRow.SetText("");
        _urlRow.SetText("");
        _usernameRow.SetText("");
        _passwordRow.SetText("");
        SetValidation(CredentialCheckStatus.Valid);
        SetDefaultWidget(_credentialAddButton);
    }

    /// <summary>
    /// Loads the EditCredential page
    /// </summary>
    /// <param name="credential">The Credential model</param>
    private void LoadEditCredentialPage(Credential credential)
    {
        _viewStack.SetVisibleChildName("credential");
        _backButton.SetVisible(true);
        _titleLabel.SetLabel(_("Credential"));
        _buttonViewStack.SetVisibleChildName("edit");
        _editId = credential.Id;
        _nameRow.SetText(credential.Name);
        _urlRow.SetText(credential.Uri?.ToString() ?? "");
        _usernameRow.SetText(credential.Username);
        _passwordRow.SetText(credential.Password);
        SetValidation(CredentialCheckStatus.Valid);
        SetDefaultWidget(_credentialEditButton);
    }

    /// <summary>
    /// Adapts the UI to the current validation status
    /// </summary>
    /// <param name="checkStatus">CredentialCheckStatus</param>
    private void SetValidation(CredentialCheckStatus checkStatus)
    {
        _nameRow.RemoveCssClass("error");
        _nameRow.SetTitle(_("Name"));
        _urlRow.RemoveCssClass("error");
        _urlRow.SetTitle(_("URL"));
        _usernameRow.RemoveCssClass("error");
        _usernameRow.SetTitle(_("Username"));
        _passwordRow.RemoveCssClass("error");
        _passwordRow.SetTitle(_("Password"));
        if(checkStatus.HasFlag(CredentialCheckStatus.EmptyName))
        {
            _nameRow.AddCssClass("error");
            _nameRow.SetTitle(_("Name (Empty)"));
        }
        if(checkStatus.HasFlag(CredentialCheckStatus.EmptyUsernamePassword))
        {
            _usernameRow.AddCssClass("error");
            _usernameRow.SetTitle(_("Username (Empty)"));
            _passwordRow.AddCssClass("error");
            _passwordRow.SetTitle(_("Password (Empty)"));
        }
        if(checkStatus.HasFlag(CredentialCheckStatus.InvalidUri))
        {
            _urlRow.AddCssClass("error");
            _urlRow.SetTitle(_("URL (Invalid)"));
        }
    }

    /// <summary>
    /// Occurs when the add button is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private async void AddCredential(Gtk.Button sender, EventArgs e)
    {
        var checkStatus = _controller.ValidateCredential(_nameRow.GetText(), _urlRow.GetText(), _usernameRow.GetText(), _passwordRow.GetText());
        SetValidation(checkStatus);
        if(checkStatus == CredentialCheckStatus.Valid)
        {
            await _controller.AddCredentialAsync(_nameRow.GetText(), _urlRow.GetText(), _usernameRow.GetText(), _passwordRow.GetText());
            await LoadHomePageAsync();
        }
    }

    /// <summary>
    /// Occurs when the apply button is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private async void EditCredential(Gtk.Button sender, EventArgs e)
    {
        var checkStatus = _controller.ValidateCredential(_nameRow.GetText(), _urlRow.GetText(), _usernameRow.GetText(), _passwordRow.GetText());
        SetValidation(checkStatus);
        if(checkStatus == CredentialCheckStatus.Valid)
        {
            await _controller.UpdateCredentialAsync(_editId!.Value, _nameRow.GetText(), _urlRow.GetText(), _usernameRow.GetText(), _passwordRow.GetText());
            await LoadHomePageAsync();
        }
    }

    /// <summary>
    /// Occurs when the delete button is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private void DeleteCredential(Gtk.Button sender, EventArgs e)
    {
        var disableDialog = new MessageDialog(this, _appID, _("Delete Credential?"), _("This action is irreversible. Are you sure you want to delete it?"), _("No"), _("Yes"));
        disableDialog.OnResponse += async (sender, e) =>
        {
            if(disableDialog.Response == MessageDialogResponse.Destructive)
            {
                await _controller.DeleteCredentialAsync(_editId!.Value);
                await LoadHomePageAsync();
            }
            disableDialog.Destroy();
        };
        disableDialog.Present();
    }
}