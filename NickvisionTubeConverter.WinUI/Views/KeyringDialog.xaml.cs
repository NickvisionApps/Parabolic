using CommunityToolkit.WinUI.Controls;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Nickvision.Aura.Keyring;
using System;
using System.Threading.Tasks;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.WinUI.Views;

public sealed partial class KeyringDialog : ContentDialog
{
    private readonly KeyringDialogController _controller;
    private bool _showingAnotherDialog;
    private bool _opened;
    private int? _editId;

    public KeyringDialog(KeyringDialogController controller)
    {
        InitializeComponent();
        _controller = controller;
        _showingAnotherDialog = false;
        _opened = false;
        _editId = null;
        //Localize Strings
        Title = _("Keyring");
        CloseButtonText = _("Close");
        LblBtnBack.Text = _("Back");
        StatusPageDisabled.Title = _("Keyring Disabled");
        StatusPageDisabled.Description = _("Use keyring to safely store credentials for sites that require a user name and password to login.");
        BtnEnable.Content = _("Enable");
        StatusPageDisable.Title = _("Disable Keyring?");
        StatusPageDisable.Description = _("Disabling the keyring will delete all data currently stored inside. Are you sure you want to disable it?");
        BtnDisable.Content = _("Disable");
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
        BtnInfoBarReset.Content = _("Reset");
    }

    /// <summary>
    /// Shows the dialog
    /// </summary>
    /// <returns>ContentDialogResult</returns>
    public new async Task<ContentDialogResult> ShowAsync()
    {
        var result = await base.ShowAsync();
        if (_showingAnotherDialog)
        {
            while (_showingAnotherDialog)
            {
                await Task.Delay(100);
            }
            result = await base.ShowAsync();
        }
        return result;
    }

    /// <summary>
    /// Occurs when the dialog is loaded
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void Dialog_Loaded(object sender, RoutedEventArgs e)
    {
        if (!_opened)
        {
            ViewStack.CurrentPageName = "Disabled";
            if (!_controller.IsValid)
            {
                StatusPageDisabled.Description += $"\n\n{_("Restart the app to unlock the keyring.")}";
                InfoBar.Content = _("Keyring locked");
                InfoBar.Severity = InfoBarSeverity.Warning;
                InfoBar.IsClosable = false;
                InfoBar.IsOpen = true;
                BtnInfoBarReset.Visibility = Visibility.Visible;
                BtnEnable.IsEnabled = false;
            }
            else if (_controller.IsEnabled)
            {
                await LoadHomePageAsync();
            }
            _opened = true;
        }
    }

    /// <summary>
    /// Occurs when the ScrollViewer's size is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SizeChangedEventArgs</param>
    private void ScrollViewer_SizeChanged(object sender, SizeChangedEventArgs e) => ViewStack.Margin = new Thickness(0, 0, ScrollViewer.ComputedVerticalScrollBarVisibility == Visibility.Visible ? 14 : 0, 0);

    /// <summary>
    /// Occurs when the enable button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void Enable(object sender, RoutedEventArgs e)
    {
        ViewStack.CurrentPageName = "Loading";
        var success = await _controller.EnableKeyringAsync();
        if (success)
        {
            await LoadHomePageAsync();
        }
        else
        {
            ViewStack.CurrentPageName = "Disabled";
            InfoBar.Content = _("Failed to enable keyring.");
            InfoBar.Severity = InfoBarSeverity.Error;
            InfoBar.IsOpen = true;
        }
    }

    /// <summary>
    /// Occurs when the disable button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Disable(object sender, RoutedEventArgs e)
    {
        BtnBack.Visibility = Visibility.Visible;
        ViewStack.CurrentPageName = "Disable";
    }

    /// <summary>
    /// Occurs when the confirm disable button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void ConfirmDisable(object sender, RoutedEventArgs e)
    {
        if (await _controller.DisableKeyringAsync())
        {
            Hide();
        }
        else
        {
            InfoBar.Content = _("Unable to disable keyring.");
            InfoBar.Severity = InfoBarSeverity.Error;
            InfoBar.IsOpen = true;
        }
    }

    /// <summary>
    /// Occurs when the reset button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void Reset(object sender, RoutedEventArgs e)
    {
        var dialog = new ContentDialog()
        {
            Title = _("Reset Keyring?"),
            Content = _("This will delete the previous keyring removing all passwords with it. This action is irreversible."),
            PrimaryButtonText = _("Yes"),
            CloseButtonText = _("No"),
            DefaultButton = ContentDialogButton.Primary,
            XamlRoot = XamlRoot
        };
        _showingAnotherDialog = true;
        Hide();
        var result = await dialog.ShowAsync();
        _showingAnotherDialog = false;
        if (result == ContentDialogResult.Primary)
        {
            if (await _controller.ResetKeyringAsync())
            {
                Hide();
            }
            else
            {
                InfoBar.Content = _("Unable to reset keyring.");
                InfoBar.Severity = InfoBarSeverity.Error;
                InfoBar.IsOpen = true;
            }
        }
    }

