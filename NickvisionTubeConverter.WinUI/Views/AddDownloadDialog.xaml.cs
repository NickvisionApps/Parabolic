using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using NickvisionTubeConverter.Shared.Controllers;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.WinUI.Views;

public sealed partial class AddDownloadDialog : ContentDialog
{
    private readonly AddDownloadDialogController _controller;

    public AddDownloadDialog(AddDownloadDialogController controller)
    {
        InitializeComponent();
        _controller = controller;
        //Localize Strings
        Title = _("Add Download");
        CloseButtonText = _("Close");
        CardUrl.Header = _("Media URL");
        TxtUrl.PlaceholderText = _("Ender media url here");
        CardAuthenticate.Header = _("Authenticate");
        TglAuthenticate.OnContent = _("On");
        TglAuthenticate.OffContent = _("Off");
        CardKeyringCredentials.Header = _("Keyring Credential");
        CardUsername.Header = _("Username");
        TxtUsername.PlaceholderText = _("Enter user name here");
        CardPassword.Header = _("Password");
        TxtPassword.PlaceholderText = _("Enter password here");
        BtnValidate.Content = _("Validate");
        ViewStack.CurrentPageName = "Url";
    }

    /// <summary>
    /// Occurs when the ScrollViewer's size is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SizeChangedEventArgs</param>
    private void ScrollViewer_SizeChanged(object sender, SizeChangedEventArgs e) => ViewStack.Margin = new Thickness(0, 0, ScrollViewer.ComputedVerticalScrollBarVisibility == Visibility.Visible ? 14 : 0, 0);

    /// <summary>
    /// Occurs when the validate button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Validate(object sender, RoutedEventArgs e)
    {

    }
}
