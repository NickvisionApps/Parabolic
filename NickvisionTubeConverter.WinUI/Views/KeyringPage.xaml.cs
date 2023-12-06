using CommunityToolkit.WinUI.Controls;
using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using Nickvision.Aura.Events;
using Nickvision.Aura.Keyring;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.WinUI.Views;

/// <summary>
/// A page for managing credentials
/// </summary>
public sealed partial class KeyringPage : UserControl
{
    private readonly KeyringDialogController _controller;
    private bool _opened;
    private readonly Button _btnConfirmDisable;
    private readonly Flyout _disableFlyout;

    /// <summary>
    /// Occurs when a notification is sent
    /// </summary>
    public event EventHandler<NotificationSentEventArgs>? NotificationSent;
    /// <summary>
    /// Occurs when the keyring is updated
    /// </summary>
    public event EventHandler<KeyringDialogController>? KeyringUpdated;

    /// <summary>
    /// Constructs a KeyringPage
    /// </summary>
    /// <param name="controller">KeyringDialogController</param>
    public KeyringPage(KeyringDialogController controller)
    {
        InitializeComponent();
        _controller = controller;
        _opened = false;
        //Localize Strings
        LblTitle.Text = _("Keyring");
        IconBtnEnableDisable.Glyph = "\uE785";
        LblBtnEnableDisable.Text = _("Enable");
        StatusPageDisabled.Title = _("Keyring Disabled");
        StatusPageDisabled.Description = _("Use keyring to safely store credentials for sites that require a user name and password to login.");
        BtnReset.Content = _("Reset");
        BtnAddCredential.Label = _("Add Login");
        StatusPageNoCredentials.Title = _("No Credentials");
        StatusPageNoCredentials.Description = _("Add a login to get started.");
        _btnConfirmDisable = new Button()
        {
            Margin = new Thickness(0, 6, 0, 0),
            Content = _("Disable"),
            Style = (Style)Application.Current.Resources["AccentButtonStyle"],
        };
        _btnConfirmDisable.Click += ConfirmDisable;
        _disableFlyout = new Flyout()
        {
            Content = new StackPanel()
            {
                Margin = new Thickness(6, 6, 6, 6),
                Orientation = Orientation.Vertical,
                Spacing = 6,
                MaxWidth = 300,
                Children =
                {
                    new TextBlock()
                    {
                        TextWrapping = TextWrapping.WrapWholeWords,
                        Text = _("Disable Keyring?")
                    },
                    new TextBlock()
                    {
                        TextWrapping = TextWrapping.WrapWholeWords,
                        Text = _("Disabling the keyring will delete all data currently stored inside. Are you sure you want to disable it?"),
                        Foreground = new SolidColorBrush(Colors.Gray)
                    },
                    _btnConfirmDisable
                }
            }
        };
    }

    /// <summary>
    /// Occurs when the page is loaded
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void Page_Loaded(object sender, RoutedEventArgs e)
    {
        if (!_opened)
        {
            ViewStack.CurrentPageName = "Disabled";
            if(!_controller.IsValid)
            {
                StatusPageDisabled.Description += $"\n\n{_("Restart the app to unlock the keyring.")}";
                BtnEnableDisable.IsEnabled = false;
                BtnReset.Visibility = Visibility.Visible;
                NotificationSent?.Invoke(this, new NotificationSentEventArgs(_("Keyring locked."), NotificationSeverity.Warning, "no-close"));
            }
            else if(_controller.IsEnabled)
            {
                await LoadCredentialsAsync();
            }
            _opened = true;
        }
    }

    /// <summary>
    /// Occurs when the BtnEnableDisable is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void EnableDisable(object sender, RoutedEventArgs e)
    {
        if (LblBtnEnableDisable.Text == _("Enable"))
        {
            if(await _controller.EnableKeyringAsync())
            {
                await LoadCredentialsAsync();
            }
            else
            {
                ViewStack.CurrentPageName = "Disabled";
                NotificationSent?.Invoke(this, new NotificationSentEventArgs(_("Failed to enable keyring."), NotificationSeverity.Error));
            }
        }
    }

