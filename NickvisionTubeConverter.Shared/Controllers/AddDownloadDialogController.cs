using Nickvision.Keyring;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using static NickvisionTubeConverter.Shared.Helpers.Gettext;

namespace NickvisionTubeConverter.Shared.Controllers;

/// <summary>
/// Statuses for when a download options are checked
/// </summary>
[Flags]
public enum DownloadOptionsCheckStatus
{
    Valid = 1,
    InvalidSaveFolder = 2,
    InvalidTimeframeStart = 4,
    InvalidTimeframeEnd = 8
}

/// <summary>
/// A controller for a AddDownloadDialog
/// </summary>
public class AddDownloadDialogController
{
    private Keyring? _keyring;
    private MediaUrlInfo? _mediaUrlInfo;

    /// <summary>
    /// The downloads created by the dialog
    /// </summary>
    public List<Download> Downloads { get; init; }
    /// <summary>
    /// The array of audio qualities
    /// </summary>
    public string[] AudioQualityArray { get; init; }
    /// <summary>
    /// The list of video qualities
    /// </summary>
    public List<string> VideoQualityList { get; init; }

    /// <summary>
    /// Gets the AppInfo object
    /// </summary>
    public AppInfo AppInfo => AppInfo.Current;
    /// <summary>
    /// Whether or not keyring auth is available
    /// </summary>
    public bool KeyringAuthAvailable => _keyring != null;
    /// <summary>
    /// Whether or not media info is available (Check after running SearchUrlAsync)
    /// </summary>
    public bool HasMediaInfo => _mediaUrlInfo != null;
    /// <summary>
    /// The list of media available from media info
    /// </summary>
    public List<MediaInfo> MediaList => _mediaUrlInfo != null ? _mediaUrlInfo.MediaList : new List<MediaInfo>();
    /// <summary>
    /// Whether or not media info has video resolutions
    /// </summary>
    public bool HasVideoResolutions => _mediaUrlInfo != null ? _mediaUrlInfo.VideoResolutions.Count > 0 : false;
    /// <summary>
    /// The previously used save folder
    /// </summary>
    public string PreviousSaveFolder => Directory.Exists(Configuration.Current.PreviousSaveFolder) ? Configuration.Current.PreviousSaveFolder : "";
    /// <summary>
    /// The previously used MediaFileType
    /// </summary>
    public MediaFileType PreviousMediaFileType => Configuration.Current.PreviousMediaFileType;
    /// <summary>
    /// The previously used VideoResolution
    /// </summary>
    public VideoResolution? PreviousVideoResolution => VideoResolution.Parse(Configuration.Current.PreviousVideoResolution);
    /// <summary>
    /// The speed limit in the configuration
    /// </summary>
    public uint CurrentSpeedLimit => Configuration.Current.SpeedLimit;
    /// <summary>
    /// Whether or not to disallow converting of formats
    /// </summary>
    public bool DisallowConversions => Configuration.Current.DisallowConversions;
    /// <summary>
    /// Whether to embed metadata
    /// </summary>
    public bool EmbedMetadata => Configuration.Current.EmbedMetadata;
    /// <summary>
    /// Whether to turn on crop thumbnail for audio downloads
    /// </summary>
    public bool CropAudioThumbnails => Configuration.Current.CropAudioThumbnails;

    /// <summary>
    /// Constructs a AddDownloadDialogController
    /// </summary>
    /// <param name="keyring">The application Keyring</param>
    public AddDownloadDialogController(Keyring? keyring)
    {
        _keyring = keyring;
        Downloads = new List<Download>();
        AudioQualityArray = new string[] { _("Best"), _("Worst") };
        VideoQualityList = new List<string>();
    }

    /// <summary>
    /// Whether or not to number titles
    /// </summary>
    public bool NumberTitles
    {
        get => Configuration.Current.NumberTitles;

        set
        {
            Configuration.Current.NumberTitles = value;
            Configuration.Current.Save();
        }
    }

