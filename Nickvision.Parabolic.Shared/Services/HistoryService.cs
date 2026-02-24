using Microsoft.Data.Sqlite;
using Microsoft.Extensions.Logging;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class HistoryService : IAsyncDisposable, IDisposable, IHistoryService
{
    private readonly ILogger<HistoryService> _logger;
    private readonly string _path;
    private SqliteConnection _connection;

    public HistoryService(ILogger<HistoryService> logger, AppInfo appInfo)
    {
        _logger = logger;
        _path = Path.Combine(appInfo.IsPortable ? Desktop.System.Environment.ExecutingDirectory : Path.Combine(UserDirectories.Config, appInfo.Name), "history.db");
        _logger.LogInformation($"Loading history database: {_path}");
        Directory.CreateDirectory(Path.GetDirectoryName(_path)!);
        _connection = new SqliteConnection(new SqliteConnectionStringBuilder($"Data Source='{_path}'")
        {
            Mode = SqliteOpenMode.ReadWriteCreate,
            Pooling = false
        }.ToString());
        _connection.Open();
        _logger.LogInformation("Ensuring settings tables exist...");
        using var settingsTableCommand = _connection.CreateCommand();
        settingsTableCommand.CommandText = "CREATE TABLE IF NOT EXISTS settings (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT UNIQUE, value TEXT)";
        settingsTableCommand.ExecuteNonQuery();
        _logger.LogInformation("Ensuring history table exists...");
        using var historyTableCommand = _connection.CreateCommand();
        historyTableCommand.CommandText = "CREATE TABLE IF NOT EXISTS history (id INTEGER PRIMARY KEY AUTOINCREMENT, url TEXT UNIQUE, title TEXT, path TEXT, downloadedOn TEXT)";
        historyTableCommand.ExecuteNonQuery();
        _logger.LogInformation("History database loaded.");
    }

    ~HistoryService()
    {
        Dispose(false);
    }

    public bool SortNewest
    {
        get
        {
            using var command = _connection.CreateCommand();
            command.CommandText = "SELECT value FROM settings WHERE name = $name";
            command.Parameters.AddWithValue("$name", "sort_newest");
            using var reader = command.ExecuteReader();
            while (reader.Read())
            {
                return reader.GetInt32(0) == 1;
            }
            return true;
        }

        set
        {
            using var command = _connection.CreateCommand();
            command.CommandText = "INSERT INTO settings (name, value) VALUES ($name, $value) ON CONFLICT(name) DO UPDATE SET value = $value WHERE name = $name";
            command.Parameters.AddWithValue("$name", "sort_newest");
            command.Parameters.AddWithValue("$value", value ? 1 : 0);
            command.ExecuteNonQuery();
        }
    }

    public HistoryLength Length
    {
        get
        {
            using var command = _connection.CreateCommand();
            command.CommandText = "SELECT value FROM settings WHERE name = $name";
            command.Parameters.AddWithValue("$name", "length");
            using var reader = command.ExecuteReader();
            while (reader.Read())
            {
                return (HistoryLength)reader.GetInt32(0);
            }
            return HistoryLength.OneWeek;
        }

        set
        {
            using var command = _connection.CreateCommand();
            command.CommandText = "INSERT INTO settings (name, value) VALUES ($name, $value) ON CONFLICT(name) DO UPDATE SET value = $value WHERE name = $name";
            command.Parameters.AddWithValue("$name", "length");
            command.Parameters.AddWithValue("$value", (int)value);
            command.ExecuteNonQuery();
        }
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
        using var command = _connection.CreateCommand();
        command.CommandText = "INSERT INTO history (url, title, path, downloadedOn) VALUES ($url, $title, $path, $downloadedOn) ON CONFLICT(url) DO UPDATE SET title = $title, path = $path, downloadedOn = $downloadedOn WHERE url = $url";
        command.Parameters.AddWithValue("$url", download.Url.ToString());
        command.Parameters.AddWithValue("$title", download.Title);
        command.Parameters.AddWithValue("$path", download.Path);
        command.Parameters.AddWithValue("$downloadedOn", download.DownloadedOn.ToString("o"));
        var res = await command.ExecuteNonQueryAsync() > 0;
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
        using var transaction = await _connection.BeginTransactionAsync();
        foreach (var download in downloads)
        {
            _logger.LogInformation($"Adding historic download ({download.Url}): {download.Title} @ {download.Path}");
            download.DownloadedOn = DateTime.Now;
            using var command = _connection.CreateCommand();
            command.CommandText = "INSERT INTO history (url, title, path, downloadedOn) VALUES ($url, $title, $path, $downloadedOn) ON CONFLICT(url) DO UPDATE SET title = $title, path = $path, downloadedOn = $downloadedOn WHERE url = $url";
            command.Parameters.AddWithValue("$url", download.Url.ToString());
            command.Parameters.AddWithValue("$title", download.Title);
            command.Parameters.AddWithValue("$path", download.Path);
            command.Parameters.AddWithValue("$downloadedOn", download.DownloadedOn.ToString("o"));
            if (await command.ExecuteNonQueryAsync() <= 0)
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
        using var command = _connection.CreateCommand();
        command.CommandText = "DELETE FROM history";
        var res = await command.ExecuteNonQueryAsync() >= 0;
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

    public async Task<IReadOnlyList<HistoricDownload>> GetAllAsync()
    {
        _logger.LogInformation("Fetching all historic downloads...");
        var downloads = new List<HistoricDownload>();
        var toRemove = new List<int>();
        var length = Length;
        using var command = _connection.CreateCommand();
        command.CommandText = "SELECT id, url, title, path, downloadedOn FROM history ORDER BY downloadedOn DESC";
        using var reader = await command.ExecuteReaderAsync();
        while (await reader.ReadAsync())
        {
            var download = new HistoricDownload(new Uri(reader.GetString(1)))
            {
                Title = reader.GetString(2),
                Path = reader.GetString(3),
                DownloadedOn = DateTime.Parse(reader.GetString(4))
            };
            if (length != HistoryLength.Forever)
            {
                var daysSinceDownload = (DateTime.Now - download.DownloadedOn).TotalDays;
                if (daysSinceDownload > (int)length)
                {
                    _logger.LogWarning($"Removing historic download ({download.Url}) due to old age.");
                    toRemove.Add(reader.GetInt32(0));
                    continue;
                }
            }
            _logger.LogInformation($"Fetched historic download ({download.Url}).");
            downloads.Add(download);
        }
        if (toRemove.Count > 0)
        {
            using var transaction = await _connection.BeginTransactionAsync();
            foreach (var id in toRemove)
            {
                using var deleteCommand = _connection.CreateCommand();
                deleteCommand.CommandText = "DELETE FROM history WHERE id = $id";
                deleteCommand.Parameters.AddWithValue("$id", id);
                await deleteCommand.ExecuteNonQueryAsync();
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
        using var command = _connection.CreateCommand();
        command.CommandText = "DELETE FROM history WHERE url = $url";
        command.Parameters.AddWithValue("$url", download.Url.ToString());
        var res = await command.ExecuteNonQueryAsync() > 0;
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
        using var command = _connection.CreateCommand();
        command.CommandText = "DELETE FROM history WHERE url = $url";
        command.Parameters.AddWithValue("$url", url.ToString());
        var res = await command.ExecuteNonQueryAsync() > 0;
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
        using var command = _connection.CreateCommand();
        command.CommandText = "UPDATE history SET title = $title, path = $path, downloadedOn = $downloadedOn WHERE url = $url";
        command.Parameters.AddWithValue("$url", download.Url.ToString());
        command.Parameters.AddWithValue("$title", download.Title);
        command.Parameters.AddWithValue("$path", download.Path);
        command.Parameters.AddWithValue("$downloadedOn", download.DownloadedOn.ToString("o"));
        var res = await command.ExecuteNonQueryAsync() > 0;
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
