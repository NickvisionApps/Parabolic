using NickvisionTubeConverter.Shared.Helpers;
using Python.Runtime;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using static NickvisionTubeConverter.Shared.Helpers.Gettext;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// Qualities for a Download
/// </summary>
public enum Quality
{
    Best = 0,
    Worst,
    Resolution
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
/// A model of a media download
/// </summary>
public class Download
{
    private readonly string _tempDownloadPath;
    private readonly string _logPath;
    private bool _limitSpeed;
    private uint _speedLimit;
    private bool _cropThumbnail;
    private Timeframe? _timeframe;
    private string? _username;
    private string? _password;
    private ulong? _pid;
    private Dictionary<string, dynamic>? _ytOpt;
    private dynamic? _outFile;

    /// <summary>
    /// The id of the download
    /// </summary>
    public Guid Id { get; init; }
    /// <summary>
    /// The url of the media
    /// </summary>
    public string MediaUrl { get; init; }
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
    /// The video resolution (null for audio)
    /// </summary>
    public VideoResolution? Resolution { get; init; }
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
    /// Whether or not the download was stopped
    /// </summary>
    public bool WasStopped { get; private set; }

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
    /// <param name="mediaUrl">The url of the media to download</param>
    /// <param name="fileType">The file type to download the media as</param>
    /// <param name="saveFolder">The folder to save the download to</param>
    /// <param name="saveFilename">The filename to save the download as</param>
    /// <param name="limitSpeed">Whether or not to limit the download speed</param>
    /// <param name="speedLimit">The speed at which to limit the download</param>
    /// <param name="resolution">The video resolution if available</param>
    /// <param name="quality">The quality of the download</param>
    /// <param name="subtitle">The subtitles for the download</param>
    /// <param name="cropThumbnail">Whether or not to crop the thumbnail</param>
    /// <param name="timeframe">A Timeframe to restrict the timespan of the media download</param>
    /// <param name="username">A username for the website (if available)</param>
    /// <param name="password">A password for the website (if available)</param>
    /// <exception cref="ArgumentException">Thrown if timeframe is specified and limitSpeed is enabled</exception>
    public Download(string mediaUrl, MediaFileType fileType, string saveFolder, string saveFilename, bool limitSpeed, uint speedLimit, Quality quality, VideoResolution? resolution, Subtitle subtitle, bool cropThumbnail, Timeframe? timeframe, string? username, string? password)
    {
        Id = Guid.NewGuid();
        MediaUrl = mediaUrl;
        SaveFolder = saveFolder;
        FileType = fileType;
        Quality = quality;
        Resolution = resolution;
        Subtitle = subtitle;
        Filename = $"{saveFilename}{(FileType.GetIsGeneric() ? "" : FileType.GetDotExtension())}";
        IsRunning = false;
        IsDone = false;
        IsSuccess = false;
        WasStopped = false;
        _tempDownloadPath = $"{Configuration.TempDir}{Path.DirectorySeparatorChar}{Id}{Path.DirectorySeparatorChar}";
        _logPath = $"{_tempDownloadPath}log";
        _limitSpeed = limitSpeed;
        _speedLimit = speedLimit;
        _cropThumbnail = cropThumbnail;
        _timeframe = timeframe;
        _username = username;
        _password = password;
        _pid = null;
        if(_timeframe != null && _limitSpeed)
        {
            throw new ArgumentException("A timeframe can only be specified if limit speed is disabled");
        }
    }