    /// <summary>
    /// Gets a list of names of credentials in the keyring
    /// </summary>
    /// <returns>The list of names of credentials</returns>
    public async Task<List<string>> GetKeyringCredentialNamesAsync()
    {
        if(_keyring != null)
        {
            var names = (await _keyring.GetAllCredentialsAsync()).Select(x => x.Name).ToList();
            names.Sort();
            return names;
        }
        return new List<string>();
    }

    /// <summary>
    /// Searches for information about a media url
    /// </summary>
    /// <param name="mediaUrl">The media url</param>
    /// <param name="username">A username for the website (if available)</param>
    /// <param name="password">A password for the website (if available)</param>
    /// <returns>Whether media info was loaded or not</returns>
    public async Task<bool> SearchUrlAsync(string mediaUrl, string? username, string? password)
    {
        _mediaUrlInfo = await MediaUrlInfo.GetAsync(mediaUrl, username, password);
        foreach (var resolution in _mediaUrlInfo.VideoResolutions)
        {
            VideoQualityList.Add(resolution.ToString());
        }
        return _mediaUrlInfo != null;
    }

    /// <summary>
    /// Searches for information about a media url
    /// </summary>
    /// <param name="mediaUrl">The media url</param>
    /// <param name="credentialIndex">The index of the credential to use</param>
    /// <returns>Whether media info was loaded or not</returns>
    public async Task<bool> SearchUrlAsync(string mediaUrl, int credentialIndex)
    {
        if(_keyring != null)
        {
            var credentials = await _keyring.GetAllCredentialsAsync();
            credentials.Sort((a, b) => a.Name.CompareTo(b.Name));
            return await SearchUrlAsync(mediaUrl, credentials[credentialIndex].Username, credentials[credentialIndex].Password);
        }
        return await SearchUrlAsync(mediaUrl, "", "");
    }

    /// <summary>
    /// Numbers the titles in a MediaUrlInfo object
    /// </summary>
    /// <param name="toggled">Whether or not to number titles</param>
    public void ToggleNumberTitles(bool toggled)
    {
        var numberedRegex = new Regex(@"[0-9]+ - ", RegexOptions.None);
        for (var i = 0; i < _mediaUrlInfo.MediaList.Count; i++)
        {
            if(_mediaUrlInfo != null)
            {
                if (toggled)
                {
                    _mediaUrlInfo.MediaList[i].Title = $"{i + 1} - {_mediaUrlInfo.MediaList[i].Title}";
                }
                else
                {
                    var match = numberedRegex.Match(_mediaUrlInfo.MediaList[i].Title);
                    if (match.Success)
                    {
                        _mediaUrlInfo.MediaList[i].Title = _mediaUrlInfo.MediaList[i].Title.Remove(_mediaUrlInfo.MediaList[i].Title.IndexOf(match.Value), match.Value.Length);
                    }
                }
            }
        }
    }

    /// <summary>
    /// Gets the index of a resolution in the VideoResolutions list
    /// <summary>
    /// <param name="resolution">The resoltuion to find</param>
    /// <returns>The index of the resolution. -1 if not found</returns>
    public int IndexOfResolution(VideoResolution resolution)
    {
        if(_mediaUrlInfo == null)
        {
            return -1;
        }
        return _mediaUrlInfo.VideoResolutions.IndexOf(resolution);
    }

    /// <summary>
    /// Validates download options
    /// </summary>
    /// <param name="saveFolder">Save folder path</param>
    /// <param name="downloadTimeframe">Whether or not to download a specific timeframe</param>
    /// <param name="timeframeStart">Download timeframe start string</param>
    /// <param name="timeframeEnd">Download timeframe end string</param>
    /// <returns>DownloadOptionsCheckStatus</returns>
    public DownloadOptionsCheckStatus ValidateDownloadOptions(string saveFolder, bool downloadTimeframe, string timeframeStart, string timeframeEnd, double duration)
    {
        DownloadOptionsCheckStatus result = 0;
        if (!Directory.Exists(saveFolder))
        {
            result |= DownloadOptionsCheckStatus.InvalidSaveFolder;
        }
        if (downloadTimeframe)
        {
            try
            {
                var timeframe = Timeframe.Parse(timeframeStart, timeframeEnd, duration);
            }
            catch(ArgumentException e)
            {
                if(e.Message.Contains("start time"))
                {
                    result |= DownloadOptionsCheckStatus.InvalidTimeframeStart;
                }
                else if(e.Message.Contains("end time"))
                {
                    result |= DownloadOptionsCheckStatus.InvalidTimeframeEnd;
                }
            }
        }
        return result == 0 ? DownloadOptionsCheckStatus.Valid : result;
    }

