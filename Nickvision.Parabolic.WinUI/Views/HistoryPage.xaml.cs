using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Nickvision.Desktop.Globalization;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.WinUI.Helpers;
using System;
using System.Collections.Generic;
using System.Diagnostics;
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
    private readonly ITranslationService _translationService;
    private List<BindableHistoricDownloadSelectionItem> _historicDownloads;

    public HistoryPage(HistoryViewController controller, ITranslationService translationService)
    {
        InitializeComponent();
        _controller = controller;
        _translationService = translationService;
        _historicDownloads = [];
        LblHistory.Text = _translationService._("History");
        BtnSort.Label = _translationService._("Sort");
        TglSortNewest.Text = _translationService._("Newest");
        TglSortOldest.Text = _translationService._("Oldest");
        BtnLength.Label = _translationService._("Save Length");
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
        LblClearAll.Text = _translationService._("Clear All");
        TxtSearch.PlaceholderText = _translationService._("Search...");
        LblLoading.Text = _translationService._("Please wait...");
        StatusNone.Title = _translationService._("No History");
        StatusNone.Description = _translationService._("There are no downloads in your history");
        StatusNoneSearch.Title = _translationService._("No History");
        StatusNoneSearch.Description = _translationService._("There are no downloads found with the current filters");
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
            Title = _translationService._("Clear All History?"),
            Content = _translationService._("Are you sure you want to clear all download history? This action is irreversible"),
            PrimaryButtonText = _translationService._("Yes"),
            CloseButtonText = _translationService._("No"),
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

    private async void Play(object? sender, RoutedEventArgs e)
    {
        var path = ((sender as Button)!.Tag as string)!;
        try
        {
            using var _ = Process.Start(new ProcessStartInfo()
            {
                FileName = path,
                UseShellExecute = true
            });
        }
        catch
        {
            await Launcher.LaunchFileAsync(await StorageFile.GetFileFromPathAsync(path));
        }
    }

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
        _historicDownloads = (await _controller.GetAllAsync()).ToBindableHistoricDownloadSelectionItems();
        ListDownloads.ItemsSource = _historicDownloads;
        ViewStack.SelectedIndex = _historicDownloads.Count == 0 ? (int)Pages.None : (int)Pages.History;
    }
}
