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
/// A model of a video download
/// </summary>
public class Download
{
    private int _pid;

    public string VideoUrl { get; init; }
    public MediaFileType FileType { get; init; }
    public string Path { get; private set; }
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
    }

    /// <summary>
    /// Gets whether or not a video url is valid
    /// </summary>
    /// <param name="url">The video url to check</param>
    /// <returns>True if valid, else false</returns>
    public static bool GetIsValidVideoUrl(string url)
    {
        return true;
    }

    /// <summary>
    /// Runs the download
    /// </summary>
    /// <param name="embedMetadata">Whether or not to embed video metadata in the downloaded file</param>
    /// <returns>True if successful, else false</returns>
    public bool Run(bool embedMetadata)
    {
        if (string.IsNullOrEmpty(System.IO.Path.GetFileName(Path)))
        {
            //Path += VideoTitle
        }
        return false;
    }

    /// <summary>
    /// Stops the download
    /// </summary>
    public void Stop()
    {

    }
}
