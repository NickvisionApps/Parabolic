using NickvisionTubeConverter.Shared.Helpers;
using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// Qualities for a Download
/// </summary>
public enum Quality
{
    Best = 0,
    Good,
    Worst
}

/// <summary>
/// Subtitle types for a download
/// </summary>
public enum Subtitle
{
    None = 0,
    VTT,
    SRT
}

/// <summary>
/// A model of a video download
/// </summary>
public class Download
{
    private CancellationTokenSource? _cancellationToken;
    private MediaFileType _fileType;
    private Quality _quality;
    private Subtitle _subtitle;

    /// <summary>
    /// The url of the video
    /// </summary>
    public string VideoUrl { get; init; }
    /// <summary>
    /// The save folder for the download
    /// </summary>
    public string SaveFolder { get; init; }
    /// <summary>
    /// The filename of the download
    /// </summary>
    public string Filename { get; private set; }
    /// <summary>
    /// Whether or not the download has completed
    /// </summary>
    public bool IsDone { get; private set; }

    private Action<Dictionary<string, string>>? _progressCallback;

    /// <summary>
    /// Constructs a Download
    /// </summary>
    /// <param name="videoUrl">The url of the video to download</param>
    /// <param name="fileType">The file type to download the video as</param>
    /// <param name="saveFolder">The folder to save the download to</param>
    /// <param name="newFilename">The filename to save the download as</param>
    /// <param name="quality">The quality of the download</param>
    /// <param name="subtitle">The subtitles for the download</param>
    public Download(string videoUrl, MediaFileType fileType, string saveFolder, string saveFilename, Quality quality = Quality.Best, Subtitle subtitle = Subtitle.None)
    {
        _cancellationToken = null;
        _fileType = fileType;
        _quality = quality;
        _subtitle = subtitle;
        VideoUrl = videoUrl;
        SaveFolder = saveFolder;
        Filename = saveFilename;
        IsDone = false;
    }

    /// <summary>
    /// Gets whether or not a video url is valid
    /// </summary>
    /// <param name="url">The video url to check</param>
    /// <returns>True if valid, else false</returns>
    public static async Task<bool> GetIsValidVideoUrlAsync(string url)
    {
        return await Task.Run(() =>
        {
            try
            {
                using (Python.Runtime.Py.GIL())
                {
                    dynamic ytdlp = Python.Runtime.Py.Import("yt_dlp");
                    var ytOpt = new Dictionary<string, dynamic>() { { "quiet", true }, { "merge_output_format", "/" } };
                    ytdlp.YoutubeDL(ytOpt).extract_info(url, download: false);
                }
                return true;
            }
            catch(Exception e)
            {
                return false;
            }
        });
    }

    /// <summary>
    /// Gets video title from metadata
    /// </summary>
    /// <param name="url">The video url to check</param>
    /// <returns>Title string</returns>
    public static async Task<string> GetVideoTitleAsync(string url)
    {
        return await Task.Run(() =>
        {
            var title = "";
            try
            {
                using (Python.Runtime.Py.GIL())
                {
                    dynamic ytdlp = Python.Runtime.Py.Import("yt_dlp");
                    var ytOpt = new Dictionary<string, dynamic>() { { "quiet", true }, { "merge_output_format", "/" } };
                    title = ytdlp.YoutubeDL(ytOpt).extract_info(url, download: false)["title"];
                }
            }
            catch(Exception e)
            {
                title = "video";
            }
            return title;
        });
        
    }

    private void YtdlpHook(Python.Runtime.PyDict entries)
    {
        using (Python.Runtime.Py.GIL())
        {
            var result = new Dictionary<string, string> {};
            result.Add("status", entries["status"].As<string>());
            if (entries.HasKey("downloaded_bytes"))
            {
                var progress = entries["downloaded_bytes"].As<double>() / entries["total_bytes"].As<double>();
                result.Add("progress", progress.ToString());
            }
            try // entries["speed"] is None if speed is unknown
            {
                result.Add("speed", entries["speed"].As<double>().ToString());
            }
            catch
            {
                result.Add("speed", "0");
            }
            _progressCallback(result);
        }
    }

    /// <summary>
    /// Runs the download
    /// </summary>
    /// <param name="embedMetadata">Whether or not to embed video metadata in the downloaded file</param>
    /// <param name="progressCallback">A callback function for DownloadProgresss</param>
    /// <returns>True if successful, else false</returns>
    public async Task<bool> RunAsync(bool embedMetadata, Action<Dictionary<string, string>>? progressCallback = null)
    {
        _progressCallback = progressCallback;
        return await Task.Run(() =>
        {
            IsDone = false;
            _cancellationToken = new CancellationTokenSource();
            using (Python.Runtime.Py.GIL())
            {
                dynamic ytdlp = Python.Runtime.Py.Import("yt_dlp");
                var hooks = new List<Action<Python.Runtime.PyDict>> {};
                hooks.Add(YtdlpHook);
                var ytOpt = new Dictionary<string, dynamic> {
                    {"quiet", true},
                    {"ignoreerrors", "downloadonly"},
                    {"merge_output_format", "mp4/webm/mp3/opus/flac/wav"},
                    {"progress_hooks", hooks},
                    {"postprocessor_hooks", hooks}
                };
                if (_fileType.GetIsAudio())
                {
                    ytOpt.Add("format", "ba/b");
                }
                else if (_fileType == MediaFileType.MP4)
                {
                    ytOpt.Add("format", "bv[ext=mp4]+ba[ext=m4a]/b[ext=mp4]");
                }
                else
                {
                    ytOpt.Add("format", "bv+ba/b");
                }
                try
                {
                    ytdlp.YoutubeDL(ytOpt).download(new string[1] {VideoUrl});
                    IsDone = true;
                    return true;
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                    IsDone = true;
                    return false;
                }
            }
        });
    }

    /// <summary>
    /// Stops the download
    /// </summary>
    public void Stop() => _cancellationToken?.Cancel();
}
