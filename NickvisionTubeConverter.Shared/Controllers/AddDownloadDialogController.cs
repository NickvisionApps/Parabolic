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
    /// Searches for information about a video url
    /// </summary>
    /// <param name="videoUrl">The video url</param>
    /// <returns>A VideoUrlInfo object for the url or null if url is invalid</returns>
    public async Task<VideoUrlInfo?> SearchUrlAsync(string videoUrl) => await VideoUrlInfo.GetAsync(videoUrl);

    /// <summary>
    /// Numbers the videos in a VideoUrlInfo object
    /// </summary>
    /// <param name="videoUrlInfo">The VideoUrlInfo object</param>
    public void ToggleNumberVideos(VideoUrlInfo videoUrlInfo, bool toggled)
    {
        var numberedRegex = new Regex(@"[0-9]+ - ", RegexOptions.None);
        for (var i = 0; i < videoUrlInfo.Videos.Count; i++)
        {
            if (toggled)
            {
                videoUrlInfo.Videos[i].Title = $"{i + 1} - {videoUrlInfo.Videos[i].Title}";
            }
            else
            {
                var match = numberedRegex.Match(videoUrlInfo.Videos[i].Title);
                if (match.Success)
                {
                    videoUrlInfo.Videos[i].Title = videoUrlInfo.Videos[i].Title.Remove(videoUrlInfo.Videos[i].Title.IndexOf(match.Value), match.Value.Length);
                }
            }
        }
    }

    /// <summary>
    /// Populates the downloads list
    /// </summary>
    /// <param name="videoUrlInfo">The VideoUrlInfo object</param>
    /// <param name="mediaFileType">The media file type to download</param>
    /// <param name="audioQuality">The audio quality of the downloads</param>
    /// <param name="subtitles">The subtitle format of the downloads</param>
    /// <param name="saveFolder">The save folder of the downloads</param>
    /// <param name="overwriteFiles">Whether or not to overwrite existing files</param>
    /// <param name="limitSpeed">Whether or not to use speed limit</param>
    public void PopulateDownloads(VideoUrlInfo videoUrlInfo, MediaFileType mediaFileType, AudioQuality audioQuality, Subtitle subtitles, string saveFolder, bool overwriteFiles, bool limitSpeed)
    {
        Downloads.Clear();
        foreach (var video in videoUrlInfo.Videos)
        {
            if (video.ToDownload)
            {
                Downloads.Add(new Download(video.Url, mediaFileType, saveFolder, video.Title, overwriteFiles, limitSpeed, Configuration.Current.UseAria, audioQuality, subtitles, Configuration.Current.SpeedLimit));
            }
        }
        Configuration.Current.PreviousSaveFolder = saveFolder;
        Configuration.Current.PreviousMediaFileType = mediaFileType;
        Configuration.Current.Save();
    }
}