    /// <summary>
    /// Starts the download
    /// </summary>
    /// <param name="options">The DownloadOptions</param>
    public void Start(DownloadOptions options)
    {
        if (!IsRunning)
        {
            using (Py.GIL())
            {
                IsRunning = true;
                IsDone = false;
                IsSuccess = false;
                WasStopped = false;
                //Check if can overwrite
                if (File.Exists($"{SaveFolder}{Path.DirectorySeparatorChar}{Filename}") && !options.OverwriteExistingFiles)
                {
                    ProgressChanged?.Invoke(this, new DownloadProgressState()
                    {
                        Status = DownloadProgressStatus.Other,
                        Progress = 0.0,
                        Speed = 0.0,
                        Log = _("File already exists, and overwriting is disallowed")
                    });
                    IsDone = true;
                    IsRunning = false;
                    IsSuccess = false;
                    Completed?.Invoke(this, IsSuccess);
                    return;
                }
                //Setup logs
                Directory.CreateDirectory(_tempDownloadPath);
                _outFile = PythonHelpers.SetConsoleOutputFilePath(_logPath);
                //Setup download params
                var hooks = new List<Action<PyDict>>();
                hooks.Add(ProgressHook);
                _ytOpt = new Dictionary<string, dynamic> {
                    { "quiet", false },
                    { "ignoreerrors", "downloadonly" },
                    { "progress_hooks", hooks },
                    { "postprocessor_hooks", hooks },
                    { "merge_output_format", null },
                    { "outtmpl", $"{Id.ToString()}.%(ext)s" },
                    { "ffmpeg_location", DependencyManager.FfmpegPath },
                    { "windowsfilenames", RuntimeInformation.IsOSPlatform(OSPlatform.Windows) },
                    { "encoding", "utf_8" },
                    { "overwrites", options.OverwriteExistingFiles }
                };
                if(!FileType.GetIsGeneric())
                {
                    _ytOpt.Add("final_ext", FileType.ToString().ToLower());
                }
                if (options.UseAria && _timeframe == null)
                {
                    _ytOpt.Add("external_downloader", new Dictionary<string, dynamic>() { { "default", DependencyManager.Aria2Path } });
                    dynamic ariaDict = new PyDict();
                    dynamic ariaParams = new PyList();
                    ariaParams.Append(new PyString($"--max-overall-download-limit={(_limitSpeed ? _speedLimit : 0)}K"));
                    ariaParams.Append(new PyString("--allow-overwrite=true"));
                    ariaParams.Append(new PyString("--show-console-readout=false"));
                    ariaParams.Append(new PyString($"--max-connection-per-server={options.AriaMaxConnectionsPerServer}"));
                    ariaParams.Append(new PyString($"--min-split-size={options.AriaMinSplitSize}M"));
                    ariaDict["default"] = ariaParams;
                    _ytOpt.Add("external_downloader_args", ariaDict);
                }
                else if (_limitSpeed)
                {
                    _ytOpt.Add("ratelimit", _speedLimit * 1024);
                }
                var postProcessors = new List<Dictionary<string, dynamic>>();
                if (FileType.GetIsAudio())
                {
                    if(FileType.GetIsGeneric())
                    {
                        _ytOpt.Add("format", Quality != Quality.Worst ? "ba/b" : "wa/w");
                        postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "FFmpegExtractAudio" } });
                    }
                    else
                    {
                        _ytOpt.Add("format", Quality != Quality.Worst ? $"ba[ext={FileType.ToString().ToLower()}]/ba/b" : $"wa[ext={FileType.ToString().ToLower()}]/wa/w");
                        postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "FFmpegExtractAudio" }, { "preferredcodec", FileType.ToString().ToLower() } });
                    }
                }
                else if (FileType.GetIsVideo())
                {
                    if(Resolution!.Width == 0 && Resolution.Height == 0)
                    {
                        _ytOpt.Add("format", FileType == MediaFileType.MP4 ? "bv*[ext=mp4]+ba[ext=m4a]/b[ext=mp4] / bv+ba/b" : "bv+ba/b");
                    }
                    else if (FileType == MediaFileType.MP4)
                    {
                        _ytOpt.Add("format", $"bv*[ext=mp4][width<={Resolution!.Width}][height<={Resolution.Height}]+ba[ext=m4a]/b[ext=mp4][width<={Resolution.Width}][height<={Resolution.Height}] / bv*[width<={Resolution.Width}][height<={Resolution.Height}]+ba/b[width<={Resolution.Width}][height<={Resolution.Height}]");
                    }
                    else
                    {
                        _ytOpt.Add("format", $"bv*[width<={Resolution!.Width}][height<={Resolution.Height}]+ba/b[width<={Resolution.Width}][height<={Resolution.Height}]");
                    }
                    if(!FileType.GetIsGeneric())
                    {
                        postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "FFmpegVideoConvertor" }, { "preferedformat", FileType.ToString().ToLower() } });
                    }
                    if (Subtitle != Subtitle.None)
                    {
                        var subtitleLangs = options.SubtitleLangs;
                        if(subtitleLangs[subtitleLangs.Length - 1] == ',')
                        {
                            subtitleLangs = subtitleLangs.Remove(subtitleLangs.Length - 1);
                        }
                        _ytOpt.Add("writesubtitles", true);
                        _ytOpt.Add("writeautomaticsub", true);
                        _ytOpt.Add("subtitleslangs", subtitleLangs.Split(",").ToList());
                        postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "FFmpegSubtitlesConvertor" }, { "format", Subtitle.ToString().ToLower() } });
                        postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "FFmpegEmbedSubtitle" } });
                    }
                }
                if (options.EmbedMetadata)
                {
                    postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "MetadataFromField" }, { "formats", new List<string>() { ":(?P<meta_comment>)", ":(?P<meta_description>)", ":(?P<meta_synopsis>)", ":(?P<meta_purl>)" } } });
                    postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "TCMetadata" }, { "add_metadata", true } });
                    if (FileType.GetSupportsThumbnails())
                    {
                        _ytOpt.Add("writethumbnail", true);
                        postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "TCEmbedThumbnail" } });
                        if (_cropThumbnail)
                        {
                            dynamic cropDict = new PyDict();
                            dynamic cropParams = new PyList();
                            cropParams.Append(new PyString("-vf"));
                            cropParams.Append(new PyString("crop=\'if(gt(ih,iw),iw,ih)\':\'if(gt(iw,ih),ih,iw)\'"));
                            cropDict["thumbnailsconvertor"] = cropParams;
                            _ytOpt.Add("postprocessor_args", cropDict);
                        }
                    }
                }
                if (options.EmbedChapters)
                {
                    postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "TCMetadata" }, { "add_chapters", true } });
                }
                if (postProcessors.Count != 0)
                {
                    _ytOpt.Add("postprocessors", postProcessors);
                }
                if(!string.IsNullOrEmpty(_username))
                {
                    _ytOpt.Add("username", _username);
                }
                if(!string.IsNullOrEmpty(_password))
                {
                    _ytOpt.Add("password", _password);
                }
            }
            //Run download
            Task.Run(() =>
            {
                try
                {
                    using (Py.GIL())
                    {
                        _pid = PythonEngine.GetPythonThreadID();
                        dynamic ytdlp = Py.Import("yt_dlp");
                        var paths = new PyDict();
                        paths["home"] = new PyString($"{SaveFolder}{Path.DirectorySeparatorChar}");
                        paths["temp"] = new PyString(_tempDownloadPath);
                        _ytOpt.Add("paths", paths);
                        if (File.Exists(options.CookiesPath))
                        {
                            _ytOpt.Add("cookiefile", new PyString(options.CookiesPath));
                        }
                        if (options.UseAria)
                        {
                            ProgressChanged?.Invoke(this, new DownloadProgressState()
                            {
                                Status = DownloadProgressStatus.DownloadingAria,
                                Progress = 0.0,
                                Speed = 0.0,
                                Log = _("Download using aria2 has started")
                            });
                        }
                        if(_timeframe != null)
                        {
                            _ytOpt.Add("download_ranges", ytdlp.utils.download_range_func(null, new List<List<double>>() { new List<double>() { _timeframe.Start.TotalSeconds, _timeframe.End.TotalSeconds } }));
                            ProgressChanged?.Invoke(this, new DownloadProgressState()
                            {
                                Status = DownloadProgressStatus.DownloadingFfmpeg,
                                Progress = 0.0,
                                Speed = 0.0,
                                Log = _("Download using ffmpeg has started")
                            });
                        }
                        PyObject success_code = ytdlp.YoutubeDL(_ytOpt).download(new List<string>() { MediaUrl });
                        ForceUpdateLog();
                        IsDone = true;
                        IsRunning = false;
                        _outFile.close();
                        IsSuccess = (success_code.As<int?>() ?? 1) == 0;
                        if(IsSuccess)
                        {
                            var genericExtensionFound = false;
                            foreach (var path in Directory.EnumerateFiles(SaveFolder))
                            {
                                if(path.Contains(Id.ToString()))
                                {
                                    if(FileType.GetIsGeneric() && !genericExtensionFound)
                                    {
                                        var extension = Path.GetExtension(path).ToLower();
                                        if(extension != ".srt" && extension != ".vtt")
                                        {
                                            Filename += extension;
                                            genericExtensionFound = true;
                                        }
                                    }
                                    try
                                    {
                                        File.Move(path, path.Replace(Id.ToString(), Path.GetFileNameWithoutExtension(Filename)), options.OverwriteExistingFiles);
                                    }
                                    catch
                                    {
                                        var chars = new char[] { '"', '*', '/', ':', '<', '>', '?', '\\' };
                                        foreach(var c in chars.Where(x => Filename.Contains(x)))
                                        {
                                            Filename = Filename.Replace(c, '_');
                                        }
                                        File.Move(path, path.Replace(Id.ToString(), Path.GetFileNameWithoutExtension(Filename)), options.OverwriteExistingFiles);
                                    }
                                }
                            }
                        }
                        Completed?.Invoke(this, IsSuccess);
                    }
                }
                catch (Exception e)
                {
                    try
                    {
                        Console.WriteLine(e);
                    }
                    catch { }
                    ForceUpdateLog();
                    IsDone = true;
                    IsRunning = false;
                    _outFile.close();
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
                using (Py.GIL())
                {
                    // Kill ffmpeg and aria
                    dynamic psutil = Py.Import("psutil");
                    var pythonProcessChildren = psutil.Process().children(recursive: true);
                    foreach(PyObject child in pythonProcessChildren)
                    {
                        var processName = child.GetAttr(new PyString("name")).Invoke().As<string?>() ?? "";
                        if(processName == "ffmpeg" || processName == "aria2c")
                        {
                            child.InvokeMethod("kill");
                        }
                    }
                    // Kill Python
                    PythonEngine.Interrupt(_pid.Value);
                }
            }
            IsDone = true;
            IsRunning = false;
            IsSuccess = false;
            WasStopped = true;
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
    /// <param name="entries">PyDict</param>
    private void ProgressHook(PyDict entries)
    {
        if (ProgressChanged != null)
        {
            using (Py.GIL())
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
                if (state.Status == DownloadProgressStatus.Processing)
                {
                    state.Progress = 1;
                }
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
}
