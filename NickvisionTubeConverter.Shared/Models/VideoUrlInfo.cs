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
/// A model of information about a video
/// </summary>
public class VideoInfo : INotifyPropertyChanged
{
    private string _title;
    private bool _toDownload;

    /// <summary>
    /// The video url
    /// </summary>
    public string Url { get; init; }
    /// <summary>
    /// The title of the video
    /// </summary>
    public string OriginalTitle { get; init; }
    /// <summary>
    /// Whether or not the video is part of a playlist 
    /// </summary>
    public bool IsPartOfPlaylist { get; init; }

    public event PropertyChangedEventHandler? PropertyChanged;

    /// <summary>
    /// Constructs a VideoInfo
    /// </summary>
    /// <param name="url">The url of the video</param>
    /// <param name="title">The title of the video</param>
    /// <param name="partOfPlaylist">Whether or not the video is part of a playlist</param>
    public VideoInfo(string url, string title, bool partOfPlaylist = false)
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
    /// Whether or not to download the video
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

    /// <summary>
    /// Parses video info from the Python dictionary returned by yt-dlp
    /// </summary>
    /// <param name="videoInfo">The dictionary</param>
    /// <param name="isPartOfPlaylist">Whether or not the video is part of a playlist</param>
    /// <returns>A VideoUrlInfo object</returns>
    internal static VideoInfo ParseFromPyDict(Python.Runtime.PyDict videoInfo, bool isPartOfPlaylist = false)
    {
        var title = videoInfo.HasKey("title") ? videoInfo["title"].As<string?>() ?? "Media" : "Media";
        var url = videoInfo["webpage_url"].As<string>();       
        foreach (var c in Path.GetInvalidFileNameChars())
        {
            title = title.Replace(c, '_');
        } 
        return new VideoInfo(url, title, isPartOfPlaylist);
    }
}

/// <summary>
/// A model of information about a video url
/// </summary>
public class VideoUrlInfo
{
    /// <summary>
    /// The video url
    /// </summary>
    public string Url { get; init; }
    /// <summary>
    /// All videos found under a video url
    /// </summary>
    public List<VideoInfo> Videos { get; init; }
    /// <summary>
    /// The title of the playlist, if available
    /// </summary>
    public string? PlaylistTitle { get; private set; }

    /// <summary>
    /// Constructs a VideoUrlInfo
    /// </summary>
    /// <param name="url">The url of the video</param>
    private VideoUrlInfo(string url)
    {
        Url = url;
        Videos = new List<VideoInfo>();
    }

    /// <summary>
    /// Gets a VideoUrlInfo from a url string
    /// </summary>
    /// <param name="url">The video url string</param>
    /// <returns>A VideoUrlInfo object. Null if url invalid</returns>
    public static async Task<VideoUrlInfo?> GetAsync(string url)
    {
        var pathToOutput = $"{Configuration.TempDir}{Path.DirectorySeparatorChar}output.log";
        dynamic outFile = PythonHelpers.SetConsoleOutputFilePath(pathToOutput);
        return await Task.Run(() =>
        {
            try
            {
                var videoUrlInfo = new VideoUrlInfo(url);
                using (Python.Runtime.Py.GIL())
                {
                    dynamic ytdlp = Python.Runtime.Py.Import("yt_dlp");
                    var ytOpt = new Dictionary<string, dynamic>() {
                        { "quiet", true },
                        { "merge_output_format", "/" },
                        { "windowsfilenames", RuntimeInformation.IsOSPlatform(OSPlatform.Windows) },
                        { "ignoreerrors", true }
                    };
                    Python.Runtime.PyDict? videoInfo = ytdlp.YoutubeDL(ytOpt).extract_info(url, download: false);
                    if (videoInfo == null)
                    {
                        outFile.close();
                        return null;
                    }
                    if (videoInfo.HasKey("entries"))
                    {
                        videoUrlInfo.PlaylistTitle = videoInfo.HasKey("title") ? videoInfo["title"].As<string>() ?? "Playlist" : "Playlist";
                        foreach (var e in videoInfo["entries"].As<Python.Runtime.PyList>())
                        {
                            videoUrlInfo.Videos.Add(VideoInfo.ParseFromPyDict(e.As<Python.Runtime.PyDict>(), true));
                        }
                    }
                    else
                    {
                        videoUrlInfo.Videos.Add(VideoInfo.ParseFromPyDict(videoInfo));
                    }
                    outFile.close();
                }
                return videoUrlInfo;
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
