using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Keyring;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.WinUI.Helpers;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.WinUI.Views;

public sealed partial class KeyringDialog : ContentDialog
{
    private enum Pages
    {
        None = 0,
        NoneSearch,
        Keyring
    }

    private enum CredentialEditMode
    {
        None = 0,
        Add,
        Edit
    }

    private readonly KeyringViewController _controller;
    private readonly ITranslationService _translationService;
    private List<BindableCredentialSelectionItem> _credentials;
    private bool _needsCredentialDialog;
    private bool _needsDeleteDialog;
    private CredentialEditMode _credentialEditMode;
    private Credential? _selectedCredential;

    public KeyringDialog(KeyringViewController controller, ITranslationService translationService)
    {
        InitializeComponent();
        _controller = controller;
        _translationService = translationService;
        _credentials = [];
        _needsCredentialDialog = false;
        _needsDeleteDialog = false;
        _credentialEditMode = CredentialEditMode.None;
        _selectedCredential = null;
        Title = _translationService._("Keyring");
        PrimaryButtonText = _translationService._("Close");
        SearchBox.PlaceholderText = _translationService._("Search...");
        BtnAdd.Label = _translationService._("Add");
        StatusNone.Title = _translationService._("No Credentials");
        StatusNone.Description = _translationService._("There are no credentials in your keyring");
        StatusNoneSearch.Title = _translationService._("No Credentials");
        StatusNoneSearch.Description = _translationService._("There are no credentials found with the current filters");
    }

    public new async Task<ContentDialogResult> ShowAsync()
    {
        var result = await base.ShowAsync();
        while (result != ContentDialogResult.Primary && (_needsCredentialDialog || _needsDeleteDialog))
        {
            if (_needsCredentialDialog)
            {
                var dialog = new ContentDialog()
                {
                    Title = _translationService._("Credential"),
                    PrimaryButtonText = _credentialEditMode == CredentialEditMode.Add ? _translationService._("Add") : _translationService._("Update"),
                    CloseButtonText = _translationService._("Cancel"),
                    DefaultButton = ContentDialogButton.Primary,
                    XamlRoot = XamlRoot,
                    RequestedTheme = ActualTheme
                };
                var txtName = new TextBox()
                {
                    Header = _translationService._("Name"),
                    PlaceholderText = _translationService._("Enter name here"),
                    IsReadOnly = _credentialEditMode == CredentialEditMode.Edit
                };
                var txtUrl = new TextBox()
                {
                    Header = _translationService._("URL"),
                    PlaceholderText = _translationService._("Enter url here")
                };
                var txtUsername = new TextBox()
                {
                    Header = _translationService._("Username"),
                    PlaceholderText = _translationService._("Enter username here")
                };
                var txtPassword = new PasswordBox()
                {
                    Header = _translationService._("Password"),
                    PlaceholderText = _translationService._("Enter password here")
                };
                if (_credentialEditMode == CredentialEditMode.Edit)
                {
                    txtName.Text = _selectedCredential!.Name;
                    txtUrl.Text = _selectedCredential.Url.ToString();
                    txtUsername.Text = _selectedCredential.Username;
                    txtPassword.Password = _selectedCredential.Password;
                }
                dialog.Content = new StackPanel()
                {
                    Orientation = Orientation.Vertical,
                    Spacing = 12,
                    Children = { txtName, txtUrl, txtUsername, txtPassword }
                };
                string? error = null;
                do
                {
                    error = null;
                    if ((await dialog.ShowAsync()) == ContentDialogResult.Primary)
                    {
                        error = _credentialEditMode == CredentialEditMode.Add
                            ? await _controller.AddAsync(txtName.Text, txtUrl.Text, txtUsername.Text, txtPassword.Password)
                            : await _controller.UpdateAsync(txtName.Text, txtUrl.Text, txtUsername.Text, txtPassword.Password);
                        if (error is not null)
                        {
                            var errorDialog = new ContentDialog()
                            {
                                Title = _translationService._("Error"),
                                Content = error,
                                CloseButtonText = _translationService._("OK"),
                                DefaultButton = ContentDialogButton.Close,
                                XamlRoot = XamlRoot,
                                RequestedTheme = ActualTheme
                            };
                            await errorDialog.ShowAsync();
                        }
                    }
                    else
                    {
                        break;
                    }
                } while (error is not null);
                _needsCredentialDialog = false;
                _credentialEditMode = CredentialEditMode.None;
                _selectedCredential = null;
            }
            else if (_needsDeleteDialog)
            {
                var confirmDialog = new ContentDialog()
                {
                    Title = _translationService._("Delete Credential?"),
                    Content = _translationService._("Are you sure you want to delete this credential? This action is irreversible"),
                    PrimaryButtonText = _translationService._("Yes"),
                    CloseButtonText = _translationService._("No"),
                    DefaultButton = ContentDialogButton.Close,
                    XamlRoot = XamlRoot,
                    RequestedTheme = ActualTheme
                };
                if ((await confirmDialog.ShowAsync()) == ContentDialogResult.Primary)
                {
                    await _controller.RemoveAsync(_selectedCredential!);
                }
                _needsDeleteDialog = false;
                _selectedCredential = null;
            }
            result = await base.ShowAsync();
        }
        return result;
    }

    private async void Dialog_Loaded(object sender, RoutedEventArgs e)
    {
        SearchBox.Text = string.Empty;
        _credentials = (await _controller.GetAllAsync()).ToBindableCredentialSelectionItems().ToList();
        ListCredentials.ItemsSource = _credentials;
        ViewStack.SelectedIndex = _credentials.Count == 0 ? (int)Pages.None : (int)Pages.Keyring;
    }

    private void SearchBox_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs args)
    {
        if (_credentials.Count == 0)
        {
            return;
        }
        if (args.Reason == AutoSuggestionBoxTextChangeReason.UserInput)
        {
            if (string.IsNullOrEmpty(sender.Text))
            {
                ListCredentials.ItemsSource = _credentials;
                ViewStack.SelectedIndex = _credentials.Count == 0 ? (int)Pages.None : (int)Pages.Keyring;
            }
            else
            {
                var search = sender.Text.ToLower();
                var filtered = _credentials.Where(x => x.Label.ToLower().Contains(search) || x.Value.Username.ToLower().Contains(search) || x.Value.Url.ToString().ToLower().Contains(search)).ToList();
                ListCredentials.ItemsSource = filtered;
                ViewStack.SelectedIndex = filtered.Any() ? (int)Pages.Keyring : (int)Pages.NoneSearch;
            }
        }
    }

    private void Add(object sender, RoutedEventArgs e)
    {
        _selectedCredential = null;
        _credentialEditMode = CredentialEditMode.Add;
        _needsCredentialDialog = true;
        Hide();
    }

    private void Edit(object sender, RoutedEventArgs e)
    {
        _selectedCredential = ((sender as Button)!.Tag as Credential)!;
        _credentialEditMode = CredentialEditMode.Edit;
        _needsCredentialDialog = true;
        Hide();
    }

    private void Remove(object sender, RoutedEventArgs e)
    {
        _selectedCredential = ((sender as Button)!.Tag as Credential)!;
        _needsDeleteDialog = true;
        Hide();
    }
}
