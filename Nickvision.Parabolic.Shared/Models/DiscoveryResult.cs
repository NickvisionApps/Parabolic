using Nickvision.Desktop.Globalization;
using System;
using System.Collections.Generic;
using System.Text.Json;

namespace Nickvision.Parabolic.Shared.Models;

public class DiscoveryResult
{
    private static int _nextId = 0;

    public int Id { get; }
    public Uri Url { get; }
    public string Title { get; }
    public bool HasSuggestedSaveFolder { get; }
    public List<Media> Media { get; }

    public bool IsPlaylist => Media.Count > 1;

    static DiscoveryResult()
    {
        _nextId = 0;
    }

    private DiscoveryResult(Uri url, string suggestedSaveFolder, string suggestedSaveFilename)
    {
        Id = _nextId++;
        Url = url;
        Title = suggestedSaveFilename;
        HasSuggestedSaveFolder = !string.IsNullOrEmpty(suggestedSaveFolder);
        Media = new List<Media>();
    }

    public DiscoveryResult(JsonElement ytdlp, ITranslationService translator, DownloaderOptions downloaderOptions, Uri url, string suggestedSaveFolder, string suggestedSaveFilename) : this(url, suggestedSaveFolder, suggestedSaveFilename)
    {
        if (ytdlp.TryGetProperty("title", out var titleProperty) && titleProperty.ValueKind != JsonValueKind.Null)
        {
            Title = titleProperty.GetString() ?? string.Empty;
        }
        if (ytdlp.TryGetProperty("entries", out var entriesProperty) && entriesProperty.ValueKind == JsonValueKind.Array && entriesProperty.GetArrayLength() > 0)
        {
            var position = 0;
            foreach (var mediaObject in entriesProperty.EnumerateArray())
            {
                if(mediaObject.ValueKind != JsonValueKind.Object)
                {
                    continue;
                }
                Media.Add(new Media(mediaObject, translator, downloaderOptions, suggestedSaveFolder, string.Empty, position));
                position++;
            }
        }
        else
        {
            Media.Add(new Media(ytdlp, translator, downloaderOptions, suggestedSaveFolder, suggestedSaveFilename, null));
        }
    }

    public DiscoveryResult(Uri url, string title, List<DiscoveryResult> urlInfos) : this(url, string.Empty, title)
    {
        Title = title;
        var position = 0;
        foreach (var urlInfo in urlInfos)
        {
            HasSuggestedSaveFolder |= urlInfo.HasSuggestedSaveFolder;
            Media.EnsureCapacity(Media.Count + urlInfo.Media.Count);
            foreach (var media in urlInfo.Media)
            {
                media.PlaylistPosition = ++position;
                Media.Add(media);
            }
        }
    }
}
