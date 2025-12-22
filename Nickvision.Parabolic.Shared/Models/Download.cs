using Nickvision.Desktop.Globalization;
using Nickvision.Parabolic.Shared.Events;
using Nickvision.Parabolic.Shared.Helpers;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;

namespace Nickvision.Parabolic.Shared.Models;

public partial class Download : IDisposable
{
    private static readonly string[] PartialDownloadFilePatterns;
    private static readonly Dictionary<int, IEnumerable<string>> DownloadArgumentsCache;
    private static int _nextId;

    private ITranslationService? _translator;
    private Process? _process;

    public int Id { get; }
    public DownloadOptions Options { get; }
    public string FilePath { get; private set; }
    public DownloadStatus Status { get; private set; }
    public string Log { get; private set; }

    public event EventHandler<DownloadCompletedEventArgs>? Completed;
    public event EventHandler<DownloadProgressChangedEventArgs>? ProgressChanged;

    static Download()
    {
        PartialDownloadFilePatterns = ["*.part*", "*.vtt", "*.srt", "*.ass", "*.lrc"];
        DownloadArgumentsCache = new Dictionary<int, IEnumerable<string>>();
        _nextId = 0;
    }

    public Download(DownloadOptions options, ITranslationService? translator)
    {
        _translator = translator;
        _process = null;
        Id = _nextId++;
        Options = options;
        FilePath = Path.Combine(Options.SaveFolder, $"{Options.SaveFilename}{Options.FileType.DotExtension}");
        Status = DownloadStatus.Queued;
        Log = string.Empty;
    }

    ~Download()
    {
        Dispose(false);
    }

    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    public void Pause()
    {
        if (Status != DownloadStatus.Running)
        {
            return;
        }
        Status = DownloadStatus.Paused;
        _process?.Suspend(true);
    }

    public void Resume()
    {
        if (Status != DownloadStatus.Paused)
        {
            return;
        }
        Status = DownloadStatus.Running;
        _process?.Resume(true);
    }

