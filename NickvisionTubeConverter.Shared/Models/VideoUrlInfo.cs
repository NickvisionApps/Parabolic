using NickvisionTubeConverter.Shared.Helpers;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// A model of information about a video
/// </summary>
public class VideoInfo
{
    /// <summary>
    /// The video url
    /// </summary>
    public string Url { get; init; }
    /// <summary>
    /// The video id
    /// </summary>
    public string Id { get; init; }
    /// <summary>
    /// The video title
    /// </summary>
    public string Title { get; init; }

    public VideoInfo()
    {
        Url = "";
        Id = "";
        Title = "";
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
    public List<VideoInfo> Videos { get; private set; }

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
                    if (videoInfo.HasKey("entries"))
                    {
                        var entries = videoInfo["entries"].As<Python.Runtime.PyList>();
                        foreach (var e in entries)
                        {
                            var entry = e.As<Python.Runtime.PyDict>();
                            videoUrlInfo.Videos.Add(new VideoInfo()
                            {
                                Url = entry["webpage_url"].As<string>(),
                                Id = entry["id"].As<string>(),
                                Title = entry.HasKey("title") ? entry["title"].As<string>() ?? "Video" : "Video"
                            });
                        }
                    }
                    else
                    {
                        videoUrlInfo.Videos.Add(new VideoInfo()
                        {
                            Url = videoInfo["webpage_url"].As<string>(),
                            Id = videoInfo["id"].As<string>(),
                            Title = videoInfo.HasKey("title") ? videoInfo["title"].As<string>() ?? "Video" : "Video"
                        });
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
