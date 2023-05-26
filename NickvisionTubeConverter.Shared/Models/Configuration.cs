using System;
using System.IO;
using System.Net;
using System.Runtime.InteropServices;
using System.Text.Json;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// A model for the configuration of the application
/// </summary>
public class Configuration
{
    /// <summary>
    /// The directory of the application configuration
    /// </summary>
    public static readonly string ConfigDir = $"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}{Path.DirectorySeparatorChar}Nickvision{Path.DirectorySeparatorChar}{AppInfo.Current.Name}";
    private static readonly string ConfigPath = $"{ConfigDir}{Path.DirectorySeparatorChar}config.json";
    /// <summary>
    /// The directory to store temporary files
    /// </summary>
    public static string TempDir = RuntimeInformation.IsOSPlatform(OSPlatform.Windows) ? $"{Environment.GetFolderPath(Environment.SpecialFolder.UserProfile)}{Path.DirectorySeparatorChar}.tc-temp" :  $"{ConfigDir}{Path.DirectorySeparatorChar}temp";

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
    /// The maximum number of active downloads (should be between 1-10)
    /// </summary>
    public int MaxNumberOfActiveDownloads { get; set; }
    /// <summary>
    /// Whether to allow running in the background
    /// </summary>
    public bool RunInBackground { get; set; }
    /// <summary>
    /// Speed limit in KiB/s (should be between 512-10240)
    /// </summary>
    public uint SpeedLimit { get; set; }
    /// <summary>
    /// Whether or not to use aria2
    /// </summary>
    public bool UseAria { get; set; }
    /// <summary>
    /// The maximum number of connections to one server for each download (-x)
    /// </summary>
    public int AriaMaxConnectionsPerServer { get; set; }
    /// <summary>
    /// The minimum size of which to split a file (-k)
    /// </summary>
    public int AriaMinSplitSize { get; set; }
    /// <summary>
    /// The path of the cookies file to use for yt-dlp
    /// </summary>
    public string CookiesPath { get; set; }

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
        PreviousSaveFolder = Environment.GetFolderPath(Environment.SpecialFolder.MyVideos);
        PreviousMediaFileType = MediaFileType.MP4;
        EmbedMetadata = true;
        MaxNumberOfActiveDownloads = 5;
        RunInBackground = RuntimeInformation.IsOSPlatform(OSPlatform.Windows);
        SpeedLimit = 1024;
        UseAria = false;
        AriaMaxConnectionsPerServer = 16;
        AriaMinSplitSize = 20;
        CookiesPath = "";
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
