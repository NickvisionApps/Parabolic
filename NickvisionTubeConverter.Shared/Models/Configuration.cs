using Nickvision.Aura;
using System;
using System.Globalization;
using System.IO;
using System.Runtime.InteropServices;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// A model for the configuration of the application
/// </summary>
public class Configuration : ConfigurationBase
{
    /// <summary>
    /// The directory to store temporary files
    /// </summary>
    /// <remarks>TODO: https://github.com/NickvisionApps/Aura/issues/5</remarks>
    public static string TempDir = RuntimeInformation.IsOSPlatform(OSPlatform.Windows) ? $"{Environment.GetFolderPath(Environment.SpecialFolder.UserProfile)}{Path.DirectorySeparatorChar}.tc-temp" : UserDirectories.ApplicationCache;

    /// <summary>
    /// The preferred theme for the application
    /// </summary>
    public Theme Theme { get; set; }
    /// <summary>
    /// The preference of how often to show completed notifications
    /// </summary>
    public NotificationPreference CompletedNotificationPreference { get; set; }
    /// <summary>
    /// Whether or not to prevent suspend when downloads are in progress
    /// </summary>
    public bool PreventSuspendWhenDownloading { get; set; }
    /// <summary>
    /// Whether to allow running in the background
    /// </summary>
    public bool RunInBackground { get; set; }
    /// <summary>
    /// The maximum number of active downloads (should be between 1-10)
    /// </summary>
    public int MaxNumberOfActiveDownloads { get; set; }
    /// <summary>
    /// Whether or not to overwrite existing files
    /// </summary>
    public bool OverwriteExistingFiles { get; set; }
    /// <summary>
    /// Limit characters in filenames to Windows supported
    /// </summary>
    public bool LimitCharacters { get; set; }
    /// <summary>
    /// Speed limit in KiB/s (should be between 512-10240)
    /// </summary>
    public uint SpeedLimit { get; set; }
    /// <summary>
    /// Whether or not to use aria2
    /// </summary>
    public bool UseAria { get; set; }
    /// <summary>
    /// The maximum number of connections to one server for each download (-x)
    /// </summary>
    public int AriaMaxConnectionsPerServer { get; set; }
    /// <summary>
    /// The minimum size of which to split a file (-k)
    /// </summary>
    public int AriaMinSplitSize { get; set; }
    /// <summary>
    /// Whether or not to use the SponsorBlock extension for YouTube downloads
    /// </summary>
    public bool YouTubeSponsorBlock { get; set; }
    /// <summary>
    /// A comma separated list of language codes for subtitle downloads
    /// </summary>
    public string SubtitleLangs { get; set; }
    /// <summary>
    /// The url of the proxy server to use
    /// </summary>
    public string ProxyUrl { get; set; }
    /// <summary>
    /// The path of the cookies file to use for yt-dlp
    /// </summary>
    public string CookiesPath { get; set; }
    /// <summary>
    /// Whether or not to disallow converting of formats
    /// </summary>
    public bool DisallowConversions { get; set; }
    /// <summary>
    /// Whether or not to embed metadata in a download
    /// </summary>
    public bool EmbedMetadata { get; set; }
    /// <summary>
    /// Whether or not to turn on crop thumbnail in an audio download
    /// </summary>
    public bool CropAudioThumbnails { get; set; }
    /// <summary>
    /// Whether or not to remove data about media source from metadata
    /// </summary>
    /// <remarks>This includes comment, description, synopsis and purl fields</remarks>
    public bool RemoveSourceData { get; set; }
    /// <summary>
    /// Whether or not to embed chapters in a download
    /// </summary>
    public bool EmbedChapters { get; set; }
    /// <summary>
    /// Whether or not to embed subtitle in a download
    /// </summary>
    public bool EmbedSubtitle { get; set; }
    /// <summary>
    /// The previously used download save folder
    /// </summary>
    public string PreviousSaveFolder { get; set; }
    /// <summary>
    /// The previously used media file type
    /// </summary>
    public MediaFileType PreviousMediaFileType { get; set; }
    /// <summary>
    /// The previously used video resolution
    /// </summary>
    public string PreviousVideoResolution { get; set; }
    /// <summary>
    /// The previously used subtitle downloading state
    /// </summary>
    public bool PreviousSubtitleState { get; set; }
    /// <summary>
    /// Whether or not to number titles
    /// </summary>
    public bool NumberTitles { get; set; }

    /// <summary>
    /// Constructs a Configuration
    /// </summary>
    public Configuration()
    {
        Theme = Theme.System;
        CompletedNotificationPreference = NotificationPreference.ForEach;
        PreventSuspendWhenDownloading = false;
        RunInBackground = RuntimeInformation.IsOSPlatform(OSPlatform.Windows);
        MaxNumberOfActiveDownloads = 5;
        OverwriteExistingFiles = true;
        LimitCharacters = RuntimeInformation.IsOSPlatform(OSPlatform.Windows);
        SpeedLimit = 1024;
        UseAria = false;
        AriaMaxConnectionsPerServer = 16;
        AriaMinSplitSize = 20;
        YouTubeSponsorBlock = false;
        SubtitleLangs = $"{CultureInfo.CurrentCulture.TwoLetterISOLanguageName},{CultureInfo.CurrentCulture.Name},{CultureInfo.CurrentCulture.ThreeLetterISOLanguageName}";
        ProxyUrl = "";
        CookiesPath = "";
        DisallowConversions = false;
        EmbedMetadata = true;
        CropAudioThumbnails = false;
        RemoveSourceData = false;
        EmbedChapters = false;
        EmbedSubtitle = true;
        PreviousSaveFolder = "";
        PreviousMediaFileType = MediaFileType.MP4;
        PreviousVideoResolution = "";
        PreviousSubtitleState = false;
        NumberTitles = false;
    }

    /// <summary>
    /// Gets the singleton object
    /// </summary>
    internal static Configuration Current => (Configuration)Aura.Active.ConfigFiles["config"];
}
