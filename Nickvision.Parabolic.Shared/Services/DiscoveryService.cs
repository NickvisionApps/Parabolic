using Microsoft.Extensions.Logging;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Keyring;
using Nickvision.Parabolic.Shared.Helpers;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class DiscoveryService : IDiscoveryService
{
    private static readonly char BatchFileDelimiter;

    private readonly ILogger<DiscoveryService> _logger;
    private readonly IConfigurationService _configurationService;
    private readonly IThumbnailService _thumbnailService;
    private readonly ITranslationService _translationService;
    private readonly IYtdlpExecutableService _ytdlpExecutableService;

    static DiscoveryService()
    {
        BatchFileDelimiter = '|';
    }

    public DiscoveryService(ILogger<DiscoveryService> logger, IConfigurationService configurationService, IThumbnailService thumbnailService, ITranslationService translationService, IYtdlpExecutableService ytdlpExecutableService)
    {
        _logger = logger;
        _configurationService = configurationService;
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
        var processResult = await _ytdlpExecutableService.ExecuteAsync(_ytdlpExecutableService.GetDiscoveryProcessArguments(url, credential), cancellationToken);
        if (processResult.ExitCode != 0 && (string.IsNullOrEmpty(processResult.Output) || processResult.Output[0] != '{'))
        {
            _logger.LogError($"Failed to discover media for {url}: {processResult.Error.TrimEnd()}");
            throw new YtdlpException(processResult.Error);
        }
        cancellationToken.ThrowIfCancellationRequested();
        using var json = JsonDocument.Parse(processResult.Output);
        if (json.RootElement.ValueKind != JsonValueKind.Object)
        {
            _logger.LogError($"Unexpected output format from yt-dlp for {url}: {processResult.Output.TrimEnd()}");
            throw new YtdlpException($"Unexpected output format from yt-dlp: {processResult.Output}");
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
            result = new DiscoveryResult(json.RootElement, _configurationService, _translationService, url, suggestedSaveFolder, suggestedFilename);
        }
        foreach (var media in result.Media)
        {
            _thumbnailService.MapMedia(media);
        }
        _logger.LogInformation($"Discovered media for {url}: {result.Media.Count} item(s)");
        return result;
    }

    private async Task<List<BatchFileEntry>> ParseBatchFileAsync(string batchFilePath, CancellationToken cancellationToken = default)
    {
        _logger.LogInformation($"Parsing batch file: {batchFilePath}");
        if (!File.Exists(batchFilePath) || !Path.GetExtension(batchFilePath).Equals(".txt", StringComparison.OrdinalIgnoreCase))
        {
            return [];
        }
        var result = new List<BatchFileEntry>();
        await foreach (var line in File.ReadLinesAsync(batchFilePath))
        {
            cancellationToken.ThrowIfCancellationRequested();
            var firstDelimiterIndex = line.IndexOf(BatchFileDelimiter);
            var secondDelimiterIndex = firstDelimiterIndex == -1 ? -1 : line.IndexOf(BatchFileDelimiter, firstDelimiterIndex + 1);
            var thirdDelimiterIndex = secondDelimiterIndex == -1 ? -1 : line.IndexOf(BatchFileDelimiter, secondDelimiterIndex + 1);
            if (thirdDelimiterIndex != -1)
            {
                continue;
            }
            var urlField = firstDelimiterIndex == -1 ? line : line[..firstDelimiterIndex];
            if (!Uri.TryCreate(urlField.Clean(), UriKind.Absolute, out var url))
            {
                continue;
            }
            var entry = new BatchFileEntry(url);
            if (firstDelimiterIndex != -1)
            {
                var saveFolder = (secondDelimiterIndex == -1 ? line[(firstDelimiterIndex + 1)..] : line[(firstDelimiterIndex + 1)..secondDelimiterIndex]).Clean();
                if (saveFolder.StartsWith("~", StringComparison.Ordinal))
                {
                    saveFolder = UserDirectories.Home + saveFolder[1..];
                }
                if (Path.IsPathRooted(saveFolder))
                {
                    entry.SuggestedSaveFolder = saveFolder;
                }
            }
            if (secondDelimiterIndex != -1)
            {
                var filename = line[(secondDelimiterIndex + 1)..].Clean();
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