    public void Start(string ytdlpExecutablePath, DownloaderOptions downloader)
    {
        if (Status == DownloadStatus.Running || Status == DownloadStatus.Paused)
        {
            return;
        }
        if (File.Exists(FilePath) && !downloader.OverwriteExistingFiles)
        {
            Status = DownloadStatus.Error;
            ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, _translator?._("ERROR: The file already exists and overwriting is disabled.") ?? "ERROR: The file already exists and overwriting is disabled."));
            Completed?.Invoke(this, new DownloadCompletedEventArgs(Id, Status, FilePath, false));
            return;
        }
        _process = new Process()
        {
            StartInfo = new ProcessStartInfo(ytdlpExecutablePath, GetDownloadArguments(downloader))
            {
                UseShellExecute = false,
                CreateNoWindow = true,
                RedirectStandardOutput = true,
                RedirectStandardError = true
            }
        };
        Status = DownloadStatus.Running;
        _process.Exited += Process_Exited;
        _process.OutputDataReceived += Process_OutputDataReceived;
        _process.Start();
        _process.BeginOutputReadLine();
        ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, _translator?._("Starting download...") ?? "Starting download..."));
    }

    public void Stop()
    {
        if (Status != DownloadStatus.Running)
        {
            return;
        }
        Status = DownloadStatus.Stopped;
        _process?.Kill(true);
    }

    private void Dispose(bool disposing)
    {
        if (!disposing)
        {
            return;
        }
        _process?.Dispose();
        _process = null;
    }

    private IEnumerable<string> GetDownloadArguments(DownloaderOptions downloader)
    {
        var hash = HashCode.Combine(downloader, Options);
        if (DownloadArgumentsCache.TryGetValue(hash, out var cache))
        {
            return cache;
        }
        Directory.CreateDirectory(Options.SaveFolder);
        var pluginsDir = Path.Combine(Desktop.System.Environment.ExecutingDirectory, "plugins");
        var arguments = new List<string>(128)
        {
            Options.Url.ToString(),
            "--ignore-config",
            "--verbose",
            "--no-warnings",
            "--progress",
            "--newline",
            "--progress-template",
            "[download] PROGRESS;%(progress.status)s;%(progress.downloaded_bytes)s;%(progress.total_bytes)s;%(progress.total_bytes_estimate)s;%(progress.speed)s;%(progress.eta)s",
            "--progress-delta",
            ".25",
            "--sleep-requests",
            "1",
            "--no-mtime",
            "--no-embed-info-json",
            "--ffmpeg-location",
            Desktop.System.Environment.FindDependency("ffmpeg") ?? "ffmpeg",
            "--js-runtimes",
            $"deno:{Desktop.System.Environment.FindDependency("deno") ?? "deno"}",
            "--paths",
            Options.SaveFolder,
            "--paths",
            $"temp:{Options.SaveFolder}",
            "--output",
            $"{Options.SaveFilename}.%(ext)s",
            "--output",
            $"chapter:%(section_number)03d - {Options.SaveFilename}.%(ext)s",
            "--print",
            "filepath"
        };
        if (Directory.Exists(pluginsDir))
        {
            arguments.Add("--plugin-dir");
            arguments.Add(pluginsDir);
            if (downloader.PreferredSubtitleFormat == SubtitleFormat.SRT)
            {
                arguments.Add("--use-postprocessor");
                arguments.Add("srt_fix");
            }
        }
        if (downloader.OverwriteExistingFiles && !HasPartialDownloadFiles())
        {
            arguments.Add("--force-overwrites");
        }
        else
        {
            arguments.Add("--no-overwrites");
        }
        if (downloader.LimitCharacters)
        {
            arguments.Add("--windows-filenames");
        }
        var formatSort = Options.TimeFrame is not null ? "proto:https" : string.Empty;
        if (downloader.PreferredVideoCodec != VideoCodec.Any)
        {
            if (!string.IsNullOrEmpty(formatSort))
            {
                formatSort += ',';
            }
            formatSort += "+vcodec:";
            formatSort += downloader.PreferredVideoCodec switch
            {
                VideoCodec.VP9 => "vp9",
                VideoCodec.AV01 => "av01",
                VideoCodec.H264 => "h264",
                VideoCodec.H265 => "h265",
                _ => string.Empty
            };
            formatSort += ",res";
        }
        if (downloader.PreferredAudioCodec != AudioCodec.Any)
        {
            if (!string.IsNullOrEmpty(formatSort))
            {
                formatSort += ',';
            }
            formatSort += "+acodec:";
            formatSort += downloader.PreferredAudioCodec switch
            {
                AudioCodec.FLAC => "flac",
                AudioCodec.WAV => "wav",
                AudioCodec.OPUS => "opus",
                AudioCodec.AAC => "aac",
                AudioCodec.MP4A => "mp4a",
                AudioCodec.MP3 => "mp3",
                _ => string.Empty
            };
            formatSort += ",quality";
        }
        if (downloader.PreferredAudioCodec == AudioCodec.Any && OperatingSystem.IsWindows() && Options.AudioFormat is not null && Options.AudioFormat == Format.BestAudio)
        {
            if (!string.IsNullOrEmpty(formatSort))
            {
                formatSort += ',';
            }
            formatSort += "+acodec:mp4a";
        }
        if (!string.IsNullOrEmpty(formatSort))
        {
            arguments.Add("--format-sort");
            arguments.Add(formatSort);
        }
        if (!downloader.UsePartFiles)
        {
            arguments.Add("--no-part");
        }
        if (downloader.YouTubeSponsorBlock)
        {
            arguments.Add("--sponsorblock-remove");
            arguments.Add("default");
        }
        if (downloader.SpeedLimit.HasValue && Options.TimeFrame is null)
        {
            arguments.Add("--limit-rate");
            arguments.Add($"{downloader.SpeedLimit.Value}K");
        }
        if (!string.IsNullOrEmpty(downloader.ProxyUrl))
        {
            arguments.Add("--proxy");
            arguments.Add(downloader.ProxyUrl);
        }
        if (downloader.CookiesBrowser != Browser.None)
        {
            arguments.Add("--cookies-from-browser");
            arguments.Add(downloader.CookiesBrowser switch
            {
                Browser.Brave => "brave",
                Browser.Chrome => "chrome",
                Browser.Chromium => "chromium",
                Browser.Edge => "edge",
                Browser.Firefox => "firefox",
                Browser.Opera => "opera",
                Browser.Vivaldi => "vivaldi",
                Browser.Whale => "whale",
                _ => string.Empty
            });
        }
        else if (File.Exists(downloader.CookiesPath))
        {
            arguments.Add("--cookies");
            arguments.Add(downloader.CookiesPath);
        }
        if (downloader.EmbedMetadata)
        {
            arguments.Add("--embed-metadata");
            if (downloader.RemoveSourceData)
            {
                arguments.Add("--postprocessor-args");
                arguments.Add("Metadata+ffmpeg:-metadata comment= -metadata description= -metadata synopsis= -metadata purl= ");
            }
            if (Options.PlaylistPosition != -1)
            {
                arguments.Add("--postprocessor-args");
                arguments.Add($"Metadata+ffmpeg:-metadata track={Options.PlaylistPosition}");
            }
        }
        if (downloader.EmbedThumbnails)
        {
            if (Options.FileType.SupportsThumbnails)
            {
                arguments.Add("--embed-thumbnail");
            }
            else
            {
                arguments.Add("--write-thumbnail");
            }
            arguments.Add("--convert-thumbnails");
            arguments.Add("png>png/jpg");
            if (downloader.CropAudioThumbnails && Options.FileType.IsAudio)
            {
                arguments.Add("--postprocessor-args");
                arguments.Add("ThumbnailsConvertor:-vf crop=ih:ih");
            }
        }
        if (downloader.EmbedChapters)
        {
            arguments.Add("--embed-chapters");
        }
        if (downloader.UseAria)
        {
            arguments.Add("--downloader");
            arguments.Add(Desktop.System.Environment.FindDependency("aria2c") ?? "aria2c");
            arguments.Add("--downloader-args");
            arguments.Add($"aria2c:--summary-interval={(OperatingSystem.IsWindows() ? "0" : "1")} --enable-color=false -x {downloader.AriaMaxConnectionsPerServer} -k {downloader.AriaMinSplitSize}M");
            arguments.Add("--concurrent-fragments");
            arguments.Add("8");
        }
        if (Options.Credential is not null)
        {
            if (!string.IsNullOrEmpty(Options.Credential.Username) && !string.IsNullOrEmpty(Options.Credential.Password))
            {
                arguments.Add("--username");
                arguments.Add(Options.Credential.Username);
                arguments.Add("--password");
                arguments.Add(Options.Credential.Password);
            }
            else if (string.IsNullOrEmpty(Options.Credential.Password))
            {
                arguments.Add("--video-password");
                arguments.Add(Options.Credential.Password);
            }
        }
        if (Options.FileType.IsAudio)
        {
            arguments.Add("--extract-audio");
            arguments.Add("--audio-quality");
            arguments.Add("0");
            if (!Options.FileType.IsGeneric)
            {
                arguments.Add("--audio-format");
                if (Options.FileType == MediaFileType.OGG)
                {
                    arguments.Add("vorbis");
                }
                else
                {
                    arguments.Add(Options.FileType.ToString().ToLower());
                }
            }
        }
        else if (Options.FileType.IsVideo && !Options.FileType.IsGeneric)
        {
            arguments.Add("--remux-video");
            arguments.Add(Options.FileType.ToString().ToLower());
            if (Options.FileType.ShouldRecode)
            {
                arguments.Add("--recode-video");
                arguments.Add(Options.FileType.ToString().ToLower());
            }
        }
        var formatString = string.Empty;
        if (Options.VideoFormat is not null && Options.VideoFormat != Format.NoneVideo)
        {
            if (Options.VideoFormat == Format.BestVideo)
            {
                formatString += "bv";
                if (Options.AudioFormat is not null && Options.AudioFormat == Format.NoneAudio)
                {
                    formatString += "*";
                }
            }
            else if (Options.VideoFormat == Format.WorstVideo)
            {
                formatString += "wv";
                if (Options.AudioFormat is not null && Options.AudioFormat == Format.NoneAudio)
                {
                    formatString += "*";
                }
            }
            else
            {
                formatString += Options.VideoFormat.Id;
            }
        }
        if (Options.AudioFormat is not null && Options.AudioFormat != Format.NoneAudio)
        {
            if (!string.IsNullOrEmpty(formatString))
            {
                formatString += "+";
            }
            formatString += Options.AudioFormat switch
            {
                var f when f == Format.BestAudio => "ba",
                var f when f == Format.WorstAudio => "wa",
                _ => Options.AudioFormat.Id
            };
        }
        if (formatString == "bv*" && Options.FileType.IsAudio)
        {
            formatString += "+ba";
        }
        else if (formatString == "wv*" && Options.FileType.IsAudio)
        {
            formatString += "+wa";
        }
        if (formatString == "bv+ba" || formatString == "bv*+ba" || formatString == "bv*" || formatString == "ba")
        {
            formatString += "/b";
        }
        else if (formatString == "wv+wa" || formatString == "wv*+wa" || formatString == "wv*" || formatString == "wa")
        {
            formatString += "/w";
        }
        if (!string.IsNullOrEmpty(formatString))
        {
            arguments.Add("--format");
            arguments.Add(formatString);
        }
        if (Options.SubtitleLanguages.Count > 0)
        {
            var languages = string.Empty;
            foreach (var language in Options.SubtitleLanguages)
            {
                languages += $"{language.Language},";
            }
            languages += "-live_chat";
            arguments.Add("--sub-langs");
            arguments.Add(languages);
            arguments.Add("--sleep-subtitles");
            arguments.Add("60");
            arguments.Add("--write-subs");
            if (downloader.IncludeAutoGeneratedSubtitles)
            {
                arguments.Add("--write-auto-subs");
            }
            arguments.Add("--sub-format");
            arguments.Add(downloader.PreferredSubtitleFormat switch
            {
                SubtitleFormat.SRT => "srt/best",
                SubtitleFormat.ASS => "ass/best",
                SubtitleFormat.LRC => "lrc/best",
                _ => "vtt/best"
            });
            arguments.Add("--convert-subs");
            arguments.Add(downloader.PreferredSubtitleFormat switch
            {
                SubtitleFormat.SRT => "srt",
                SubtitleFormat.ASS => "ass",
                SubtitleFormat.LRC => "lrc",
                _ => "vtt"
            });
            if (downloader.EmbedSubtitles && Options.FileType.GetSupportsSubtitleFormat(downloader.PreferredSubtitleFormat))
            {
                arguments.Add("--embed-subs");
                arguments.Add("--compact-options");
                arguments.Add("no-keep-subs");
            }
        }
        if (Options.SplitChapters)
        {
            arguments.Add("--split-chapters");
            arguments.Add("--postprocessor-args");
            arguments.Add($"SplitChapters:-map_metadata 0 -map_chapters -1{(Options.FileType == MediaFileType.FLAC ? " -c:a flac" : string.Empty)}");
        }
        if (Options.ExportDescription)
        {
            arguments.Add("--write-description");
        }
        if (Options.PostProcessorArgument is not null)
        {
            arguments.Add("-postprocessor-args");
            if (Options.PostProcessorArgument.Executable == Executable.FFmpeg && Options.PostProcessorArgument.PostProcessor == PostProcessor.None)
            {
                arguments.Add($"{Options.PostProcessorArgument.ToString()} -threads {downloader.PostprocessingThreads}");
            }
            else
            {
                arguments.Add(Options.PostProcessorArgument.ToString());
            }
        }
        else
        {
            arguments.Add("-postprocessor-args");
            arguments.Add($"ffmpeg:-threads {downloader.PostprocessingThreads}");
        }
        if (Options.TimeFrame is not null)
        {
            arguments.Add("--download-sections");
            arguments.Add($"*{Options.TimeFrame.ToString()}");
            arguments.Add("--force-keyframes-at-cuts");
        }
        DownloadArgumentsCache[hash] = arguments;
        return arguments;
    }

    private bool HasPartialDownloadFiles()
    {
        if (!Directory.Exists(Options.SaveFolder))
        {
            return false;
        }
        foreach (var pattern in PartialDownloadFilePatterns)
        {
            if (Directory.EnumerateFiles(Options.SaveFolder, $"{Options.SaveFilename}{pattern}").Any())
            {
                return true;
            }
        }
        return false;
    }

    private void Process_Exited(object? sender, EventArgs e)
    {
        if (Status != DownloadStatus.Stopped)
        {
            Status = _process?.ExitCode == 0 ? DownloadStatus.Success : DownloadStatus.Error;
        }
        if (Status == DownloadStatus.Success)
        {
            var lines = Log.Split('\n');
            try
            {
                var finalPath = lines[^1];
                if (!File.Exists(finalPath))
                {
                    finalPath = lines[^2];
                }
                if (File.Exists(finalPath))
                {
                    FilePath = finalPath;
                }
            }
            catch { }
        }
        ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, Log));
        Completed?.Invoke(this, new DownloadCompletedEventArgs(Id, Status, FilePath, true));
    }

    private void Process_OutputDataReceived(object? sender, DataReceivedEventArgs e)
    {
        if (string.IsNullOrEmpty(e.Data))
        {
            return;
        }
        Log += $"{e.Data}\n";
        if ((!e.Data.Contains("PROGRESS;") && !e.Data.Contains("[#")) || e.Data.Contains("[debug"))
        {
            return;
        }
        try
        {
            if (e.Data.Contains("[#"))
            {
                var line = e.Data;
#if OS_WINDOWS
                var lines = e.Data.Split('\r', StringSplitOptions.RemoveEmptyEntries);
                for (var i = lines.Length - 1; i >= 0; i--)
                {
                    var fields = lines[i].Split(' ', StringSplitOptions.RemoveEmptyEntries);
                    if (lines[i].Contains("[download]") || (fields.Length == 5 && fields[1].Split('/').Length == 2))
                    {
                        line = lines[i];
                        break;
                    }
                }
#endif
                if (line.Contains("[download]"))
                {
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, Log, double.NaN, 0.0, 0));
                }
                else
                {
                    var fields = line.Split(' ', StringSplitOptions.RemoveEmptyEntries);
                    if (fields.Length != 5)
                    {
                        return;
                    }
                    var sizes = fields[1].Split('/');
                    if (sizes.Length != 2)
                    {
                        return;
                    }
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id,
                        Log,
                        sizes[0].AriaSizeToBytes() / sizes[1].AriaSizeToBytes(),
                        fields[3].Substring(3).AriaSizeToBytes(),
                        fields[4].Substring(4, fields[4].Length - 4 - 1).AriaEtaToSeconds()));
                }
            }
            else
            {
                var fields = e.Data.Split(';', StringSplitOptions.RemoveEmptyEntries);
                if (fields.Length != 7 || fields[1] == "NA")
                {
                    return;
                }
                if (fields[1] == "finished" || fields[1] == "processing")
                {
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, Log, double.NaN, 0.0, 0));
                }
                else
                {
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id,
                        Log,
                        (fields[2] != "NA" ? double.Parse(fields[2]) : 0.0) / (fields[3] != "NA" ? double.Parse(fields[3]) : (fields[4] != "NA" ? double.Parse(fields[4]) : 1.0)),
                        fields[5] != "NA" ? double.Parse(fields[5]) : 0.0,
                        fields[6] == "NA" || fields[6] == "Unknown" ? -1 : int.Parse(fields[6])));
                }
            }
        }
        catch { }
    }
}
