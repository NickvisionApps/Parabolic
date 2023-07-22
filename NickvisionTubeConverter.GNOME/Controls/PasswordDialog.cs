using NickvisionTubeConverter.GNOME.Helpers;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.GNOME.Controls;

/// <summary>
/// A dialog for receiving a password
/// </summary>
public partial class PasswordDialog : Adw.Window
{
    [Gtk.Connect] private readonly Gtk.Label _titleLabel;
    [Gtk.Connect] private readonly Adw.PasswordEntryRow _passwordEntry;
    [Gtk.Connect] private readonly Gtk.Button _unlockButton;

    private PasswordDialog(Gtk.Builder builder, Gtk.Window parent, string title, TaskCompletionSource<string?> tcs) : base(builder.GetPointer("_root"), false)
    {
        var unlock = false;
        builder.Connect(this);
        //Dialog Settings
        SetTransientFor(parent);
        _titleLabel.SetLabel(title);
        _unlockButton.OnClicked += (sender, e) =>
        {
            unlock = true;
            Close();
        };
        OnCloseRequest += (sender, e) =>
        {
            tcs.SetResult(unlock ? _passwordEntry.GetText() : null);
            return false;
        };
    }

    /// <summary>
    /// Constructs a PasswordDialog
    /// </summary>
    /// <param name="parentWindow">Gtk.Window</param>
    /// <param name="accountTitle">The title of the account requiring the password</param>
    /// <param name="tcs">TaskCompletionSource used to pass result to the controller</param>
    public PasswordDialog(Gtk.Window parent, string accountTitle, TaskCompletionSource<string?> tcs) : this(Builder.FromFile("password_dialog.ui"), parent, accountTitle, tcs)
    {
    }
}