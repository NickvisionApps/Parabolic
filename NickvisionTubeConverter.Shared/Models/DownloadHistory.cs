using Nickvision.Aura;
using System;
using System.Collections.Generic;
using System.Text.Json.Serialization;

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