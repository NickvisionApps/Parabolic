using Nickvision.Keyring;
using NickvisionTubeConverter.GNOME.Controls;
using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controllers;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using static NickvisionTubeConverter.Shared.Helpers.Gettext;

namespace NickvisionTubeConverter.GNOME.Views;

public class KeyringDialog : Adw.Window
{
    private readonly Gtk.Window _parent;
    private readonly KeyringDialogController _controller;
    private readonly Gtk.ShortcutController _shortcutController;
    private bool _handlingEnableToggle;
    private int? _editId;
    private readonly List<Gtk.Widget> _credentialRows;
    
    [Gtk.Connect] private readonly Gtk.Button _backButton;
    [Gtk.Connect] private readonly Gtk.Label _titleLabel;
    [Gtk.Connect] private readonly Adw.ToastOverlay _toastOverlay;
    [Gtk.Connect] private readonly Adw.ViewStack _viewStack;
    [Gtk.Connect] private readonly Gtk.Box _mainBox;
    [Gtk.Connect] private readonly Adw.ActionRow _enableKeyringRow;
    [Gtk.Connect] private readonly Gtk.Switch _enableKeyringSwitch;
    [Gtk.Connect] private readonly Adw.PreferencesGroup _credentialsGroup;
    [Gtk.Connect] private readonly Gtk.Button _addCredentialButton;
    [Gtk.Connect] private readonly Adw.StatusPage _noCredentialsPage;
    [Gtk.Connect] private readonly Adw.EntryRow _nameRow;
    [Gtk.Connect] private readonly Adw.EntryRow _urlRow;
    [Gtk.Connect] private readonly Adw.EntryRow _usernameRow;
    [Gtk.Connect] private readonly Adw.PasswordEntryRow _passwordRow;
    [Gtk.Connect] private readonly Gtk.Button _credentialDeleteButton;
    [Gtk.Connect] private readonly Gtk.Button _credentialActionButton;

