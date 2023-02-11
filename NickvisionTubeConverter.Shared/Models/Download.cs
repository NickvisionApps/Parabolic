using System.IO;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// Qualities for a Download
/// </summary>
public enum Quality
{
    Best = 0,
    Good,
    Worst
}

/// <summary>
/// Subtitle types for a download
/// </summary>
public enum Subtitle
{
    None = 0,
    VTT,
    SRT
}

/// <summary>
/// Statuses for when a download is checked
/// </summary>
public enum DownloadCheckStatus
{
    Valid = 1,
    EmptyVideoUrl = 2,
    InvalidVideoUrl = 4,
    InvalidSaveFolder = 8
}

/// <summary>
/// A model of a video download
/// </summary>
public class Download
{
    private bool _isValidUrl;
    private int _pid;

    public string VideoUrl { get; init; }
    public MediaFileType FileType { get; init; }
    public string Path { get; init; }
    public Quality Quality { get; init; }
    public Subtitle Subtitle { get; init; }
    public string Log { get; private set; }
    public bool IsDone { get; private set; }

    /// <summary>
    /// Constructs a Download
    /// </summary>
    /// <param name="videoUrl">The url of the video to download</param>
    /// <param name="fileType">The file type to download the video as</param>
    /// <param name="saveFolder">The folder to save the download to</param>
    /// <param name="newFilename">The filename to save the download as</param>
    /// <param name="quality">The quality of the download</param>
    /// <param name="subtitle">The subtitles for the download</param>
    public Download(string videoUrl, MediaFileType fileType, string saveFolder, string newFilename, Quality quality = Quality.Best, Subtitle subtitle = Subtitle.None)
    {
        VideoUrl = videoUrl;
        FileType = fileType;
        Path = $"{saveFolder}{System.IO.Path.DirectorySeparatorChar}{newFilename}";
        Quality = quality;
        Subtitle = subtitle;
        Log = "";
        IsDone = false;
        _pid = -1;
        var videoTitle = GetVideoTitle();
        _isValidUrl = !string.IsNullOrEmpty(videoTitle);
        if(string.IsNullOrEmpty(newFilename))
        {
            Path += videoTitle;
        }
    }

    /// <summary>
    /// The validation status of the Download object
    /// </summary>
    public DownloadCheckStatus CheckStatus
    {
        get
        {
            DownloadCheckStatus result = 0;
            if(string.IsNullOrEmpty(VideoUrl))
            {
                result |= DownloadCheckStatus.EmptyVideoUrl;
            }
            if(!_isValidUrl)
            {
                result |= DownloadCheckStatus.InvalidVideoUrl;
            }
            if(!Directory.Exists(Path))
            {
                result |= DownloadCheckStatus.InvalidSaveFolder;
            }
            if(result == 0)
            {
                result = DownloadCheckStatus.Valid;
            }
            return result;
        }
    }

    /// <summary>
    /// Runs the download
    /// </summary>
    /// <param name="embedMetadata">Whether or not to embed video metadata in the downloaded file</param>
    /// <returns>True if successful, else false</returns>
    public bool Run(bool embedMetadata)
    {
        return false;
    }

    /// <summary>
    /// Stops the download
    /// </summary>
    public void Stop()
    {

    }

    /// <summary>
    /// Gets the title of the source video
    /// </summary>
    /// <returns></returns>
    private string GetVideoTitle()
    {
        return "";
    }

    /// <summary>
    /// Gets whether or not a source video contains subtitles
    /// </summary>
    /// <returns></returns>
    private bool GetContainsSubtitles()
    {
        return false;
    }
}
