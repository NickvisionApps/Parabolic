namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// Options for a download
/// </summary>
public class DownloadOptions
{
    /// <summary>
    /// Whether or not to overwrite existing files
    /// </summary>
    public bool OverwriteExistingFiles { get; set; }
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
    /// A comma separated list of language codes for subtitle downloads
    /// </summary>
    public string SubtitleLangs { get; init; }
    /// <summary>
    /// The url of the proxy server to use
    /// </summary>
    public string ProxyUrl { get; set; }
    /// <summary>
    /// The path to the cookies file to use for yt-dlp
    /// </summary>
    public string CookiesPath { get; init; }
    /// <summary>
    /// Whether or not to embed media metadata in the downloaded file
    /// </summary>
    public bool EmbedMetadata { get; init; }
    /// <summary>
    /// Whether or not to embed chapters in the downloaded file
    /// </summary>
    public bool EmbedChapters { get; init; }
    
    /// <summary>
    /// Constructs a DownloadOptions
    /// </summary>
    /// <param name="overwriteExistingFiles">Whether or not to overwrite existing files</param>
    /// <param name="useAria">Whether or not to use aria2 for the download</param>
    /// <param name="ariaMaxConnectionsPerServer">The maximum number of connections to one server for each download (-x)</param>
    /// <param name="ariaMinSplitSize">The minimum size of which to split a file (-k)</param>
    /// <param name="subtitleLangs">A comma separated list of language codes for subtitle downloads</param>
    /// <param name="proxyUrl">The url of the proxy server to use</param>
    /// <param name="cookiesPath">The path to the cookies file to use for yt-dlp</param>
    /// <param name="embedMetadata">Whether or not to embed media metadata in the downloaded file</param>
    /// <param name="embedChapters">Whether or not to embed chapters in the downloaded file</param>
    public DownloadOptions(bool overwriteExistingFiles, bool useAria, int ariaMaxConnectionsPerServer, int ariaMinSplitSize, string subtitleLangs, string proxyUrl, string cookiesPath, bool embedMetadata, bool embedChapters)
    {
        OverwriteExistingFiles = overwriteExistingFiles;
        UseAria = useAria;
        AriaMaxConnectionsPerServer = ariaMaxConnectionsPerServer;
        AriaMinSplitSize = ariaMinSplitSize;
        SubtitleLangs = subtitleLangs;
        ProxyUrl = proxyUrl;
        CookiesPath = cookiesPath;
        EmbedMetadata = embedMetadata;
        EmbedChapters = embedChapters;
    }
}