using Nickvision.Aura;
using Nickvision.Aura.Configuration;
using System;
using System.Collections.Generic;

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

/// <summary>
/// A model for download history
/// </summary>
public class DownloadHistory : ConfigurationBase
{
    /// <summary>
    /// The download history
    /// </summary>
    public Dictionary<string, DownloadHistoryItem> History { get; set; }

    /// <summary>
    /// Constructs a DownloadHistory
    /// </summary>
    public DownloadHistory()
    {
        History = new Dictionary<string, DownloadHistoryItem>();
    }

    /// <summary>
    /// Gets the singleton object
    /// </summary>
    public static DownloadHistory Current => (DownloadHistory)Aura.Active.ConfigFiles["downloadHistory"];
}