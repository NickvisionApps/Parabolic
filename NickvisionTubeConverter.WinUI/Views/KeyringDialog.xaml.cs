using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using Nickvision.Aura.Keyring;
using Windows.UI;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.WinUI.Views;

public sealed partial class KeyringDialog : ContentDialog
{
    private readonly KeyringDialogController _controller;

    public KeyringDialog(KeyringDialogController controller)
    {
        InitializeComponent();
        _controller = controller;
        Title = _("Keyring");
        CloseButtonText = _("Close");
        LblBtnBack.Text = _("Back");
        StatusPageDisabled.Title = _("Keyring Disabled");
        StatusPageDisabled.Description = _("Use keyring to safely store credentials for sites that require a user name and password to login.");
        BtnEnable.Content = _("Enable");
        StatusPageDisable.Title = _("Disable Keyring?");
        StatusPageDisable.Description = _("Disabling the keyring will delete all data currently stored inside. Are you sure you want to disable it?");
        BtnDisable.Content = _("Disable");
        BtnDisable.Background = new SolidColorBrush(ActualTheme == ElementTheme.Light ? Color.FromArgb(255, 192, 28, 40) : Color.FromArgb(255, 255, 123, 99));
        BtnDisable.Foreground = new SolidColorBrush(ActualTheme == ElementTheme.Light ? Colors.White : Colors.Black);
        StatusPageNoCredentials.Title = _("No Credentials");
        BtnNoCredentialsAddLogin.Content = _("Add Login");
        BtnNoCrednetialsDisable.Content = _("Disable Keyring");
        BtnCredentialsAddLogin.Content = _("Add Login");
        BtnCrednetialsDisable.Content = _("Disable Keyring");
        CardName.Header = _("Name");
        TxtName.PlaceholderText = _("Enter name here");
        CardUrl.Header = _("URL");
        TxtUrl.PlaceholderText = _("Enter url here");
        CardUsername.Header = _("User Name");
        TxtUsername.PlaceholderText = _("Enter user name here");
        CardPassword.Header = _("Password");
        TxtPassword.PlaceholderText = _("Enter password here");
        LblBtnAddCredential.Text = _("Add");
        LblBtnDeleteCredential.Text = _("Delete");
        ViewStack.CurrentPageName = "Credentials";
    }

    /// <summary>
    /// Occurs when the ScrollViewer's size is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SizeChangedEventArgs</param>
    private void ScrollViewer_SizeChanged(object sender, SizeChangedEventArgs e) => ViewStack.Margin = new Thickness(0, 0, ScrollViewer.ComputedVerticalScrollBarVisibility == Visibility.Visible ? 14 : 0, 0);

    /// <summary>
    /// Occurs when the back button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Back(object sender, RoutedEventArgs e)
    {

    }

    /// <summary>
    /// Occurs when the enable button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Enable(object sender, RoutedEventArgs e)
    {

    }

    /// <summary>
    /// Occurs when the disable button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Disable(object sender, RoutedEventArgs e)
    {

    }

    /// <summary>
    /// Occurs when the confirm disable button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void ConfirmDisable(object sender, RoutedEventArgs e)
    {

    }

    /// <summary>
    /// Occurs when the add credential button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void AddCredential(object sender, RoutedEventArgs e)
    {

    }

    /// <summary>
    /// Occurs when the confirm add credential button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void ConfirmAddCredential(object sender, RoutedEventArgs e)
    {

    }

    /// <summary>
    /// Occurs when the confirm delete credential button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void ConfirmDeleteCredential(object sender, RoutedEventArgs e)
    {

    }
}
