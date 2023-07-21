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
    
    /// <summary>
    /// The download history
    /// </summary>
    public List<string> History { get; set; }
    
    /// <summary>
    /// Constructs a DownloadHistory
    /// </summary>
    public DownloadHistory()
    {
        History = new List<string>();
    }
    
    /// <summary>
    /// Gets a DownloadHistory object
    /// </summary>
    public static DownloadHistory Current
    {
        get
        {
            try
            {
                return JsonSerializer.Deserialize<DownloadHistory>(File.ReadAllText(HistoryPath)) ?? new DownloadHistory();
            }
            catch
            {
                return new DownloadHistory();
            }
        }
    }
    
    /// <summary>
    /// Saves the DownloadHistory to disk
    /// </summary>
    public void Save() => File.WriteAllText(HistoryPath, JsonSerializer.Serialize(this));
}