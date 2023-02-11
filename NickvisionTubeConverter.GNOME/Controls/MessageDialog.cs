namespace NickvisionTubeConverter.GNOME.Controls;

/// <summary>
/// Responses for the MessageDialog
/// </summary>
public enum MessageDialogResponse
{
    Suggested,
    Destructive,
    Cancel
}

/// <summary>
/// A dialog for showing a message
/// </summary>
public partial class MessageDialog
{
    private readonly Adw.MessageDialog _dialog;

    public MessageDialogResponse Response { get; private set; }

    /// <summary>
    /// Constructs a MessageDialog
    /// </summary>
    /// <param name="parentWindow">Gtk.Window</param>
    /// <param name="title">The title of the dialog</param>
    /// <param name="message">The message of the dialog</param>
    /// <param name="cancelText">The text of the cancel button</param>
    /// <param name="destructiveText">The text of the destructive button</param>
    /// <param name="suggestedText">The text of the suggested button</param>
    public MessageDialog(Gtk.Window parentWindow, string title, string message, string? cancelText, string? destructiveText = null, string? suggestedText = null)
    {
        _dialog = Adw.MessageDialog.New(parentWindow, title, message);
        _dialog.SetHideOnClose(true);
        Response = MessageDialogResponse.Cancel;
        if (!string.IsNullOrEmpty(cancelText))
        {
            _dialog.AddResponse("cancel", cancelText);
            _dialog.SetDefaultResponse("cancel");
            _dialog.SetCloseResponse("cancel");
        }
        if (!string.IsNullOrEmpty(destructiveText))
        {
            _dialog.AddResponse("destructive", destructiveText);
            _dialog.SetResponseAppearance("destructive", Adw.ResponseAppearance.Destructive);
        }
        if (!string.IsNullOrEmpty(suggestedText))
        {
            _dialog.AddResponse("suggested", suggestedText);
            _dialog.SetResponseAppearance("suggested", Adw.ResponseAppearance.Suggested);
        }
        _dialog.OnResponse += (sender, e) => SetResponse(e.Response);
    }

    public event GObject.SignalHandler<Adw.MessageDialog, Adw.MessageDialog.ResponseSignalArgs> OnResponse
    {
        add
        {
            _dialog.OnResponse += value;
        }
        remove
        {
            _dialog.OnResponse -= value;
        }
    }

    /// <summary>
    /// Shows the dialog
    /// </summary>
    public void Show() => _dialog.Show();

    /// <summary>
    /// Destroys the dialog
    /// </summary>
    public void Destroy() => _dialog.Destroy();

    /// <summary>
    /// Resets the destructive response appearance to default
    /// </summary>
    public void UnsetDestructiveApperance() => _dialog.SetResponseAppearance("destructive", Adw.ResponseAppearance.Default);

    /// <summary>
    /// Resets the suggested response appearance to default
    /// </summary>
    public void UnsetSuggestedApperance() => _dialog.SetResponseAppearance("suggested", Adw.ResponseAppearance.Default);

    /// <summary>
    /// Sets the response of the dialog as a MessageDialogResponse
    /// </summary>
    /// <param name="response">The string response of the dialog</param>
    private void SetResponse(string response)
    {
        Response = response switch
        {
            "suggested" => MessageDialogResponse.Suggested,
            "destructive" => MessageDialogResponse.Destructive,
            _ => MessageDialogResponse.Cancel
        };
    }
}