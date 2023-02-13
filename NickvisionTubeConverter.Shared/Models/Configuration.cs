using System;
using System.IO;
using System.Text.Json;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// A model for the configuration of the application
/// </summary>
public class Configuration
{
    private static readonly string ConfigDir = $"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}{Path.DirectorySeparatorChar}Nickvision{Path.DirectorySeparatorChar}{AppInfo.Current.Name}";
    private static readonly string ConfigPath = $"{ConfigDir}{Path.DirectorySeparatorChar}config.json";
    private static Configuration? _instance;

    /// <summary>
    /// The preferred theme for the application
    /// </summary>
    public Theme Theme { get; set; }
    /// <summary>
    /// The previously used download save folder
    /// </summary>
    public string PreviousSaveFolder { get; set; }
    /// <summary>
    /// The previously used media file type
    /// </summary>
    public MediaFileType PreviousMediaFileType { get; set; }
    /// <summary>
    /// Whether or not to embed metadata in a download
    /// </summary>
    public bool EmbedMetadata { get; set; }
    /// <summary>
    /// The installed version of yt-dlp
    /// </summary>
    public Version YtdlpVersion { get; set; }

    /// <summary>
    /// Occurs when the configuration is saved to disk
    /// </summary>
    public event EventHandler? Saved;

    /// <summary>
    /// Constructs a Configuration
    /// </summary>
    public Configuration()
    {
        if (!Directory.Exists(ConfigDir))
        {
            Directory.CreateDirectory(ConfigDir);
        }
        Theme = Theme.System;
        PreviousSaveFolder = "";
        PreviousMediaFileType = MediaFileType.MP4;
        EmbedMetadata = true;
        YtdlpVersion = new Version(0, 0, 0);
    }

    /// <summary>
    /// Gets the singleton object
    /// </summary>
    internal static Configuration Current
    {
        get
        {
            if (_instance == null)
            {
                try
                {
                    _instance = JsonSerializer.Deserialize<Configuration>(File.ReadAllText(ConfigPath)) ?? new Configuration();
                }
                catch
                {
                    _instance = new Configuration();
                }
            }
            return _instance;
        }
    }

    /// <summary>
    /// Saves the configuration to disk
    /// </summary>
    public void Save()
    {
        File.WriteAllText(ConfigPath, JsonSerializer.Serialize(this));
        Saved?.Invoke(this, EventArgs.Empty);
    }
}
