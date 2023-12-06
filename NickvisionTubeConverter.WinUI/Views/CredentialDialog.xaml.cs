using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Nickvision.Aura.Keyring;
using System;
using System.Threading.Tasks;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.WinUI.Views;

/// <summary>
/// A dialog for managing a credential
/// </summary>
public sealed partial class CredentialDialog : ContentDialog
{
    private readonly KeyringDialogController _controller;
    private readonly bool _isEditing;
    
    /// <summary>
    /// The credential managed by the dialog
    /// </summary>
    public Credential Credential { get; private set; }

    /// <summary>
    /// Constructs a CredentialDialog
    /// </summary>
    /// <param name="controller">KeyringDialogController</param>
    /// <param name="credential">The credential to edit, if available</param>
    public CredentialDialog(KeyringDialogController controller, Credential? credential = null)
    {
        InitializeComponent();
        _controller = controller;
        //Localize Strings
        Title = _("Login");
        CloseButtonText = _("Cancel");
        CardName.Header = _("Name");
        TxtName.PlaceholderText = _("Enter name here");
        CardUrl.Header = _("URL");
        TxtUrl.PlaceholderText = _("Enter URL here");
        CardUsername.Header = _("User Name");
        TxtUsername.PlaceholderText = _("Enter user name here");
        CardPassword.Header = _("Password");
        TxtPassword.PlaceholderText = _("Enter password here");
        //Load
        if(credential == null) //Add mode
        {
            PrimaryButtonText = _("Add");
            IsPrimaryButtonEnabled = false;
            Credential = new Credential("", null, "", "");
            _isEditing = false;
        }
        else //Edit mode
        {
            PrimaryButtonText = _("Update");
            SecondaryButtonText = _("Delete");
            Credential = credential;
            _isEditing = true;
        }
        TxtName.Text = Credential.Name;
        TxtUrl.Text = Credential.Uri?.ToString() ?? "";
        TxtUsername.Text = Credential.Username;
        TxtPassword.Password = Credential.Password;
    }

    /// <summary>
    /// Shows the dialog
    /// </summary>
    /// <returns>ContentDialogResult</returns>
    public new async Task<ContentDialogResult> ShowAsync()
    {
        var result = await base.ShowAsync();
        if(!_isEditing)
        {
            Credential = new Credential(TxtName.Text, string.IsNullOrEmpty(TxtUrl.Text) ? null : new Uri(TxtUrl.Text), TxtUsername.Text, TxtPassword.Password);
        }
        else
        {
            Credential.Name = TxtName.Text;
            Credential.Uri = string.IsNullOrEmpty(TxtUrl.Text) ? null : new Uri(TxtUrl.Text);
            Credential.Username = TxtUsername.Text;
            Credential.Password = TxtPassword.Password;
        }
        return result;
    }

    /// <summary>
    /// Occurs when the ScrollViewer's size is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SizeChangedEventArgs</param>
    private void ScrollViewer_SizeChanged(object sender, SizeChangedEventArgs e) => StackPanel.Margin = new Thickness(0, 0, ScrollViewer.ComputedVerticalScrollBarVisibility == Visibility.Visible ? 14 : 0, 0);

    /// <summary>
    /// Occurs when a TextBox's text is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">TextChangedEventArgs</param>
    private void TextChanged(object sender, TextChangedEventArgs e) => Validate();

    /// <summary>
    /// Occurs when a PasswordBox's text is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void PasswordChanged(object sender, RoutedEventArgs e) => Validate();

    /// <summary>
    /// Validates the fields of the dialog
    /// </summary>
    private void Validate()
    {
        CardName.Header = _("Name");
        CardUrl.Header = _("URL");
        CardUsername.Header = _("User Name");
        CardPassword.Header = _("Password");
        IsPrimaryButtonEnabled = true;
        var checkStatus = _controller.ValidateCredential(TxtName.Text, TxtUrl.Text, TxtUsername.Text, TxtPassword.Password);
        if (checkStatus.HasFlag(CredentialCheckStatus.EmptyName))
        {
            CardName.Header = _("Name (Empty)");
        }
        if (checkStatus.HasFlag(CredentialCheckStatus.EmptyUsernamePassword))
        {
            CardUsername.Header = _("User Name (Empty)");
            CardPassword.Header = _("Password (Empty)");
        }
        if (checkStatus.HasFlag(CredentialCheckStatus.InvalidUri))
        {
            CardUrl.Header = _("URL (Invalid)");
        }
        IsPrimaryButtonEnabled = checkStatus == CredentialCheckStatus.Valid;
    }
}
