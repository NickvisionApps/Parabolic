using CommunityToolkit.WinUI.Controls;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Nickvision.Aura;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.IO;
using System.Linq;
using Windows.Storage;
using Windows.System;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.WinUI.Controls;

/// <summary>
/// A dialog to manage history
/// </summary>
public sealed partial class HistoryDialog : ContentDialog
{
    private readonly DownloadHistory _history;

    /// <summary>
    /// Occurs when a download is requested to be downloaded again
    /// </summary>
    public event EventHandler<string>? DownloadAgainRequested;

    /// <summary>
    /// Constructs a HistoryDialog
    /// </summary>
    /// <param name="history">DownloadHistory</param>
    public HistoryDialog(DownloadHistory history)
    {
        InitializeComponent();
        _history = history;
        //Localize Strings
        Title = _("History");
        CloseButtonText = _("Close");
        LblMessage.Text = _("Manage previously downloaded media.");
        ToolTipService.SetToolTip(BtnClear, _("Clear History"));
        TxtSearch.PlaceholderText = _("Search history");
        StatusNoHistory.Title = _("No Previous Downloads");
        //Load
        TxtSearch.Visibility = _history.History.Count > 0 ? Visibility.Visible : Visibility.Collapsed;
        ViewStack.CurrentPageName = _history.History.Count > 0 ? "History" : "NoHistory";
        foreach (var pair in _history.History.OrderByDescending(x => x.Value.Date))
        {
            var row = new SettingsCard();
            if (string.IsNullOrEmpty(pair.Value.Title))
            {
                row.Header = pair.Key;
            }
            else
            {
                row.Header = pair.Value.Title;
                row.Description = pair.Key;
            }
            var btnStack = new StackPanel()
            {
                Orientation = Orientation.Horizontal,
                Spacing = 6
            };
            if (File.Exists(pair.Value.Path))
            {
                var openButton = new Button()
                {
                    VerticalAlignment = VerticalAlignment.Center,
                    Content = new SymbolIcon(Symbol.Play)
                };
                ToolTipService.SetToolTip(openButton, _("Play"));
                openButton.Click += async (sender, e) => await Launcher.LaunchFileAsync(await StorageFile.GetFileFromPathAsync(pair.Value.Path));
                btnStack.Children.Add(openButton);
            }
            var downloadButton = new Button()
            {
                VerticalAlignment = VerticalAlignment.Center,
                Content = new SymbolIcon(Symbol.Refresh)
            };
            ToolTipService.SetToolTip(downloadButton, _("Download Again"));
            downloadButton.Click += (sender, e) =>
            {
                Hide();
                DownloadAgainRequested?.Invoke(this, pair.Key);
            };
            btnStack.Children.Add(downloadButton);
            row.Content = btnStack;
            ListHistory.Children.Add(row);
        }
    }

    /// <summary>
    /// Occurs when the ScrollViewer's size is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SizeChangedEventArgs</param>
    private void ScrollViewer_SizeChanged(object sender, SizeChangedEventArgs e) => StackPanel.Margin = new Thickness(0, 0, ScrollViewer.ComputedVerticalScrollBarVisibility == Visibility.Visible ? 14 : 0, 0);

    /// <summary>
    /// Occurs when the clear history button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Clear(object sender, RoutedEventArgs e)
    {
        _history.History.Clear();
        Aura.Active.SaveConfig("downloadHistory");
        TxtSearch.Visibility = Visibility.Collapsed;
        ViewStack.CurrentPageName = "NoHistory";
        ListHistory.Children.Clear();
    }

    /// <summary>
    /// Occurs when the TxtSearch's text is changed
    /// </summary>
    /// <param name="sender">AutoSuggestBox</param>
    /// <param name="args">AutoSuggestBoxTextChangedEventArgs</param>
    private void TxtSearch_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs args)
    {
        var search = TxtSearch.Text.ToLower();
        foreach (SettingsCard row in ListHistory.Children)
        {
            ListHistory.Visibility = string.IsNullOrEmpty(search) || row.Header.ToString()!.ToLower().Contains(search) ? Visibility.Visible : Visibility.Collapsed;
        }
    }
}
