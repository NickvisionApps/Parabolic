using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Keyring;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class DiscoveryService : IDiscoveryService
{
    private static readonly char BatchFileDelimiter;

    private readonly IJsonFileService _jsonFileService;
    private readonly ITranslationService _translationService;
    private readonly IYtdlpExecutableService _ytdlpExecutableService;

    static DiscoveryService()
    {
        BatchFileDelimiter = '|';
    }

    public DiscoveryService(IJsonFileService jsonFileService, ITranslationService translationService, IYtdlpExecutableService ytdlpExecutableService)
    {
        _jsonFileService = jsonFileService;
        _translationService = translationService;
        _ytdlpExecutableService = ytdlpExecutableService;
    }

    public async Task<UrlInfo?> GetForBatchFileAsync(string path, Credential? credential, CancellationToken cancellationToken = default)
    {
        var entries = await ParseBatchFileAsync(path, cancellationToken);
        if (entries.Count == 0)
        {
            return null;
        }
        var entryInfos = new List<UrlInfo>();
        foreach (var entry in entries)
        {
            cancellationToken.ThrowIfCancellationRequested();
            var urlInfo = await GetForUrlAsync(entry.Url, credential, entry.SuggestedSaveFolder, entry.SuggestedFilename, cancellationToken);
            if (urlInfo is not null)
            {
                entryInfos.Add(urlInfo);
            }
        }
        cancellationToken.ThrowIfCancellationRequested();
        return new UrlInfo(new Uri($"file://{path}"), Path.GetFileNameWithoutExtension(path), entryInfos);
    }

    public async Task<UrlInfo?> GetForUrlAsync(Uri url, Credential? credential, CancellationToken cancellationToken = default) => await GetForUrlAsync(url, credential, string.Empty, string.Empty, cancellationToken);

    private async Task<UrlInfo?> GetForUrlAsync(Uri url, Credential? credential, string suggestedSaveFolder, string suggestedFilename, CancellationToken cancellationToken = default)
    {
        var downloaderOptions = (await _jsonFileService.LoadAsync<Configuration>(Configuration.Key)).DownloaderOptions;
        var arguments = new List<string>
        {
            url.ToString(),
            "--ignore-config",
            "--xff",
            "default",
            "--dump-single-json",
            "--skip-download",
            "--ignore-errors",
            "--no-warnings",
            "--ffmpeg-location",
            Desktop.System.Environment.FindDependency("ffmpeg") ?? "ffmpeg",
            "--js-runtimes",
            $"deno:{Desktop.System.Environment.FindDependency("deno") ?? "deno"}",
            "--plugin-dir",
            Path.Combine(Desktop.System.Environment.ExecutingDirectory, "plugins"),
            "--paths",
            $"temp:{UserDirectories.Cache}"
        };
        if (url.ToString().Contains("soundcloud.com"))
        {
            arguments.Add("--flat-playlist");
        }
        if (downloaderOptions.LimitCharacters)
        {
            arguments.Add("--windows-filenames");
        }
        if (!string.IsNullOrEmpty(downloaderOptions.ProxyUrl))
        {
            arguments.AddRange(["--proxy", downloaderOptions.ProxyUrl]);
        }
        if (credential is not null)
        {
            arguments.AddRange(["--username", credential.Username, "--password", credential.Password]);
        }
        if (downloaderOptions.CookiesBrowser != Browser.None)
        {
            arguments.AddRange(["--cookies-from-browser", downloaderOptions.CookiesBrowser switch
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
            }]);
        }
        else if (File.Exists(downloaderOptions.CookiesPath))
        {
            arguments.AddRange(["--cookies", downloaderOptions.CookiesPath]);
        }
        using var process = new Process()
        {
            StartInfo = new ProcessStartInfo(_ytdlpExecutableService.ExecutablePath ?? "yt-dlp", arguments)
            {
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true
            }
        };
        cancellationToken.ThrowIfCancellationRequested();
        process.Start();
        await process.WaitForExitAsync(cancellationToken);
        var output = await process.StandardOutput.ReadToEndAsync(cancellationToken);
        if (process.ExitCode != 0 || string.IsNullOrEmpty(output))
        {
            return null;
        }
        cancellationToken.ThrowIfCancellationRequested();
        try
        {
            using var json = JsonDocument.Parse(output);
            if (json.RootElement.TryGetProperty("entries", out var entriesProperty) && entriesProperty.GetArrayLength() > 0)
            {
                var urlInfos = new List<UrlInfo>();
                foreach (var entry in entriesProperty.EnumerateArray())
                {
                    cancellationToken.ThrowIfCancellationRequested();
                    if (entry.TryGetProperty("ie_key", out var ieProperty) && (ieProperty.GetString() ?? string.Empty) == "YoutubeTab" && entry.TryGetProperty("url", out var urlProperty))
                    {
                        var urlInfo = await GetForUrlAsync(new Uri(urlProperty.GetString() ?? string.Empty), credential, suggestedSaveFolder, suggestedFilename, cancellationToken);
                        if (urlInfo is not null)
                        {
                            urlInfos.Add(urlInfo);
                        }
                    }
                }
                if (urlInfos.Count > 0 && json.RootElement.TryGetProperty("title", out var titleProperty))
                {
                    return new UrlInfo(url, titleProperty.GetString() ?? "Tab", urlInfos);
                }
            }
            return new UrlInfo(json.RootElement, _translationService, downloaderOptions, url, suggestedSaveFolder, suggestedFilename);
        }
        catch
        {
            return null;
        }
    }

    private async Task<List<BatchFileEntry>> ParseBatchFileAsync(string batchFilePath, CancellationToken cancellationToken = default)
    {
        if (!File.Exists(batchFilePath) || Path.GetExtension(batchFilePath).ToLower() != ".txt")
        {
            return [];
        }
        var result = new List<BatchFileEntry>();
        await foreach (var line in File.ReadLinesAsync(batchFilePath))
        {
            cancellationToken.ThrowIfCancellationRequested();
            var fields = line.Split(BatchFileDelimiter);
            if (fields.Length < 1 || fields.Length > 3)
            {
                continue;
            }
            if (!Uri.TryCreate(fields[0].Trim().Trim('"').Trim(), UriKind.Absolute, out var url))
            {
                continue;
            }
            var entry = new BatchFileEntry(url);
            if (fields.Length >= 2)
            {
                var saveFolder = fields[1].Trim().Trim('"').Trim();
                if (Path.IsPathRooted(saveFolder))
                {
                    entry.SuggestedSaveFolder = saveFolder;
                }
            }
            if (fields.Length == 3)
            {
                var filename = fields[2].Trim().Trim('"').Trim();
                if (!string.IsNullOrEmpty(filename))
                {
                    entry.SuggestedFilename = filename;
                }
            }
            result.Add(entry);
        }
        cancellationToken.ThrowIfCancellationRequested();
        return result;
    }
}
