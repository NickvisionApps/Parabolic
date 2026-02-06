using Nickvision.Desktop.GNOME.Helpers;
using Nickvision.Desktop.Keyring;
using Nickvision.Parabolic.GNOME.Helpers;
using Nickvision.Parabolic.Shared.Controllers;
using System;
using System.Collections.Generic;
using System.Collections.Specialized;

namespace Nickvision.Parabolic.GNOME.Views;

public class KeyringDialog : Adw.PreferencesDialog
{
    private readonly KeyringViewController _controller;
    private readonly Gtk.Builder _builder;
    private readonly List<Adw.ActionRow> _credentialRows;
    private EditMode _credentialEditMode;

    [Gtk.Connect("credentialsGroup")]
    private Adw.PreferencesGroup? _credentialsGroup;
    [Gtk.Connect("addButton")]
    private Gtk.Button? _addButton;
    [Gtk.Connect("editCredentialDialog")]
    private Adw.Dialog? _editCredentialDialog;
    [Gtk.Connect("editCredentialNameRow")]
    private Adw.EntryRow? _editCredentialNameRow;
    [Gtk.Connect("editCredentialUrlRow")]
    private Adw.EntryRow? _editCredentialUrlRow;
    [Gtk.Connect("editCredentialUsernameRow")]
    private Adw.EntryRow? _editCredentialUsernameRow;
    [Gtk.Connect("editCredentialPasswordRow")]
    private Adw.PasswordEntryRow? _editCredentialPasswordRow;
    [Gtk.Connect("editConfirmCredentialButton")]
    private Gtk.Button? _editConfirmCredentialButton;

    public KeyringDialog(KeyringViewController controller) : this(controller, Gtk.Builder.NewFromBlueprint("KeyringDialog", controller.Translator))
    {

    }

    public KeyringDialog(KeyringViewController controller, Gtk.Builder builder) : base(new Adw.Internal.PreferencesDialogHandle(builder.GetPointer("root"), false))
    {
        _controller = controller;
        _builder = builder;
        _credentialRows = new List<Adw.ActionRow>();
        _credentialEditMode = EditMode.None;
        _builder.Connect(this);
        // Load
        Credentials_CollectionChanged(null, null);
        // Events
        _controller.Credentials.CollectionChanged += Credentials_CollectionChanged;
        _addButton!.OnClicked += AddButton_OnClicked;
        _editConfirmCredentialButton!.OnClicked += EditConfirmCredentialButton_OnClicked;
    }

    private void Credentials_CollectionChanged(object? sender, NotifyCollectionChangedEventArgs e)
    {
        foreach (var row in _credentialRows)
        {
            _credentialsGroup!.Remove(row);
        }
        _credentialRows.Clear();
        foreach (var credential in _controller.Credentials)
        {
            var editButton = Gtk.Button.NewFromIconName("document-edit-symbolic");
            editButton.Valign = Gtk.Align.Center;
            editButton.TooltipText = _controller.Translator._("Edit");
            editButton.AddCssClass("flat");
            editButton.OnClicked += (_, _) => Edit(credential.Value);
            var deleteButton = Gtk.Button.NewFromIconName("user-trash-symbolic");
            deleteButton.Valign = Gtk.Align.Center;
            deleteButton.TooltipText = _controller.Translator._("Delete");
            deleteButton.AddCssClass("flat");
            deleteButton.OnClicked += (_, _) => Remove(credential.Value);
            var row = Adw.ActionRow.New();
            row.Title = credential.Label;
            row.Subtitle = credential.Value.Url.ToString();
            row.AddSuffix(editButton);
            row.AddSuffix(deleteButton);
            row.ActivatableWidget = editButton;
            _credentialRows.Add(row);
            _credentialsGroup!.Add(row);
        }
        if (_controller.Credentials.Count == 0)
        {
            var row = Adw.ActionRow.New();
            row.Title = _controller.Translator._("No Credentials");
            _credentialRows.Add(row);
            _credentialsGroup!.Add(row);
        }
    }

    private void AddButton_OnClicked(Gtk.Button sender, EventArgs e)
    {
        _credentialEditMode = EditMode.Add;
        _editCredentialNameRow!.Text_ = string.Empty;
        _editCredentialNameRow.Sensitive = true;
        _editCredentialUrlRow!.Text_ = string.Empty;
        _editCredentialUsernameRow!.Text_ = string.Empty;
        _editCredentialPasswordRow!.Text_ = string.Empty;
        _editConfirmCredentialButton!.Label = _controller.Translator._("Add");
        _editCredentialDialog!.Present(this);
    }

    private async void EditConfirmCredentialButton_OnClicked(Gtk.Button sender, EventArgs e)
    {
        string? error = null;
        switch (_credentialEditMode)
        {
            case EditMode.Add:
                error = await _controller.AddAsync(
                    _editCredentialNameRow!.Text_ ?? string.Empty,
                    _editCredentialUrlRow!.Text_ ?? string.Empty,
                    _editCredentialUsernameRow!.Text_ ?? string.Empty,
                    _editCredentialPasswordRow!.Text_ ?? string.Empty);
                break;
            case EditMode.Edit:
                error = await _controller.UpdateAsync(
                    _editCredentialNameRow!.Text_ ?? string.Empty,
                    _editCredentialUrlRow!.Text_ ?? string.Empty,
                    _editCredentialUsernameRow!.Text_ ?? string.Empty,
                    _editCredentialPasswordRow!.Text_ ?? string.Empty);
                break;
            default:
                break;
        }
        if (error is not null)
        {
            var alert = Adw.AlertDialog.New(_controller.Translator._("Error"), error);
            alert.AddResponse("ok", _controller.Translator._("OK"));
            alert.SetDefaultResponse("ok");
            alert.Present(this);
        }
        else
        {
            _editCredentialDialog!.ForceClose();
        }
    }

    private void Edit(Credential credential)
    {
        _credentialEditMode = EditMode.Edit;
        _editCredentialNameRow!.Text_ = credential.Name;
        _editCredentialNameRow.Sensitive = true;
        _editCredentialUrlRow!.Text_ = credential.Url.ToString();
        _editCredentialUsernameRow!.Text_ = credential.Username;
        _editCredentialPasswordRow!.Text_ = credential.Password;
        _editConfirmCredentialButton!.Label = _controller.Translator._("Edit");
        _editCredentialDialog!.Present(this);
    }

    private void Remove(Credential credential)
    {
        var dialog = Adw.AlertDialog.New(_controller.Translator._("Delete Credential?"), _controller.Translator._("Are you sure you want to delete this credential? This action is irreversible"));
        dialog.AddResponse("delete", _controller.Translator._("Delete"));
        dialog.AddResponse("cancel", _controller.Translator._("Cancel"));
        dialog.SetResponseAppearance("delete", Adw.ResponseAppearance.Destructive);
        dialog.SetDefaultResponse("cancel");
        dialog.SetCloseResponse("cancel");
        dialog.OnResponse += async (_, e) =>
        {
            if (e.Response == "delete")
            {
                await _controller.RemoveAsync(credential);
            }
        };
        dialog.Present(this);
    }
}