    /// <summary>
    /// Populates the downloads list
    /// </summary>
    /// <param name="mediaFileType">The media file type to download</param>
    /// <param name="quality">The quality of the downloads</param>
    /// <param name="resolution">The index of the video resolution if available</param>
    /// <param name="subtitles">The subtitle format of the downloads</param>
    /// <param name="saveFolder">The save folder of the downloads</param>
    /// <param name="limitSpeed">Whether or not to use speed limit</param>
    /// <param name="cropThumbnail">Whether or not to crop the thumbnail</param>
    /// <param name="timeframe">A Timeframe to restrict the timespan of the media download</param>
    /// <param name="username">A username for the website (if available)</param>
    /// <param name="password">A password for the website (if available)</param>
    public void PopulateDownloads(MediaFileType mediaFileType, Quality quality, int? resolution, Subtitle subtitles, string saveFolder, bool limitSpeed, bool cropThumbnail, Timeframe? timeframe, string? username, string? password)
    {
        Downloads.Clear();
        foreach (var media in _mediaUrlInfo.MediaList)
        {
            if (media.ToDownload)
            {
                Downloads.Add(new Download(media.Url, mediaFileType, saveFolder, media.Title, limitSpeed, Configuration.Current.SpeedLimit, quality, resolution == null ? null : _mediaUrlInfo.VideoResolutions[resolution.Value], subtitles, cropThumbnail, timeframe, username, password));
            }
        }
        Configuration.Current.PreviousSaveFolder = saveFolder;
        if (!mediaFileType.GetIsGeneric())
        {
            Configuration.Current.PreviousMediaFileType = mediaFileType;
        }
        if(resolution != null)
        {
            Configuration.Current.PreviousVideoResolution = resolution.ToString();
        }
        Configuration.Current.Save();
    }

    /// <summary>
    /// Populates the downloads list
    /// </summary>
    /// <param name="mediaFileType">The media file type to download</param>
    /// <param name="quality">The quality of the downloads</param>
    /// <param name="resolution">The index of the video resolution if available</param>
    /// <param name="subtitles">The subtitle format of the downloads</param>
    /// <param name="saveFolder">The save folder of the downloads</param>
    /// <param name="limitSpeed">Whether or not to use speed limit</param>
    /// <param name="cropThumbnail">Whether or not to crop the thumbnail</param>
    /// <param name="timeframe">A Timeframe to restrict the timespan of the media download</param>
    /// <param name="credentialIndex">The index of the credential to use</param>
    public async Task PopulateDownloadsAsync(MediaFileType mediaFileType, Quality quality, int? resolution, Subtitle subtitles, string saveFolder, bool limitSpeed, bool cropThumbnail, Timeframe? timeframe, int credentialIndex)
    {
        if(_keyring != null)
        {
            var credentials = await _keyring.GetAllCredentialsAsync();
            credentials.Sort((a, b) => a.Name.CompareTo(b.Name));
            PopulateDownloads(mediaFileType, quality, resolution, subtitles, saveFolder, limitSpeed, cropThumbnail, timeframe, credentials[credentialIndex].Username, credentials[credentialIndex].Password);
        }
        else
        {
            PopulateDownloads(mediaFileType, quality, resolution, subtitles, saveFolder, limitSpeed, cropThumbnail, timeframe, "", "");
        }
    }
}
