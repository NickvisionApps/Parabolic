using Nickvision.Desktop.Globalization;
using System.Collections.Generic;
using System.Text.Json;

namespace Nickvision.Parabolic.Shared.Models;

public class UrlInfo
{
    public string Url { get; }
    public string Title { get; }
    public bool HasSuggestedSaveFolder { get; }
    public List<Media> Media { get; }

    public bool IsPlaylist => Media.Count > 1;

    private UrlInfo(string url, string suggestedSaveFolder, string suggestedSaveFilename)
    {
        Url = url;
        Title = suggestedSaveFilename;
        HasSuggestedSaveFolder = !string.IsNullOrEmpty(suggestedSaveFolder);
        Media = new List<Media>();
    }

    public UrlInfo(JsonElement ytdlp, ITranslationService translator, DownloaderOptions downloaderOptions, string url, string suggestedSaveFolder, string suggestedSaveFilename) : this(url, suggestedSaveFolder, suggestedSaveFilename)
    {
        if (ytdlp.TryGetProperty("title", out var titleProperty))
        {
            Title = titleProperty.GetString() ?? string.Empty;
        }
        if (ytdlp.TryGetProperty("entries", out var entriesProperty) && entriesProperty.ValueKind == JsonValueKind.Array && entriesProperty.GetArrayLength() > 0)
        {
            var position = 0;
            foreach (var mediaObject in entriesProperty.EnumerateArray())
            {
                Media.Add(new Media(mediaObject, translator, downloaderOptions, suggestedSaveFolder, string.Empty, position));
                position++;
            }
        }
        else
        {
            Media.Add(new Media(ytdlp, translator, downloaderOptions, suggestedSaveFolder, suggestedSaveFilename, null));
        }
    }

    public UrlInfo(string url, string title, List<UrlInfo> urlInfos) : this(url, string.Empty, title)
    {
        Title = title;
        foreach (var urlInfo in urlInfos)
        {
            HasSuggestedSaveFolder |= urlInfo.HasSuggestedSaveFolder;
            Media.EnsureCapacity(Media.Count + urlInfo.Media.Count);
            foreach (var media in urlInfo.Media)
            {
                Media.Add(media);
            }
        }
    }
}
