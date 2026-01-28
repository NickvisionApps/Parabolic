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

    public KeyringDialog(KeyringViewController controller, Gtk.Window parent) : this(controller, parent, Gtk.Builder.NewFromBlueprint("KeyringDialog", controller.Translator))
    {

    }

    public KeyringDialog(KeyringViewController controller, Gtk.Window parent, Gtk.Builder builder) : base(new Adw.Internal.PreferencesDialogHandle(builder.GetPointer("root"), false))
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

    }

    private void Edit(Credential credential)
    {

    }

    private void Remove(Credential credential)
    {
        var dialog = Adw.AlertDialog.New(_controller.Translator._("Delete Credential?"), _controller.Translator._("Are you sure you want to delete this credential? The action is irreversible"));
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
