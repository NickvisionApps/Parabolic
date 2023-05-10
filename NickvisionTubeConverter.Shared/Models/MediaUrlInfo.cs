using NickvisionTubeConverter.Shared.Helpers;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// A model of information about a media
/// </summary>
public class MediaInfo : INotifyPropertyChanged
{
    private string _title;
    private bool _toDownload;

    /// <summary>
    /// The media url
    /// </summary>
    public string Url { get; init; }
    /// <summary>
    /// The title of the media
    /// </summary>
    public string OriginalTitle { get; init; }
    /// <summary>
    /// Whether or not the media is part of a playlist 
    /// </summary>
    public bool IsPartOfPlaylist { get; init; }

    public event PropertyChangedEventHandler? PropertyChanged;

    /// <summary>
    /// Constructs a MediaInfo
    /// </summary>
    /// <param name="url">The url of the media</param>
    /// <param name="title">The title of the media</param>
    /// <param name="partOfPlaylist">Whether or not the media is part of a playlist</param>
    public MediaInfo(string url, string title, bool partOfPlaylist = false)
    {
        _title = title;
        _toDownload = true;
        Url = url;
        OriginalTitle = title;
        IsPartOfPlaylist = partOfPlaylist;
    }

    /// <summary>
    /// The title to use for downloading
    /// </summary>
    public string Title
    {
        get => _title;

        set
        {
            _title = !string.IsNullOrEmpty(value) ? value : OriginalTitle;
            NotifyPropertyChanged();
        }
    }

    /// <summary>
    /// Whether or not to download the media
    /// </summary>
    public bool ToDownload
    {
        get => _toDownload;

        set
        {
            _toDownload = value;
            NotifyPropertyChanged();
        }
    }

    private void NotifyPropertyChanged([CallerMemberName] string propertyName = "") => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
}

/// <summary>
/// A model of information about a media url
/// </summary>
public class MediaUrlInfo
{
    /// <summary>
    /// The media url
    /// </summary>
    public string Url { get; init; }
    /// <summary>
    /// All medias found under a media url
    /// </summary>
    public List<MediaInfo> MediaList { get; init; }
    /// <summary>
    /// The title of the playlist, if available
    /// </summary>
    public string? PlaylistTitle { get; private set; }

    /// <summary>
    /// Constructs a MediaUrlInfo
    /// </summary>
    /// <param name="url">The url of the media</param>
    private MediaUrlInfo(string url)
    {
        Url = url;
        MediaList = new List<MediaInfo>();
    }

    /// <summary>
    /// Gets a MediaUrlInfo from a url string
    /// </summary>
    /// <param name="url">The media url string</param>
    /// <returns>A MediaUrlInfo object. Null if url invalid</returns>
    public static async Task<MediaUrlInfo?> GetAsync(string url)
    {
        var pathToOutput = $"{Configuration.TempDir}{Path.DirectorySeparatorChar}output.log";
        dynamic outFile = PythonHelpers.SetConsoleOutputFilePath(pathToOutput);
        return await Task.Run(() =>
        {
            try
            {
                var mediaUrlInfo = new MediaUrlInfo(url);
                using (Python.Runtime.Py.GIL())
                {
                    dynamic ytdlp = Python.Runtime.Py.Import("yt_dlp");
                    var ytOpt = new Dictionary<string, dynamic>() {
                        { "quiet", true },
                        { "merge_output_format", "/" },
                        { "windowsfilenames", RuntimeInformation.IsOSPlatform(OSPlatform.Windows) },
                        { "ignoreerrors", true }
                    };
                    Python.Runtime.PyDict? mediaInfo = ytdlp.YoutubeDL(ytOpt).extract_info(url, download: false);
                    if (mediaInfo == null)
                    {
                        outFile.close();
                        return null;
                    }
                    if (mediaInfo.HasKey("entries"))
                    {
                        mediaUrlInfo.PlaylistTitle = mediaInfo.HasKey("title") ? mediaInfo["title"].As<string>() ?? "Playlist" : "Playlist";
                        foreach (var e in mediaInfo["entries"].As<Python.Runtime.PyList>())
                        {
                            if (e.IsNone())
                            {
                                continue;
                            }
                            var entry = e.As<Python.Runtime.PyDict>();
                            var title = entry.HasKey("title") ? (entry["title"].As<string?>() ?? "Media") : "Media";
                            foreach (var c in Path.GetInvalidFileNameChars())
                            {
                                title = title.Replace(c, '_');
                            }
                            mediaUrlInfo.MediaList.Add(new MediaInfo(entry["webpage_url"].As<string>(), title, true));
                        }
                    }
                    else
                    {
                        var title = mediaInfo.HasKey("title") ? (mediaInfo["title"].As<string?>() ?? "Media") : "Media";
                        foreach (var c in Path.GetInvalidFileNameChars())
                        {
                            title = title.Replace(c, '_');
                        }
                        mediaUrlInfo.MediaList.Add(new MediaInfo(mediaInfo["webpage_url"].As<string>(), title));
                    }
                    outFile.close();
                }
                return mediaUrlInfo;
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                using (Python.Runtime.Py.GIL())
                {
                    outFile.close();
                }
                return null;
            }
        });
    }
}
