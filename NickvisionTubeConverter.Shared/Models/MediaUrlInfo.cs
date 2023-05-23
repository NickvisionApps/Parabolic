using NickvisionTubeConverter.Shared.Helpers;
using Python.Runtime;
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.Shared.Models;

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
    /// All media found under a media url
    /// </summary>
    public List<MediaInfo> MediaList { get; init; }
    /// <summary>
    /// The available video resolutions
    /// </summary>
    public List<VideoResolution> VideoResolutions { get; init; }

    /// <summary>
    /// Constructs a MediaUrlInfo
    /// </summary>
    /// <param name="url">The url of the media</param>
    private MediaUrlInfo(string url)
    {
        Url = url;
        MediaList = new List<MediaInfo>();
        VideoResolutions = new List<VideoResolution>();
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
                using (Py.GIL())
                {
                    dynamic ytdlp = Py.Import("yt_dlp");
                    var ytOpt = new Dictionary<string, dynamic>() {
                        { "quiet", true },
                        { "merge_output_format", "/" },
                        { "windowsfilenames", RuntimeInformation.IsOSPlatform(OSPlatform.Windows) },
                        { "ignoreerrors", true }
                    };
                    PyDict? mediaInfo = ytdlp.YoutubeDL(ytOpt).extract_info(url, download: false);
                    if (mediaInfo == null)
                    {
                        outFile.close();
                        return null;
                    }
                    if (mediaInfo.HasKey("entries"))
                    {
                        foreach (var e in mediaInfo["entries"].As<PyList>())
                        {
                            if (e.IsNone())
                            {
                                continue;
                            }
                            mediaUrlInfo.ParseFromPyDict(e.As<PyDict>(), true);
                        }
                    }
                    else
                    {
                        mediaUrlInfo.ParseFromPyDict(mediaInfo);
                    }
                    outFile.close();
                }
                return mediaUrlInfo;
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                using (Py.GIL())
                {
                    outFile.close();
                }
                return null;
            }
        });
    }

    /// <summary>
    /// Adds data retrieved from yt-dlp to MediaInfo
    /// </summary>
    /// <param name="mediaInfo">Python dictionary with media info</param>
    /// <param name="isPartOfPlaylist">Whether or not the media is part of a playlist</param>
    private void ParseFromPyDict(PyDict mediaInfo, bool isPartOfPlaylist = false)
    {
        var title = mediaInfo.HasKey("title") ? (mediaInfo["title"].As<string?>() ?? "Media") : "Media";
        foreach (var c in Path.GetInvalidFileNameChars())
        {
            title = title.Replace(c, '_');
        }
        foreach (var f in mediaInfo["formats"].As<PyList>())
        {
            var format = f.As<PyDict>();
            if (format.HasKey("vbr"))
            {
                var resolution = new VideoResolution(format["width"].As<int>(), format["height"].As<int>());
                if (!VideoResolutions.Contains(resolution))
                {
                    VideoResolutions.Add(resolution);
                }
            }
        }
        VideoResolutions.Sort((a, b) => b.CompareTo(a));
        MediaList.Add(new MediaInfo(mediaInfo["webpage_url"].As<string>(), title, mediaInfo["duration"].As<double>(), isPartOfPlaylist));
    }
}
