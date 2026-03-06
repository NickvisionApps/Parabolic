using Microsoft.Data.Sqlite;
using Microsoft.Extensions.Logging;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text.Json;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class RecoveryService : IAsyncDisposable, IDisposable, IRecoveryService
{
    private readonly ILogger<RecoveryService> _logger;
    private readonly string _path;
    private SqliteConnection _connection;

    public RecoveryService(ILogger<RecoveryService> logger, AppInfo appInfo)
    {
        _logger = logger;
        _path = Path.Combine(appInfo.IsPortable ? Desktop.System.Environment.ExecutingDirectory : Path.Combine(UserDirectories.Config, appInfo.Name), "recovery.db");
        _logger.LogInformation($"Loading recovery database: {_path}");
        Directory.CreateDirectory(Path.GetDirectoryName(_path)!);
        _connection = new SqliteConnection(new SqliteConnectionStringBuilder($"Data Source='{_path}'")
        {
            Mode = SqliteOpenMode.ReadWriteCreate,
            Pooling = false
        }.ToString());
        _connection.Open();
        _logger.LogInformation("Ensuring recovery table exists...");
        using var recoveryTableCommand = _connection.CreateCommand();
        recoveryTableCommand.CommandText = "CREATE TABLE IF NOT EXISTS recovery (id INTEGER PRIMARY KEY, options TEXT, credentialRequired INTEGER)";
        recoveryTableCommand.ExecuteNonQuery();
        _logger.LogInformation("Recovery database loaded.");
    }

    ~RecoveryService()
    {
        Dispose(false);
    }

    public int Count
    {
        get
        {
            using var command = _connection.CreateCommand();
            command.CommandText = "SELECT COUNT(*) FROM recovery";
            var count = Convert.ToInt32(command.ExecuteScalar());
            _logger.LogInformation($"{count} recoverable downloads found.");
            return count;
        }
    }

    public async Task<bool> AddAsync(RecoverableDownload download)
    {
        _logger.LogInformation($"Adding recoverable download ({download.Id}): {download.Options.Url} {(download.CredentialRequired ? "*" : string.Empty)}");
        using var command = _connection.CreateCommand();
        command.CommandText = "INSERT INTO recovery (id, options, credentialRequired) VALUES ($id, $options, $credentialRequired)";
        command.Parameters.AddWithValue("$id", download.Id);
        command.Parameters.AddWithValue("$options", JsonSerializer.Serialize(download.Options));
        command.Parameters.AddWithValue("$credentialRequired", download.CredentialRequired ? 1 : 0);
        var res = await command.ExecuteNonQueryAsync() > 0;
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
        using var transaction = await _connection.BeginTransactionAsync();
        foreach (var download in downloads)
        {
            _logger.LogInformation($"Adding recoverable download ({download.Id}): {download.Options.Url} {(download.CredentialRequired ? "*" : string.Empty)}");
            using var command = _connection.CreateCommand();
            command.CommandText = "INSERT INTO recovery (id, options, credentialRequired) VALUES ($id, $options, $credentialRequired)";
            command.Parameters.AddWithValue("$id", download.Id);
            command.Parameters.AddWithValue("$options", JsonSerializer.Serialize(download.Options));
            command.Parameters.AddWithValue("$credentialRequired", download.CredentialRequired);
            if (await command.ExecuteNonQueryAsync() <= 0)
            {
                _logger.LogError($"Failed to add recoverable download ({download.Id}). Rolling back...");
                await transaction.RollbackAsync();
                return false;
            }
            _logger.LogInformation($"Added recoverable download ({download.Id}).");
        }
        await transaction.CommitAsync();
        _logger.LogInformation($"Added {downloads.Count} recoverable download(s).");
        return true;
    }

    public async Task<bool> ClearAsync()
    {
        _logger.LogInformation("Clearing all recoverable downloads...");
        using var command = _connection.CreateCommand();
        command.CommandText = "DELETE FROM recovery";
        var res = await command.ExecuteNonQueryAsync() >= 0;
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

    public async ValueTask DisposeAsync()
    {
        await DisposeAsyncCore().ConfigureAwait(false);
        Dispose(false);
        GC.SuppressFinalize(this);
    }

    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    public async Task<IReadOnlyList<RecoverableDownload>> GetAllAsync()
    {
        _logger.LogInformation("Fetching all recoverable downloads...");
        var downloads = new List<RecoverableDownload>();
        using var command = _connection.CreateCommand();
        command.CommandText = "SELECT id, options, credentialRequired FROM recovery";
        using var reader = await command.ExecuteReaderAsync();
        while (await reader.ReadAsync())
        {
            var id = reader.GetInt32(0);
            var options = JsonSerializer.Deserialize<DownloadOptions>(reader.GetString(1))!;
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
        using var command = _connection.CreateCommand();
        command.CommandText = "DELETE FROM recovery WHERE id = $id";
        command.Parameters.AddWithValue("$id", download.Id);
        var res = await command.ExecuteNonQueryAsync() > 0;
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
        using var command = _connection.CreateCommand();
        command.CommandText = "DELETE FROM recovery WHERE id = $id";
        command.Parameters.AddWithValue("$id", id);
        var res = await command.ExecuteNonQueryAsync() > 0;
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
        using var transaction = await _connection.BeginTransactionAsync();
        foreach (var id in ids)
        {
            using var command = _connection.CreateCommand();
            command.CommandText = "DELETE FROM recovery WHERE id = $id";
            command.Parameters.AddWithValue("$id", id);
            if (await command.ExecuteNonQueryAsync() <= 0)
            {
                _logger.LogError($"Failed to remove recoverable download ({id}). Rolling back...");
                await transaction.RollbackAsync();
                return false;
            }
            _logger.LogInformation($"Removed recoverable download ({id}).");
        }
        await transaction.CommitAsync();
        _logger.LogInformation($"Removed {ids.Count} recoverable download(s).");
        return true;
    }

    protected virtual async ValueTask DisposeAsyncCore()
    {
        await _connection.CloseAsync().ConfigureAwait(false);
        await _connection.DisposeAsync().ConfigureAwait(false);
    }

    private void Dispose(bool disposing)
    {
        if (!disposing)
        {
            return;
        }
        _connection.Close();
        _connection.Dispose();
    }
}
