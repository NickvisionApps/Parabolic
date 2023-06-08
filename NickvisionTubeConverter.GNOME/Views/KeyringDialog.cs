using NickvisionTubeConverter.GNOME.Controls;
using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controllers;
using System;
using static NickvisionTubeConverter.Shared.Helpers.Gettext;

namespace NickvisionTubeConverter.GNOME.Views;

public class KeyringDialog : Adw.Window
{
    private readonly Gtk.Window _parent;
    private readonly KeyringDialogController _controller;
    private readonly Gtk.ShortcutController _shortcutController;
    private bool _handlingEnableToggle;
    
    [Gtk.Connect] private readonly Gtk.Label _titleLabel;
    [Gtk.Connect] private readonly Adw.ToastOverlay _toastOverlay;
    [Gtk.Connect] private readonly Adw.ActionRow _enableKeyringRow;
    [Gtk.Connect] private readonly Gtk.Switch _enableKeyringSwitch;
    
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
        //Dialog Settings
        SetTransientFor(parent);
        SetIconName(_controller.AppInfo.ID);
        //Build UI
        builder.Connect(this);
        //Shortcut Controller
        _shortcutController = Gtk.ShortcutController.New();
        _shortcutController.SetScope(Gtk.ShortcutScope.Managed);
        _shortcutController.AddShortcut(Gtk.Shortcut.New(Gtk.ShortcutTrigger.ParseString("Escape"), Gtk.CallbackAction.New(OnEscapeKey)));
        AddController(_shortcutController);
        //Load
        _enableKeyringSwitch.SetActive(_controller.IsEnabled);
        _enableKeyringSwitch.OnNotify += (sender, e) =>
        {
            if(e.Pspec.GetName() == "active")
            {
                ToggleEnable();
            }
        };
    }
    
    /// <summary>
    /// Constructs a KeyringDialog
    /// </summary>
    /// <param name="controller">KeyringDialogController</param>
    /// <param name="parent">Gtk.Window</param>
    public KeyringDialog(KeyringDialogController controller, Gtk.Window parent) : this(Builder.FromFile("keyring_dialog.ui"), controller, parent)
    {
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
    private void ToggleEnable()
    {
        if(!_handlingEnableToggle)
        {
            _handlingEnableToggle = true;
            if(_enableKeyringSwitch.GetActive())
            {
                
            }
            else
            {
                var dialog = new MessageDialog(this, _controller.AppInfo.ID, _("Disable Keyring?"), _("Disabling the Keyring will delete all current data currently stored in the Keyring. Are you sure you want to delete?"), _("No"), _("Yes"));
                dialog.OnResponse += (sender, e) =>
                {
                    if(dialog.Response == MessageDialogResponse.Destructive)
                    {
                        _controller.DisableKeyring();
                    }
                    else
                    {
                        _handlingEnableToggle = true;
                        _enableKeyringSwitch.SetActive(true);
                        _handlingEnableToggle = false;
                    }
                    dialog.Destroy();
                };
                dialog.Present();
            }
            _handlingEnableToggle = false;
        }
    }
}