namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// Configurable options for all downloads
/// </summary>
public class DownloadOptions
{
    /// <summary>
    /// Whether or not to overwrite existing files
    /// </summary>
    public bool OverwriteExistingFiles { get; init; }
    /// <summary>
    /// Limit characters in filenames to Windows supported
    /// </summary>
    public bool LimitCharacters { get; init; }
    /// <summary>
    /// A comma separated list of language codes for subtitle downloads
    /// </summary>
    public string SubtitleLangs { get; init; }
    /// <summary>
    /// Whether or not to include and download auto-generated subtitles
    /// </summary>
    public bool IncludeAutoGenertedSubtitles { get; init; }
    /// <summary>
    /// Whether or not to use aria2 for the download
    /// </summary>
    public bool UseAria { get; init; }
    /// <summary>
    /// The maximum number of connections to one server for each download (-x)
    /// </summary>
    public int AriaMaxConnectionsPerServer { get; init; }
    /// <summary>
    /// The minimum size of which to split a file (-k)
    /// </summary>
    public int AriaMinSplitSize { get; init; }
    /// <summary>
    /// Speed limit in KiB/s (should be between 512-10240)
    /// </summary>
    public uint SpeedLimit { get; init; }
    /// <summary>
    /// The url of the proxy server to use
    /// </summary>
    public string ProxyUrl { get; init; }
    /// <summary>
    /// The path to the cookies file to use for yt-dlp
    /// </summary>
    public string CookiesPath { get; init; }
    /// <summary>
    /// Whether or not to use the SponsorBlock extension for YouTube downloads
    /// </summary>
    public bool YouTubeSponsorBlock { get; init; }
    /// <summary>
    /// Whether or not to embed media metadata in the downloaded file
    /// </summary>
    public bool EmbedMetadata { get; init; }
    /// <summary>
    /// Whether or not to remove data about media source from metadata
    /// </summary>
    /// <remarks>This includes comment, description, synopsis and purl fields</remarks>
    public bool RemoveSourceData { get; init; }
    /// <summary>
    /// Whether or not to embed chapters in the downloaded file
    /// </summary>
    public bool EmbedChapters { get; init; }
    /// <summary>
    /// Whether or not to embed subtitles in the downloaded file
    /// </summary>
    public bool EmbedSubtitle { get; init; }

    /// <summary>
    /// Constructs a DownloadOptions
    /// </summary>
    public DownloadOptions()
    {
        OverwriteExistingFiles = true;
        LimitCharacters = false;
        SubtitleLangs = "";
        IncludeAutoGenertedSubtitles = true;
        UseAria = false;
        AriaMaxConnectionsPerServer = 16;
        AriaMinSplitSize = 20;
        SpeedLimit = 1024;
        ProxyUrl = "";
        CookiesPath = "";
        YouTubeSponsorBlock = false;
        EmbedMetadata = true;
        RemoveSourceData = false;
        EmbedChapters = false;
        EmbedSubtitle = true;
    }

    /// <summary>
    /// A DownloadOptions object based on the current Configuration
    /// </summary>
    public static DownloadOptions Current => new DownloadOptions()
    {
        OverwriteExistingFiles = Configuration.Current.OverwriteExistingFiles,
        LimitCharacters = Configuration.Current.LimitCharacters,
        SubtitleLangs = Configuration.Current.SubtitleLangs,
        IncludeAutoGenertedSubtitles = Configuration.Current.IncludeAutoGenertedSubtitles,
        UseAria = Configuration.Current.UseAria,
        AriaMaxConnectionsPerServer = Configuration.Current.AriaMaxConnectionsPerServer,
        AriaMinSplitSize = Configuration.Current.AriaMinSplitSize,
        SpeedLimit = Configuration.Current.SpeedLimit,
        ProxyUrl = Configuration.Current.ProxyUrl,
        CookiesPath = Configuration.Current.CookiesPath,
        YouTubeSponsorBlock = Configuration.Current.YouTubeSponsorBlock,
        EmbedMetadata = Configuration.Current.EmbedMetadata,
        RemoveSourceData = Configuration.Current.RemoveSourceData,
        EmbedChapters = Configuration.Current.EmbedChapters,
        EmbedSubtitle = Configuration.Current.EmbedSubtitle
    };
}