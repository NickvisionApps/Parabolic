using System;
using System.Collections.Generic;
using System.IO;
using System.Text.Json;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// A model for download history
/// </summary>
public class DownloadHistory
{
    private static readonly string HistoryPath = $"{Configuration.ConfigDir}{Path.DirectorySeparatorChar}downloadHistory.json";
    private static DownloadHistory? _instance;

    /// <summary>
    /// The download history
    /// </summary>
    public Dictionary<string, string> History { get; set; }
    
    /// <summary>
    /// Constructs a DownloadHistory
    /// </summary>
    public DownloadHistory()
    {
        History = new Dictionary<string, string>();
    }
    
    /// <summary>
    /// Gets a DownloadHistory object
    /// </summary>
    internal static DownloadHistory Current
    {
        get
        {
            if (_instance == null)
            {
                try
                {
                    _instance = JsonSerializer.Deserialize<DownloadHistory>(File.ReadAllText(HistoryPath)) ?? new DownloadHistory();
                }
                catch
                {
                    _instance = new DownloadHistory();
                }
            }
            return _instance;
        }
    }
    
    /// <summary>
    /// Saves the DownloadHistory to disk
    /// </summary>
    public void Save() => File.WriteAllText(HistoryPath, JsonSerializer.Serialize(this));
}