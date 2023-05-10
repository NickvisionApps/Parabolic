using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// A model of information about a media
/// </summary>
public class MediaInfo : INotifyPropertyChanged
{
    private string _title;
    private bool _toDownload;

    /// <summary>
    /// The media url
    /// </summary>
    public string Url { get; init; }
    /// <summary>
    /// The title of the media
    /// </summary>
    public string OriginalTitle { get; init; }
    /// <summary>
    /// Whether or not the media is part of a playlist 
    /// </summary>
    public bool IsPartOfPlaylist { get; init; }

    public event PropertyChangedEventHandler? PropertyChanged;

    /// <summary>
    /// Constructs a MediaInfo
    /// </summary>
    /// <param name="url">The url of the media</param>
    /// <param name="title">The title of the media</param>
    /// <param name="partOfPlaylist">Whether or not the media is part of a playlist</param>
    public MediaInfo(string url, string title, bool partOfPlaylist = false)
    {
        _title = title;
        _toDownload = true;
        Url = url;
        OriginalTitle = title;
        IsPartOfPlaylist = partOfPlaylist;
    }

    /// <summary>
    /// The title to use for downloading
    /// </summary>
    public string Title
    {
        get => _title;

        set
        {
            _title = !string.IsNullOrEmpty(value) ? value : OriginalTitle;
            NotifyPropertyChanged();
        }
    }

    /// <summary>
    /// Whether or not to download the media
    /// </summary>
    public bool ToDownload
    {
        get => _toDownload;

        set
        {
            _toDownload = value;
            NotifyPropertyChanged();
        }
    }

    private void NotifyPropertyChanged([CallerMemberName] string propertyName = "") => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
}