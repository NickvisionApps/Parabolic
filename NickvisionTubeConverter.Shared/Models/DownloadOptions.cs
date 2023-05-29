namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// Options for a download
/// </summary>
public class DownloadOptions
{
    /// <summary>
    /// Whether or not to use aria2 for the download
    /// </summary>
    public bool UseAria { get; init; }
    /// <summary>
    /// Whether or not to embed media metadata in the downloaded file
    /// </summary>
    public bool EmbedMetadata { get; init; }
    /// <summary>
    /// The path to the cookies file to use for yt-dlp
    /// </summary>
    public string? CookiesPath { get; init; }
    /// <summary>
    /// The maximum number of connections to one server for each download (-x)
    /// </summary>
    public int AriaMaxConnectionsPerServer { get; init; }
    /// <summary>
    /// The minimum size of which to split a file (-k)
    /// </summary>
    public int AriaMinSplitSize { get; init; }
    
    /// <summary>
    /// Constructs a DownloadOptions
    /// </summary>
    /// <param name="useAria">Whether or not to use aria2 for the download</param>
    /// <param name="embedMetadata">Whether or not to embed media metadata in the downloaded file</param>
    /// <param name="cookiesPath">The path to the cookies file to use for yt-dlp</param>
    /// <param name="ariaMaxConnectionsPerServer">The maximum number of connections to one server for each download (-x)</param>
    /// <param name="ariaMinSplitSize">The minimum size of which to split a file (-k)</param>
    public DownloadOptions(bool useAria, bool embedMetadata, string? cookiesPath, int ariaMaxConnectionsPerServer, int ariaMinSplitSize)
    {
        UseAria = useAria;
        EmbedMetadata = embedMetadata;
        CookiesPath = cookiesPath;
        AriaMaxConnectionsPerServer = ariaMaxConnectionsPerServer;
        AriaMinSplitSize = ariaMinSplitSize;
    }
}