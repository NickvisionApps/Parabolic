using System;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// Item in download history
/// </summary>
public class DownloadHistoryItem
{
    /// <summary>
    /// Media URL
    /// </summary>
    public string Url { get; set; }
    /// <summary>
    /// Media title
    /// </summary>
    public string Title { get; set; }
    /// <summary>
    /// Date and time when download was added
    /// </summary>
    public DateTime Date { get; set; }
    /// <summary>
    /// Path to the downloaded file
    /// </summary>
    public string Path { get; set; }

    public DownloadHistoryItem(string url)
    {
        Url = url;
        Title = "";
        Date = DateTime.Now;
        Path = "";
    }
}