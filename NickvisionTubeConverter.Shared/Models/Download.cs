using NickvisionTubeConverter.Shared.Helpers;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
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
    private readonly string _tempDownloadPath;
    private readonly string _logPath;
    private bool _limitSpeed;
    private uint _speedLimit;
    private bool _overwriteFiles;
    private ulong? _pid;
    private Process? _ariaKeeper;

    /// <summary>
    /// The id of the download
    /// </summary>
    public Guid Id { get; init; }
    /// <summary>
    /// The url of the video
    /// </summary>
    public string VideoUrl { get; init; }
    /// <summary>
    /// The save folder for the download
    /// </summary>
    public string SaveFolder { get; init; }
    /// <summary>
    /// The file type of the download
    /// </summary>
    public MediaFileType FileType { get; init; }
    /// <summary>
    /// The quality of the download
    /// </summary>
    public Quality Quality { get; init; }
    /// <summary>
    /// The subtitles for the download
    /// </summary>
    public Subtitle Subtitle { get; init; }
    /// <summary>
    /// The filename of the download
    /// </summary>
    public string Filename { get; private set; }
    /// <summary>
    /// Whether or not the download is running
    /// </summary>
    public bool IsRunning { get; private set; }
    /// <summary>
    /// Whether or not the download is done
    /// </summary>
    public bool IsDone { get; private set; }
    /// <summary>
    /// Whether or not the download was successful
    /// </summary>
    public bool IsSuccess { get; private set; }

    /// <summary>
    /// Occurs when the download's progress is changed
    /// </summary>
    public event EventHandler<DownloadProgressState>? ProgressChanged;
    /// <summary>
    /// Occurs when the download is finished
    /// </summary>
    public event EventHandler<bool>? Completed;

    /// <summary>
    /// Constructs a Download
    /// </summary>
    /// <param name="videoUrl">The url of the video to download</param>
    /// <param name="fileType">The file type to download the video as</param>
    /// <param name="saveFolder">The folder to save the download to</param>
    /// <param name="saveFilename">The filename to save the download as</param>
    /// <param name="limitSpeed">Whether or not to limit the download speed</param>
    /// <param name="speedLimit">The speed at which to limit the download</param>
    /// <param name="quality">The quality of the download</param>
    /// <param name="subtitle">The subtitles for the download</param>
    /// <param name="overwriteFiles">Whether or not to overwrite existing files</param>
    public Download(string videoUrl, MediaFileType fileType, string saveFolder, string saveFilename, bool limitSpeed, uint speedLimit, Quality quality, Subtitle subtitle, bool overwriteFiles)
    {
        Id = Guid.NewGuid();
        VideoUrl = videoUrl;
        SaveFolder = saveFolder;
        FileType = fileType;
        Quality = quality;
        Subtitle = subtitle;
        Filename = $"{saveFilename}{FileType.GetDotExtension()}";
        IsRunning = false;
        IsDone = false;
        IsSuccess = false;
        _tempDownloadPath = $"{Configuration.TempDir}{Path.DirectorySeparatorChar}{Id}{Path.DirectorySeparatorChar}";
        _logPath = $"{_tempDownloadPath}log";
        _limitSpeed = limitSpeed;
        _speedLimit = speedLimit;
        _overwriteFiles = overwriteFiles;
        _pid = null;
        _ariaKeeper = null;
    }

    /// <summary>
    /// Starts the download
    /// </summary>
    /// <param name="useAria">Whether or not to use aria2 for the download</param>
    /// <param name="embedMetadata">Whether or not to embed video metadata in the downloaded file</param>
    /// <param name="localizer">Localizer</param>
    public void Start(bool useAria, bool embedMetadata, Localizer localizer)
    {
        if (!IsRunning)
        {
            IsRunning = true;
            IsSuccess = false;
            //Check if can overwrite
            if (File.Exists($"{SaveFolder}{Path.DirectorySeparatorChar}{Filename}") && !_overwriteFiles)
            {
                ProgressChanged?.Invoke(this, new DownloadProgressState()
                {
                    Status = DownloadProgressStatus.Other,
                    Progress = 0.0,
                    Speed = 0.0,
                    Log = localizer["FileExistsError"]
                });
                IsDone = true;
                IsRunning = false;
                IsSuccess = false;
                Completed?.Invoke(this, IsSuccess);
                return;
            }
            //Escape filename
            if (!RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                Filename = Regex.Escape(Filename);
            }
            //Setup logs
            Directory.CreateDirectory(_tempDownloadPath);
            dynamic outFile = PythonHelpers.SetConsoleOutputFilePath(_logPath);
            //Setup download params
            var hooks = new List<Action<Python.Runtime.PyDict>>();
            hooks.Add(ProgressHook);
            var postHooks = new List<Action<Python.Runtime.PyString>>();
            if (!RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                postHooks.Add(UnescapeHook);
            }
            var ytOpt = new Dictionary<string, dynamic> {
            { "quiet", false },
            { "ignoreerrors", "downloadonly" },
            { "merge_output_format", "mp4/webm/mp3/opus/flac/wav/mkv" },
            { "final_ext", FileType.ToString().ToLower() },
            { "progress_hooks", hooks },
            { "postprocessor_hooks", hooks },
            { "post_hooks", postHooks },
            { "outtmpl", $"{Path.GetFileNameWithoutExtension(Filename)}.%(ext)s" },
            { "ffmpeg_location", DependencyManager.FfmpegPath },
            { "windowsfilenames", RuntimeInformation.IsOSPlatform(OSPlatform.Windows) },
            { "encoding", "utf_8" },
            { "overwrites", _overwriteFiles }
        };
            if (useAria)
            {
                _ariaKeeper = new Process()
                {
                    StartInfo = new ProcessStartInfo()
                    {
                        FileName = DependencyManager.PythonPath,
                        Arguments = $"\"{Configuration.ConfigDir}{Path.DirectorySeparatorChar}aria2_keeper.py\"",
                        UseShellExecute = false,
                        CreateNoWindow = true
                    }
                };
                _ariaKeeper.Start();
                ytOpt.Add("external_downloader", new Dictionary<string, dynamic>() { { "default", DependencyManager.Aria2Path } });
                var ariaArgs = new string[]
                {
                        $"--max-overall-download-limit={(_limitSpeed ? _speedLimit : 0)}K",
                        "--allow-overwrite=true",
                        "--show-console-readout=false",
                        $"--stop-with-process={_ariaKeeper.Id}"
                };
                ytOpt.Add("external_downloader_args", Python.Runtime.PythonEngine.Eval($"{{'default': ['{string.Join("', '", ariaArgs)}']}}")); // stupid, but working
            }
            else if (_limitSpeed)
            {
                ytOpt.Add("ratelimit", _speedLimit * 1024);
            }
            var postProcessors = new List<Dictionary<string, dynamic>>();
            if (FileType.GetIsAudio())
            {
                ytOpt.Add("format", Quality != Quality.Worst ? "ba/b" : "wa/w");
                postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "FFmpegExtractAudio" }, { "preferredcodec", FileType.ToString().ToLower() } });
            }
            else if (FileType.GetIsVideo())
            {
                if (FileType == MediaFileType.MP4)
                {
                    ytOpt.Add("format", Quality switch
                    {
                        Quality.Best => "bv*[ext=mp4]+ba[ext=m4a]/b[ext=mp4] / bv*+ba/b",
                        Quality.Good => "bv*[ext=mp4][height<=720]+ba[ext=m4a]/b[ext=mp4][height<=720] / bv*[height<=720]+ba/b[height<=720]",
                        _ => "wv[ext=mp4]*+wa[ext=m4a]/w[ext=mp4] / wv*+wa/w"
                    });
                }
                else
                {
                    ytOpt.Add("format", Quality switch
                    {
                        Quality.Best => "bv*+ba/b",
                        Quality.Good => "bv*[height<=720]+ba/b[height<=720]",
                        _ => "wv*+wa/w"
                    });
                }
                postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "FFmpegVideoConvertor" }, { "preferedformat", FileType.ToString().ToLower() } });
                if (Subtitle != Subtitle.None)
                {
                    ytOpt.Add("writesubtitles", true);
                    ytOpt.Add("writeautomaticsub", true);
                    ytOpt.Add("subtitleslangs", new List<string> { "en", CultureInfo.CurrentCulture.TwoLetterISOLanguageName });
                    postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "FFmpegSubtitlesConvertor" }, { "format", Subtitle.ToString().ToLower() } });
                    postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "FFmpegEmbedSubtitle" } });
                }
            }
            if (embedMetadata)
            {
                if (FileType.GetSupportsThumbnails())
                {
                    ytOpt.Add("writethumbnail", true);
                    postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "TCEmbedThumbnail" } });
                }
                postProcessors.Insert(0, new Dictionary<string, dynamic>() { { "key", "TCMetadata" }, { "add_metadata", true } });
            }
            if (postProcessors.Count != 0)
            {
                ytOpt.Add("postprocessors", postProcessors);
            }
            //Run download
            Task.Run(() =>
            {
                try
                {
                    using (Python.Runtime.Py.GIL())
                    {
                        _pid = Python.Runtime.PythonEngine.GetPythonThreadID();
                        var paths = new Python.Runtime.PyDict();
                        paths["home"] = new Python.Runtime.PyString($"{SaveFolder}{Path.DirectorySeparatorChar}");
                        paths["temp"] = new Python.Runtime.PyString(_tempDownloadPath);
                        ytOpt.Add("paths", paths);
                        dynamic ytdlp = Python.Runtime.Py.Import("yt_dlp");
                        if (useAria)
                        {
                            ProgressChanged?.Invoke(this, new DownloadProgressState()
                            {
                                Status = DownloadProgressStatus.DownloadingAria,
                                Progress = 0.0,
                                Speed = 0.0,
                                Log = localizer["StartAria"]
                            });
                        }
                        Python.Runtime.PyObject success_code = ytdlp.YoutubeDL(ytOpt).download(new List<string>() { VideoUrl });
                        if ((success_code.As<int?>() ?? 1) != 0)
                        {
                            Filename = Regex.Unescape(Filename);
                        }
                        KillAriaKeeper();
                        ForceUpdateLog();
                        IsDone = true;
                        IsRunning = false;
                        outFile.close();
                        IsSuccess = (success_code.As<int?>() ?? 1) == 0;
                        Completed?.Invoke(this, IsSuccess);
                    }
                }
                catch (Exception e)
                {
                    KillAriaKeeper();
                    Filename = Regex.Unescape(Filename);
                    try
                    {
                        Console.WriteLine(e);
                    }
                    catch { }
                    ForceUpdateLog();
                    IsDone = true;
                    IsRunning = false;
                    outFile.close();
                    IsSuccess = false;
                    Completed?.Invoke(this, IsSuccess);
                }
            }).FireAndForget();
        }
    }

    /// <summary>
    /// Stops the download
    /// </summary>
    public void Stop()
    {
        if (IsRunning)
        {
            if (_pid != null)
            {
                KillAriaKeeper();
                using (Python.Runtime.Py.GIL())
                {
                    Python.Runtime.PythonEngine.Interrupt(_pid.Value);
                }
            }
            IsDone = true;
            IsRunning = false;
            IsSuccess = false;
        }
    }

    /// <summary>
    /// Kills the aria keeper (if used)
    /// </summary>
    private void KillAriaKeeper()
    {
        if (_ariaKeeper != null)
        {
            try
            {
                _ariaKeeper.Kill();
            }
            catch { }
        }
    }

    /// <summary>
    /// Call progress callback to only update the log
    /// </summary>
    private void ForceUpdateLog()
    {
        if (ProgressChanged != null)
        {
            var state = new DownloadProgressState()
            {
                Status = DownloadProgressStatus.Other,
                Progress = 0.0,
                Speed = 0.0
            };
            if (File.Exists(_logPath))
            {
                using var fs = new FileStream(_logPath, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
                using var sr = new StreamReader(fs);
                state.Log = sr.ReadToEnd();
                sr.Close();
                fs.Close();
            }
            ProgressChanged.Invoke(this, state);
        }
    }

    /// <summary>
    /// Handles progress of the download
    /// </summary>
    /// <param name="entries">Python.Runtime.PyDict</param>
    private void ProgressHook(Python.Runtime.PyDict entries)
    {
        if (ProgressChanged != null)
        {
            using (Python.Runtime.Py.GIL())
            {
                var downloaded = entries.HasKey("downloaded_bytes") ? (entries["downloaded_bytes"].As<double?>() ?? 0) : 0;
                var total = 1.0;
                if (entries.HasKey("total_bytes"))
                {
                    total = entries["total_bytes"].As<double?>() ?? 1;
                }
                else if (entries.HasKey("total_bytes_estimate"))
                {
                    total = entries["total_bytes_estimate"].As<double?>() ?? 1;
                }
                var state = new DownloadProgressState()
                {
                    Status = entries["status"].As<string>() switch
                    {
                        "started" or "finished" or "processing" => DownloadProgressStatus.Processing,
                        "downloading" => DownloadProgressStatus.Downloading,
                        _ => DownloadProgressStatus.Other
                    },
                    Progress = downloaded / total,
                    Speed = entries.HasKey("speed") ? (entries["speed"].As<double?>() ?? 0.0) : 0.0
                };
                if (File.Exists(_logPath))
                {
                    using var fs = new FileStream(_logPath, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
                    using var sr = new StreamReader(fs);
                    state.Log = sr.ReadToEnd();
                    sr.Close();
                    fs.Close();
                }
                ProgressChanged.Invoke(this, state);
            }
        }
    }

    /// <summary>
    /// Unescape filename after downloading
    /// </summary>
    /// <param name="path">Python.Runtime.PyString</param>
    private void UnescapeHook(Python.Runtime.PyString path)
    {
        using (Python.Runtime.Py.GIL())
        {
            Filename = Regex.Unescape(Filename);
            var directory = Path.GetDirectoryName(path.As<string>());
            File.Move(path.As<string>(), $"{directory}{Path.DirectorySeparatorChar}{Filename}", _overwriteFiles);
        }
    }
}
