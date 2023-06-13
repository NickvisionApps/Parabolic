using NickvisionTubeConverter.Shared.Models;

namespace NickvisionTubeConverter.Shared.Controllers;

/// <summary>
/// A controller for a PreferencesView
/// </summary>
public class PreferencesViewController
{
    /// <summary>
    /// Gets the AppInfo object
    /// </summary>
    public AppInfo AppInfo => AppInfo.Current;

    /// <summary>
    /// Constructs a PreferencesViewController
    /// </summary>
    internal PreferencesViewController()
    {
        
    }

    /// <summary>
    /// The preferred theme of the application
    /// </summary>
    public Theme Theme
    {
        get => Configuration.Current.Theme;

        set => Configuration.Current.Theme = value;
    }

    /// <summary>
    /// The preference of how often to show completed notifications
    /// </summary>
    public NotificationPreference CompletedNotificationPreference
    {
        get => Configuration.Current.CompletedNotificationPreference;

        set => Configuration.Current.CompletedNotificationPreference = value;
    }

    /// <summary>
    /// Whether to allow running in the background
    /// </summary>
    public bool RunInBackground
    {
        get => Configuration.Current.RunInBackground;

        set => Configuration.Current.RunInBackground = value;
    }

    /// <summary>
    /// The maximum number of active downloads (should be between 1-10)
    /// </summary>
    public int MaxNumberOfActiveDownloads
    {
        get => Configuration.Current.MaxNumberOfActiveDownloads;

        set => Configuration.Current.MaxNumberOfActiveDownloads = value;
    }

    /// <summary>
    /// Whether or not to overwrite existing files
    /// </summary>
    public bool OverwriteExistingFiles
    {
        get => Configuration.Current.OverwriteExistingFiles;

        set => Configuration.Current.OverwriteExistingFiles = value;
    }

    /// <summary>
    /// Speed limit in KiB/s (should be between 512-10240)
    /// </summary>
    public uint SpeedLimit
    {
        get => Configuration.Current.SpeedLimit;

        set => Configuration.Current.SpeedLimit = value;
    }

    /// <summary>
    /// Whether or not to use aria2
    /// </summary>
    public bool UseAria
    {
        get => Configuration.Current.UseAria;

        set => Configuration.Current.UseAria = value;
    }

    /// <summary>
    /// The maximum number of connections to one server for each download (-x)
    /// </summary>
    public int AriaMaxConnectionsPerServer
    {
         get => Configuration.Current.AriaMaxConnectionsPerServer;

         set => Configuration.Current.AriaMaxConnectionsPerServer = value;
    }

    /// <summary>
    /// The minimum size of which to split a file (-k)
    /// </summary>
    public int AriaMinSplitSize
    {
        get => Configuration.Current.AriaMinSplitSize;

        set => Configuration.Current.AriaMinSplitSize = value;
    }

    /// <summary>
    /// A comma separated list of language codes for subtitle downloads
    /// </summary>
    public string SubtitleLangs
    {
        get => Configuration.Current.SubtitleLangs;

        set => Configuration.Current.SubtitleLangs = value;
    }

    /// <summary>
    /// The path of the cookies file to use for yt-dlp
    /// </summary>
    public string CookiesPath
    {
        get => Configuration.Current.CookiesPath;

        set => Configuration.Current.CookiesPath = value;
    }

    /// <summary>
    /// Whether or not to disallow converting of formats
    /// </summary>
    public bool DisallowConversions
    {
        get => Configuration.Current.DisallowConversions;

        set => Configuration.Current.DisallowConversions = value;
    }

    /// <summary>
    /// Whether or not to embed metadata in a download
    /// </summary>
    public bool EmbedMetadata
    {
        get => Configuration.Current.EmbedMetadata;

        set => Configuration.Current.EmbedMetadata = value;
    }

    /// <summary>
    /// Whether or not to turn on crop thumbnail in an audio download
    /// </summary>
    public bool CropAudioThumbnails
    {
        get => Configuration.Current.CropAudioThumbnails;

        set => Configuration.Current.CropAudioThumbnails = value;
    }

    /// <summary>
    /// Whether or not to embed chapters in a download
    /// </summary>
    public bool EmbedChapters
    {
        get => Configuration.Current.EmbedChapters;

        set => Configuration.Current.EmbedChapters = value;
    }

    /// <summary>
    /// Saves the configuration to disk
    /// </summary>
    public void SaveConfiguration() => Configuration.Current.Save();
}
