using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.Shared.Controllers;

/// <summary>
/// Statuses for when a download is checked
/// </summary>
public enum DownloadCheckStatus
{
    Valid = 1,
    EmptyVideoUrl = 2,
    InvalidVideoUrl = 4,
    InvalidSaveFolder = 8,
    PlaylistNotSupported = 16
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
    /// The downloads created by the dialog
    /// </summary>
    public List<Download> Downloads { get; init; }
    /// <summary>
    /// Whether or not the dialog was accepted (response)
    /// </summary>
    public bool Accepted { get; set; }

    /// <summary>
    /// The previously used save folder
    /// </summary>
    public string PreviousSaveFolder => Directory.Exists(Configuration.Current.PreviousSaveFolder) ? Configuration.Current.PreviousSaveFolder : "";
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
        Downloads = new List<Download>();
        Accepted = false;
    }

    /// <summary>
    /// Searches for information about a video url
    /// </summary>
    /// <param name="videoUrl">The video url</param>
    /// <returns>A VideoUrlInfo object for the url or null if url is invalid</returns>
    public async Task<VideoUrlInfo?> SearchUrlAsync(string videoUrl) => await VideoUrlInfo.GetAsync(videoUrl);

    /// <summary>
    /// Populates the downloads list
    /// </summary>
    /// <param name="videoUrlInfo">The VideoUrlInfo object</param>
    /// <param name="mediaFileType">The media file type to download</param>
    /// <param name="quality">The quality of the downloads</param>
    /// <param name="subtitles">The subtitle format of the downloads</param>
    /// <param name="saveFolder">The save folder of the downloads</param>
    public void PopulateDownloads(VideoUrlInfo videoUrlInfo, MediaFileType mediaFileType, Quality quality, Subtitle subtitles, string saveFolder)
    {
        Downloads.Clear();
        foreach (var video in videoUrlInfo.Videos)
        {
            if (video.ToDownload)
            {
                Downloads.Add(new Download(video.Url, mediaFileType, saveFolder, video.Title, quality, subtitles));
            }
        }
        Configuration.Current.PreviousSaveFolder = saveFolder;
        Configuration.Current.PreviousMediaFileType = mediaFileType;
        Configuration.Current.Save();
    }
}
