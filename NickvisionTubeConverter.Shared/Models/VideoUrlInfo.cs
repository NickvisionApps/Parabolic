using NickvisionTubeConverter.Shared.Helpers;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.Shared.Models;

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
    /// Whether or not the url is a signle video url
    /// </summary>
    public bool IsSingleVideo { get; private set; }
    /// <summary>
    /// Whether or not the url is a playlist url
    /// </summary>
    public bool IsPlaylist { get; private set; }
    /// <summary>
    /// The title of the single video (Null if playlist)
    /// </summary>
    public string? SingleTitle { get; private set; }

    /// <summary>
    /// Constructs a VideoUrlInfo
    /// </summary>
    /// <param name="url">The url of the video</param>
    private VideoUrlInfo(string url)
    {
        Url = url;
        IsSingleVideo = false;
        IsPlaylist = false;
        SingleTitle = null;
    }

    /// <summary>
    /// Gets a VideoUrlInfo from a url string
    /// </summary>
    /// <param name="url">The video url string</param>
    /// <returns>A VideoUrlInfo object. Null if url invalid</returns>
    public static async Task<VideoUrlInfo?> GetAsync(string url)
    {
        var pathToOutput = $"{Configuration.ConfigDir}{Path.DirectorySeparatorChar}output.log";
        dynamic outFile = PythonExtensions.SetConsoleOutputFilePath(pathToOutput);
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
                        { "merge_output_format", "/" }
                    };
                    Python.Runtime.PyDict videoInfo = ytdlp.YoutubeDL(ytOpt).extract_info(url, download: false);
                    if (videoInfo.HasKey("playlist_count"))
                    {
                        videoUrlInfo.IsPlaylist = true;
                    }
                    else
                    {
                        videoUrlInfo.IsSingleVideo = true;
                        videoUrlInfo.SingleTitle = videoInfo.HasKey("title") ? (videoInfo["title"].As<string?>() ?? "Video") : "Video";
                    }
                    outFile.close();
                }
                return videoUrlInfo;
            }
            catch
            {
                using (Python.Runtime.Py.GIL())
                {
                    outFile.close();
                }
                return null;
            }
        });
    }
}
