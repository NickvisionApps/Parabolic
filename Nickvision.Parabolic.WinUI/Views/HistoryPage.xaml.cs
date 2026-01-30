using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Nickvision.Desktop.Application;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.System;

namespace Nickvision.Parabolic.WinUI.Views;

public sealed partial class HistoryPage : Page
{
    private enum Pages
    {
        Loading = 0,
        None,
        NoneSearch,
        History
    }

    private readonly HistoryViewController _controller;
    private IReadOnlyList<SelectionItem<HistoricDownload>> _historicDownloads;

    public HistoryPage(HistoryViewController controller)
    {
        InitializeComponent();
        _controller = controller;
        _historicDownloads = [];
        LblHistory.Text = _controller.Translator._("History");
        BtnSort.Label = _controller.Translator._("Sort");
        TglSortNewest.Text = _controller.Translator._("Newest");
        TglSortOldest.Text = _controller.Translator._("Oldest");
        BtnLength.Label = _controller.Translator._("Save Length");
        foreach (var length in _controller.Lengths)
        {
            var item = new RadioMenuFlyoutItem()
            {
                Tag = length.Value,
                GroupName = "Length",
                Text = length.Label,
                IsChecked = length.ShouldSelect,

            };
            item.Click += TglLength_Click;
            MenuLength.Items.Add(item);
        }
        LblClearAll.Text = _controller.Translator._("Clear All");
        TxtSearch.PlaceholderText = _controller.Translator._("Search...");
        LblLoading.Text = _controller.Translator._("Please wait...");
        StatusNone.Title = _controller.Translator._("No History");
        StatusNone.Description = _controller.Translator._("There are no downloads in your history");
        StatusNoneSearch.Title = _controller.Translator._("No History");
        StatusNoneSearch.Description = _controller.Translator._("There are no downloads found with the current filters");
    }

    private async void Page_Loaded(object? sender, RoutedEventArgs e)
    {
        ViewStack.SelectedIndex = (int)Pages.Loading;
        if (_controller.SortNewest)
        {
            TglSortNewest.IsChecked = true;
        }
        else
        {
            TglSortOldest.IsChecked = true;
        }
        await LoadDownloadsAsync();
    }

    private async void ClearAll(object? sender, RoutedEventArgs e)
    {
        var confirmDialog = new ContentDialog()
        {
            Title = _controller.Translator._("Clear All History?"),
            Content = _controller.Translator._("Are you sure you want to clear all download history? This action is irreversible"),
            PrimaryButtonText = _controller.Translator._("Yes"),
            CloseButtonText = _controller.Translator._("No"),
            DefaultButton = ContentDialogButton.Close,
            XamlRoot = XamlRoot,
            RequestedTheme = ActualTheme
        };
        if ((await confirmDialog.ShowAsync()) == ContentDialogResult.Primary)
        {
            await _controller.ClearAllAsync();
            await LoadDownloadsAsync();
        }
    }

    private void DownloadAgain(object? sender, RoutedEventArgs e) => _controller.RequestDownload(((sender as Button)!.Tag as Uri)!);

    private async void Play(object? sender, RoutedEventArgs e) => await Launcher.LaunchFileAsync(await StorageFile.GetFileFromPathAsync(((sender as Button)!.Tag as string)!));

    private async void Remove(object? sender, RoutedEventArgs e)
    {
        var tag = ((sender as Button)!.Tag as Uri)!;
        await _controller.RemoveAsync(tag);
        await LoadDownloadsAsync();
    }

    private async void TglSort_Click(object? sender, RoutedEventArgs e)
    {
        _controller.SortNewest = TglSortNewest.IsChecked;
        await LoadDownloadsAsync();
    }

    private async void TglLength_Click(object? sender, RoutedEventArgs e)
    {
        _controller.Length = (HistoryLength)(sender as RadioMenuFlyoutItem)!.Tag;
        await LoadDownloadsAsync();
    }

    private void TxtSearch_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs e)
    {
        if (_historicDownloads.Count == 0)
        {
            return;
        }
        if (e.Reason == AutoSuggestionBoxTextChangeReason.UserInput)
        {
            if (string.IsNullOrEmpty(sender.Text))
            {
                ListDownloads.ItemsSource = _historicDownloads;
                ViewStack.SelectedIndex = _historicDownloads.Count == 0 ? (int)Pages.None : (int)Pages.History;
            }
            else
            {
                var filtered = _historicDownloads.Where(x => x.Label.ToLower().Contains(sender.Text.ToLower()));
                ListDownloads.ItemsSource = filtered;
                ViewStack.SelectedIndex = filtered.Any() ? (int)Pages.History : (int)Pages.NoneSearch;
            }
        }
    }

    private async Task LoadDownloadsAsync()
    {
        ViewStack.SelectedIndex = (int)Pages.Loading;
        TxtSearch.Text = string.Empty;
        _historicDownloads = await _controller.GetAllAsync();
        ListDownloads.ItemsSource = _historicDownloads;
        ViewStack.SelectedIndex = _historicDownloads.Count == 0 ? (int)Pages.None : (int)Pages.History;
    }
}
