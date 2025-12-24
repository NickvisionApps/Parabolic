using Microsoft.Data.Sqlite;
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
    private readonly string _path;
    private SqliteConnection _connection;

    public RecoveryService(AppInfo appInfo)
    {
        _path = Path.Combine(UserDirectories.Config, appInfo.Name, "recovery.db");
        Directory.CreateDirectory(Path.GetDirectoryName(_path)!);
        _connection = new SqliteConnection(new SqliteConnectionStringBuilder($"Data Source='{_path}'")
        {
            Mode = SqliteOpenMode.ReadWriteCreate,
            Pooling = false
        }.ToString());
        _connection.Open();
        using var recoveryTableCommand = _connection.CreateCommand();
        recoveryTableCommand.CommandText = "CREATE TABLE IF NOT EXISTS recovery (id INTEGER PRIMARY KEY, options TEXT, credentialRequired INTEGER)";
        recoveryTableCommand.ExecuteNonQuery();
    }

    ~RecoveryService()
    {
        Dispose(false);
    }

    public async Task<bool> AddAsync(RecoverableDownload download)
    {
        using var command = _connection.CreateCommand();
        command.CommandText = "INSERT INTO recovery (id, options, credentialRequired) VALUES ($id, $options, $credentialRequired)";
        command.Parameters.AddWithValue("$id", download.Id);
        command.Parameters.AddWithValue("$options", JsonSerializer.Serialize(download.Options));
        command.Parameters.AddWithValue("$credentialRequired", download.CredentialRequired ? 1 : 0);
        return await command.ExecuteNonQueryAsync() > 0;
    }

    public async Task<bool> AddAsync(IEnumerable<RecoverableDownload> downloads)
    {
        using var transaction = await _connection.BeginTransactionAsync();
        foreach (var download in downloads)
        {
            using var command = _connection.CreateCommand();
            command.CommandText = "INSERT INTO recovery (id, options, credentialRequired) VALUES ($id, $options, $credentialRequired)";
            command.Parameters.AddWithValue("$id", download.Id);
            command.Parameters.AddWithValue("$options", JsonSerializer.Serialize(download.Options));
            command.Parameters.AddWithValue("$credentialRequired", download.CredentialRequired);
            if (await command.ExecuteNonQueryAsync() <= 0)
            {
                await transaction.RollbackAsync();
                return false;
            }
        }
        await transaction.CommitAsync();
        return true;
    }

    public async Task<bool> ClearAsync()
    {
        using var command = _connection.CreateCommand();
        command.CommandText = "DELETE FROM recovery";
        return await command.ExecuteNonQueryAsync() >= 0;
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

    public async Task<IEnumerable<RecoverableDownload>> GetAllAsync()
    {
        var downloads = new List<RecoverableDownload>();
        using var command = _connection.CreateCommand();
        command.CommandText = "SELECT id, options, credentialRequired FROM recovery";
        using var reader = await command.ExecuteReaderAsync();
        while (await reader.ReadAsync())
        {
            var id = reader.GetInt32(0);
            var options = JsonSerializer.Deserialize<DownloadOptions>(reader.GetString(1))!;
            var credentialRequired = reader.GetInt32(2) == 1;
            downloads.Add(new RecoverableDownload(id, options, credentialRequired));
        }
        return downloads;
    }

    public async Task<bool> RemoveAsync(RecoverableDownload download)
    {
        using var command = _connection.CreateCommand();
        command.CommandText = "DELETE FROM recovery WHERE id = $id";
        command.Parameters.AddWithValue("$id", download.Id);
        return await command.ExecuteNonQueryAsync() > 0;
    }

    public async Task<bool> RemoveAsync(int id)
    {
        using var command = _connection.CreateCommand();
        command.CommandText = "DELETE FROM recovery WHERE id = $id";
        command.Parameters.AddWithValue("$id", id);
        return await command.ExecuteNonQueryAsync() > 0;
    }

    public async Task<bool> RemoveAsync(IEnumerable<int> ids)
    {
        using var transaction = await _connection.BeginTransactionAsync();
        foreach (var id in ids)
        {
            using var command = _connection.CreateCommand();
            command.CommandText = "DELETE FROM recovery WHERE id = $id";
            command.Parameters.AddWithValue("$id", id);
            if (await command.ExecuteNonQueryAsync() <= 0)
            {
                await transaction.RollbackAsync();
                return false;
            }
        }
        await transaction.CommitAsync();
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