    /// <summary>
    /// Occurs when the BtnConfirmDisable is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void ConfirmDisable(object sender, RoutedEventArgs e)
    {
        _disableFlyout.Hide();
        if(await _controller.DisableKeyringAsync())
        {
            ViewStack.CurrentPageName = "Disabled";
            BtnEnableDisable.Flyout = null;
            IconBtnEnableDisable.Glyph = "\uE785";
            LblBtnEnableDisable.Text = _("Enable");
        }
        else
        {
            NotificationSent?.Invoke(this, new NotificationSentEventArgs(_("Unable to disable keyring."), NotificationSeverity.Error));
        }
    }

    /// <summary>
    /// Occurs when the BtnReset is clicked
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
        var result = await dialog.ShowAsync();
        if (result == ContentDialogResult.Primary)
        {
            if (await _controller.ResetKeyringAsync())
            {
                ViewStack.CurrentPageName = "Disabled";
                StatusPageDisabled.Description = _("Use keyring to safely store credentials for sites that require a user name and password to login.");
                BtnEnableDisable.IsEnabled = true;
                BtnEnableDisable.Flyout = null;
                IconBtnEnableDisable.Glyph = "\uE785";
                LblBtnEnableDisable.Text = _("Enable");
                BtnReset.Visibility = Visibility.Collapsed;
            }
            else
            {
                NotificationSent?.Invoke(this, new NotificationSentEventArgs(_("Unable to reset keyring."), NotificationSeverity.Error));
            }
        }
    }

    /// <summary>
    /// Occurs when the BtnAddCredential is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void AddCredential(object sender, RoutedEventArgs e)
    {
        var addDialog = new CredentialDialog(_controller)
        {
            XamlRoot = XamlRoot
        };
        var result = await addDialog.ShowAsync();
        if(result == ContentDialogResult.Primary)
        {
            await _controller.AddCredentialAsync(addDialog.Credential.Name, addDialog.Credential.Uri?.ToString(), addDialog.Credential.Username, addDialog.Credential.Password);
            await LoadCredentialsAsync();
        }
    }

    /// <summary>
    /// Occurs when the edit button on a credential is clicked
    /// </summary>
    /// <param name="credential">Credential</param>
    private async void EditCredential(Credential credential)
    {
        var editDialog = new CredentialDialog(_controller, credential)
        {
            XamlRoot = XamlRoot
        };
        var result = await editDialog.ShowAsync();
        if (result == ContentDialogResult.Primary)
        {
            await _controller.UpdateCredentialAsync(editDialog.Credential.Id, editDialog.Credential.Name, editDialog.Credential.Uri?.ToString(), editDialog.Credential.Username, editDialog.Credential.Password);
            await LoadCredentialsAsync();
        }
        else if (result == ContentDialogResult.Secondary)
        {
            var deleteDialog = new ContentDialog()
            {
                Title = _("Delete Credential?"),
                Content = _("This action is irreversible. Are you sure you want to delete it?"),
                PrimaryButtonText = _("Yes"),
                CloseButtonText = _("No"),
                DefaultButton = ContentDialogButton.Primary,
                XamlRoot = XamlRoot
            };
            result = await deleteDialog.ShowAsync();
            if (result == ContentDialogResult.Primary)
            {
                await _controller.DeleteCredentialAsync(editDialog.Credential.Id);
                await LoadCredentialsAsync();
            }
        }
    }

    /// <summary>
    /// Loads the credentials in the keyring
    /// </summary>
    private async Task LoadCredentialsAsync()
    {
        ViewStack.CurrentPageName = "Loading";
        ListCredentials.Children.Clear();
        List<Credential>? credentials = null;
        await Task.Run(async () => credentials = await _controller.GetAllCredentialsAsync());
        foreach (var credential in credentials!)
        {
            var row = new SettingsCard()
            {
                Header = credential.Name,
                Description = credential.Uri?.ToString() ?? "",
                IsClickEnabled = true,
                ActionIcon = new SymbolIcon(Symbol.Edit)
            };
            row.Click += (sender, e) => EditCredential(credential);
            ListCredentials.Children.Add(row);
        }
        ViewStack.CurrentPageName = "Enabled";
        ViewStackCredentials.CurrentPageName = credentials.Count > 0 ? "Credentials" : "NoCredentials";
        BtnEnableDisable.Flyout = _disableFlyout;
        IconBtnEnableDisable.Glyph = "\uE72E";
        LblBtnEnableDisable.Text = _("Disable");
    }
}
