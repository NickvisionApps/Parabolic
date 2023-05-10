using Microsoft.UI.Xaml.Controls;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System.IO;

namespace NickvisionTubeConverter.WinUI.Controls;

/// <summary>
/// A row for managing a media
/// </summary>
public sealed partial class MediaRow : UserControl
{
    private MediaInfo _mediaInfo;

    /// <summary>
    /// Constructs a MediaRow
    /// </summary>
    /// <param name="mediaInfo">The MediaInfo object</param>
    /// <param name="localizer">The Localizer</param>
    public MediaRow(MediaInfo mediaInfo, Localizer localizer)
    {
        InitializeComponent();
        _mediaInfo = mediaInfo;
        DataContext = _mediaInfo;
    }

    /// <summary>
    /// Occurs when the title textbox is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">TextChangedEventArgs</param>
    private void TxtTitle_TextChanged(object sender, TextChangedEventArgs e)
    {
        foreach (var c in Path.GetInvalidFileNameChars())
        {
            TxtTitle.Text = TxtTitle.Text.Replace(c, '_');
        }
        if (TxtTitle.Text != _mediaInfo.Title)
        {
            TxtTitle.Text = _mediaInfo.Title;
        }
    }
}
