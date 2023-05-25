using NickvisionTubeConverter.Shared.Helpers;
using NickvisionTubeConverter.Shared.Models;

namespace NickvisionTubeConverter.Shared.Controllers;

/// <summary>
/// A controller for a PreferencesView
/// </summary>
public class PreferencesViewController
{
    /// <summary>
    /// The localizer to get translated strings from
    /// </summary>
    public Localizer Localizer { get; init; }

    /// <summary>
    /// Gets the AppInfo object
    /// </summary>
    public AppInfo AppInfo => AppInfo.Current;

    /// <summary>
    /// Constructs a PreferencesViewController
    /// </summary>
    internal PreferencesViewController(Localizer localizer)
    {
        Localizer = localizer;
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
    /// Whether to allow running in the background
    /// </summary>
    public bool RunInBackground
    {
        get => Configuration.Current.RunInBackground;

        set => Configuration.Current.RunInBackground = value;
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
    /// The maximum number of active downloads (should be between 1-10)
    /// </summary>
    public int MaxNumberOfActiveDownloads
    {
        get => Configuration.Current.MaxNumberOfActiveDownloads;

        set => Configuration.Current.MaxNumberOfActiveDownloads = value;
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
    /// The path of the cookies file to use for yt-dlp
    /// </summary>
    public string CookiesPath
    {
        get => Configuration.Current.CookiesPath;

        set => Configuration.Current.CookiesPath = value;
    }

    /// <summary>
    /// Saves the configuration to disk
    /// </summary>
    public void SaveConfiguration() => Configuration.Current.Save();
}
