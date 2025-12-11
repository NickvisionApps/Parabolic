using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class DiscoveryService : IDiscoveryService
{
    private static readonly char BatchFileDelimiter;

    static DiscoveryService()
    {
        BatchFileDelimiter = '|';
    }

    private async Task<List<BatchFileEntry>> ParseBatchFileAsync(string batchFilePath)
    {
        if (!File.Exists(batchFilePath) || Path.GetExtension(batchFilePath).ToLower() != ".txt")
        {
            return [];
        }
        var result = new List<BatchFileEntry>();
        await foreach (var line in File.ReadLinesAsync(batchFilePath))
        {
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
        return result;
    }
}
