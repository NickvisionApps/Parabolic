using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Nickvision.Desktop.Application;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.IO;
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
        History
    }

    private readonly HistoryPageController _controller;

    public HistoryPage(HistoryPageController controller)
    {
        InitializeComponent();
        _controller = controller;
        BtnClearAll.Label = _controller.Translator._("Clear All");
        BtnSort.Label = _controller.Translator._("Sort");
        TglSortNewest.Text = _controller.Translator._("Newest");
        TglSortOldest.Text = _controller.Translator._("Oldest");
        BtnLength.Label = _controller.Translator._("Save Length");
        TglLengthNever.Text = _controller.Translator._("Never");
        TglLengthOneDay.Text = _controller.Translator._("1 Day");
        TglLengthOneWeek.Text = _controller.Translator._("1 Week");
        TglLengthOneMonth.Text = _controller.Translator._("1 Month");
        TglLengthThreeMonths.Text = _controller.Translator._("3 Months");
        TglLengthSixMonths.Text = _controller.Translator._("6 Months");
        TglLengthOneYear.Text = _controller.Translator._("1 Year");
        TglLengthForever.Text = _controller.Translator._("Forever");
        BtnDownloadAgain.Label = _controller.Translator._("Download Again");
        BtnPlay.Label = _controller.Translator._("Play");
        BtnRemove.Label = _controller.Translator._("Remove");
        TxtSerach.PlaceholderText = _controller.Translator._("Search...");
        LblLoading.Text = _controller.Translator._("Please wait...");
        StatusNone.Title = _controller.Translator._("No History");
        StatusNone.Description = _controller.Translator._("There are no downloads in your history");
        MenuClearAll.Text = _controller.Translator._("Clear All");
        MenuDownloadAgain.Text = _controller.Translator._("Download Again");
        MenuPlay.Text = _controller.Translator._("Play");
        MenuRemove.Text = _controller.Translator._("Remove");
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
        switch (_controller.Length)
        {
            case HistoryLength.Never:
                TglLengthNever.IsChecked = true;
                break;
            case HistoryLength.OneDay:
                TglLengthOneDay.IsChecked = true;
                break;
            case HistoryLength.OneMonth:
                TglLengthOneMonth.IsChecked = true;
                break;
            case HistoryLength.ThreeMonths:
                TglLengthThreeMonths.IsChecked = true;
                break;
            case HistoryLength.SixMonths:
                TglLengthSixMonths.IsChecked = true;
                break;
            case HistoryLength.OneYear:
                TglLengthOneYear.IsChecked = true;
                break;
            case HistoryLength.Forever:
                TglLengthForever.IsChecked = true;
                break;
            default:
                TglLengthOneWeek.IsChecked = true;
                break;
        }
        await LoadDownloadsAsync();
    }

    private async void TglSort_Click(object? sender, RoutedEventArgs e)
    {
        _controller.SortNewest = TglSortNewest.IsChecked;
        await LoadDownloadsAsync();
    }

    private async void TglLength_Click(object? sender, RoutedEventArgs e)
    {
        if (TglLengthNever.IsChecked)
        {
            _controller.Length = HistoryLength.Never;
        }
        else if (TglLengthOneDay.IsChecked)
        {
            _controller.Length = HistoryLength.OneDay;
        }
        else if (TglLengthOneWeek.IsChecked)
        {
            _controller.Length = HistoryLength.OneWeek;
        }
        else if (TglLengthOneMonth.IsChecked)
        {
            _controller.Length = HistoryLength.OneMonth;
        }
        else if (TglLengthThreeMonths.IsChecked)
        {
            _controller.Length = HistoryLength.ThreeMonths;
        }
        else if (TglLengthSixMonths.IsChecked)
        {
            _controller.Length = HistoryLength.SixMonths;
        }
        else if (TglLengthOneYear.IsChecked)
        {
            _controller.Length = HistoryLength.OneYear;
        }
        else if (TglLengthForever.IsChecked)
        {
            _controller.Length = HistoryLength.Forever;
        }
        await LoadDownloadsAsync();
    }

    private async void ClearAll(object? sender, RoutedEventArgs e)
    {
        var confirmDialog = new ContentDialog()
        {
            Title = _controller.Translator._("Clear All History?"),
            Content = _controller.Translator._("Are you sure you want to clear all download history? This action is irreversible."),
            PrimaryButtonText = _controller.Translator._("Yes"),
            CloseButtonText = _controller.Translator._("No"),
            DefaultButton = ContentDialogButton.Close,
            XamlRoot = XamlRoot,
            RequestedTheme = RequestedTheme
        };
        var res = await confirmDialog.ShowAsync();
        if (res == ContentDialogResult.Primary)
        {
            await _controller.ClearAllAsync();
            await LoadDownloadsAsync();
        }
    }

    private void DownloadAgain(object? sender, RoutedEventArgs e)
    {
        var selected = ListDownloads.SelectedItems.Cast<SelectionItem<HistoricDownload>>().First();
        _controller.RequestDownload(selected.Value.Url);
    }

    private async void Play(object? sender, RoutedEventArgs e)
    {
        var selected = ListDownloads.SelectedItems.Cast<SelectionItem<HistoricDownload>>().First();
        await Launcher.LaunchFileAsync(await StorageFile.GetFileFromPathAsync(selected.Value.Path));
    }

    private async void Remove(object? sender, RoutedEventArgs e)
    {
        await _controller.RemoveAsync(ListDownloads.SelectedItems.Cast<SelectionItem<HistoricDownload>>());
        await LoadDownloadsAsync();
    }

    private void ListDownloads_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        var selected = ListDownloads.SelectedItems.Cast<SelectionItem<HistoricDownload>>();
        var any = selected.Any();
        var one = selected.Count() == 1;
        var firstExists = one && File.Exists(selected.First().Value.Path);
        BtnDownloadAgain.IsEnabled = one;
        BtnPlay.IsEnabled = firstExists;
        BtnRemove.IsEnabled = any;
        MenuDownloadAgain.IsEnabled = one;
        MenuPlay.IsEnabled = firstExists;
        MenuRemove.IsEnabled = any;
    }

    private async Task LoadDownloadsAsync()
    {
        ViewStack.SelectedIndex = (int)Pages.Loading;
        var downloads = await _controller.GetAllAsync();
        ListDownloads.ItemsSource = downloads;
        ViewStack.SelectedIndex = downloads.Count == 0 ? (int)Pages.None : (int)Pages.History;
    }
}
