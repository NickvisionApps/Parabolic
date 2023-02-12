using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System.IO;

namespace NickvisionTubeConverter.Shared.Controllers;

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
/// A controller for a AddDownloadDialog
/// </summary>
public class AddDownloadDialogController
{
    /// <summary>
    /// The localizer to get translated strings from
    /// </summary>
    public Localizer Localizer { get; init; }
    /// <summary>
    /// The download represented by the controller
    /// </summary>
    public Download? Download { get; private set; }
    /// <summary>
    /// Whether or not the dialog was accepted (response)
    /// </summary>
    public bool Accepted { get; set; }

    /// <summary>
    /// The previously used save folder
    /// </summary>
    public string PreviousSaveFolder => Path.Exists(Configuration.Current.PreviousSaveFolder) ? Configuration.Current.PreviousSaveFolder : "";
    /// <summary>
    /// The previously used MediaFileType
    /// </summary>
    public MediaFileType PreviousMediaFileType => Configuration.Current.PreviousMediaFileType;
    
    /// <summary>
    /// Constructs a AddDownloadDialogController
    /// </summary>
    public AddDownloadDialogController(Localizer localizer)
    {
        Localizer = localizer;
        Download = null;
        Accepted = false;
    }

    /// <summary>
    /// Updates the Download object
    /// </summary>
    /// <param name="videoUrl">The url of the video to download</param>
    /// <param name="mediaFileType">The file type to download the video as</param>
    /// <param name="saveFolder">The folder to save the download to</param>
    /// <param name="newFilename">The filename to save the download as</param>
    /// <param name="quality">The quality of the download</param>
    /// <param name="subtitles">The subtitles for the download</param>
    /// <returns>The DownloadCheckStatus</returns>
    public DownloadCheckStatus UpdateDownload(string videoUrl, MediaFileType mediaFileType, string saveFolder, string newFilename, Quality quality, Subtitle subtitles)
    {
        DownloadCheckStatus result = 0;
        if (string.IsNullOrEmpty(videoUrl))
        {
            result |= DownloadCheckStatus.EmptyVideoUrl;
        }
        if (!Download.GetIsValidVideoUrl(videoUrl).GetAwaiter().GetResult())
        {
            result |= DownloadCheckStatus.InvalidVideoUrl;
        }
        if (!Directory.Exists(saveFolder))
        {
            result |= DownloadCheckStatus.InvalidSaveFolder;
        }
        if (result != 0)
        {
            return result;
        }
        Download = new Download(videoUrl, mediaFileType, saveFolder, newFilename, quality, subtitles);
        Configuration.Current.PreviousSaveFolder = saveFolder;
        Configuration.Current.PreviousMediaFileType = mediaFileType;
        Configuration.Current.Save();
        return DownloadCheckStatus.Valid;
    }
}
