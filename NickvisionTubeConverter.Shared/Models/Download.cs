using Nickvision.Aura;
using Nickvision.Aura.Helpers;
using NickvisionTubeConverter.Shared.Helpers;
using Python.Runtime;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using static Nickvision.Aura.Localization.Gettext;

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
/// A model of a media download
/// </summary>
public class Download
{
    internal static readonly string[] YoutubeLangCodes = { "af", "az", "id", "ms", "bs", "ca", "cs", "da", "de", "et", "en-IN", "en-GB", "en", "es", "es-419", "es-US", "eu", "fil", "fr", "fr-CA", "gl", "hr", "zu", "is", "it", "sw", "lv", "lt", "hu", "nl", "no", "uz", "pl", "pt-PT", "pt", "ro", "sq", "sk", "sl", "sr-Latn", "fi", "sv", "vi", "tr", "be", "bg", "ky", "kk", "mk", "mn", "ru", "sr", "uk", "el", "hy", "iw", "ur", "ar", "fa", "ne", "mr", "hi", "as", "bn", "pa", "gu", "or", "ta", "te", "kn", "ml", "si", "th", "lo", "my", "ka", "am", "km", "zh-CN", "zh-TW", "zh-HK", "ja", "ko" };

    private readonly string _tempDownloadPath;
    private readonly string _logPath;
    private readonly uint _playlistPosition;
    private readonly AdvancedDownloadOptions _advancedOptions;
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
    /// Whether or not to download the subtitles
    /// </summary>
    public bool Subtitle { get; init; }
    /// <summary>
    /// The audio language code
    /// </summary>
    public string AudioLanguage { get; init; }
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
    /// <param name="quality">The quality of the download</param>
    /// <param name="resolution">The video resolution if available</param>
    /// <param name="audioLanguage">The audio language code</param>
    /// <param name="subtitle">Whether or not to download the subtitles</param>
    /// <param name="saveFolder">The folder to save the download to</param>
    /// <param name="saveFilename">The filename to save the download as</param>
    /// <param name="playlistPosition">Position in playlist starting with 1, or 0 if not in playlist</param>
    /// <param name="options">AdvancedDownloadOptions</param>
    /// <exception cref="ArgumentException">Thrown if timeframe is specified and limitSpeed is enabled</exception>
    public Download(string mediaUrl, MediaFileType fileType, Quality quality, VideoResolution? resolution, string? audioLanguage, bool subtitle, string saveFolder, string saveFilename, uint playlistPosition, AdvancedDownloadOptions options)
    {
        Id = Guid.NewGuid();
        MediaUrl = mediaUrl;
        SaveFolder = saveFolder;
        FileType = fileType;
        Quality = quality;
        Resolution = resolution;
        Subtitle = subtitle;
        AudioLanguage = string.IsNullOrEmpty(audioLanguage) ? CultureInfo.CurrentCulture.TwoLetterISOLanguageName : audioLanguage;
        Filename = $"{saveFilename}{(FileType.GetIsGeneric() ? "" : FileType.GetDotExtension())}";
        IsRunning = false;
        IsDone = false;
        IsSuccess = false;
        WasStopped = false;
        _tempDownloadPath = $"{UserDirectories.ApplicationCache}{Path.DirectorySeparatorChar}{Id}{Path.DirectorySeparatorChar}";
        _logPath = $"{_tempDownloadPath}log";
        _playlistPosition = playlistPosition;
        _advancedOptions = options;
        _pid = null;
        if (_advancedOptions.Timeframe != null && _advancedOptions.LimitSpeed)
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
                var hooks = new List<Action<PyDict>> { ProgressHook };
                var postProcessors = new List<Dictionary<string, dynamic>>();
                _ytOpt = new Dictionary<string, dynamic> {
                    { "quiet", false },
                    { "ignoreerrors", "downloadonly" },
                    { "progress_hooks", hooks },
                    { "postprocessor_hooks", hooks },
                    { "merge_output_format", null },
                    { "outtmpl", $"{Id.ToString()}.%(ext)s" },
                    { "ffmpeg_location", DependencyLocator.Find("ffmpeg")! },
                    { "windowsfilenames", options.LimitCharacters },
                    { "encoding", "utf_8" },
                    { "overwrites", options.OverwriteExistingFiles },
                    { "noprogress", true }
                };
                //Authentication
                if (!string.IsNullOrEmpty(_advancedOptions.Username))
                {
                    _ytOpt.Add("username", _advancedOptions.Username);
                }
                if (!string.IsNullOrEmpty(_advancedOptions.Password))
                {
                    _ytOpt.Add("password", _advancedOptions.Password);
                }
                //Translated Metadata 
                string? metadataLang = null;
                if (YoutubeLangCodes.Contains(CultureInfo.CurrentCulture.Name))
                {
                    metadataLang = CultureInfo.CurrentCulture.Name;
                }
                else if (YoutubeLangCodes.Contains(CultureInfo.CurrentCulture.TwoLetterISOLanguageName))
                {
                    metadataLang = CultureInfo.CurrentCulture.TwoLetterISOLanguageName;
                }
                if (!string.IsNullOrEmpty(metadataLang))
                {
                    var youtubeLang = new PyList();
                    youtubeLang.Append(new PyString(metadataLang));
                    var youtubeExtractorOpt = new PyDict();
                    youtubeExtractorOpt["lang"] = youtubeLang;
                    var extractorArgs = new PyDict();
                    extractorArgs["youtube"] = youtubeExtractorOpt;
                    _ytOpt.Add("extractor_args", extractorArgs);
                }
                //Aria2
                if (options.UseAria && _advancedOptions.Timeframe == null)
                {
                    _ytOpt.Add("external_downloader", new Dictionary<string, dynamic>() { { "default", DependencyLocator.Find("aria2c") } });
                    dynamic ariaDict = new PyDict();
                    dynamic ariaParams = new PyList();
                    ariaParams.Append(new PyString($"--max-overall-download-limit={(_advancedOptions.LimitSpeed ? options.SpeedLimit : 0)}K"));
                    ariaParams.Append(new PyString("--allow-overwrite=true"));
                    ariaParams.Append(new PyString("--show-console-readout=false"));
                    ariaParams.Append(new PyString($"--max-connection-per-server={options.AriaMaxConnectionsPerServer}"));
                    ariaParams.Append(new PyString($"--min-split-size={options.AriaMinSplitSize}M"));
                    ariaDict["default"] = ariaParams;
                    _ytOpt.Add("external_downloader_args", ariaDict);
                }
                //Speed limit (Cannot be applied with Aria2 enabled)
                else if (_advancedOptions.LimitSpeed)
                {
                    _ytOpt.Add("ratelimit", options.SpeedLimit * 1024);
                }
                //Proxy Url
                if (!string.IsNullOrEmpty(options.ProxyUrl))
                {
                    _ytOpt.Add("proxy", new PyString(options.ProxyUrl));
                }
                //Cookies File
                if (File.Exists(options.CookiesPath))
                {
                    _ytOpt.Add("cookiefile", new PyString(options.CookiesPath));
                }
                //File Format
                if (!FileType.GetIsGeneric())
                {
                    _ytOpt.Add("final_ext", FileType.ToString().ToLower());
                }
                if (FileType.GetIsAudio())
                {
                    if (FileType.GetIsGeneric())
                    {
                        _ytOpt.Add("format", Quality != Quality.Worst ? $"ba[language={AudioLanguage}]/ba/b" : $"wa[language={AudioLanguage}]/wa/w");
                        postProcessors.Add(new Dictionary<string, dynamic> { { "key", "FFmpegExtractAudio" }, { "preferredquality", Quality != Quality.Worst ? 0 : 5 } });
                    }
                    else
                    {
                        _ytOpt.Add("format", Quality != Quality.Worst ? $"ba[ext={FileType.ToString().ToLower()}][language={AudioLanguage}]/ba[ext={FileType.ToString().ToLower()}]/ba/b" : $"wa[ext={FileType.ToString().ToLower()}][language={AudioLanguage}]/wa[ext={FileType.ToString().ToLower()}]/wa/w");
                        postProcessors.Add(new Dictionary<string, dynamic> { { "key", "FFmpegExtractAudio" }, { "preferredcodec", FileType.ToString().ToLower() }, { "preferredquality", Quality != Quality.Worst ? 0 : 5 } });
                    }
                }
                else if (FileType.GetIsVideo())
                {
                    var ext = FileType switch
                    {
                        MediaFileType.MP4 => "[ext=mp4]",
                        MediaFileType.WEBM => "[ext=webm]",
                        _ => ""
                    };
                    var proto = _advancedOptions.Timeframe != null ? "[protocol!*=m3u8]" : "";
                    var vcodec = _advancedOptions.PreferAV1 ? "[vcodec=vp9.2]" : "[vcodec!*=vp]";
                    var resolution = Resolution! == VideoResolution.Best ? "" : $"[width<={Resolution!.Width}][height<={Resolution!.Height}]";
                    var formats = new HashSet<string>() //using a HashSet ensures no duplicates, for example if ext == ""
                    {
                        $"bv*{ext}{vcodec}{resolution}{proto}+ba{ext}[language={AudioLanguage}]",
                        $"bv*{ext}{vcodec}{resolution}{proto}+ba{ext}",
                        $"bv*{ext}{resolution}{proto}+ba{ext}[language={AudioLanguage}]",
                        $"bv*{ext}{resolution}{proto}+ba{ext}",
                        $"b{ext}{resolution}",
                        $"bv*{vcodec}{resolution}{proto}+ba[language={AudioLanguage}]",
                        $"bv*{vcodec}{resolution}{proto}+ba",
                        $"bv*{vcodec}{resolution}{proto}+ba",
                        $"bv*{resolution}{proto}+ba[language={AudioLanguage}]",
                        $"bv*{resolution}{proto}+ba",
                        $"b{resolution}"
                    };
                    _ytOpt.Add("format", $"{string.Join('/', formats)}/");
                    if (!FileType.GetIsGeneric())
                    {
                        postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "FFmpegVideoConvertor" }, { "preferedformat", FileType.ToString().ToLower() } });
                    }
                    //Subtitles
                    if (Subtitle)
                    {
                        _ytOpt.Add("writesubtitles", true);
                        _ytOpt.Add("writeautomaticsub", options.IncludeAutoGenertedSubtitles);
                        var subtitleLangsString = options.SubtitleLangs;
                        if (subtitleLangsString[^1] == ',')
                        {
                            subtitleLangsString = subtitleLangsString.Remove(subtitleLangsString.Length - 1);
                        }
                        var subtitleLangs = subtitleLangsString.Split(',').Select(x => x.Trim());
                        if (subtitleLangsString == _p("subtitle", "all") || subtitleLangsString == "all")
                        {
                            _ytOpt.Add("subtitleslangs", new List<string>() { "all" });
                        }
                        else if (options.IncludeAutoGenertedSubtitles)
                        {
                            var subtitleFromOtherLangs = new List<string>();
                            foreach (var l in subtitleLangs)
                            {
                                if (l.Length == 2 || l.Length == 3)
                                {
                                    var fromOther = $"{l}-*";
                                    if (!subtitleFromOtherLangs.Contains(fromOther))
                                    {
                                        subtitleFromOtherLangs.Add(fromOther);
                                    }
                                }
                            }
                            _ytOpt.Add("subtitleslangs", subtitleLangs.Union(subtitleFromOtherLangs).ToList());
                        }
                        else
                        {
                            _ytOpt.Add("subtitleslangs", subtitleLangs.ToList());
                        }
                        postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "TCSubtitlesConvertor" }, { "format", "vtt" } });
                        if (options.EmbedSubtitle)
                        {
                            postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "TCEmbedSubtitle" } });
                        }
                    }
                }
                //SponsorBlock for Youtube
                if (options.YouTubeSponsorBlock)
                {
                    postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "SponsorBlock" }, { "when", "after_filter" }, { "categories", new List<string>() { "sponsor", "intro", "outro", "selfpromo", "preview", "filler", "interaction", "music_offtopic" } } });
                    postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "ModifyChapters" }, { "remove_sponsor_segments", new List<string>() { "sponsor", "intro", "outro", "selfpromo", "preview", "filler", "interaction", "music_offtopic" } } });
                }
                //Split Chapters
                if (_advancedOptions.SplitChapters)
                {
                    postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "FFmpegSplitChapters" } });
                }
                //Metadata & Chapters
                if (options.EmbedMetadata)
                {
                    dynamic ppDict = new PyDict();
                    if (options.RemoveSourceData)
                    {
                        postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "MetadataFromField" }, { "formats", new List<string>() { ":(?P<meta_comment>)", ":(?P<meta_description>)", ":(?P<meta_synopsis>)", ":(?P<meta_purl>)", $"{_playlistPosition}:%(meta_track)s" } } });
                        dynamic rsdParams = new PyList();
                        rsdParams.Append(new PyString("-metadata:s"));
                        rsdParams.Append(new PyString("handler_name="));
                        ppDict["tcmetadata"] = rsdParams;
                    }
                    // TCMetadata should be added after MetadataFromField
                    postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "TCMetadata" }, { "add_metadata", true }, { "add_chapters", options.EmbedChapters } });
                    if (FileType.GetSupportsThumbnails())
                    {
                        _ytOpt.Add("writethumbnail", true);
                        postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "TCEmbedThumbnail" } });
                        if (_advancedOptions.CropThumbnail)
                        {
                            dynamic cropParams = new PyList();
                            cropParams.Append(new PyString("-vf"));
                            cropParams.Append(new PyString("crop=\'if(gt(ih,iw),iw,ih)\':\'if(gt(iw,ih),ih,iw)\'"));
                            ppDict["thumbnailsconvertor"] = cropParams;
                        }
                    }
                    _ytOpt.Add("postprocessor_args", ppDict);
                }
                else if (options.EmbedChapters)
                {
                    postProcessors.Add(new Dictionary<string, dynamic>() { { "key", "TCMetadata" }, { "add_chapters", true } });
                }
                //Postprocessors
                if (postProcessors.Count != 0)
                {
                    _ytOpt.Add("postprocessors", postProcessors);
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
                        if (_advancedOptions.Timeframe != null)
                        {
                            _ytOpt.Add("download_ranges", ytdlp.utils.download_range_func(null, new List<List<double>>() { new List<double>() { _advancedOptions.Timeframe.Start.TotalSeconds, _advancedOptions.Timeframe.End.TotalSeconds } }));
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
                        if (IsSuccess)
                        {
                            var genericExtensionFound = false;
                            foreach (var path in Directory.EnumerateFiles(SaveFolder))
                            {
                                if (path.Contains(Id.ToString()))
                                {
                                    if (FileType.GetIsGeneric() && !genericExtensionFound)
                                    {
                                        var extension = Path.GetExtension(path).ToLower();
                                        if (extension != ".srt" && extension != ".vtt")
                                        {
                                            Filename += extension;
                                            genericExtensionFound = true;
                                        }
                                    }
                                    var baseFilename = Path.GetFileNameWithoutExtension(Filename);
                                    var baseExtension = Path.GetExtension(baseFilename).ToLower();
                                    var i = 0;
                                    while (!options.OverwriteExistingFiles && File.Exists(path.Replace(Id.ToString(), Path.GetFileNameWithoutExtension(Filename))))
                                    {
                                        i++;
                                        Filename = $"{baseFilename} ({i}){baseExtension}";
                                    }
                                    IEnumerable<char> invalidChars = Path.GetInvalidFileNameChars();
                                    if (options.LimitCharacters)
                                    {
                                        invalidChars = invalidChars.Union(new char[] { '"', '<', '>', ':', '\\', '/', '|', '?', '*' });
                                    }
                                    foreach (var c in invalidChars)
                                    {
                                        Filename = Filename.Replace(c, '_');
                                    }
                                    File.Move(path, path.Replace(Id.ToString(), Path.GetFileNameWithoutExtension(Filename)), options.OverwriteExistingFiles);
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
                        ForceUpdateLog();
                        _outFile.close();
                    }
                    catch { }
                    finally
                    {
                        IsDone = true;
                        IsRunning = false;
                        IsSuccess = false;
                        Completed?.Invoke(this, IsSuccess);
                    }
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
                    foreach (PyObject child in pythonProcessChildren)
                    {
                        var processName = child.GetAttr(new PyString("name")).Invoke().As<string?>() ?? "";
                        if (processName == "ffmpeg" || processName == "aria2c")
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
                try
                {
                    using var fs = new FileStream(_logPath, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
                    using var sr = new StreamReader(fs);
                    state.Log = sr.ReadToEnd();
                    sr.Close();
                    fs.Close();
                }
                catch
                {
                    state.Log = "";
                }
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
                    try
                    {
                        state.Log = File.ReadAllText(_logPath);
                    }
                    catch
                    {
                        state.Log = "";
                    }
                }
                ProgressChanged.Invoke(this, state);
            }
        }
    }
}
