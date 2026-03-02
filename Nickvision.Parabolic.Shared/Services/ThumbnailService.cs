using Nickvision.Desktop.Helpers;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Http;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class ThumbnailService : IThumbnailService
{
    private readonly HttpClient _httpClient;
    private readonly Dictionary<Uri, byte[]> _cache;

    public ThumbnailService(HttpClient httpClient)
    {
        _httpClient = httpClient;
        _cache = [];
        using var stream = typeof(ThumbnailService).Assembly.GetManifestResourceStream("Nickvision.Parabolic.Shared.Resources.default_thumbnail.jpg")!;
        using var memoryStream = new MemoryStream();
        stream.CopyTo(memoryStream);
        _cache[Uri.Empty] = memoryStream.ToArray();
    }

    public async Task<byte[]> GetImageBytesAsync(Uri url)
    {
        if (_cache.TryGetValue(url, out var bytes))
        {
            return bytes;
        }
        await DownloadImageBytesAsync(url);
        return _cache[url];
    }

    public async Task<byte[]> GetImageBytesAsync(Media media)
    {
        if (_cache.TryGetValue(media.Url, out var bytes))
        {
            return bytes;
        }
        await DownloadImageBytesAsync(media);
        return _cache[media.Url];
    }

    public async Task<MemoryStream> GetImageStreamAsync(Uri url)
    {
        if (_cache.TryGetValue(url, out var bytes))
        {
            return new MemoryStream(bytes);
        }
        await DownloadImageBytesAsync(url);
        return new MemoryStream(_cache[url]);
    }

    public async Task<MemoryStream> GetImageStreamAsync(Media media)
    {
        if (_cache.TryGetValue(media.Url, out var bytes))
        {
            return new MemoryStream(bytes);
        }
        await DownloadImageBytesAsync(media);
        return new MemoryStream(_cache[media.Url]);
    }

    private async Task DownloadImageBytesAsync(Uri url)
    {
        if (_cache.ContainsKey(url) || url == Uri.Empty)
        {
            return;
        }
        var bytes = await _httpClient.GetByteArrayAsync(url);
        _cache[url] = bytes.Length == 0 ? _cache[Uri.Empty] : bytes;
    }

    private async Task DownloadImageBytesAsync(Media media)
    {
        if (_cache.ContainsKey(media.Url) || media.ThumbnailUrl == Uri.Empty)
        {
            return;
        }
        var bytes = await _httpClient.GetByteArrayAsync(media.ThumbnailUrl);
        _cache[media.Url] = bytes.Length == 0 ? _cache[Uri.Empty] : bytes;
        _cache[media.ThumbnailUrl] = bytes.Length == 0 ? _cache[Uri.Empty] : bytes;
    }
}
