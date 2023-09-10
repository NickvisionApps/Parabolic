using NickvisionTubeConverter.GNOME.Helpers;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.GNOME.Controls;

/// <summary>
/// A dialog for receiving a password
/// </summary>
public partial class PasswordDialog : Adw.Window
{
    private bool _unlocked;

    [Gtk.Connect] private readonly Gtk.Button _migrateButton;
    [Gtk.Connect] private readonly Gtk.Label _titleLabel;
    [Gtk.Connect] private readonly Adw.PasswordEntryRow _passwordEntry;
    [Gtk.Connect] private readonly Gtk.Button _skipButton;
    [Gtk.Connect] private readonly Gtk.Button _unlockButton;

    /// <summary>
    /// Whether or not the dialog was skipped
    /// </summary>
    public bool WasSkipped { get; private set;  }
    
    /// <summary>
    /// Constructs a PasswordDialog
    /// </summary>
    /// <param name="builder">Gtk.Builder</param>
    /// <param name="parent">Gtk.Window</param>
    /// <param name="title">The title of the account requiring the password</param>
    /// <param name="tcs">TaskCompletionSource used to pass result to the controller</param>
    private PasswordDialog(Gtk.Builder builder, Gtk.Window parent, string title, TaskCompletionSource<(bool WasSkipped, string Password)> tcs) : base(builder.GetPointer("_root"), false)
    {
        _unlocked = false;
        WasSkipped = false;
        builder.Connect(this);
        //Dialog Settings
        SetTransientFor(parent);
        _migrateButton.OnClicked += (sender, e) => Gtk.Functions.ShowUri(this, Help.GetHelpURL("keyring"), 0);
        _titleLabel.SetLabel(title);
        _skipButton.OnClicked += (sender, e) =>
        {
            _unlocked = false;
            WasSkipped = true;
            Close();
        };
        _unlockButton.OnClicked += (sender, e) =>
        {
            _unlocked = true;
            WasSkipped = false;
            Close();
        };
        OnCloseRequest += (sender, e) =>
        {
            tcs.SetResult((WasSkipped, _unlocked ? _passwordEntry.GetText() : ""));
            return false;
        };
    }

    /// <summary>
    /// Constructs a PasswordDialog
    /// </summary>
    /// <param name="parent">Gtk.Window</param>
    /// <param name="title">The title of the account requiring the password</param>
    /// <param name="tcs">TaskCompletionSource used to pass result to the controller</param>
    public PasswordDialog(Gtk.Window parent, string title, TaskCompletionSource<(bool WasSkipped, string Password)> tcs) : this(Builder.FromFile("password_dialog.ui"), parent, title, tcs)
    {
    }
}