    /// <summary>
    /// Constructs a KeyringDialog
    /// </summary>
    /// <param name="builder">Gtk.Builder</param>
    /// <param name="controller">KeyringDialogController</param>
    /// <param name="parent">Gtk.Window</param>
    private KeyringDialog(Gtk.Builder builder, KeyringDialogController controller, Gtk.Window parent) : base(builder.GetPointer("_root"), false)
    {
        _parent = parent;
        _controller = controller;
        _handlingEnableToggle = false;
        _editId = null;
        _credentialRows = new List<Gtk.Widget>();
        //Dialog Settings
        SetTransientFor(parent);
        SetIconName(_controller.AppInfo.ID);
        //Build UI
        builder.Connect(this);
        _backButton.OnClicked += async (sender, e) => await LoadHomePageAsync();
        _addCredentialButton.OnClicked += (sender, e) => LoadAddCredentialPage();
        _credentialDeleteButton.OnClicked += DeleteAction;
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
        }
        else
        {
            _enableKeyringSwitch.SetActive(_controller.IsEnabled);
            _enableKeyringSwitch.OnNotify += async (sender, e) =>
            {
                if(e.Pspec.GetName() == "active")
                {
                    await ToggleEnableAsync();
                }
            };
        }
    }
    
    /// <summary>
    /// Constructs a KeyringDialog
    /// </summary>
    /// <param name="controller">KeyringDialogController</param>
    /// <param name="parent">Gtk.Window</param>
    public KeyringDialog(KeyringDialogController controller, Gtk.Window parent) : this(Builder.FromFile("keyring_dialog.ui"), controller, parent)
    {
    }
    
    public async Task PresentAsync()
    {
        base.Present();
        await LoadHomePageAsync();
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

    /// <summary>
    /// Occurs when the enable switch is toggled
    /// </summary>
    private async Task ToggleEnableAsync()
    {
        if(!_handlingEnableToggle)
        {
            _handlingEnableToggle = true;
            if(_enableKeyringSwitch.GetActive())
            {
                var tcs = new TaskCompletionSource<string?>();
                var newPasswordDialog = new NewPasswordDialog(this, _("Setup Keyring"), tcs, _("Enable"));
                newPasswordDialog.Present();
                var password = await tcs.Task;
                if(string.IsNullOrEmpty(password))
                {
                    _handlingEnableToggle = true;
                    _enableKeyringSwitch.SetActive(false);
                    _handlingEnableToggle = false;
                }
                else
                {
                    _controller.EnableKeyring(password);
                }
            }
            else
            {
                var disableDialog = new MessageDialog(this, _controller.AppInfo.ID, _("Disable Keyring?"), _("Disabling the keyring will delete all data currently stored inside. Are you sure you want to disable it?"), _("No"), _("Yes"));
                disableDialog.OnResponse += (sender, e) =>
                {
                    if(disableDialog.Response == MessageDialogResponse.Destructive)
                    {
                        _controller.DisableKeyring();
                    }
                    else
                    {
                        _handlingEnableToggle = true;
                        _enableKeyringSwitch.SetActive(true);
                        _handlingEnableToggle = false;
                    }
                    disableDialog.Destroy();
                };
                disableDialog.Present();
            }
            _handlingEnableToggle = false;
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
        _titleLabel.SetLabel(_("Keyring"));
        //Update Rows
        foreach(var row in _credentialRows)
        {
            _credentialsGroup.Remove(row);
        }
        _credentialRows.Clear();
        var credentials = await _controller.GetAllCredentialsAsync();
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
            _credentialsGroup.Add(row);
            _credentialRows.Add(row);
        }
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
        _credentialDeleteButton.SetVisible(false);
        _credentialActionButton.SetLabel("Add");
        _nameRow.SetText("");
        _urlRow.SetText("");
        _usernameRow.SetText("");
        _passwordRow.SetText("");
        _credentialActionButton.OnClicked += AddAction;
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
        _credentialDeleteButton.SetVisible(true);
        _credentialActionButton.SetLabel("Apply");
        _editId = credential.Id;
        _nameRow.SetText(credential.Name);
        _urlRow.SetText(credential.Uri?.ToString() ?? "");
        _usernameRow.SetText(credential.Username);
        _passwordRow.SetText(credential.Password);
        _credentialActionButton.OnClicked += EditAction;
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
    private async void AddAction(Gtk.Button sender, EventArgs e)
    {
        var checkStatus = _controller.ValidateCredential(_nameRow.GetText(), _urlRow.GetText(), _usernameRow.GetText(), _passwordRow.GetText());
        SetValidation(checkStatus);
        if(checkStatus == CredentialCheckStatus.Valid)
        {
            _credentialActionButton.OnClicked -= AddAction;
            await _controller.AddCredentialAsync(_nameRow.GetText(), _urlRow.GetText(), _usernameRow.GetText(), _passwordRow.GetText());
            await LoadHomePageAsync();
        }
    }

    /// <summary>
    /// Occurs when the apply button is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private async void EditAction(Gtk.Button sender, EventArgs e)
    {
        var checkStatus = _controller.ValidateCredential(_nameRow.GetText(), _urlRow.GetText(), _usernameRow.GetText(), _passwordRow.GetText());
        SetValidation(checkStatus);
        if(checkStatus == CredentialCheckStatus.Valid)
        {
            _credentialActionButton.OnClicked -= EditAction;
            await _controller.UpdateCredentialAsync(_editId!.Value, _nameRow.GetText(), _urlRow.GetText(), _usernameRow.GetText(), _passwordRow.GetText());
            await LoadHomePageAsync();
        }
    }

    /// <summary>
    /// Occurs when the delete button is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private void DeleteAction(Gtk.Button sender, EventArgs e)
    {
        var disableDialog = new MessageDialog(this, _controller.AppInfo.ID, _("Delete Credential?"), _("This action is irreversible. Are you sure you want to delete it?"), _("No"), _("Yes"));
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