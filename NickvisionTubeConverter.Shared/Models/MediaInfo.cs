namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// A model of information about a media
/// </summary>
public class MediaInfo
{
    private string _title;

    /// <summary>
    /// The media url
    /// </summary>
    public string Url { get; init; }
    /// <summary>
    /// The title of the media
    /// </summary>
    public string OriginalTitle { get; init; }
    /// <summary>
    /// Media duration in seconds
    /// </summary>
    public double Duration { get; init; }
    /// <summary>
    /// Position of media in playlist, starting with 1 (0 means it's not part of playlist)
    /// </summary>
    public uint PlaylistPosition { get; init; }
    /// <summary>
    /// Whether or not to download the media
    /// </summary>
    public bool ToDownload { get; set; }

    /// <summary>
    /// Constructs a MediaInfo
    /// </summary>
    /// <param name="url">The url of the media</param>
    /// <param name="title">The title of the media</param>
    /// <param name="duration">The duration of the media in seconds</param>
    /// <param name="playlistPosition">Position in playlist starting with 1, or 0 if not in playlist</param>
    public MediaInfo(string url, string title, double duration, uint playlistPosition)
    {
        _title = title;
        Url = url;
        OriginalTitle = title;
        Duration = duration;
        if (Duration == double.NaN)
        {
            Duration = 0;
        }
        PlaylistPosition = playlistPosition;
        ToDownload = true;
    }

    /// <summary>
    /// The title to use for downloading
    /// </summary>
    public string Title
    {
        get => _title;

        set => _title = !string.IsNullOrEmpty(value) ? value : OriginalTitle;
    }
}