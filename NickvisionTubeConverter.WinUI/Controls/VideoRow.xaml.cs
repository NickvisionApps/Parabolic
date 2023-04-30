using Microsoft.UI.Xaml.Controls;
using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System.IO;

namespace NickvisionTubeConverter.WinUI.Controls;

/// <summary>
/// A row for managing a video
/// </summary>
public sealed partial class VideoRow : UserControl
{
    private VideoInfo _videoInfo;

    /// <summary>
    /// Constructs a VideoRow
    /// </summary>
    /// <param name="videoInfo">The VideoInfo object</param>
    /// <param name="localizer">The Localizer</param>
    public VideoRow(VideoInfo videoInfo, Localizer localizer)
    {
        InitializeComponent();
        _videoInfo = videoInfo;
        DataContext = _videoInfo;
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
        if (TxtTitle.Text != _videoInfo.Title)
        {
            TxtTitle.Text = _videoInfo.Title;
        }
    }
}
