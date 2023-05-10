using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.Shared.Controllers;

/// <summary>
/// Statuses for when a download is checked
/// </summary>
public enum DownloadCheckStatus
{
    Valid = 1,
    EmptyMediaUrl = 2,
    InvalidMediaUrl = 4,
    InvalidSaveFolder = 8,
    PlaylistNotSupported = 16
}

/// <summary>
/// A controller for a AddDownloadDialog
/// </summary>
public class AddDownloadDialogController
{
    /// <summary>
    /// Gets the AppInfo object
    /// </summary>
    public AppInfo AppInfo => AppInfo.Current;
    /// <summary>
    /// The localizer to get translated strings from
    /// </summary>
    public Localizer Localizer { get; init; }
    /// <summary>
    /// The downloads created by the dialog
    /// </summary>
    public List<Download> Downloads { get; init; }

    /// <summary>
    /// The previously used save folder
    /// </summary>
    public string PreviousSaveFolder => Directory.Exists(Configuration.Current.PreviousSaveFolder) ? Configuration.Current.PreviousSaveFolder : "";
    /// <summary>
    /// The previously used MediaFileType
    /// </summary>
    public MediaFileType PreviousMediaFileType => Configuration.Current.PreviousMediaFileType;
    /// <summary>
    /// The speed limit in the configuration
    /// </summary>
    public uint CurrentSpeedLimit => Configuration.Current.SpeedLimit;

    /// <summary>
    /// Constructs a AddDownloadDialogController
    /// </summary>
    public AddDownloadDialogController(Localizer localizer)
    {
        Localizer = localizer;
        Downloads = new List<Download>();
    }

    /// <summary>
    /// Searches for information about a media url
    /// </summary>
    /// <param name="mediaUrl">The media url</param>
    /// <returns>A MediaUrlInfo object for the url or null if url is invalid</returns>
    public async Task<MediaUrlInfo?> SearchUrlAsync(string mediaUrl) => await MediaUrlInfo.GetAsync(mediaUrl);

    /// <summary>
    /// Numbers the titles in a MediaUrlInfo object
    /// </summary>
    /// <param name="mediaUrlInfo">The MediaUrlInfo object</param>
    public void ToggleNumberTitles(MediaUrlInfo mediaUrlInfo, bool toggled)
    {
        var numberedRegex = new Regex(@"[0-9]+ - ", RegexOptions.None);
        for (var i = 0; i < mediaUrlInfo.MediaList.Count; i++)
        {
            if (toggled)
            {
                mediaUrlInfo.MediaList[i].Title = $"{i + 1} - {mediaUrlInfo.MediaList[i].Title}";
            }
            else
            {
                var match = numberedRegex.Match(mediaUrlInfo.MediaList[i].Title);
                if (match.Success)
                {
                    mediaUrlInfo.MediaList[i].Title = mediaUrlInfo.MediaList[i].Title.Remove(mediaUrlInfo.MediaList[i].Title.IndexOf(match.Value), match.Value.Length);
                }
            }
        }
    }

    /// <summary>
    /// Populates the downloads list
    /// </summary>
    /// <param name="mediaUrlInfo">The MediaUrlInfo object</param>
    /// <param name="mediaFileType">The media file type to download</param>
    /// <param name="quality">The quality of the downloads</param>
    /// <param name="subtitles">The subtitle format of the downloads</param>
    /// <param name="saveFolder">The save folder of the downloads</param>
    /// <param name="overwriteFiles">Whether or not to overwrite existing files</param>
    /// <param name="limitSpeed">Whether or not to use speed limit</param>
    public void PopulateDownloads(MediaUrlInfo mediaUrlInfo, MediaFileType mediaFileType, Quality quality, Subtitle subtitles, string saveFolder, bool overwriteFiles, bool limitSpeed)
    {
        Downloads.Clear();
        foreach (var media in mediaUrlInfo.MediaList)
        {
            if (media.ToDownload)
            {
                Downloads.Add(new Download(media.Url, mediaFileType, saveFolder, media.Title, limitSpeed, Configuration.Current.SpeedLimit, quality, subtitles, overwriteFiles));
            }
        }
        Configuration.Current.PreviousSaveFolder = saveFolder;
        Configuration.Current.PreviousMediaFileType = mediaFileType;
        Configuration.Current.Save();
    }
}
