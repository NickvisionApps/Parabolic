using Nickvision.Desktop.Globalization;
using Nickvision.Parabolic.Shared.Events;
using Nickvision.Parabolic.Shared.Helpers;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Models;

public partial class Download : IDisposable
{
    private static readonly string[] PartialDownloadFilePatterns;
    private static readonly Dictionary<int, IReadOnlyList<string>> DownloadArgumentsCache;
    private static int _nextId;

    private ITranslationService? _translator;
    private readonly StringBuilder _logBuilder;
    private Process? _process;

    public int Id { get; }
    public DownloadOptions Options { get; }
    public string FilePath { get; private set; }
    public DownloadStatus Status { get; private set; }
    public string Log => _logBuilder.ToString();

    public event EventHandler<DownloadCompletedEventArgs>? Completed;
    public event EventHandler<DownloadProgressChangedEventArgs>? ProgressChanged;

    static Download()
    {
        PartialDownloadFilePatterns = ["*.part*", "*.vtt", "*.srt", "*.ass", "*.lrc"];
        DownloadArgumentsCache = new Dictionary<int, IReadOnlyList<string>>();
        _nextId = 0;
    }

    public Download(DownloadOptions options, ITranslationService? translator)
    {
        _translator = translator;
        _logBuilder = new StringBuilder();
        _process = null;
        Id = _nextId++;
        Options = options;
        FilePath = Path.Combine(Options.SaveFolder, $"{Options.SaveFilename}{Options.FileType.DotExtension}");
        Status = DownloadStatus.Queued;
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
            var log = _translator?._("The file already exists and overwriting is disabled.") ?? "The file already exists and overwriting is disabled.";
            _logBuilder.AppendLine(log);
            Status = DownloadStatus.Error;
            ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, log.AsMemory(), double.NaN, 0.0, 0));
            Completed?.Invoke(this, new DownloadCompletedEventArgs(Id, Status, FilePath, log.AsMemory(), false));
            return;
        }
        _process = new Process()
        {
            EnableRaisingEvents = true,
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
        _process.ErrorDataReceived += Process_OutputDataReceived;
        _process.Start();
        _process.BeginOutputReadLine();
        _process.BeginErrorReadLine();
        ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, (_translator?._("Starting download...") ?? "Starting download...").AsMemory(), double.NaN, 0.0, 0));
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
        if (_process is not null && !_process.HasExited)
        {
            _process?.Kill(true);
            _process?.WaitForExit();
        }
    }

    private IReadOnlyList<string> GetDownloadArguments(DownloaderOptions downloader)
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
            "[Parabolic] PROGRESS;%(progress.status)s;%(progress.downloaded_bytes)s;%(progress.total_bytes)s;%(progress.total_bytes_estimate)s;%(progress.speed)s;%(progress.eta)s",
            "--progress-delta",
            ".25",
            "-t",
            "sleep",
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
            "after_move:filepath"
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
        if (Options.VideoFormat is not null && Options.VideoFormat != Format.NoneVideo && !Options.FileType.IsAudio)
        {
            formatString += Options.VideoFormat switch
            {
                var f when f == Format.BestVideo => "bestvideo*",
                var f when f == Format.WorstVideo => "worstvideo*",
                _ => Options.VideoFormat.Id
            };
        }
        else if (Options.VideoResolution is not null && !Options.FileType.IsAudio)
        {
            formatString += Options.VideoResolution switch
            {
                var v when v == VideoResolution.Best => "bestvideo*",
                var v when v == VideoResolution.Worst => "worstvideo*",
                _ => $"bestvideo*[height<={Options.VideoResolution.Height}]/bestvideo*"
            };
        }
        if (Options.AudioFormat is not null && Options.AudioFormat != Format.NoneAudio)
        {
            if (!string.IsNullOrEmpty(formatString))
            {
                formatString += "+";
            }
            else if (Options.FileType.IsVideo)
            {
                formatString += Options.AudioFormat switch
                {
                    var f when f == Format.WorstAudio => "worstvideo*+",
                    _ => "bestvideo*+",
                };
            }
            formatString += Options.AudioFormat switch
            {
                var f when f == Format.BestAudio => downloader.PreferredAudioCodec == AudioCodec.Any && OperatingSystem.IsWindows() ? "bestaudio[acodec!=opus]" : "bestaudio",
                var f when f == Format.WorstAudio => "worstaudio",
                _ => Options.AudioFormat.Id
            };
        }
        else if (Options.AudioBitrate.HasValue)
        {
            if (!string.IsNullOrEmpty(formatString))
            {
                formatString += "+";
            }
            formatString += Options.AudioBitrate.Value switch
            {
                var b when b == double.MaxValue => "bestaudio",
                var b when b == -1.0 => "worstaudio",
                _ => $"bestaudio[abr<={Options.AudioBitrate.Value}]/worstaudio"
            };
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
            arguments.Add("--postprocessor-args");
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
            arguments.Add("--postprocessor-args");
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
        ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, ReadOnlyMemory<char>.Empty));
        Completed?.Invoke(this, new DownloadCompletedEventArgs(Id, Status, FilePath, Log.AsMemory(), true));
        if (_process is not null)
        {
            _process.Exited -= Process_Exited;
            _process.ErrorDataReceived -= Process_OutputDataReceived;
            _process.OutputDataReceived -= Process_OutputDataReceived;
            _process.Dispose();
            _process = null;
        }
    }

    private async void Process_OutputDataReceived(object? sender, DataReceivedEventArgs e)
    {
        if (string.IsNullOrEmpty(e.Data))
        {
            return;
        }
        _logBuilder.AppendLine(e.Data);
        try
        {
            if (e.Data.StartsWith("[Parabolic]"))
            {
                var fields = e.Data.Split(';', StringSplitOptions.RemoveEmptyEntries);
                if (fields.Length != 7 || fields[1] == "NA")
                {
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, e.Data.AsMemory(), double.NaN, 0.0, 0));
                }
                else if (fields[1] == "finished" || fields[1] == "processing")
                {
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, e.Data.AsMemory(), double.NaN, 0.0, 0));
                }
                else
                {
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id,
                        e.Data.AsMemory(),
                        (fields[2] != "NA" ? double.Parse(fields[2]) : 0.0) / (fields[3] != "NA" ? double.Parse(fields[3]) : (fields[4] != "NA" ? double.Parse(fields[4]) : 1.0)),
                        fields[5] != "NA" ? double.Parse(fields[5]) : 0.0,
                        fields[6] == "NA" || fields[6] == "Unknown" ? -1 : int.Parse(fields[6])));
                }
            }
            else if (e.Data.StartsWith("[#"))
            {
                var line = e.Data;
                if (OperatingSystem.IsWindows())
                {
                    foreach (var l in e.Data.Split('\r', StringSplitOptions.RemoveEmptyEntries).Reverse())
                    {
                        var f = l.Split(' ', StringSplitOptions.RemoveEmptyEntries);
                        if (f.Length == 5 && f[1].Split('/').Length == 2)
                        {
                            line = l;
                            break;
                        }
                    }
                }
                var fields = line.Split(' ', StringSplitOptions.RemoveEmptyEntries);
                if (fields.Length != 5)
                {
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, e.Data.AsMemory(), double.NaN, 0.0, 0));
                    return;
                }
                var sizes = fields[1].Split('/');
                if (sizes.Length != 2)
                {
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, e.Data.AsMemory(), double.NaN, 0.0, 0));
                    return;
                }
                ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id,
                    e.Data.AsMemory(),
                    sizes[0].AriaSizeToBytes() / sizes[1].AriaSizeToBytes(),
                    fields[3].Substring(3).AriaSizeToBytes(),
                    fields[4].Substring(4, fields[4].Length - 4 - 1).AriaEtaToSeconds()));
            }
            else if (e.Data.StartsWith("[download] Sleeping"))
            {
                var fields = e.Data.Split(' ', StringSplitOptions.RemoveEmptyEntries);
                if (fields.Length < 3 || !double.TryParse(fields[2], out var seconds))
                {
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, e.Data.AsMemory(), double.NegativeInfinity, 0.0, 0));
                }
                else
                {
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, e.Data.AsMemory(), double.NegativeInfinity, seconds, 0));
                    while (seconds > 1)
                    {
                        ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, ReadOnlyMemory<char>.Empty, double.NegativeInfinity, Math.Floor(seconds--), 0));
                        await Task.Delay(1000);
                    }
                    ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, e.Data.AsMemory(), double.NaN, 0.0, 0));
                }
            }
            else
            {
                ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, e.Data.AsMemory(), double.NaN, 0.0, 0));
            }
        }
        catch
        {
            ProgressChanged?.Invoke(this, new DownloadProgressChangedEventArgs(Id, e.Data.AsMemory(), double.NaN, 0.0, 0));
        }
    }
}
