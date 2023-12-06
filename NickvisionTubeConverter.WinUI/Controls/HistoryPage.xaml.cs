using CommunityToolkit.WinUI.Controls;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.IO;
using System.Linq;
using Windows.Storage;
using Windows.System;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.WinUI.Controls;

/// <summary>
/// A page to manage history
/// </summary>
public sealed partial class HistoryPage : UserControl
{
    private readonly DownloadHistory _history;

    /// <summary>
    /// Occurs when a download is requested to be downloaded again
    /// </summary>
    public event EventHandler<string>? DownloadAgainRequested;

    /// <summary>
    /// Constructs a HistoryPage
    /// </summary>
    /// <param name="history">DownloadHistory</param>
    public HistoryPage(DownloadHistory history)
    {
        InitializeComponent();
        _history = history;
        //Localize Strings
        LblTitle.Text = _("History");
        LblBtnClear.Text = _("Clear");
        StatusNoHistory.Title = _("No Previous Downloads");
        //Load
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
                DownloadAgainRequested?.Invoke(this, pair.Key);
            };
            btnStack.Children.Add(downloadButton);
            row.Content = btnStack;
            ListHistory.Children.Add(row);
        }
    }

    /// <summary>
    /// Occurs when the clear history button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Clear(object sender, RoutedEventArgs e)
    {
        _history.History.Clear();
        _history.Save();
        ViewStack.CurrentPageName = "NoHistory";
        ListHistory.Children.Clear();
    }
}
