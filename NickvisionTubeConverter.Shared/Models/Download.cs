using NickvisionTubeConverter.Shared.Helpers;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
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
    private Action<DownloadProgressState>? _progressCallback;

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
        _progressCallback = null;
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
                    var ytOpt = new Dictionary<string, dynamic>() { 
                        { "quiet", true }, 
                        { "merge_output_format", "/" } 
                    };
                    ytdlp.YoutubeDL(ytOpt).extract_info(url, download: false);
                }
                return true;
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
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
                    var ytOpt = new Dictionary<string, dynamic>() { 
                        { "quiet", true }, 
                        { "merge_output_format", "/" } 
                    };
                    title = ytdlp.YoutubeDL(ytOpt).extract_info(url, download: false)["title"];
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                title = "video";
            }
            return title;
        });
    }

    /// <summary>
    /// Runs the download
    /// </summary>
    /// <param name="embedMetadata">Whether or not to embed video metadata in the downloaded file</param>
    /// <param name="progressCallback">A callback function for DownloadProgressState</param>
    /// <returns>True if successful, else false</returns>
    public async Task<bool> RunAsync(bool embedMetadata, Action<DownloadProgressState>? progressCallback = null)
    {
        _progressCallback = progressCallback;
        _cancellationToken = new CancellationTokenSource();
        try
        {
            IsDone = false;
            return await Task.Run(() =>
            {
                using (Python.Runtime.Py.GIL())
                {
                    dynamic ytdlp = Python.Runtime.Py.Import("yt_dlp");
                    var hooks = new List<Action<Python.Runtime.PyDict>> { };
                    hooks.Add(ProgressHook);
                    var ytOpt = new Dictionary<string, dynamic> {
                        { "quiet", true },
                        { "ignoreerrors", "downloadonly" },
                        { "merge_output_format", "mp4/webm/mp3/opus/flac/wav" },
                        { "final_ext", _fileType.ToString().ToLower() },
                        { "progress_hooks", hooks },
                        { "postprocessor_hooks", hooks },
                        { "outtmpl", $"{SaveFolder}{Path.DirectorySeparatorChar}{Path.GetFileNameWithoutExtension(Filename)}.%(ext)s" },
                        { "ffmpeg_location", DependencyManager.Ffmpeg },
                        { "windowsfilenames", RuntimeInformation.IsOSPlatform(OSPlatform.Windows) },
                        { "encoding", "utf_8" }
                    };
                    if(RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
                    {
                        dynamic sys = Python.Runtime.Py.Import("sys");
                        var pathToOutput = $"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}{Path.DirectorySeparatorChar}Nickvision{Path.DirectorySeparatorChar}{AppInfo.Current.Name}{Path.DirectorySeparatorChar}output.txt";
                        sys.stdout = Python.Runtime.PythonEngine.Eval($"open(\"{Regex.Replace(pathToOutput, @"\\", @"\\")}\", \"w\")");
                    }
                    var postProcessors = new List<Dictionary<string, dynamic>>();
                    if (embedMetadata)
                    {
                        if (_fileType.GetSupportsThumbnails())
                        {
                            postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "EmbedThumbnail" } });
                        }
                        postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "FFmpegMetadata" }, { "add_metadata", true } });
                    }
                    if (_fileType.GetIsAudio())
                    {
                        ytOpt.Add("format", _quality != Quality.Worst ? "ba/b" : "wa/w");
                        postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "FFmpegExtractAudio" }, { "preferredcodec", _fileType.ToString().ToLower() } });
                    }
                    else if (_fileType.GetIsVideo())
                    {
                        if(_fileType == MediaFileType.MP4)
                        {
                            ytOpt.Add("format", _quality switch
                            {
                                Quality.Best => "bv*[ext=mp4]+ba[ext=m4a]/b[ext=mp4] / bv*+ba/b",
                                Quality.Good => "bv*[ext=mp4][height<=720]+ba[ext=m4a]/b[ext=mp4][height<=720] / bv*[height<=720]+ba/b[height<=720]",
                                _ => "wv[ext=mp4]*+wa[ext=m4a]/w[ext=mp4] / wv*+wa/w"
                            });
                        }
                        else
                        {
                            ytOpt.Add("format", _quality switch
                            {
                                Quality.Best => "bv*+ba/b",
                                Quality.Good => "bv*[height<=720]+ba/b[height<=720]",
                                _ => "wv*+wa/w"
                            });
                        }
                        postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "FFmpegVideoRemuxer" }, { "preferedformat", _fileType.ToString().ToLower() } });
                        if (_subtitle != Subtitle.None)
                        {
                            ytOpt.Add("writesubtitles", true);
                            ytOpt.Add("writeautomaticsub", true);
                            ytOpt.Add("subtitleslangs", new List<string> {"en", CultureInfo.CurrentCulture.TwoLetterISOLanguageName});
                            postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "FFmpegSubtitlesConvertor" }, { "format", _subtitle.ToString().ToLower() } });
                            postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "FFmpegEmbedSubtitle" } });
                        }
                    }
                    if (postProcessors.Count != 0)
                    {
                        ytOpt.Add("postprocessors", postProcessors);
                    }
                    try
                    {
                        Python.Runtime.PyObject success_code = ytdlp.YoutubeDL(ytOpt).download(new List<string>() { VideoUrl });
                        IsDone = true;
                        return (success_code.As<int?>() ?? 1) == 0;
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e);
                        IsDone = true;
                        return false;
                    }
                }
            }, _cancellationToken.Token);
        }
        catch (TaskCanceledException)
        {
            IsDone = false;
            return false;
        }
    }

    /// <summary>
    /// Stops the download
    /// </summary>
    public void Stop() => _cancellationToken?.Cancel();

    /// <summary>
    /// Handles progress of the download
    /// </summary>
    /// <param name="entries">Python.Runtime.PyDict</param>
    private void ProgressHook(Python.Runtime.PyDict entries)
    {
        if (_progressCallback != null)
        {
            using (Python.Runtime.Py.GIL())
            {
                _progressCallback(new DownloadProgressState()
                {
                    Status = entries["status"].As<string>() switch
                    {
                        "processing" => DownloadProgressStatus.Processing,
                        "downloading" => DownloadProgressStatus.Downloading,
                        _ => DownloadProgressStatus.Other
                    },
                    Progress = entries.HasKey("downloaded_bytes") ? (entries["downloaded_bytes"].As<double?>() ?? 0) / (entries["total_bytes"].As<double?>() ?? 1) : 0,
                    Speed = entries.HasKey("speed") ? (entries["speed"].As<double?>() ?? 0) / 1024f : 0
                });
            }

        }
    }
}
