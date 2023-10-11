using Nickvision.Aura.Configuration;
using System.Collections.Generic;

namespace NickvisionTubeConverter.Shared.Models;

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
}