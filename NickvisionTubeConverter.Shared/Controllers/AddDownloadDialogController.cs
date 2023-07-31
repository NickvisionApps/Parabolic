using Nickvision.Aura;
using Nickvision.Keyring.Models;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

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
    /// The list of video resolutions as strings
    /// </summary>
    public List<string> VideoResolutions { get; init; }

    /// <summary>
    /// Gets the AppInfo object
    /// </summary>
    public AppInfo AppInfo => Aura.Active.AppInfo;
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
    public bool HasVideoResolutions => _mediaUrlInfo != null && _mediaUrlInfo.VideoResolutions.Count > 0;
    /// <summary>
    /// The list of audio language codes
    /// </summary>
    public List<string> AudioLanguages => _mediaUrlInfo != null ? _mediaUrlInfo.AudioLanguages : new List<string>();
    /// <summary>
    /// The previously used save folder
    /// </summary>
    public string PreviousSaveFolder => Directory.Exists(Configuration.Current.PreviousSaveFolder) ? Configuration.Current.PreviousSaveFolder : "";
    /// <summary>
    /// The previously used MediaFileType
    /// </summary>
    public MediaFileType PreviousMediaFileType => Configuration.Current.PreviousMediaFileType;
    /// <summary>
    /// Whether or not to number titles
    /// </summary>
    public bool NumberTitles => Configuration.Current.NumberTitles;
    /// <summary>
    /// The speed limit in the configuration
    /// </summary>
    public uint CurrentSpeedLimit => Configuration.Current.SpeedLimit;
    /// <summary>
    /// The url of the proxy server to use
    /// </summary>
    public string ProxyUrl => Configuration.Current.ProxyUrl;
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
    /// Limit characters in filenames to Windows supported
    /// </summary>
    public bool LimitCharacters
    {
        get => Configuration.Current.LimitCharacters;

        set => Configuration.Current.LimitCharacters = value;
    }

    /// <summary>
    /// Constructs a AddDownloadDialogController
    /// </summary>
    /// <param name="keyring">The application Keyring</param>
    public AddDownloadDialogController(Keyring? keyring)
    {
        _keyring = keyring;
        Downloads = new List<Download>();
        VideoResolutions = new List<string>();
    }

    /// <summary>
    /// The previously used VideoResolution's index
    /// </summary>
    public int PreviousVideoResolutionIndex
    {
        get
        {
            var videoResolution = VideoResolution.Parse(Configuration.Current.PreviousVideoResolution);
            if(videoResolution != null)
            {
                if(_mediaUrlInfo != null)
                {
                    return _mediaUrlInfo.VideoResolutions.IndexOf(videoResolution);
                }
            }
            return -1;
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
        _mediaUrlInfo = await MediaUrlInfo.GetAsync(mediaUrl, username, password, ProxyUrl);
        if(_mediaUrlInfo != null)
        {
            foreach (var resolution in _mediaUrlInfo.VideoResolutions)
            {
                VideoResolutions.Add(resolution.ToString());
            }
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
    /// <returns>True if successful, else false</returns>
    public bool ToggleNumberTitles(bool toggled)
    {
        if(_mediaUrlInfo != null)
        {
            foreach (var m in _mediaUrlInfo.MediaList)
            {
                var numberTitle = $"{m.PlaylistPosition} - ";
                m.Title = toggled ? numberTitle + m.Title : m.Title.Substring(numberTitle.Length);
            }
            Configuration.Current.NumberTitles = toggled;
            Aura.Active.SaveConfig("config");
            return true;
        }
        return false;
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
    /// <param name="audioLanguage">The audio language code</param>
    /// <param name="subtitles">The subtitle format of the downloads</param>
    /// <param name="saveFolder">The save folder of the downloads</param>
    /// <param name="limitSpeed">Whether or not to use speed limit</param>
    /// <param name="cropThumbnail">Whether or not to crop the thumbnail</param>
    /// <param name="timeframe">A Timeframe to restrict the timespan of the media download</param>
    /// <param name="username">A username for the website (if available)</param>
    /// <param name="password">A password for the website (if available)</param>
    public void PopulateDownloads(MediaFileType mediaFileType, Quality quality, int? resolution, string? audioLanguage, Subtitle subtitles, string saveFolder, bool limitSpeed, bool cropThumbnail, Timeframe? timeframe, string? username, string? password)
    {
        Downloads.Clear();
        foreach (var media in _mediaUrlInfo.MediaList)
        {
            if (media.ToDownload)
            {
                Downloads.Add(new Download(media.Url, mediaFileType, saveFolder, media.Title, limitSpeed, Configuration.Current.SpeedLimit, quality, resolution == null ? null : _mediaUrlInfo.VideoResolutions[resolution.Value], audioLanguage, subtitles, cropThumbnail, timeframe, media.PlaylistPosition, username, password));
            }
        }
        Configuration.Current.PreviousSaveFolder = saveFolder;
        if (!mediaFileType.GetIsGeneric())
        {
            Configuration.Current.PreviousMediaFileType = mediaFileType;
        }
        if(resolution != null)
        {
            Configuration.Current.PreviousVideoResolution = _mediaUrlInfo.VideoResolutions[resolution.Value].ToString();
        }
        Aura.Active.SaveConfig("config");
    }

    /// <summary>
    /// Populates the downloads list
    /// </summary>
    /// <param name="mediaFileType">The media file type to download</param>
    /// <param name="quality">The quality of the downloads</param>
    /// <param name="resolution">The index of the video resolution if available</param>
    /// <param name="audioLanguage">The audio language code</param>
    /// <param name="subtitles">The subtitle format of the downloads</param>
    /// <param name="saveFolder">The save folder of the downloads</param>
    /// <param name="limitSpeed">Whether or not to use speed limit</param>
    /// <param name="cropThumbnail">Whether or not to crop the thumbnail</param>
    /// <param name="timeframe">A Timeframe to restrict the timespan of the media download</param>
    /// <param name="credentialIndex">The index of the credential to use</param>
    public async Task PopulateDownloadsAsync(MediaFileType mediaFileType, Quality quality, int? resolution, string? audioLanguage, Subtitle subtitles, string saveFolder, bool limitSpeed, bool cropThumbnail, Timeframe? timeframe, int credentialIndex)
    {
        if(_keyring != null)
        {
            var credentials = await _keyring.GetAllCredentialsAsync();
            credentials.Sort((a, b) => a.Name.CompareTo(b.Name));
            PopulateDownloads(mediaFileType, quality, resolution, audioLanguage, subtitles, saveFolder, limitSpeed, cropThumbnail, timeframe, credentials[credentialIndex].Username, credentials[credentialIndex].Password);
        }
        else
        {
            PopulateDownloads(mediaFileType, quality, resolution, audioLanguage, subtitles, saveFolder, limitSpeed, cropThumbnail, timeframe, "", "");
        }
    }
}
