using Nickvision.Aura.Keyring;
using NickvisionTubeConverter.GNOME.Helpers;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading.Tasks;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// A dialog for managing credentials
/// </summary>
public partial class KeyringDialog : Adw.Window
{
    private readonly Gtk.Window _parent;
    private readonly KeyringDialogController _controller;
    private int? _editId;
    private readonly List<Gtk.Widget> _credentialRows;
    private readonly string _appID;
    private readonly bool _snap;
    private readonly string? _snapCommand;

    [Gtk.Connect] private readonly Gtk.Button _backButton;
    [Gtk.Connect] private readonly Adw.ToastOverlay _toastOverlay;
    [Gtk.Connect] private readonly Adw.Banner _banner;
    [Gtk.Connect] private readonly Gtk.Label _titleLabel;
    [Gtk.Connect] private readonly Adw.ViewStack _viewStack;
    [Gtk.Connect] private readonly Adw.StatusPage _disabledPage;
    [Gtk.Connect] private readonly Gtk.Button _enableKeyringButton;
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

    /// <summary>
    /// Constructs a KeyringDialog
    /// </summary>
    /// <param name="builder">Gtk.Builder</param>
    /// <param name="controller">KeyringDialogController</param>
    /// <param name="appID">The application's id</param>
    /// <param name="parent">Gtk.Window</param>
    private KeyringDialog(Gtk.Builder builder, KeyringDialogController controller, string appID, Gtk.Window parent) : base(builder.GetPointer("_root"), false)
    {
        _parent = parent;
        _controller = controller;
        _editId = null;
        _credentialRows = new List<Gtk.Widget>();
        _appID = appID;
        _snap = !string.IsNullOrEmpty(Environment.GetEnvironmentVariable("SNAP"));
        _snapCommand = _snap ? "sudo snap connect tube-converter:password-manager-service" : null;
        //Dialog Settings
        SetTransientFor(_parent);
        SetIconName(_appID);
        //Build UI
        builder.Connect(this);
        _enableKeyringButton.OnClicked += async (sender, e) =>
        {
            if (_snap)
            {
                var res = await SnapSlotCheckAsync();
                if (!res)
                {
                    return;
                }
            }
            _enableKeyringButton.SetSensitive(false);
            var success = await _controller.EnableKeyringAsync();
            _enableKeyringButton.SetSensitive(true);
            if (success)
            {
                await LoadHomePageAsync();
            }
            else
            {
                _toastOverlay.AddToast(Adw.Toast.New(_("Failed to enable keyring.")));
            }
        };
        _disableKeyringButton.OnClicked += (sender, e) =>
        {
            _backButton.SetVisible(true);
            _titleLabel.SetVisible(false);
            _viewStack.SetVisibleChildName("disable");
        };
        _confirmDisableKeyringButton.OnClicked += async (sender, e) => await DisableKeyringAsync();
        _backButton.OnClicked += async (sender, e) => await LoadHomePageAsync();
        _addCredentialButton.OnClicked += (sender, e) => LoadAddCredentialPage();
        _credentialAddButton.OnClicked += AddCredential;
        _credentialDeleteButton.OnClicked += DeleteCredential;
        //Load
        if (!_controller.IsValid)
        {
            _mainBox.SetSensitive(false);
            _banner.SetTitle(_("Keyring locked"));
            _banner.SetButtonLabel(_("Reset"));
            _banner.OnButtonClicked += ResetKeyring;
            _banner.SetRevealed(true);
            _disabledPage.SetDescription($"{_disabledPage.GetDescription()}\n\n{_("Restart the app to unlock the keyring.")}");
            _enableKeyringButton.SetVisible(false);
        }
    }

    /// <summary>
    /// Constructs a KeyringDialog
    /// </summary>
    /// <param name="controller">KeyringDialogController</param>
    /// <param name="appID">The application's id</param>
    /// <param name="parent">Gtk.Window</param>
    public KeyringDialog(KeyringDialogController controller, string appID, Gtk.Window parent) : this(Builder.FromFile("keyring_dialog.ui"), controller, appID, parent)
    {
    }

    public async Task PresentAsync()
    {
        Present();
        if (_controller.IsEnabled)
        {
            await LoadHomePageAsync();
        }
    }

