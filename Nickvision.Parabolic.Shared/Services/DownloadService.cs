using Nickvision.Desktop.Filesystem;
using Nickvision.Parabolic.Shared.Events;
using Nickvision.Parabolic.Shared.Helpers;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class DownloadService : IDownloadService
{
    private static readonly string[] PartialDownloadFilePatterns;

    private readonly IJsonFileService _jsonFileService;
    private readonly IYtdlpExecutableService _ytdlpService;
    private readonly IHistoryService _historyService;
    private readonly IRecoveryService _recoveryService;
    private readonly Dictionary<int, IEnumerable<string>> _downloadArgumentsCache;

    public event EventHandler<DownloadAddedEventArgs>? DownloadAdded;

    static DownloadService()
    {
        PartialDownloadFilePatterns = ["*.part*", "*.vtt", "*.srt", "*.ass", "*.lrc"];
    }

    public DownloadService(IJsonFileService jsonFileService, IYtdlpExecutableService ytdlpService, IHistoryService historyService, IRecoveryService recoveryService)
    {
        _jsonFileService = jsonFileService;
        _ytdlpService = ytdlpService;
        _historyService = historyService;
        _recoveryService = recoveryService;
        _downloadArgumentsCache = new Dictionary<int, IEnumerable<string>>();
    }

    public async Task AddDownloadAsync(DownloadOptions options, bool excludeFromHistory)
    {

    }

    public async Task AddDownloadsAsync(IEnumerable<DownloadOptions> options, bool exlucdeFromHistory)
    {

    }

    private IEnumerable<string> GetDownloadArguments(DownloaderOptions downloader, DownloadOptions download)
    {
        var hash = HashCode.Combine(downloader, download);
        if (_downloadArgumentsCache.TryGetValue(hash, out var cache))
        {
            return cache;
        }
        Directory.CreateDirectory(download.SaveFolder);
        var pluginsDir = Path.Combine(Desktop.System.Environment.ExecutingDirectory, "plugins");
        var arguments = new List<string>(128)
        {
            download.Url.ToString(),
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
            download.SaveFolder,
            "--paths",
            $"temp:{download.SaveFolder}",
            "--output",
            $"{download.SaveFilename}.%(ext)s",
            "--output",
            $"chapter:%(section_number)03d - {download.SaveFilename}.%(ext)s",
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
        if (downloader.OverwriteExistingFiles && !HasPartialDownloadFiles(download))
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
        var formatSort = download.TimeFrame is not null ? "proto:https" : string.Empty;
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
        if (downloader.PreferredAudioCodec == AudioCodec.Any && OperatingSystem.IsWindows() && download.AudioFormat is not null && download.AudioFormat == Format.BestAudio)
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
        if (downloader.SpeedLimit.HasValue && download.TimeFrame is null)
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
            if (download.PlaylistPosition != -1)
            {
                arguments.Add("--postprocessor-args");
                arguments.Add($"Metadata+ffmpeg:-metadata track={download.PlaylistPosition}");
            }
        }
        if (downloader.EmbedThumbnails)
        {
            if (download.FileType.SupportsThumbnails)
            {
                arguments.Add("--embed-thumbnail");
            }
            else
            {
                arguments.Add("--write-thumbnail");
            }
            arguments.Add("--convert-thumbnails");
            arguments.Add("png>png/jpg");
            if (downloader.CropAudioThumbnails && download.FileType.IsAudio)
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
        if (download.Credential is not null)
        {
            if (!string.IsNullOrEmpty(download.Credential.Username) && !string.IsNullOrEmpty(download.Credential.Password))
            {
                arguments.Add("--username");
                arguments.Add(download.Credential.Username);
                arguments.Add("--password");
                arguments.Add(download.Credential.Password);
            }
            else if (string.IsNullOrEmpty(download.Credential.Password))
            {
                arguments.Add("--video-password");
                arguments.Add(download.Credential.Password);
            }
        }
        if (download.FileType.IsAudio)
        {
            arguments.Add("--extract-audio");
            arguments.Add("--audio-quality");
            arguments.Add("0");
            if (!download.FileType.IsGeneric)
            {
                arguments.Add("--audio-format");
                if (download.FileType == MediaFileType.OGG)
                {
                    arguments.Add("vorbis");
                }
                else
                {
                    arguments.Add(download.FileType.ToString().ToLower());
                }
            }
        }
        else if (download.FileType.IsVideo && !download.FileType.IsGeneric)
        {
            arguments.Add("--remux-video");
            arguments.Add(download.FileType.ToString().ToLower());
            if (download.FileType.ShouldRecode)
            {
                arguments.Add("--recode-video");
                arguments.Add(download.FileType.ToString().ToLower());
            }
        }
        var formatString = string.Empty;
        if(download.VideoFormat is not null && download.VideoFormat != Format.NoneVideo)
        {
            if(download.VideoFormat == Format.BestVideo)
            {
                formatString += "bv";
                if(download.AudioFormat is not null && download.AudioFormat == Format.NoneAudio)
                {
                    formatString += "*";
                }
            }
            else if(download.VideoFormat == Format.WorstVideo)
            {
                formatString += "wv";
                if (download.AudioFormat is not null && download.AudioFormat == Format.NoneAudio)
                {
                    formatString += "*";
                }
            }
            else
            {
                formatString += download.VideoFormat.Id;
            }
        }
        if(download.AudioFormat is not null && download.AudioFormat != Format.NoneAudio)
        {
            if(!string.IsNullOrEmpty(formatString))
            {
                formatString += "+";
            }
            formatString += download.AudioFormat switch
            {
                var f when f == Format.BestAudio => "ba",
                var f when f == Format.WorstAudio => "wa",
                _ => download.AudioFormat.Id
            };
        }
        if(formatString == "bv*" && download.FileType.IsAudio)
        {
            formatString += "+ba";
        }
        else if(formatString == "wv*" && download.FileType.IsAudio)
        {
            formatString += "+wa";
        }
        if(formatString == "bv+ba" || formatString == "bv*+ba" || formatString == "bv*" || formatString == "ba")
        {
            formatString += "/b";
        }
        else if (formatString == "wv+wa" || formatString == "wv*+wa" || formatString == "wv*" || formatString == "wa")
        {
            formatString += "/w";
        }
        if(!string.IsNullOrEmpty(formatString))
        {
            arguments.Add("--format");
            arguments.Add(formatString);
        }
        if(download.SubtitleLanguages.Count > 0)
        {
            var languages = string.Empty;
            foreach(var language in download.SubtitleLanguages)
            {
                languages += $"{language.Language},";
            }
            languages += "-live_chat";
            arguments.Add("--sub-langs");
            arguments.Add(languages);
            arguments.Add("--sleep-subtitles");
            arguments.Add("60");
            arguments.Add("--write-subs");
            if(downloader.IncludeAutoGeneratedSubtitles)
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
            if(downloader.EmbedSubtitles && download.FileType.GetSupportsSubtitleFormat(downloader.PreferredSubtitleFormat))
            {
                arguments.Add("--embed-subs");
                arguments.Add("--compact-options");
                arguments.Add("no-keep-subs");
            }
        }
        if(download.SplitChapters)
        {
            arguments.Add("--split-chapters");
            arguments.Add("--postprocessor-args");
            arguments.Add($"SplitChapters:-map_metadata 0 -map_chapters -1{(download.FileType == MediaFileType.FLAC ? " -c:a flac" : string.Empty)}");
        }
        if(download.ExportDescription)
        {
            arguments.Add("--write-description");
        }
        if (download.PostProcessorArgument is not null)
        {
            arguments.Add("-postprocessor-args");
            if(download.PostProcessorArgument.Executable == Executable.FFmpeg && download.PostProcessorArgument.PostProcessor == PostProcessor.None)
            {
                arguments.Add($"{download.PostProcessorArgument.ToString()} -threads {downloader.PostprocessingThreads}");
            }
            else
            {
                arguments.Add(download.PostProcessorArgument.ToString());
            }
        }
        else
        {
            arguments.Add("-postprocessor-args");
            arguments.Add($"ffmpeg:-threads {downloader.PostprocessingThreads}");
        }
        if(download.TimeFrame is not null)
        {
            arguments.Add("--download-sections");
            arguments.Add($"*{download.TimeFrame.ToString()}");
            arguments.Add("--force-keyframes-at-cuts");
        }
        _downloadArgumentsCache[hash] = arguments;
        return arguments;
    }

    private bool HasPartialDownloadFiles(DownloadOptions download)
    {
        if (!Directory.Exists(download.SaveFolder))
        {
            return false;
        }
        foreach (var pattern in PartialDownloadFilePatterns)
        {
            if (Directory.EnumerateFiles(download.SaveFolder, $"{download.SaveFilename}{pattern}").Any())
            {
                return true;
            }
        }
        return false;
    }
}
