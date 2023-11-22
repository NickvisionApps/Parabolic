using Nickvision.Aura;
using System.Globalization;
using System.Runtime.InteropServices;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// A model for the configuration of the application
/// </summary>
public class Configuration : ConfigurationBase
{
    //////////////////////
    /// User Interface ///
    //////////////////////

    /// <summary>
    /// The preferred theme for the application
    /// </summary>
    public Theme Theme { get; set; }
    /// <summary>
    /// Whether or not to automatically check for updates
    /// </summary>
    public bool AutomaticallyCheckForUpdates { get; set; }
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
    /// <remarks>Only used on GNOME running via Flatpak</remarks>
    public bool RunInBackground { get; set; }

    //////////////////////
    ///    Downloads   ///
    //////////////////////

    /// <summary>
    /// Whether or not to overwrite existing files
    /// </summary>
    public bool OverwriteExistingFiles { get; set; }
    /// <summary>
    /// The maximum number of active downloads (should be between 1-10)
    /// </summary>
    public int MaxNumberOfActiveDownloads { get; set; }
    /// <summary>
    /// Limit characters in filenames to Windows supported
    /// </summary>
    public bool LimitCharacters { get; set; }
    /// <summary>
    /// A comma separated list of language codes for subtitle downloads
    /// </summary>
    public string SubtitleLangs { get; set; }
    /// <summary>
    /// Whether or not to include and download auto-generated subtitles
    /// </summary>
    public bool IncludeAutoGenertedSubtitles { get; set; }

    //////////////////////
    ///   Downloader   ///
    //////////////////////

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
    /// Speed limit in KiB/s (should be between 512-10240)
    /// </summary>
    public uint SpeedLimit { get; set; }
    /// <summary>
    /// The url of the proxy server to use
    /// </summary>
    public string ProxyUrl { get; set; }
    /// <summary>
    /// The path of the cookies file to use for yt-dlp
    /// </summary>
    public string CookiesPath { get; set; }
    /// <summary>
    /// Whether or not to use the SponsorBlock extension for YouTube downloads
    /// </summary>
    public bool YouTubeSponsorBlock { get; set; }


    //////////////////////
    ///    Converter   ///
    //////////////////////

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
    /// Whether or not to embed subtitles in a download
    /// </summary>
    public bool EmbedSubtitle { get; set; }

    //////////////////////
    ///    Remember    ///
    //////////////////////

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
    /// The previously used prefer av1 state
    /// </summary>
    public bool PreviousPreferAV1State { get; set; }
    /// <summary>
    /// Whether or not to number titles
    /// </summary>
    public bool NumberTitles { get; set; }
    /// <summary>
    /// Whether or not to show the disclaimer on startup
    /// </summary>
    /// <remarks>Used on WinUI only</remarks>
    public bool ShowDisclaimerOnStartup { get; set; }

    /// <summary>
    /// Constructs a Configuration
    /// </summary>
    public Configuration()
    {
        //User Interface
        Theme = Theme.System;
        AutomaticallyCheckForUpdates = true;
        CompletedNotificationPreference = NotificationPreference.ForEach;
        PreventSuspendWhenDownloading = false;
        RunInBackground = RuntimeInformation.IsOSPlatform(OSPlatform.Windows);
        //Downloads
        OverwriteExistingFiles = true;
        MaxNumberOfActiveDownloads = 5;
        LimitCharacters = RuntimeInformation.IsOSPlatform(OSPlatform.Windows);
        SubtitleLangs = $"{CultureInfo.CurrentCulture.TwoLetterISOLanguageName},{CultureInfo.CurrentCulture.Name},{CultureInfo.CurrentCulture.ThreeLetterISOLanguageName}";
        IncludeAutoGenertedSubtitles = true;
        //Downloader
        UseAria = false;
        AriaMaxConnectionsPerServer = 16;
        AriaMinSplitSize = 20;
        SpeedLimit = 1024;
        ProxyUrl = "";
        CookiesPath = "";
        YouTubeSponsorBlock = false;
        //Converter
        DisallowConversions = false;
        EmbedMetadata = true;
        CropAudioThumbnails = false;
        RemoveSourceData = false;
        EmbedChapters = false;
        EmbedSubtitle = true;
        //Remember
        PreviousSaveFolder = "";
        PreviousMediaFileType = MediaFileType.MP4;
        PreviousVideoResolution = "";
        PreviousSubtitleState = false;
        PreviousPreferAV1State = false;
        NumberTitles = false;
        ShowDisclaimerOnStartup = true;
    }

    /// <summary>
    /// Gets the singleton object
    /// </summary>
    internal static Configuration Current => Aura.Active.GetConfig<Configuration>("config");
}
