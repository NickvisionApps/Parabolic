using Microsoft.UI.Xaml.Controls;
using NickvisionTubeConverter.Shared.Models;

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
    public VideoRow(VideoInfo videoInfo)
    {
        InitializeComponent();
        _videoInfo = videoInfo;
        DataContext = _videoInfo;
    }
}
