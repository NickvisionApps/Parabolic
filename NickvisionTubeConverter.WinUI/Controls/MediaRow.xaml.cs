using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using NickvisionTubeConverter.Shared.Models;
using System;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.WinUI.Controls;

/// <summary>
/// A row for a media in the AddDownloadDialog
/// </summary>
public sealed partial class MediaRow : UserControl
{
    private readonly MediaInfo _mediaInfo;
    private string _numberString;

    /// <summary>
    /// Occurs when the check to select the download is changed
    /// </summary>
    public event EventHandler<EventArgs>? OnSelectionChanged;

    /// <summary>
    /// Constructs a MediaRow
    /// </summary>
    /// <param name="mediaInfo">MediaInfo</param>
    public MediaRow(MediaInfo mediaInfo)
    {
        InitializeComponent();
        _mediaInfo = mediaInfo;
        _numberString = "";
        //Localize Strings
        ToolTipService.SetToolTip(BtnUndo, _("Undo Title Editing"));
        //Load
        ViewStack.CurrentPageName = _mediaInfo.PlaylistPosition > 0 ? "Playlist" : "Single";
        LblFileName.Text = _("File Name");
        LblUrl.Text = _mediaInfo.Url;
        ChkDownload.IsChecked = _mediaInfo.ToDownload;
        TxtTitle.Text = _mediaInfo.Title;
    }

    /// <summary>
    /// Whether or not the row is checked
    /// </summary>
    public bool IsChecked
    {
        get => ChkDownload.IsChecked ?? false;

        set => ChkDownload.IsChecked = value;
    }

    /// <summary>
    /// Updates the title of the row
    /// </summary>
    /// <param name="numbered"></param>
    public void UpdateTitle(bool numbered)
    {
        TxtTitle.Text = _mediaInfo.Title;
        _numberString = numbered ? $"{_mediaInfo.PlaylistPosition} - " : "";
    }

    /// <summary>
    /// Occurs when the ChkDownload's checked is toggled
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void ChkDownload_CheckedToggled(object sender, RoutedEventArgs e)
    {
        _mediaInfo.ToDownload = ChkDownload.IsChecked ?? false;
        OnSelectionChanged?.Invoke(this, new EventArgs());
    }

    /// <summary>
    /// Occurs when the TxtTitle's text is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">TextChangedEventArgs</param>
    private void TxtTitle_TextChanged(object sender, TextChangedEventArgs e) => _mediaInfo.Title = TxtTitle.Text;

    /// <summary>
    /// Occurs when the undo button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Undo(object sender, RoutedEventArgs e) => TxtTitle.Text = $"{_numberString}{_mediaInfo.OriginalTitle}";
}
