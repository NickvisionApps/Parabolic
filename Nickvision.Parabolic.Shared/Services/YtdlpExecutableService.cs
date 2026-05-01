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

    private readonly IDenoExecutableService _denoExecutableService;

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

    public YtdlpExecutableService(ILogger<YtdlpExecutableService> logger, ILogger<UpdaterService> updaterLogger, IConfigurationService configurationService, IDenoExecutableService denoExecutableService, IHttpClientFactory httpClientFactory)
        : base(logger, "yt-dlp", YtdlpBundledVersion, YtdlpAssetName, configurationService, new UpdaterService(updaterLogger, "yt-dlp", "yt-dlp", httpClientFactory.CreateClient()), new UpdaterService(updaterLogger, "yt-dlp", "yt-dlp-nightly-builds", httpClientFactory.CreateClient()))
    {
        _denoExecutableService = denoExecutableService;
    }

    public IReadOnlyList<string> GetDiscoveryProcessArguments(Uri url, Credential? credential)
    {
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
        if (_configurationService.LimitCharacters)
        {
            arguments.Add("--windows-filenames");
        }
        if (!string.IsNullOrEmpty(_configurationService.ProxyUrl))
        {
            arguments.Add("--proxy");
            arguments.Add(_configurationService.ProxyUrl);
        }
        if (credential is not null)
        {
            if (!string.IsNullOrEmpty(credential.Username) && !string.IsNullOrEmpty(credential.Password))
            {
                arguments.Add("--username");
                arguments.Add(credential.Username);
                arguments.Add("--password");
                arguments.Add(credential.Password);
            }
            else if (!string.IsNullOrEmpty(credential.Password))
            {
                arguments.Add("--video-password");
                arguments.Add(credential.Password);
            }
        }
        if (_configurationService.CookiesBrowser != Browser.None)
        {
            arguments.Add("--cookies-from-browser");
            arguments.Add(CookiesFromBrowserArgument);
        }
        else if (File.Exists(_configurationService.CookiesPath))
        {
            arguments.Add("--cookies");
            arguments.Add(_configurationService.CookiesPath);
        }
        arguments.AddRange(_configurationService.YtdlpDiscoveryArgs.SplitCommandLine());
        return arguments;
    }

    public Process GetDownloadProcess(DownloadOptions downloadOptions)
    {
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
        if (downloadOptions.RequiresPlaylistItems && downloadOptions.PlaylistPosition != -1)
        {
            arguments.Add("--playlist-items");
            arguments.Add($"{downloadOptions.PlaylistPosition}");
        }
        if (Directory.Exists(pluginsDir))
        {
            arguments.Add("--plugin-dir");
            arguments.Add(pluginsDir);
            if (_configurationService.PreferredSubtitleFormat == SubtitleFormat.SRT)
            {
                arguments.Add("--use-postprocessor");
                arguments.Add("srt_fix");
            }
        }
        if (_configurationService.OverwriteExistingFiles && !HasPartialDownloadFiles(downloadOptions.SaveFolder, downloadOptions.SaveFilename))
        {
            arguments.Add("--force-overwrites");
        }
        else
        {
            arguments.Add("--no-overwrites");
        }
        if (_configurationService.LimitCharacters)
        {
            arguments.Add("--windows-filenames");
        }
        var formatSort = downloadOptions.TimeFrame is not null ? "proto:https" : string.Empty;
        if (_configurationService.PreferredVideoCodec != VideoCodec.Any)
        {
            if (!string.IsNullOrEmpty(formatSort))
            {
                formatSort += ',';
            }
            formatSort += "+vcodec:";
            formatSort += _configurationService.PreferredVideoCodec switch
            {
                VideoCodec.VP9 => "vp9",
                VideoCodec.AV01 => "av01",
                VideoCodec.H264 => "h264",
                VideoCodec.H265 => "h265",
                _ => string.Empty
            };
            formatSort += ",res";
        }
        if (_configurationService.PreferredAudioCodec != AudioCodec.Any)
        {
            if (!string.IsNullOrEmpty(formatSort))
            {
                formatSort += ',';
            }
            formatSort += "+acodec:";
            formatSort += _configurationService.PreferredAudioCodec switch
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
        if (_configurationService.PreferredFrameRate != FrameRate.Any)
        {
            if (!string.IsNullOrEmpty(formatSort))
            {
                formatSort += ',';
            }
            formatSort += "+fps:";
            formatSort += _configurationService.PreferredFrameRate switch
            {
                FrameRate.Fps24 => "24",
                FrameRate.Fps30 => "30",
                FrameRate.Fps60 => "60",
                _ => string.Empty
            };

        }
        if (downloadOptions.FileType.ShouldRecode && _configurationService.PreferredVideoCodec == VideoCodec.Any)
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
        if (!_configurationService.UsePartFiles)
        {
            arguments.Add("--no-part");
        }
        if (_configurationService.YouTubeSponsorBlock)
        {
            arguments.Add("--sponsorblock-remove");
            arguments.Add("default");
        }
        if (_configurationService.SpeedLimit.HasValue && downloadOptions.TimeFrame is null)
        {
            arguments.Add("--limit-rate");
            arguments.Add($"{_configurationService.SpeedLimit!.Value}K");
        }
        if (!string.IsNullOrEmpty(_configurationService.ProxyUrl))
        {
            arguments.Add("--proxy");
            arguments.Add(_configurationService.ProxyUrl);
        }
        if (_configurationService.CookiesBrowser != Browser.None)
        {
            arguments.Add("--cookies-from-browser");
            arguments.Add(CookiesFromBrowserArgument);
        }
        else if (File.Exists(_configurationService.CookiesPath))
        {
            arguments.Add("--cookies");
            arguments.Add(_configurationService.CookiesPath);
        }
        if (_configurationService.TranslateMetadataAndChapters)
        {
            if (_configurationService.TranslationLanguage.IsSupportedYouTubeLanguage)
            {
                arguments.Add("--extractor-args");
                arguments.Add($"youtube:lang={_configurationService.TranslationLanguage}");
            }
            else if (CultureInfo.CurrentCulture.Name.IsSupportedYouTubeLanguage)
            {
                arguments.Add("--extractor-args");
                arguments.Add($"youtube:lang={CultureInfo.CurrentCulture.Name}");
            }
        }
        if (_configurationService.EmbedMetadata)
        {
            arguments.Add("--embed-metadata");
            if (downloadOptions.PlaylistPosition != -1)
            {
                arguments.Add("--postprocessor-args");
                arguments.Add($"Metadata+ffmpeg:-metadata track={downloadOptions.PlaylistPosition}");
            }
        }
        if (_configurationService.EmbedThumbnails)
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
            if (_configurationService.CropAudioThumbnails && downloadOptions.FileType.IsAudio)
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
        if (_configurationService.EmbedChapters)
        {
            arguments.Add("--embed-chapters");
        }
        if (_configurationService.UseAria && downloadOptions.TimeFrame is null)
        {
            arguments.Add("--downloader");
            arguments.Add(Desktop.System.Environment.FindDependency("aria2c") ?? "aria2c");
            arguments.Add("--downloader-args");
            arguments.Add($"aria2c:--summary-interval={(OperatingSystem.IsWindows() ? "0" : "1")} --enable-color=false -x {_configurationService.AriaMaxConnectionsPerServer} -k {_configurationService.AriaMinSplitSize}M");
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
        var audioSelector = string.Empty;
        var audioHandled = false;
        var avoidOpus = OperatingSystem.IsWindows() && downloadOptions.Url.Host.Contains("youtube") && _configurationService.PreferredAudioCodec == AudioCodec.Any && downloadOptions.FileType != MediaFileType.WEBM;
        if (downloadOptions.AudioFormat is not null && downloadOptions.AudioFormat != Format.NoneAudio)
        {
            audioSelector = downloadOptions.AudioFormat switch
            {
                var f when f == Format.BestAudio => avoidOpus ? "(bestaudio[acodec!=opus]/bestaudio)" : "bestaudio",
                var f when f == Format.WorstAudio => "worstaudio",
                _ => downloadOptions.AudioFormat.Id
            };
        }
        if (downloadOptions.AudioBitrate.HasValue)
        {
            var noOpus = avoidOpus ? "[acodec!=opus]" : string.Empty;
            audioSelector = downloadOptions.AudioBitrate.Value switch
            {
                var b when b == double.MaxValue => avoidOpus ? "(bestaudio[acodec!=opus]/bestaudio)" : "bestaudio",
                var b when b == -1.0 => "worstaudio",
                _ => $"(bestaudio[abr={downloadOptions.AudioBitrate.Value}]{noOpus}/bestaudio[abr<={downloadOptions.AudioBitrate.Value}]{noOpus}/bestaudio)"
            };
        }
        if (downloadOptions.VideoFormat is not null && downloadOptions.VideoFormat != Format.NoneVideo)
        {
            if (!downloadOptions.FileType.IsAudio)
            {
                formatString = downloadOptions.VideoFormat switch
                {
                    var f when f == Format.BestVideo => "bestvideo*",
                    var f when f == Format.WorstVideo => "worstvideo*",
                    _ => downloadOptions.VideoFormat.Id
                };
            }
            else if (downloadOptions.VideoFormat.ContainsAudio && (downloadOptions.AudioFormat is null || downloadOptions.AudioFormat == Format.NoneAudio))
            {
                formatString = downloadOptions.VideoFormat.Id;
            }
        }
        else if (downloadOptions.VideoResolution is not null && !downloadOptions.FileType.IsAudio)
        {
            var audio = string.IsNullOrEmpty(audioSelector) ? string.Empty : $"+{audioSelector}";
            formatString = downloadOptions.VideoResolution switch
            {
                var v when v == VideoResolution.Best => $"bestvideo*{audio}",
                var v when v == VideoResolution.Worst => $"worstvideo*{audio}",
                _ => $"bestvideo*[height={downloadOptions.VideoResolution.Height}]{audio}/bestvideo*[width={downloadOptions.VideoResolution.Height}]{audio}/bestvideo*[height<={downloadOptions.VideoResolution.Height}]{audio}/bestvideo*[width<={downloadOptions.VideoResolution.Height}]{audio}/bestvideo*{audio}"
            };
            audioHandled = !string.IsNullOrEmpty(audioSelector);
        }
        if (!audioHandled && !string.IsNullOrEmpty(audioSelector))
        {
            if (!string.IsNullOrEmpty(formatString))
            {
                formatString += $"+{audioSelector}";
            }
            else if (downloadOptions.FileType.IsVideo && downloadOptions.AudioFormat is not null && downloadOptions.AudioFormat != Format.NoneAudio)
            {
                var videoPrefix = downloadOptions.AudioFormat == Format.WorstAudio ? "worstvideo*" : "bestvideo*";
                formatString = $"{videoPrefix}+{audioSelector}";
            }
            else
            {
                formatString = audioSelector;
            }
        }
        if (!string.IsNullOrEmpty(formatString))
        {
            if (formatString.Contains('+'))
            {
                var lastSlashIndex = formatString.LastIndexOf('/');
                var lastFallback = lastSlashIndex >= 0 ? formatString[(lastSlashIndex + 1)..] : formatString;
                if (lastFallback.Contains('+'))
                {
                    formatString += formatString.StartsWith("worst", StringComparison.Ordinal) ? "/worst" : "/best";
                }
            }
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
            if (_configurationService.IncludeAutoGeneratedSubtitles)
            {
                arguments.Add("--write-auto-subs");
            }
            arguments.Add("--sub-format");
            arguments.Add(_configurationService.PreferredSubtitleFormat switch
            {
                SubtitleFormat.SRT => "srt/best",
                SubtitleFormat.ASS => "ass/best",
                SubtitleFormat.LRC => "lrc/best",
                _ => "vtt/best"
            });
            arguments.Add("--convert-subs");
            arguments.Add(_configurationService.PreferredSubtitleFormat switch
            {
                SubtitleFormat.SRT => "srt",
                SubtitleFormat.ASS => "ass",
                SubtitleFormat.LRC => "lrc",
                _ => "vtt"
            });
            if (_configurationService.EmbedSubtitles && downloadOptions.FileType.GetSupportsSubtitleFormat(_configurationService.PreferredSubtitleFormat))
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
                arguments.Add($"{downloadOptions.PostProcessorArgument.ToString()} -threads {_configurationService.PostprocessingThreads}");
            }
            else
            {
                arguments.Add(downloadOptions.PostProcessorArgument.ToString());
            }
        }
        else
        {
            arguments.Add("--postprocessor-args");
            arguments.Add($"ffmpeg:-threads {_configurationService.PostprocessingThreads}");
        }
        if (downloadOptions.TimeFrame is not null)
        {
            arguments.Add("--download-sections");
            arguments.Add($"*{downloadOptions.TimeFrame.ToString()}");
            if (downloadOptions.VideoFormat?.Protocol == "https" || downloadOptions.AudioFormat?.Protocol == "https")
            {
                arguments.Add("--force-keyframes-at-cuts");
            }
        }
        arguments.AddRange(_configurationService.YtdlpDownloadArgs.SplitCommandLine());
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
        var res = version.BaseVersion.Revision > 0 ? await _previewUpdaterService!.DownloadReleaseAssetAsync(version, path, _assetName, true, progress) : await _updaterService.DownloadReleaseAssetAsync(version, path, _assetName, true, progress);
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

    private string CookiesFromBrowserArgument
    {
        get
        {
            var browser = _configurationService.CookiesBrowser switch
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
            };
            if (Desktop.System.Environment.DeploymentMode == DeploymentMode.Flatpak)
            {
                var home = System.Environment.GetFolderPath(System.Environment.SpecialFolder.UserProfile);
                var path = string.Empty;
                if (_configurationService.CookiesBrowser == Browser.Firefox)
                {
                    var candidates = new[]
                    {
                        Path.Combine(home, ".mozilla", "firefox"),
                        Path.Combine(home, ".config", "mozilla", "firefox"),
                        Path.Combine(home, "snap", "firefox", "common", ".mozilla", "firefox")
                    };
                    path = candidates.FirstOrDefault(Directory.Exists) ?? candidates[0];
                }
                else
                {
                    path = _configurationService.CookiesBrowser switch
                    {
                        Browser.Brave => Path.Combine(home, ".config", "BraveSoftware", "Brave-Browser"),
                        Browser.Chrome => Path.Combine(home, ".config", "google-chrome"),
                        Browser.Chromium => Path.Combine(home, ".config", "chromium"),
                        Browser.Edge => Path.Combine(home, ".config", "microsoft-edge"),
                        Browser.Opera => Path.Combine(home, ".config", "opera"),
                        Browser.Vivaldi => Path.Combine(home, ".config", "vivaldi"),
                        Browser.Whale => Path.Combine(home, ".config", "naver-whale"),
                        _ => string.Empty
                    };
                }
                if (!string.IsNullOrEmpty(path))
                {
                    browser += $":{path}";
                }
            }
            return browser;
        }
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
