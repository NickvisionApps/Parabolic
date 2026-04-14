using Microsoft.Extensions.Logging;
using Nickvision.Desktop.Application;
using Nickvision.Parabolic.Shared.Helpers;
using Nickvision.Parabolic.Shared.Models;
using System.Collections.Generic;
using System.Text.Json;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class RecoveryService : IRecoveryService
{
    private static readonly string TableName;

    private readonly ILogger<RecoveryService> _logger;
    private readonly IConfigurationService _configurationService;
    private readonly IDatabaseService _databaseService;
    private bool _tableEnsured;

    static RecoveryService()
    {
        TableName = "recovery_queue";
    }

    public RecoveryService(ILogger<RecoveryService> logger, IConfigurationService configurationService, IDatabaseService databaseService)
    {
        _logger = logger;
        _configurationService = configurationService;
        _databaseService = databaseService;
        _tableEnsured = false;
    }

    public int Count
    {
        get
        {
            EnsureTable();
            var count = _databaseService.CountInTable(TableName);
            _logger.LogInformation($"{count} recoverable downloads found.");
            return count;
        }
    }

    public async Task<bool> AddAsync(RecoverableDownload download)
    {
        _logger.LogInformation($"Adding recoverable download ({download.Id}): {download.Options.Url} {(download.CredentialRequired ? "*" : string.Empty)}");
        await EnsureTableAsync();
        var res = await _databaseService.InsertIntoTableAsync(TableName, new Dictionary<string, object>()
        {
            { "id", download.Id },
            { "options", JsonSerializer.Serialize(download.Options, ApplicationJsonContext.Default.DownloadOptions) },
            { "credentialRequired", download.CredentialRequired ? 1 : 0 }
        });
        if (res)
        {
            _logger.LogInformation($"Added recoverable download ({download.Id}).");
        }
        else
        {
            _logger.LogError($"Failed to add recoverable download ({download.Id}).");
        }
        return res;
    }

    public async Task<bool> AddAsync(IReadOnlyList<RecoverableDownload> downloads)
    {
        _logger.LogInformation($"Adding {downloads.Count} recoverable download(s)...");
        if (downloads.Count == 0)
        {
            return true;
        }
        await EnsureTableAsync();
        using var transaction = await _databaseService.CreateTransactionAsync();
        foreach (var download in downloads)
        {
            _logger.LogInformation($"Adding recoverable download ({download.Id}): {download.Options.Url} {(download.CredentialRequired ? "*" : string.Empty)}");
            if (!await _databaseService.InsertIntoTableAsync(TableName, new Dictionary<string, object>()
            {
                { "id", download.Id },
                { "options", JsonSerializer.Serialize(download.Options, ApplicationJsonContext.Default.DownloadOptions) },
                { "credentialRequired", download.CredentialRequired ? 1 : 0 }
            }))
            {
                _logger.LogError($"Failed to add recoverable download ({download.Id}).");
                return false;
            }
            _logger.LogInformation($"Added recoverable download ({download.Id}).");
        }
        _logger.LogInformation($"Added {downloads.Count} recoverable download(s).");
        await transaction.CommitAsync();
        return true;
    }

    public async Task<bool> ClearAsync()
    {
        _logger.LogInformation("Clearing all recoverable downloads...");
        await EnsureTableAsync();
        var res = await _databaseService.ClearTableAsync(TableName);
        if (res)
        {
            _logger.LogInformation("Cleared all recoverable downloads.");
        }
        else
        {
            _logger.LogError("Failed to clear recoverable downloads.");
        }
        return res;
    }

    public async Task<IReadOnlyList<RecoverableDownload>> GetAllAsync()
    {
        _logger.LogInformation("Fetching all recoverable downloads...");
        var downloads = new List<RecoverableDownload>();
        await EnsureTableAsync();
        using var command = await _databaseService.SelectAllFromTableAsync(TableName);
        using var reader = await command.ExecuteReaderAsync();
        while (await reader.ReadAsync())
        {
            var id = reader.GetInt32(0);
            var options = JsonSerializer.Deserialize(reader.GetString(1), ApplicationJsonContext.Default.DownloadOptions)!;
            var credentialRequired = reader.GetInt32(2) == 1;
            _logger.LogInformation($"Fetched recoverable download ({id}): {options.Url}");
            downloads.Add(new RecoverableDownload(id, options, credentialRequired));
        }
        _logger.LogInformation($"Fetched {downloads.Count} recoverable download(s).");
        return downloads;
    }

    public async Task<bool> RemoveAsync(RecoverableDownload download)
    {
        _logger.LogInformation($"Removing recoverable download ({download.Id}): {download.Options.Url}");
        await EnsureTableAsync();
        var res = await _databaseService.DeleteFromTableAsync(TableName, "id", download.Id);
        if (res)
        {
            _logger.LogInformation($"Removed recoverable download ({download.Id}).");
        }
        else
        {
            _logger.LogError($"Failed to remove recoverable download ({download.Id}).");
        }
        return res;
    }

    public async Task<bool> RemoveAsync(int id)
    {
        _logger.LogInformation($"Removing recoverable download ({id})...");
        await EnsureTableAsync();
        var res = await _databaseService.DeleteFromTableAsync(TableName, "id", id);
        if (res)
        {
            _logger.LogInformation($"Removed recoverable download ({id}).");
        }
        else
        {
            _logger.LogError($"Failed to remove recoverable download ({id}).");
        }
        return res;
    }

    public async Task<bool> RemoveAsync(IReadOnlyList<int> ids)
    {
        _logger.LogInformation($"Removing {ids.Count} recoverable download(s)...");
        await EnsureTableAsync();
        using var transaction = await _databaseService.CreateTransactionAsync();
        foreach (var id in ids)
        {
            if (!await _databaseService.DeleteFromTableAsync(TableName, "id", id))
            {
                _logger.LogError($"Failed to remove recoverable download ({id}).");
                return false;
            }
            _logger.LogInformation($"Removed recoverable download ({id}).");
        }
        await transaction.CommitAsync();
        _logger.LogInformation($"Removed {ids.Count} recoverable download(s).");
        return true;
    }

    private void EnsureTable()
    {
        if (_tableEnsured)
        {
            return;
        }
        _databaseService.EnsureTableExists(TableName, "id INTEGER PRIMARY KEY, options TEXT, credentialRequired INTEGER");
        _tableEnsured = true;
    }

    private async Task EnsureTableAsync()
    {
        if (_tableEnsured)
        {
            return;
        }
        await _databaseService.EnsureTableExistsAsync(TableName, "id INTEGER PRIMARY KEY, options TEXT, credentialRequired INTEGER");
        _tableEnsured = true;
    }
}