    /// <summary>
    /// Loads the Home page
    /// </summary>
    private async Task LoadHomePageAsync()
    {
        if (_editId != null)
        {
            var checkStatus = _controller.ValidateCredential(_nameRow.GetText(), _urlRow.GetText(), _usernameRow.GetText(), _passwordRow.GetText());
            SetValidation(checkStatus);
            if (checkStatus == CredentialCheckStatus.Valid)
            {
                await _controller.UpdateCredentialAsync(_editId.Value, _nameRow.GetText(), _urlRow.GetText(), _usernameRow.GetText(), _passwordRow.GetText());
            }
            else
            {
                return;
            }
        }
        _editId = null;
        _viewStack.SetVisibleChildName("home");
        _backButton.SetVisible(false);
        _titleLabel.SetVisible(true);
        _titleLabel.SetLabel(_("Keyring"));
        SetDefaultWidget(null);
        //Update Rows
        foreach (var row in _credentialRows)
        {
            _credentialsGroup.Remove(row);
        }
        _credentialRows.Clear();
        _noCredentialsPage.SetVisible(false);
        _credentialsGroup.SetVisible(false);
        _loadingSpinner.SetVisible(true);
        List<Credential>? credentials = null;
        await Task.Run(async () => credentials = await _controller.GetAllCredentialsAsync());
        foreach (var credential in credentials!)
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
        if (_credentialRows.Count > 0)
        {
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
        _titleLabel.SetLabel(_("Login"));
        _buttonViewStack.SetVisibleChildName("add");
        _editId = null;
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
        _titleLabel.SetLabel(_("Login"));
        _buttonViewStack.SetVisibleChildName("edit");
        _editId = credential.Id;
        _nameRow.SetText(credential.Name);
        _urlRow.SetText(credential.Uri?.ToString() ?? "");
        _usernameRow.SetText(credential.Username);
        _passwordRow.SetText(credential.Password);
        SetValidation(CredentialCheckStatus.Valid);
        SetDefaultWidget(_backButton);
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
        if (checkStatus.HasFlag(CredentialCheckStatus.EmptyName))
        {
            _nameRow.AddCssClass("error");
            _nameRow.SetTitle(_("Name (Empty)"));
        }
        if (checkStatus.HasFlag(CredentialCheckStatus.EmptyUsernamePassword))
        {
            _usernameRow.AddCssClass("error");
            _usernameRow.SetTitle(_("User Name (Empty)"));
            _passwordRow.AddCssClass("error");
            _passwordRow.SetTitle(_("Password (Empty)"));
        }
        if (checkStatus.HasFlag(CredentialCheckStatus.InvalidUri))
        {
            _urlRow.AddCssClass("error");
            _urlRow.SetTitle(_("URL (Invalid)"));
        }
    }

    /// <summary>
    /// Occurs when the reset keyring button is clicked
    /// </summary>
    /// <param name="sender">Adw.Banner</param>
    /// <param name="e">EventArgs</param>
    private void ResetKeyring(Adw.Banner sender, EventArgs e)
    {
        var closeDialog = Adw.MessageDialog.New(this, _("Reset Keyring?"), _("This will delete the previous keyring removing all passwords with it. This action is irreversible."));
        closeDialog.SetIconName(_appID);
        closeDialog.AddResponse("no", _("No"));
        closeDialog.SetDefaultResponse("no");
        closeDialog.SetCloseResponse("no");
        closeDialog.AddResponse("yes", _("Yes"));
        closeDialog.SetResponseAppearance("yes", Adw.ResponseAppearance.Destructive);
        closeDialog.OnResponse += async (s, ex) =>
        {
            if (ex.Response == "yes")
            {
                if (await _controller.ResetKeyringAsync())
                {
                    Close();
                }
                else
                {
                    _toastOverlay.AddToast(Adw.Toast.New(_("Unable to reset keyring.")));
                }
            }
            closeDialog.Destroy();
        };
        closeDialog.Present();
    }

    /// <summary>
    /// Occurs when the disable keyring button is clicked
    /// </summary>
    private async Task DisableKeyringAsync()
    {
        if (await _controller.DisableKeyringAsync())
        {
            Close();
        }
        else
        {
            _toastOverlay.AddToast(Adw.Toast.New(_("Unable to disable keyring.")));
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
        if (checkStatus == CredentialCheckStatus.Valid)
        {
            await _controller.AddCredentialAsync(_nameRow.GetText(), _urlRow.GetText(), _usernameRow.GetText(), _passwordRow.GetText());
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
        var disableDialog = Adw.MessageDialog.New(this, _("Delete Credential?"), _("This action is irreversible. Are you sure you want to delete it?"));
        disableDialog.SetIconName(_appID);
        disableDialog.AddResponse("no", _("No"));
        disableDialog.SetDefaultResponse("no");
        disableDialog.SetCloseResponse("no");
        disableDialog.AddResponse("yes", _("Yes"));
        disableDialog.SetResponseAppearance("yes", Adw.ResponseAppearance.Destructive);
        disableDialog.OnResponse += async (s, ex) =>
        {
            if (ex.Response == "yes")
            {
                await _controller.DeleteCredentialAsync(_editId!.Value);
                await LoadHomePageAsync();
            }
            disableDialog.Destroy();
        };
        disableDialog.Present();
    }

    /// <summary>
    /// Occurs when the enable button is clicked under a snap
    /// </summary>
    /// <returns>True if slot is connected and False if not</returns>
    public async Task<bool> SnapSlotCheckAsync()
    {
        var process = new Process();
        process.StartInfo.FileName = "snapctl";
        process.StartInfo.Arguments = "is-connected password-manager-service";
        process.StartInfo.CreateNoWindow = true;
        process.Start();
        await process.WaitForExitAsync();
        int connected = process.ExitCode;
        if (connected == 1)
        {
            var snapDialog = Adw.MessageDialog.New(this, _("Necessary slot not connected"), _("To enable keyring, the app needs to connect to the password-manager-service slot. To connect it please copy the command and run in the terminal."));
            snapDialog.SetIconName(_appID);
            snapDialog.AddResponse("copy", _("Copy"));
            snapDialog.SetDefaultResponse("copy");
            snapDialog.AddResponse("close", _("Close"));
            snapDialog.SetCloseResponse("close");
            snapDialog.SetResponseAppearance("close", Adw.ResponseAppearance.Destructive);
            snapDialog.Show();
            snapDialog.OnResponse += (s, ex) =>
            {
                if (ex.Response == "copy")
                {
                    GetClipboard().SetText(_snapCommand);
                    _toastOverlay.AddToast(Adw.Toast.New(_("Command copied to clipboard.")));

                }
                snapDialog.Destroy();
            };
            return false;
        }
        return true;
    }
}