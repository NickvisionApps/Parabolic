using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Keyring;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class DiscoveryService : IDiscoveryService
{
    private static readonly char BatchFileDelimiter;

    private readonly ITranslationService _translationService;

    static DiscoveryService()
    {
        BatchFileDelimiter = '|';
    }

    public DiscoveryService(ITranslationService translationService)
    {
        _translationService = translationService;
    }

    public async Task<UrlInfo?> GetForBatchFileAsync(string path, Credential? credential, CancellationToken cancellationToken = default)
    {
        var entries = await ParseBatchFileAsync(path, cancellationToken);
        if(entries.Count == 0)
        {
            return null;
        }
        var entryInfos = new List<UrlInfo>();
        foreach (var entry in entries)
        {
            cancellationToken.ThrowIfCancellationRequested();

        }
        cancellationToken.ThrowIfCancellationRequested();
        return null;
    }

    public async Task<UrlInfo?> GetForUrlAsync(Uri url, Credential? credential, CancellationToken cancellationToken = default)
    {
        return null;
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
