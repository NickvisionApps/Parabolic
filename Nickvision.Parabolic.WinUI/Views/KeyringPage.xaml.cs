using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Keyring;
using Nickvision.Parabolic.Shared.Controllers;
using System;
using System.Collections.ObjectModel;
using System.Linq;

namespace Nickvision.Parabolic.WinUI.Views;

public sealed partial class KeyringPage : Page
{
    private enum Pages
    {
        None,
        NoneSearch,
        Keyring
    }

    private readonly KeyringPageController _controller;
    private ObservableCollection<SelectionItem<Credential>> _credentials;

    public KeyringPage(KeyringPageController controller)
    {
        InitializeComponent();
        _controller = controller;
        _credentials = [];
        BtnAdd.Label = _controller.Translator._("Add");
        BtnEdit.Label = _controller.Translator._("Edit");
        BtnRemove.Label = _controller.Translator._("Remove");
        TxtSerach.PlaceholderText = _controller.Translator._("Search...");
        StatusNone.Title = _controller.Translator._("No Credentials");
        StatusNone.Description = _controller.Translator._("There are no credentials in your keyring");
        StatusNoneSearch.Title = _controller.Translator._("No Credentials");
        StatusNoneSearch.Description = _controller.Translator._("There are no credentials found with the current filters");
        MenuAdd.Text = _controller.Translator._("Add");
        MenuEdit.Text = _controller.Translator._("Edit");
        MenuRemove.Text = _controller.Translator._("Remove");
        DlgCredential.Title = _controller.Translator._("Credential");
        DlgCredential.CloseButtonText = _controller.Translator._("Cancel");
        TxtCredentialName.Header = _controller.Translator._("Name");
        TxtCredentialName.PlaceholderText = _controller.Translator._("Enter name here");
        TxtCredentialUrl.Header = _controller.Translator._("URL");
        TxtCredentialUrl.PlaceholderText = _controller.Translator._("Enter url here");
        TxtCredentialUsername.Header = _controller.Translator._("Username");
        TxtCredentialUsername.PlaceholderText = _controller.Translator._("Enter username here");
        TxtCredentialPassword.Header = _controller.Translator._("Password");
        TxtCredentialPassword.PlaceholderText = _controller.Translator._("Enter password here");
    }

    private void Page_Loaded(object sender, RoutedEventArgs e)
    {
        TxtSerach.Text = string.Empty;
        _credentials = _controller.Credentials;
        ListCredentials.ItemsSource = _credentials;
        ViewStack.SelectedIndex = _credentials.Count == 0 ? (int)Pages.None : (int)Pages.Keyring;
    }

    private async void Add(object sender, RoutedEventArgs e)
    {
        TxtCredentialName.IsReadOnly = false;
        TxtCredentialName.Text = string.Empty;
        TxtCredentialUrl.Text = string.Empty;
        TxtCredentialUsername.Text = string.Empty;
        TxtCredentialPassword.Password = string.Empty;
        DlgCredential.PrimaryButtonText = _controller.Translator._("Add");
        DlgCredential.XamlRoot = XamlRoot;
        DlgCredential.RequestedTheme = ActualTheme;
        if ((await DlgCredential.ShowAsync()) == ContentDialogResult.Primary)
        {
            await _controller.AddAsync(TxtCredentialName.Text, TxtCredentialUrl.Text, TxtCredentialUsername.Text, TxtCredentialPassword.Password);
            ViewStack.SelectedIndex = _credentials.Count == 0 ? (int)Pages.None : (int)Pages.Keyring;
        }
    }

    private async void Edit(object sender, RoutedEventArgs e)
    {
        var selected = ListCredentials.SelectedItem as SelectionItem<Credential>;
        if (selected is null)
        {
            return;
        }
        TxtCredentialName.IsReadOnly = true;
        TxtCredentialName.Text = selected.Value.Name;
        TxtCredentialUrl.Text = selected.Value.Url.ToString();
        TxtCredentialUsername.Text = selected.Value.Username;
        TxtCredentialPassword.Password = selected.Value.Password;
        DlgCredential.PrimaryButtonText = _controller.Translator._("Update");
        DlgCredential.XamlRoot = XamlRoot;
        DlgCredential.RequestedTheme = ActualTheme;
        if ((await DlgCredential.ShowAsync()) == ContentDialogResult.Primary)
        {
            await _controller.UpdateAsync(TxtCredentialName.Text, TxtCredentialUrl.Text, TxtCredentialUsername.Text, TxtCredentialPassword.Password);
        }
    }

    private async void Remove(object sender, RoutedEventArgs e)
    {
        var selected = (ListCredentials.SelectedItem as SelectionItem<Credential>)!;
        var confirmDialog = new ContentDialog()
        {
            Title = _controller.Translator._("Delete Credential?"),
            Content = _controller.Translator._("Are you sure you want to delete this credential? The action is irreversible"),
            PrimaryButtonText = _controller.Translator._("Yes"),
            CloseButtonText = _controller.Translator._("No"),
            DefaultButton = ContentDialogButton.Close,
            XamlRoot = XamlRoot,
            RequestedTheme = ActualTheme
        };
        if ((await confirmDialog.ShowAsync()) == ContentDialogResult.Primary)
        {
            await _controller.RemoveAsync(selected);
            ViewStack.SelectedIndex = _credentials.Count == 0 ? (int)Pages.None : (int)Pages.Keyring;
        }
    }

    private void TxtSerach_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs e)
    {
        if (_credentials.Count == 0)
        {
            return;
        }
        if (e.Reason == AutoSuggestionBoxTextChangeReason.UserInput)
        {
            if (string.IsNullOrEmpty(sender.Text))
            {
                ListCredentials.ItemsSource = _credentials;
                ViewStack.SelectedIndex = _credentials.Count == 0 ? (int)Pages.None : (int)Pages.Keyring;
            }
            else
            {
                var filtered = _credentials.Where(x => x.Label.ToLower().Contains(sender.Text.ToLower()) || x.Value.Username.ToLower().Contains(sender.Text.ToLower()) || x.Value.Url.ToString().ToLower().Contains(sender.Text.ToLower()));
                ListCredentials.ItemsSource = filtered;
                ViewStack.SelectedIndex = filtered.Any() ? (int)Pages.Keyring : (int)Pages.NoneSearch;
            }
        }
    }

    private void ListCredentials_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        var selected = ListCredentials.SelectedItem as SelectionItem<Credential>;
        BtnEdit.IsEnabled = selected is not null;
        BtnRemove.IsEnabled = selected is not null;
        MenuEdit.IsEnabled = selected is not null;
        MenuRemove.IsEnabled = selected is not null;
    }

    private void ListCredentials_DoubleTapped(object sender, DoubleTappedRoutedEventArgs e) => Edit(sender, e);
}
