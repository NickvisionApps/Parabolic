using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Keyring;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.WinUI.Helpers;
using System;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
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

    private readonly KeyringViewController _controller;
    private readonly ITranslationService _translationService;
    private readonly ObservableCollection<BindableCredentialSelectionItem> _bindableCredentials;

    public KeyringPage(KeyringViewController controller, ITranslationService translationService)
    {
        InitializeComponent();
        _controller = controller;
        _translationService = translationService;
        _bindableCredentials = _controller.Credentials.ToBindableCredentialSelectionItems();
        _controller.Credentials.CollectionChanged += OnCredentialsCollectionChanged;
        LblKeyring.Text = _translationService._("Keyring");
        LblAdd.Text = _translationService._("Add");
        TxtSearch.PlaceholderText = _translationService._("Search...");
        StatusNone.Title = _translationService._("No Credentials");
        StatusNone.Description = _translationService._("There are no credentials in your keyring");
        StatusNoneSearch.Title = _translationService._("No Credentials");
        StatusNoneSearch.Description = _translationService._("There are no credentials found with the current filters");
        DlgCredential.Title = _translationService._("Credential");
        DlgCredential.CloseButtonText = _translationService._("Cancel");
        TxtCredentialName.Header = _translationService._("Name");
        TxtCredentialName.PlaceholderText = _translationService._("Enter name here");
        TxtCredentialUrl.Header = _translationService._("URL");
        TxtCredentialUrl.PlaceholderText = _translationService._("Enter url here");
        TxtCredentialUsername.Header = _translationService._("Username");
        TxtCredentialUsername.PlaceholderText = _translationService._("Enter username here");
        TxtCredentialPassword.Header = _translationService._("Password");
        TxtCredentialPassword.PlaceholderText = _translationService._("Enter password here");
    }

    private void Page_Loaded(object sender, RoutedEventArgs e)
    {
        TxtSearch.Text = string.Empty;
        ListCredentials.ItemsSource = _bindableCredentials;
        ViewStack.SelectedIndex = _controller.Credentials.Count == 0 ? (int)Pages.None : (int)Pages.Keyring;
    }

    private void Page_Unloaded(object sender, RoutedEventArgs e)
    {
        _controller.Credentials.CollectionChanged -= OnCredentialsCollectionChanged;
    }

    private void OnCredentialsCollectionChanged(object? sender, NotifyCollectionChangedEventArgs e)
    {
        switch (e.Action)
        {
            case NotifyCollectionChangedAction.Add when e.NewItems is not null:
                foreach (SelectionItem<Credential> item in e.NewItems)
                {
                    _bindableCredentials.Add(new BindableCredentialSelectionItem(item));
                }
                break;
            case NotifyCollectionChangedAction.Remove when e.OldItems is not null:
                foreach (SelectionItem<Credential> item in e.OldItems)
                {
                    var bindable = _bindableCredentials.FirstOrDefault(b => b.Value == item.Value);
                    if (bindable is not null)
                    {
                        _bindableCredentials.Remove(bindable);
                    }
                }
                break;
            case NotifyCollectionChangedAction.Replace when e.NewItems is not null && e.NewStartingIndex >= 0 && e.NewStartingIndex < _bindableCredentials.Count:
                _bindableCredentials[e.NewStartingIndex] = new BindableCredentialSelectionItem((SelectionItem<Credential>)e.NewItems[0]!);
                break;
            default:
                _bindableCredentials.Clear();
                foreach (var item in _controller.Credentials)
                {
                    _bindableCredentials.Add(new BindableCredentialSelectionItem(item));
                }
                break;
        }
    }

    private async void Add(object sender, RoutedEventArgs e)
    {
        TxtCredentialName.IsReadOnly = false;
        TxtCredentialName.Text = string.Empty;
        TxtCredentialUrl.Text = string.Empty;
        TxtCredentialUsername.Text = string.Empty;
        TxtCredentialPassword.Password = string.Empty;
        DlgCredential.PrimaryButtonText = _translationService._("Add");
        DlgCredential.XamlRoot = XamlRoot;
        DlgCredential.RequestedTheme = ActualTheme;
        string? error = null;
        do
        {
            if ((await DlgCredential.ShowAsync()) == ContentDialogResult.Primary)
            {
                error = await _controller.AddAsync(TxtCredentialName.Text, TxtCredentialUrl.Text, TxtCredentialUsername.Text, TxtCredentialPassword.Password);
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
        } while (error is not null);
        ViewStack.SelectedIndex = _controller.Credentials.Count == 0 ? (int)Pages.None : (int)Pages.Keyring;
    }

    private async void Edit(object sender, RoutedEventArgs e)
    {
        var selected = ((sender as Button)!.Tag as Credential)!;
        TxtCredentialName.IsReadOnly = true;
        TxtCredentialName.Text = selected.Name;
        TxtCredentialUrl.Text = selected.Url.ToString();
        TxtCredentialUsername.Text = selected.Username;
        TxtCredentialPassword.Password = selected.Password;
        DlgCredential.PrimaryButtonText = _translationService._("Update");
        DlgCredential.XamlRoot = XamlRoot;
        DlgCredential.RequestedTheme = ActualTheme;
        string? error = null;
        do
        {
            if ((await DlgCredential.ShowAsync()) == ContentDialogResult.Primary)
            {
                error = await _controller.UpdateAsync(TxtCredentialName.Text, TxtCredentialUrl.Text, TxtCredentialUsername.Text, TxtCredentialPassword.Password);
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
        } while (error is not null);
    }

    private async void Remove(object sender, RoutedEventArgs e)
    {
        var selected = ((sender as Button)!.Tag as Credential)!;
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
            await _controller.RemoveAsync(selected);
            ViewStack.SelectedIndex = _controller.Credentials.Count == 0 ? (int)Pages.None : (int)Pages.Keyring;
        }
    }

    private void TxtSearch_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs e)
    {
        if (_controller.Credentials.Count == 0)
        {
            return;
        }
        if (e.Reason == AutoSuggestionBoxTextChangeReason.UserInput)
        {
            if (string.IsNullOrEmpty(sender.Text))
            {
                ListCredentials.ItemsSource = _bindableCredentials;
                ViewStack.SelectedIndex = _controller.Credentials.Count == 0 ? (int)Pages.None : (int)Pages.Keyring;
            }
            else
            {
                var filtered = _controller.Credentials.Where(x => x.Label.ToLower().Contains(sender.Text.ToLower()) || x.Value.Username.ToLower().Contains(sender.Text.ToLower()) || x.Value.Url.ToString().ToLower().Contains(sender.Text.ToLower()));
                ListCredentials.ItemsSource = filtered.ToBindableCredentialSelectionItems();
                ViewStack.SelectedIndex = filtered.Any() ? (int)Pages.Keyring : (int)Pages.NoneSearch;
            }
        }
    }
}
