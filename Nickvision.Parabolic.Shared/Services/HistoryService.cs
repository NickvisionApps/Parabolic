using Microsoft.Data.Sqlite;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class HistoryService : IAsyncDisposable, IDisposable, IHistoryService
{
    private readonly string _path;
    private SqliteConnection _connection;

    public HistoryService(AppInfo appInfo)
    {
        _path = Path.Combine(UserDirectories.Config, appInfo.Name, "history.db");
        Directory.CreateDirectory(Path.GetDirectoryName(_path)!);
        _connection = new SqliteConnection(new SqliteConnectionStringBuilder($"Data Source='{_path}'")
        {
            Mode = SqliteOpenMode.ReadWriteCreate,
            Pooling = false
        }.ToString());
        _connection.Open();
        using var settingsTableCommand = _connection.CreateCommand();
        settingsTableCommand.CommandText = "CREATE TABLE IF NOT EXISTS settings (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT UNIQUE, value TEXT)";
        settingsTableCommand.ExecuteNonQuery();
        using var historyTableCommand = _connection.CreateCommand();
        historyTableCommand.CommandText = "CREATE TABLE IF NOT EXISTS history (id INTEGER PRIMARY KEY AUTOINCREMENT, url TEXT UNIQUE, title TEXT, path TEXT, downloadedOn TEXT)";
        historyTableCommand.ExecuteNonQuery();
    }

    ~HistoryService()
    {
        Dispose(false);
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
        download.DownloadedOn = DateTime.Now;
        using var command = _connection.CreateCommand();
        command.CommandText = "INSERT INTO history (url, title, path, downloadedOn) VALUES ($url, $title, $path, $downloadedOn) ON CONFLICT(url) DO UPDATE SET title = $title, path = $path, downloadedOn = $downloadedOn WHERE url = $url";
        command.Parameters.AddWithValue("$url", download.Url.ToString());
        command.Parameters.AddWithValue("$title", download.Title);
        command.Parameters.AddWithValue("$path", download.Path);
        command.Parameters.AddWithValue("$downloadedOn", download.DownloadedOn.ToString("o"));
        return await command.ExecuteNonQueryAsync() > 0;
    }

    public async Task<bool> AddAsync(IEnumerable<HistoricDownload> downloads)
    {
        using var transaction = await _connection.BeginTransactionAsync();
        foreach (var download in downloads)
        {
            download.DownloadedOn = DateTime.Now;
            using var command = _connection.CreateCommand();
            command.CommandText = "INSERT INTO history (url, title, path, downloadedOn) VALUES ($url, $title, $path, $downloadedOn) ON CONFLICT(url) DO UPDATE SET title = $title, path = $path, downloadedOn = $downloadedOn WHERE url = $url";
            command.Parameters.AddWithValue("$url", download.Url.ToString());
            command.Parameters.AddWithValue("$title", download.Title);
            command.Parameters.AddWithValue("$path", download.Path);
            command.Parameters.AddWithValue("$downloadedOn", download.DownloadedOn.ToString("o"));
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
        command.CommandText = "DELETE FROM history";
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

    public async Task<IEnumerable<HistoricDownload>> GetAllAsync()
    {
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
                    toRemove.Add(reader.GetInt32(0));
                    continue;
                }
            }
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
        }
        return downloads;
    }

    public async Task<bool> RemoveAsync(HistoricDownload download)
    {
        using var command = _connection.CreateCommand();
        command.CommandText = "DELETE FROM history WHERE url = $url";
        command.Parameters.AddWithValue("$url", download.Url.ToString());
        return await command.ExecuteNonQueryAsync() > 0;
    }

    public async Task<bool> RemoveAsync(Uri url)
    {
        using var command = _connection.CreateCommand();
        command.CommandText = "DELETE FROM history WHERE url = $url";
        command.Parameters.AddWithValue("$url", url.ToString());
        return await command.ExecuteNonQueryAsync() > 0;
    }

    public async Task<bool> UpdateAsync(HistoricDownload download)
    {
        download.DownloadedOn = DateTime.Now;
        using var command = _connection.CreateCommand();
        command.CommandText = "UPDATE history SET title = $title, path = $path, downloadedOn = $downloadedOn WHERE url = $url";
        command.Parameters.AddWithValue("$url", download.Url.ToString());
        command.Parameters.AddWithValue("$title", download.Title);
        command.Parameters.AddWithValue("$path", download.Path);
        command.Parameters.AddWithValue("$downloadedOn", download.DownloadedOn.ToString("o"));
        return await command.ExecuteNonQueryAsync() > 0;
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