    /// <summary>
    /// Occurs when the back button is clicked or a need to go back to home page is needed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void LoadHomePage(object sender, RoutedEventArgs e) => await LoadHomePageAsync();

    /// <summary>
    /// Occurs when the add credential button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void AddCredential(object sender, RoutedEventArgs e)
    {
        ViewStack.CurrentPageName = "Credential";
        BtnBack.Visibility = Visibility.Visible;
        Title = _("Login");
        CredentialViewStack.CurrentPageName = "Add";
        _editId = null;
        TxtName.Text = "";
        TxtUrl.Text = "";
        TxtUsername.Text = "";
        TxtPassword.Password = "";
        SetValidation(CredentialCheckStatus.Valid);
    }

    /// <summary>
    /// Occurs when the confirm add credential button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void ConfirmAddCredential(object sender, RoutedEventArgs e)
    {
        var checkStatus = _controller.ValidateCredential(TxtName.Text, TxtUrl.Text, TxtUsername.Text, TxtPassword.Password);
        SetValidation(checkStatus);
        if (checkStatus == CredentialCheckStatus.Valid)
        {
            await _controller.AddCredentialAsync(TxtName.Text, TxtUrl.Text, TxtUsername.Text, TxtPassword.Password);
            await LoadHomePageAsync();
        }
    }

    /// <summary>
    /// Occurs when the delete credential button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void DeleteCredential(object sender, RoutedEventArgs e)
    {
        var dialog = new ContentDialog()
        {
            Title = _("Delete Credential?"),
            Content = _("This action is irreversible. Are you sure you want to delete it?"),
            PrimaryButtonText = _("Yes"),
            CloseButtonText = _("No"),
            DefaultButton = ContentDialogButton.Primary,
            XamlRoot = XamlRoot
        };
        _showingAnotherDialog = true;
        Hide();
        var result = await dialog.ShowAsync();
        _showingAnotherDialog = false;
        if (result == ContentDialogResult.Primary)
        {
            await _controller.DeleteCredentialAsync(_editId!.Value);
            await LoadHomePageAsync();
        }
    }

    /// <summary>
    /// Adapts the UI to the current validation status
    /// </summary>
    /// <param name="checkStatus">CredentialCheckStatus</param>
    private void SetValidation(CredentialCheckStatus checkStatus)
    {
        CardName.Header = _("Name");
        CardUrl.Header = _("URL");
        CardUsername.Header = _("User Name");
        CardPassword.Header = _("Password");
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
    }

    private async Task LoadHomePageAsync()
    {
        if (_editId != null)
        {
            var checkStatus = _controller.ValidateCredential(TxtName.Text, TxtUrl.Text, TxtUsername.Text, TxtPassword.Password);
            SetValidation(checkStatus);
            if (checkStatus == CredentialCheckStatus.Valid)
            {
                await _controller.UpdateCredentialAsync(_editId.Value, TxtName.Text, TxtUrl.Text, TxtUsername.Text, TxtPassword.Password);
            }
            else
            {
                return;
            }
        }
        _editId = null;
        BtnBack.Visibility = Visibility.Collapsed;
        Title = _("Keyring");
        ViewStack.CurrentPageName = "Loading";
        ListCredentials.Children.Clear();
        var credentials = await _controller.GetAllCredentialsAsync();
        foreach (var credential in credentials)
        {
            var row = new SettingsCard()
            {
                Header = credential.Name,
                Description = credential.Uri?.ToString() ?? "",
                IsClickEnabled = true,
                ActionIcon = new SymbolIcon(Symbol.Edit)
            };
            row.Click += (sender, e) => LoadEditCredentialPage(credential);
            ListCredentials.Children.Add(row);
        }
        ViewStack.CurrentPageName = credentials.Count > 0 ? "Credentials" : "NoCredentials";
    }

    /// <summary>
    /// Loads the EditCredential page
    /// </summary>
    /// <param name="credential">The Credential model</param>
    private void LoadEditCredentialPage(Credential credential)
    {
        ViewStack.CurrentPageName = "Credential";
        BtnBack.Visibility = Visibility.Visible;
        Title = _("Login");
        CredentialViewStack.CurrentPageName = "Edit";
        _editId = credential.Id;
        TxtName.Text = credential.Name;
        TxtUrl.Text = credential.Uri?.ToString() ?? "";
        TxtUsername.Text = credential.Username;
        TxtPassword.Password = credential.Password;
        SetValidation(CredentialCheckStatus.Valid);
    }
}
