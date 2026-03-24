using Microsoft.Extensions.Logging;
using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Keyring;
using Nickvision.Parabolic.Shared.Helpers;
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

    private readonly ILogger<DiscoveryService> _logger;
    private readonly IDenoExecutableService _denoExecutableService;
    private readonly IJsonFileService _jsonFileService;
    private readonly IThumbnailService _thumbnailService;
    private readonly ITranslationService _translationService;
    private readonly IYtdlpExecutableService _ytdlpExecutableService;

    static DiscoveryService()
    {
        BatchFileDelimiter = '|';
    }

    public DiscoveryService(ILogger<DiscoveryService> logger, IDenoExecutableService denoExecutableService, IJsonFileService jsonFileService, IThumbnailService thumbnailService, ITranslationService translationService, IYtdlpExecutableService ytdlpExecutableService)
    {
        _logger = logger;
        _denoExecutableService = denoExecutableService;
        _jsonFileService = jsonFileService;
        _thumbnailService = thumbnailService;
        _translationService = translationService;
        _ytdlpExecutableService = ytdlpExecutableService;
    }

    public async Task<DiscoveryResult> GetForBatchFileAsync(string path, Credential? credential = null, CancellationToken cancellationToken = default)
    {
        var entries = await ParseBatchFileAsync(path, cancellationToken);
        if (entries.Count == 0)
        {
            throw new ArgumentException("The batch file is empty or is of invalid syntax.");
        }
        var entryInfos = new List<DiscoveryResult>();
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
        return new DiscoveryResult(new Uri(path), Path.GetFileNameWithoutExtension(path), entryInfos);
    }

    public Task<DiscoveryResult> GetForUrlAsync(Uri url, Credential? credential = null, CancellationToken cancellationToken = default) => GetForUrlAsync(url, credential, string.Empty, string.Empty, cancellationToken);

    private async Task<DiscoveryResult> GetForUrlAsync(Uri url, Credential? credential, string suggestedSaveFolder, string suggestedFilename, CancellationToken cancellationToken = default)
    {
        _logger.LogInformation($"Discovering media for {url}...");
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
        process.Start();
        var outputTask = process.StandardOutput.ReadToEndAsync(cancellationToken);
        var errorTask = process.StandardError.ReadToEndAsync(cancellationToken);
        await process.WaitForExitAsync(cancellationToken);
        var output = await outputTask;
        var error = await errorTask;
        if (process.ExitCode != 0 && (string.IsNullOrEmpty(output) || output[0] != '{'))
        {
            _logger.LogError($"Failed to discover media for {url}: {error.TrimEnd()}");
            throw new YtdlpException(error);
        }
        cancellationToken.ThrowIfCancellationRequested();
        using var json = JsonDocument.Parse(output);
        if (json.RootElement.ValueKind != JsonValueKind.Object)
        {
            _logger.LogError($"Unexpected output format from yt-dlp for {url}: {output.TrimEnd()}");
            throw new YtdlpException($"Unexpected output format from yt-dlp: {output}");
        }
        DiscoveryResult? result = null;
        if (json!.RootElement.TryGetProperty("entries", out var entriesProperty) && entriesProperty.GetArrayLength() > 0)
        {
            var urlInfos = new List<DiscoveryResult>();
            foreach (var entry in entriesProperty.EnumerateArray())
            {
                cancellationToken.ThrowIfCancellationRequested();
                if (entry.ValueKind != JsonValueKind.Object)
                {
                    continue;
                }
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
                result = new DiscoveryResult(url, titleProperty.GetString() ?? "Tab", urlInfos);
            }
        }
        if (result is null)
        {
            result = new DiscoveryResult(json.RootElement, _translationService, downloaderOptions, url, suggestedSaveFolder, suggestedFilename);

        }
        foreach (var media in result.Media)
        {
            _thumbnailService.MapMedia(media);
        }
        _logger.LogInformation($"Discovered media for {url}: {JsonSerializer.Serialize(json.RootElement, ApplicationJsonContext.Default.JsonElement)}");
        return result;
    }

    private async Task<List<BatchFileEntry>> ParseBatchFileAsync(string batchFilePath, CancellationToken cancellationToken = default)
    {
        _logger.LogInformation($"Parsing batch file: {batchFilePath}");
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
                if (saveFolder.StartsWith("~", StringComparison.Ordinal))
                {
                    saveFolder = saveFolder.Replace("~", UserDirectories.Home);
                }
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
            _logger.LogInformation($"Found batch file entry {entry.Url} with path: {Path.Combine(entry.SuggestedSaveFolder, entry.SuggestedFilename)}");
            result.Add(entry);
        }
        cancellationToken.ThrowIfCancellationRequested();
        _logger.LogInformation($"Parsed {result.Count} entries in batch file: {batchFilePath}");
        return result;
    }
}
