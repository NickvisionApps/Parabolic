using Microsoft.Extensions.Logging;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Keyring;
using Nickvision.Desktop.Network;
using Nickvision.Desktop.System;
using Nickvision.Parabolic.Shared.Helpers;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class YtdlpExecutableService : DependencyExecutableService, IYtdlpExecutableService
{
    private static readonly AppVersion YtdlpBundledVersion;
    private static readonly string YtdlpAssetName;
    private static readonly string[] PartialDownloadFilePatterns;

    private readonly IConfigurationService _configurationService;
    private readonly IDenoExecutableService _denoExecutableService;
    private readonly IUpdaterService _previewUpdaterService;
    private AppVersion? _latestPreviewVersion;

    static YtdlpExecutableService()
    {
        if (OperatingSystem.IsLinux())
        {
            YtdlpBundledVersion = new AppVersion(Desktop.System.Environment.DeploymentMode == DeploymentMode.Local ? "0.0.0" : "2026.03.17");
        }
        else
        {
            YtdlpBundledVersion = new AppVersion("2026.03.17");
        }
        if (OperatingSystem.IsWindows())
        {
            YtdlpAssetName = RuntimeInformation.OSArchitecture == Architecture.Arm64 ? "yt-dlp_arm64.exe" : "yt-dlp.exe";
        }
        else if (OperatingSystem.IsLinux())
        {
            YtdlpAssetName = RuntimeInformation.OSArchitecture == Architecture.Arm64 ? "yt-dlp_linux_aarch64" : "yt-dlp_linux";
        }
        else if (OperatingSystem.IsMacOS())
        {
            YtdlpAssetName = "yt-dlp_macos";
        }
        else
        {
            YtdlpAssetName = "yt-dlp";
        }
        PartialDownloadFilePatterns = ["*.part*", "*.vtt", "*.srt", "*.ass", "*.lrc"];
    }

    public YtdlpExecutableService(ILogger<YtdlpExecutableService> logger, ILogger<UpdaterService> updaterLogger, IConfigurationService configurationService, IDenoExecutableService denoExecutableService, IHttpClientFactory httpClientFactory) : base(logger, "yt-dlp", YtdlpBundledVersion, YtdlpAssetName, configurationService, new UpdaterService(updaterLogger, "yt-dlp", "yt-dlp", httpClientFactory.CreateClient()))
    {
        _configurationService = configurationService;
        _denoExecutableService = denoExecutableService;
        _previewUpdaterService = new UpdaterService(updaterLogger, "yt-dlp", "yt-dlp-nightly-builds", httpClientFactory.CreateClient());
        _latestPreviewVersion = null;
    }

    public async Task<Process> CreateDiscoveryProcessAsync(Uri url, Credential? credential)
    {
        var downloaderOptions = (await _jsonFileService.LoadAsync(ApplicationJsonContext.Default.Configuration, Configuration.Key)).DownloaderOptions;
        var pluginsDir = Path.Combine(Desktop.System.Environment.ExecutingDirectory, "plugins");
        var arguments = new List<string>(23)
        {
            url.ToString(),
            "--ignore-config",
            "--dump-single-json",
            "--skip-download",
            "--ignore-errors",
            "--no-warnings",
            "--ffmpeg-location",
            Desktop.System.Environment.FindDependency("ffmpeg") ?? "ffmpeg",
            "--js-runtimes",
            $"deno:{_denoExecutableService.ExecutablePath ?? "deno"}",
            "--paths",
            $"temp:{UserDirectories.Cache}"
        };
        if (Directory.Exists(pluginsDir))
        {
            arguments.Add("--plugin-dir");
            arguments.Add(pluginsDir);
        }
        if (downloaderOptions.LimitCharacters)
        {
            arguments.Add("--windows-filenames");
        }
        if (!string.IsNullOrEmpty(downloaderOptions.ProxyUrl))
        {
            _logger.LogInformation("Using proxy...");
            arguments.Add("--proxy");
            arguments.Add(downloaderOptions.ProxyUrl);
        }
        if (credential is not null)
        {
            if (!string.IsNullOrEmpty(credential.Username) && !string.IsNullOrEmpty(credential.Password))
            {
                _logger.LogInformation("Using credential...");
                arguments.Add("--username");
                arguments.Add(credential.Username);
                arguments.Add("--password");
                arguments.Add(credential.Password);
            }
            else if (!string.IsNullOrEmpty(credential.Password))
            {
                _logger.LogInformation("Using video password...");
                arguments.Add("--video-password");
                arguments.Add(credential.Password);
            }
        }
        if (downloaderOptions.CookiesBrowser != Browser.None)
        {
            _logger.LogInformation($"Using cookies from browser: {downloaderOptions.CookiesBrowser}");
            arguments.Add("--cookies-from-browser");
            arguments.Add(downloaderOptions.CookiesBrowser switch
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
        else if (File.Exists(downloaderOptions.CookiesPath))
        {
            _logger.LogInformation($"Using cookies file...");
            arguments.Add("--cookies");
            arguments.Add(downloaderOptions.CookiesPath);
        }
        arguments.AddRange(downloaderOptions.YtdlpDiscoveryArgs.SplitCommandLine());
        return new Process()
        {
            StartInfo = new ProcessStartInfo(ExecutablePath ?? "yt-dlp", arguments)
            {
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true
            }
        };
    }

    public async Task<Process> CreateDownloadProcessAsync(DownloadOptions downloadOptions)
    {
        var config = await _jsonFileService.LoadAsync(ApplicationJsonContext.Default.Configuration, Configuration.Key);
        var downloaderOptions = config.DownloaderOptions;
        var pluginsDir = Path.Combine(Desktop.System.Environment.ExecutingDirectory, "plugins");
        Directory.CreateDirectory(downloadOptions.SaveFolder);
        var arguments = new List<string>(128)
        {
            downloadOptions.Url.ToString(),
            "--ignore-config",
            "--verbose",
            "--no-warnings",
            "--progress",
            "--newline",
            "--progress-template",
            "[Parabolic] Progress;%(progress.status)s;%(progress.downloaded_bytes)s;%(progress.total_bytes)s;%(progress.total_bytes_estimate)s;%(progress.speed)s;%(progress.eta)s",
            "--progress-delta",
            ".75",
            "-t",
            "sleep",
            "--no-mtime",
            "--no-embed-info-json",
            "--ffmpeg-location",
            Desktop.System.Environment.FindDependency("ffmpeg") ?? "ffmpeg",
            "--js-runtimes",
            $"deno:{_denoExecutableService.ExecutablePath ?? "deno"}",
            "--paths",
            downloadOptions.SaveFolder,
            "--paths",
            $"temp:{downloadOptions.SaveFolder}",
            "--output",
            $"{downloadOptions.SaveFilename}.%(ext)s",
            "--output",
            $"chapter:%(section_number)03d - {downloadOptions.SaveFilename}.%(ext)s",
            "--print",
            "after_move:filepath"
        };
        if (downloadOptions.Url.Host.Contains("instagram") && downloadOptions.PlaylistPosition != -1)
        {
            arguments.Add("--playlist-items");
            arguments.Add($"{downloadOptions.PlaylistPosition}");
        }
        if (Directory.Exists(pluginsDir))
        {
            arguments.Add("--plugin-dir");
            arguments.Add(pluginsDir);
            if (downloaderOptions.PreferredSubtitleFormat == SubtitleFormat.SRT)
            {
                arguments.Add("--use-postprocessor");
                arguments.Add("srt_fix");
            }
        }
        if (downloaderOptions.OverwriteExistingFiles && !HasPartialDownloadFiles(downloadOptions.SaveFolder, downloadOptions.SaveFilename))
        {
            arguments.Add("--force-overwrites");
        }
        else
        {
            arguments.Add("--no-overwrites");
        }
        if (downloaderOptions.LimitCharacters)
        {
            arguments.Add("--windows-filenames");
        }
        var formatSort = downloadOptions.TimeFrame is not null ? "proto:https" : string.Empty;
        if (downloaderOptions.PreferredVideoCodec != VideoCodec.Any)
        {
            if (!string.IsNullOrEmpty(formatSort))
            {
                formatSort += ',';
            }
            formatSort += "+vcodec:";
            formatSort += downloaderOptions.PreferredVideoCodec switch
            {
                VideoCodec.VP9 => "vp9",
                VideoCodec.AV01 => "av01",
                VideoCodec.H264 => "h264",
                VideoCodec.H265 => "h265",
                _ => string.Empty
            };
            formatSort += ",res";
        }
        if (downloaderOptions.PreferredAudioCodec != AudioCodec.Any)
        {
            if (!string.IsNullOrEmpty(formatSort))
            {
                formatSort += ',';
            }
            formatSort += "+acodec:";
            formatSort += downloaderOptions.PreferredAudioCodec switch
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
        if (downloaderOptions.PreferredFrameRate != FrameRate.Any)
        {
            if (!string.IsNullOrEmpty(formatSort))
            {
                formatSort += ',';
            }
            formatSort += "+fps:";
            formatSort += downloaderOptions.PreferredFrameRate switch
            {
                FrameRate.Fps24 => "24",
                FrameRate.Fps30 => "30",
                FrameRate.Fps60 => "60",
                _ => string.Empty
            };

        }
        if (downloadOptions.FileType.ShouldRecode && downloaderOptions.PreferredVideoCodec == VideoCodec.Any)
        {
            if (!string.IsNullOrEmpty(formatSort))
            {
                formatSort += ',';
            }
            formatSort += downloadOptions.FileType switch
            {
                MediaFileType.WEBM => "+vcodec:vp9",
                _ => "+vcodec:h264"
            };
        }
        if (!string.IsNullOrEmpty(formatSort))
        {
            arguments.Add("--format-sort");
            arguments.Add(formatSort);
        }
        if (!downloaderOptions.UsePartFiles)
        {
            arguments.Add("--no-part");
        }
        if (downloaderOptions.YouTubeSponsorBlock)
        {
            arguments.Add("--sponsorblock-remove");
            arguments.Add("default");
        }
        if (downloaderOptions.SpeedLimit.HasValue && downloadOptions.TimeFrame is null)
        {
            arguments.Add("--limit-rate");
            arguments.Add($"{downloaderOptions.SpeedLimit.Value}K");
        }
        if (!string.IsNullOrEmpty(downloaderOptions.ProxyUrl))
        {
            arguments.Add("--proxy");
            arguments.Add(downloaderOptions.ProxyUrl);
        }
        if (downloaderOptions.CookiesBrowser != Browser.None)
        {
            arguments.Add("--cookies-from-browser");
            arguments.Add(downloaderOptions.CookiesBrowser switch
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
        else if (File.Exists(downloaderOptions.CookiesPath))
        {
            arguments.Add("--cookies");
            arguments.Add(downloaderOptions.CookiesPath);
        }
        if (downloaderOptions.TranslateMetadataAndChapters)
        {
            if (config.TranslationLanguage.IsSupportedYouTubeLanguage)
            {
                arguments.Add("--extractor-args");
                arguments.Add($"youtube:lang={config.TranslationLanguage}");
            }
            else if (CultureInfo.CurrentCulture.Name.IsSupportedYouTubeLanguage)
            {
                arguments.Add("--extractor-args");
                arguments.Add($"youtube:lang={CultureInfo.CurrentCulture.Name}");
            }
        }
        if (downloaderOptions.EmbedMetadata)
        {
            arguments.Add("--embed-metadata");
            if (downloadOptions.PlaylistPosition != -1)
            {
                arguments.Add("--postprocessor-args");
                arguments.Add($"Metadata+ffmpeg:-metadata track={downloadOptions.PlaylistPosition}");
            }
        }
        if (downloaderOptions.EmbedThumbnails)
        {
            if (downloadOptions.FileType.SupportsThumbnails)
            {
                arguments.Add("--embed-thumbnail");
            }
            else
            {
                arguments.Add("--write-thumbnail");
            }
            arguments.Add("--convert-thumbnails");
            arguments.Add("jpg");
            if (downloaderOptions.CropAudioThumbnails && downloadOptions.FileType.IsAudio)
            {
                arguments.Add("--exec");
                arguments.Add($"before_dl:\"{Desktop.System.Environment.FindDependency("ffmpeg") ?? "ffmpeg"}\" -i %(thumbnails.-1.filepath)q -vf crop=\"'if(gt(ih,iw),iw,ih)':'if(gt(iw,ih),ih,iw)'\" \"%(thumbnails.-1.filepath)s.tmp.jpg\"");
                if (OperatingSystem.IsWindows())
                {
                    arguments.Add("--exec");
                    arguments.Add("before_dl:del %(thumbnails.-1.filepath)q");
                    arguments.Add("--exec");
                    arguments.Add("before_dl:move \"%(thumbnails.-1.filepath)s.tmp.jpg\" %(thumbnails.-1.filepath)q");
                }
                else
                {
                    arguments.Add("--exec");
                    arguments.Add("before_dl:rm %(thumbnails.-1.filepath)q");
                    arguments.Add("--exec");
                    arguments.Add("before_dl:mv \"%(thumbnails.-1.filepath)s.tmp.jpg\" %(thumbnails.-1.filepath)q");
                }
            }
        }
        if (downloaderOptions.EmbedChapters)
        {
            arguments.Add("--embed-chapters");
        }
        if (downloaderOptions.UseAria)
        {
            arguments.Add("--downloader");
            arguments.Add(Desktop.System.Environment.FindDependency("aria2c") ?? "aria2c");
            arguments.Add("--downloader-args");
            arguments.Add($"aria2c:--summary-interval={(OperatingSystem.IsWindows() ? "0" : "1")} --enable-color=false -x {downloaderOptions.AriaMaxConnectionsPerServer} -k {downloaderOptions.AriaMinSplitSize}M");
            arguments.Add("--concurrent-fragments");
            arguments.Add("8");
        }
        if (downloadOptions.Credential is not null)
        {
            if (!string.IsNullOrEmpty(downloadOptions.Credential.Username) && !string.IsNullOrEmpty(downloadOptions.Credential.Password))
            {
                arguments.Add("--username");
                arguments.Add(downloadOptions.Credential.Username);
                arguments.Add("--password");
                arguments.Add(downloadOptions.Credential.Password);
            }
            else if (!string.IsNullOrEmpty(downloadOptions.Credential.Password))
            {
                arguments.Add("--video-password");
                arguments.Add(downloadOptions.Credential.Password);
            }
        }
        if (downloadOptions.FileType.IsAudio)
        {
            arguments.Add("--extract-audio");
            arguments.Add("--audio-quality");
            arguments.Add("0");
            if (!downloadOptions.FileType.IsGeneric)
            {
                arguments.Add("--audio-format");
                if (downloadOptions.FileType == MediaFileType.OGG)
                {
                    arguments.Add("vorbis");
                }
                else
                {
                    arguments.Add(downloadOptions.FileType.ToString().ToLower());
                }
            }
        }
        else if (downloadOptions.FileType.IsVideo && !downloadOptions.FileType.IsGeneric)
        {
            arguments.Add("--remux-video");
            arguments.Add(downloadOptions.FileType.ToString().ToLower());
            if (downloadOptions.FileType.ShouldRecode)
            {
                arguments.Add("--recode-video");
                arguments.Add(downloadOptions.FileType.ToString().ToLower());
            }
        }
        var formatString = string.Empty;
        if (downloadOptions.VideoFormat is not null && downloadOptions.VideoFormat != Format.NoneVideo)
        {
            if (!downloadOptions.FileType.IsAudio)
            {
                formatString += downloadOptions.VideoFormat switch
                {
                    var f when f == Format.BestVideo => "bestvideo*",
                    var f when f == Format.WorstVideo => "worstvideo*",
                    _ => downloadOptions.VideoFormat.Id
                };
            }
            else if (downloadOptions.VideoFormat.ContainsAudio && (downloadOptions.AudioFormat is null || downloadOptions.AudioFormat == Format.NoneAudio))
            {
                formatString += downloadOptions.VideoFormat.Id;
            }
        }
        else if (downloadOptions.VideoResolution is not null && !downloadOptions.FileType.IsAudio)
        {
            formatString += downloadOptions.VideoResolution switch
            {
                var v when v == VideoResolution.Best => "bestvideo*",
                var v when v == VideoResolution.Worst => "worstvideo*",
                _ => $"bestvideo*[height={downloadOptions.VideoResolution.Height}]/bestvideo*[height<={downloadOptions.VideoResolution.Height}]/bestvideo*"
            };
        }
        var avoidOpus = OperatingSystem.IsWindows() && downloadOptions.Url.Host.Contains("youtube") && downloaderOptions.PreferredAudioCodec == AudioCodec.Any && downloadOptions.FileType != MediaFileType.WEBM;
        if (downloadOptions.AudioFormat is not null && downloadOptions.AudioFormat != Format.NoneAudio)
        {
            if (!string.IsNullOrEmpty(formatString))
            {
                formatString += "+";
            }
            else if (downloadOptions.FileType.IsVideo)
            {
                formatString += downloadOptions.AudioFormat switch
                {
                    var f when f == Format.WorstAudio => "worstvideo*+",
                    _ => "bestvideo*+",
                };
            }
            formatString += downloadOptions.AudioFormat switch
            {
                var f when f == Format.BestAudio => avoidOpus ? "bestaudio[acodec!=opus]" : "bestaudio",
                var f when f == Format.WorstAudio => "worstaudio",
                _ => downloadOptions.AudioFormat.Id
            };
        }
        else if (downloadOptions.AudioBitrate.HasValue)
        {
            if (!string.IsNullOrEmpty(formatString))
            {
                formatString += "+";
            }
            formatString += downloadOptions.AudioBitrate.Value switch
            {
                var b when b == double.MaxValue => avoidOpus ? "bestaudio[acodec!=opus]" : "bestaudio",
                var b when b == -1.0 => "worstaudio",
                _ => $"bestaudio[abr={downloadOptions.AudioBitrate.Value}]{(avoidOpus ? "[acodec!=opus]" : string.Empty)}/bestaudio[abr<={downloadOptions.AudioBitrate.Value}]{(avoidOpus ? "[acodec!=opus]" : string.Empty)}/bestaudio"
            };
        }
        if (!string.IsNullOrEmpty(formatString))
        {
            arguments.Add("--format");
            arguments.Add(formatString);
        }
        if (downloadOptions.SubtitleLanguages.Count > 0)
        {
            var languages = string.Empty;
            foreach (var language in downloadOptions.SubtitleLanguages)
            {
                languages += $"{language.Language},";
            }
            languages += "-live_chat";
            arguments.Add("--sub-langs");
            arguments.Add(languages);
            if (downloadOptions.Url.Host.Contains("youtube"))
            {
                arguments.Add("--sleep-subtitles");
                arguments.Add("30");
            }
            arguments.Add("--write-subs");
            if (downloaderOptions.IncludeAutoGeneratedSubtitles)
            {
                arguments.Add("--write-auto-subs");
            }
            arguments.Add("--sub-format");
            arguments.Add(downloaderOptions.PreferredSubtitleFormat switch
            {
                SubtitleFormat.SRT => "srt/best",
                SubtitleFormat.ASS => "ass/best",
                SubtitleFormat.LRC => "lrc/best",
                _ => "vtt/best"
            });
            arguments.Add("--convert-subs");
            arguments.Add(downloaderOptions.PreferredSubtitleFormat switch
            {
                SubtitleFormat.SRT => "srt",
                SubtitleFormat.ASS => "ass",
                SubtitleFormat.LRC => "lrc",
                _ => "vtt"
            });
            if (downloaderOptions.EmbedSubtitles && downloadOptions.FileType.GetSupportsSubtitleFormat(downloaderOptions.PreferredSubtitleFormat))
            {
                arguments.Add("--embed-subs");
                arguments.Add("--compat-options");
                arguments.Add("no-keep-subs");
            }
        }
        if (downloadOptions.SplitChapters)
        {
            arguments.Add("--split-chapters");
            arguments.Add("--postprocessor-args");
            arguments.Add($"SplitChapters:-map_metadata 0 -map_chapters -1{(downloadOptions.FileType == MediaFileType.FLAC ? " -c:a flac" : string.Empty)}");
        }
        if (downloadOptions.ExportDescription)
        {
            arguments.Add("--write-description");
        }
        if (downloadOptions.PostProcessorArgument is not null)
        {
            arguments.Add("--postprocessor-args");
            if (downloadOptions.PostProcessorArgument.Executable == Executable.FFmpeg && downloadOptions.PostProcessorArgument.PostProcessor == PostProcessor.None)
            {
                arguments.Add($"{downloadOptions.PostProcessorArgument.ToString()} -threads {downloaderOptions.PostprocessingThreads}");
            }
            else
            {
                arguments.Add(downloadOptions.PostProcessorArgument.ToString());
            }
        }
        else
        {
            arguments.Add("--postprocessor-args");
            arguments.Add($"ffmpeg:-threads {downloaderOptions.PostprocessingThreads}");
        }
        if (downloadOptions.TimeFrame is not null)
        {
            arguments.Add("--download-sections");
            arguments.Add($"*{downloadOptions.TimeFrame.ToString()}");
            arguments.Add("--force-keyframes-at-cuts");
        }
        arguments.AddRange(downloaderOptions.YtdlpDownloadArgs.SplitCommandLine());
        return new Process()
        {
            EnableRaisingEvents = true,
            StartInfo = new ProcessStartInfo(ExecutablePath ?? "yt-dlp", arguments)
            {
                UseShellExecute = false,
                CreateNoWindow = true,
                RedirectStandardOutput = true,
                RedirectStandardError = true
            }
        };
    }

    public override async Task<bool> DownloadUpdateAsync(AppVersion version, IProgress<DownloadProgress>? progress = null)
    {
        var path = OperatingSystem.IsWindows() ? Path.Combine(UserDirectories.LocalData, "yt-dlp.exe") : Path.Combine(UserDirectories.LocalData, "yt-dlp");
        var res = version.BaseVersion.Revision > 0 ? await _previewUpdaterService.DownloadReleaseAssetAsync(version, path, _assetName, true, progress) : await _updaterService.DownloadReleaseAssetAsync(version, path, _assetName, true, progress);
        if (res)
        {
            var configKey = $"installed_{_executableName}_appversion";
            await _configurationService.SetAsync(configKey, version, ApplicationJsonContext.Default.AppVersion);
            if (!OperatingSystem.IsWindows())
            {
                using var process = new Process()
                {
                    StartInfo = new ProcessStartInfo("chmod", ["0755", path])
                    {
                        UseShellExecute = false,
                        CreateNoWindow = true,
                    }
                };
                process.Start();
                await process.WaitForExitAsync();
                return process.ExitCode == 0;
            }
        }
        return res;
    }

    public override async Task<AppVersion?> GetLatestPreviewVersionAsync()
    {
        if (_latestPreviewVersion is null)
        {
            var _ = ExecutablePath;
            _latestPreviewVersion = await _previewUpdaterService.GetLatestStableVersionAsync();
        }
        return _latestPreviewVersion;
    }

    private bool HasPartialDownloadFiles(string saveFolder, string saveFilename)
    {
        if (!Directory.Exists(saveFolder))
        {
            return false;
        }
        foreach (var pattern in PartialDownloadFilePatterns)
        {
            if (Directory.EnumerateFiles(saveFolder, $"{saveFilename}{pattern}").Any())
            {
                return true;
            }
        }
        return false;
    }
}
