namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// A model of advanced options for a download
/// </summary>
public class AdvancedDownloadOptions
{
    /// <summary>
    /// The username to use for authenticating the download (if available)
    /// </summary>
    public string? Username { get; set; }
    /// <summary>
    /// The password to use for authenticating the download (if available)
    /// </summary>
    public string? Password { get; set; }
    /// <summary>
    /// Whether or not to apply a speed limit to the download
    /// </summary>
    public bool LimitSpeed { get; init; }
    /// <summary>
    /// Whether or not to prefer the AV1 codec when downloading
    /// </summary>
    public bool PreferAV1 { get; init; }
    /// <summary>
    /// Whether or not to split chapters for the download
    /// </summary>
    public bool SplitChapters { get; init; }
    /// <summary>
    /// Whether or not to crop the download's thumbnail
    /// </summary>
    public bool CropThumbnail { get; init; }
    /// <summary>
    /// The timeframe to use when downloading the video (null for the whole video)
    /// </summary>
    public Timeframe? Timeframe { get; init; }

    /// <summary>
    /// Constructs an AdvancedDownloadOptions
    /// </summary>
    public AdvancedDownloadOptions()
    {
        Username = null;
        Password = null;
        LimitSpeed = false;
        PreferAV1 = false;
        SplitChapters = false;
        CropThumbnail = false;
        Timeframe = null;
    }
}
