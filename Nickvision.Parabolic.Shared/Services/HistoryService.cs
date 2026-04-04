using Microsoft.Extensions.Logging;
using Nickvision.Desktop.Application;
using Nickvision.Parabolic.Shared.Helpers;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class HistoryService : IHistoryService
{
    private static readonly string TableName;

    private readonly ILogger<HistoryService> _logger;
    private readonly IConfigurationService _configurationService;
    private readonly IDatabaseService _databaseService;
    private bool _tableEnsured;

    static HistoryService()
    {
        TableName = "history";
    }

    public HistoryService(ILogger<HistoryService> logger, IConfigurationService configurationService, IDatabaseService databaseService, AppInfo appInfo)
    {
        _logger = logger;
        _configurationService = configurationService;
        _databaseService = databaseService;
        _tableEnsured = false;
    }

    public bool SortNewest
    {
        get => _configurationService.HistorySortNewest;

        set => _configurationService.HistorySortNewest = value;
    }

    public HistoryLength Length
    {
        get => _configurationService.HistoryLength;

        set => _configurationService.HistoryLength = value;
    }

    public async Task<bool> AddAsync(HistoricDownload download)
    {
        _logger.LogInformation($"Adding historic download ({download.Url}): {download.Title} @ {download.Path}");
        if (Length == HistoryLength.Never)
        {
            _logger.LogWarning("History length is set to Never. Skipping add.");
            return true;
        }
        download.DownloadedOn = DateTime.Now;
        await EnsureTableAsync();
        var res = await _databaseService.ReplaceIntoTableAsync(TableName, new Dictionary<string, object>()
        {
            { "url", download.Url.ToString() },
            { "title", download.Title },
            { "path", download.Path },
            { "downloadedOn", download.DownloadedOn.ToString("o") }
        });
        if (res)
        {
            _logger.LogInformation($"Added historic download ({download.Url}).");

        }
        else
        {
            _logger.LogError($"Failed to add historic download ({download.Url}).");
        }
        return res;
    }

    public async Task<bool> AddAsync(IReadOnlyList<HistoricDownload> downloads)
    {
        _logger.LogInformation($"Adding {downloads.Count} historic download(s)...");
        if (Length == HistoryLength.Never)
        {
            _logger.LogWarning("History length is set to Never. Skipping add.");
            return true;
        }
        await EnsureTableAsync();
        using var transaction = await _databaseService.CreateTransationAsync();
        foreach (var download in downloads)
        {
            _logger.LogInformation($"Adding historic download ({download.Url}): {download.Title} @ {download.Path}");
            download.DownloadedOn = DateTime.Now;
            if (!await _databaseService.ReplaceIntoTableAsync(TableName, new Dictionary<string, object>()
            {
                { "url", download.Url.ToString() },
                { "title", download.Title },
                { "path", download.Path },
                { "downloadedOn", download.DownloadedOn.ToString("o") }
            }))
            {
                _logger.LogError($"Failed to add historic download ({download.Url}). Rolling back...");
                await transaction.RollbackAsync();
                return false;
            }
            _logger.LogInformation($"Added historic download ({download.Url}).");
        }
        await transaction.CommitAsync();
        _logger.LogInformation($"Added {downloads.Count} historic download(s).");
        return true;
    }

    public async Task<bool> ClearAsync()
    {
        _logger.LogInformation("Clearing all historic downloads...");
        var res = await _databaseService.DropTableAsync(TableName);
        if (res)
        {
            _logger.LogInformation("Cleared all historic downloads.");
        }
        else
        {
            _logger.LogError("Failed to clear historic downloads.");
        }
        return res;
    }

    public async Task<IReadOnlyList<HistoricDownload>> GetAllAsync()
    {
        _logger.LogInformation("Fetching all historic downloads...");
        var downloads = new List<HistoricDownload>();
        var toRemove = new List<Uri>();
        var length = Length;
        using var command = await _databaseService.SelectAllFromTableAsync(TableName);
        using var reader = await command.ExecuteReaderAsync();
        while (await reader.ReadAsync())
        {
            var download = new HistoricDownload(new Uri(reader.GetString(0)))
            {
                Title = reader.GetString(1),
                Path = reader.GetString(2),
                DownloadedOn = DateTime.Parse(reader.GetString(3))
            };
            if (length != HistoryLength.Forever)
            {
                var daysSinceDownload = (DateTime.Now - download.DownloadedOn).TotalDays;
                if (daysSinceDownload > (int)length)
                {
                    _logger.LogWarning($"Removing historic download ({download.Url}) due to old age.");
                    toRemove.Add(download.Url);
                    continue;
                }
            }
            _logger.LogInformation($"Fetched historic download ({download.Url}).");
            downloads.Add(download);
        }
        if (toRemove.Count > 0)
        {
            using var transaction = await _databaseService.CreateTransationAsync();
            foreach (var url in toRemove)
            {
                await _databaseService.DeleteFromTableAsync(TableName, "url", url.ToString());
            }
            await transaction.CommitAsync();
            _logger.LogInformation($"Removed {toRemove.Count} old historic download(s).");
        }
        if (SortNewest)
        {
            downloads = downloads.OrderDescending().ToList();
        }
        else
        {
            downloads.Sort();
        }
        _logger.LogInformation($"Fetched {downloads.Count} historic download(s).");
        return downloads;
    }

    public async Task<bool> RemoveAsync(HistoricDownload download)
    {
        _logger.LogInformation($"Removing historic download ({download.Url})...");
        await EnsureTableAsync();
        var res = await _databaseService.DeleteFromTableAsync(TableName, "url", download.Url.ToString());
        if (res)
        {
            _logger.LogInformation($"Removed historic download ({download.Url}).");
        }
        else
        {
            _logger.LogError($"Failed to remove historic download ({download.Url}).");
        }
        return res;
    }

    public async Task<bool> RemoveAsync(Uri url)
    {
        _logger.LogInformation($"Removing historic download ({url})...");
        await EnsureTableAsync();
        var res = await _databaseService.DeleteFromTableAsync(TableName, "url", url.ToString());
        if (res)
        {
            _logger.LogInformation($"Removed historic download ({url}).");
        }
        else
        {
            _logger.LogError($"Failed to remove historic download ({url}).");
        }
        return res;
    }

    public async Task<bool> UpdateAsync(HistoricDownload download)
    {
        _logger.LogInformation($"Updating historic download ({download.Url})...");
        download.DownloadedOn = DateTime.Now;
        var res = await _databaseService.UpdateInTableAsync(TableName, "url", download.Url.ToString(), new Dictionary<string, object>()
        {
            { "title", download.Title },
            { "path", download.Path },
            { "downloadedOn", download.DownloadedOn.ToString("o") }
        });
        if (res)
        {
            _logger.LogInformation($"Updated historic download ({download.Url}).");
        }
        else
        {
            _logger.LogError($"Failed to update historic download ({download.Url}).");
        }
        return res;
    }

    private async Task EnsureTableAsync()
    {
        if (_tableEnsured)
        {
            return;
        }
        await _databaseService.EnsureTableExistsAsync(TableName, "url TEXT PRIMARY KEY, title TEXT, path TEXT, downloadedOn TEXT");
        _tableEnsured = true;
    }
}
