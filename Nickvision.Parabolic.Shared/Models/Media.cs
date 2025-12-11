using Nickvision.Desktop.Globalization;
using Nickvision.Parabolic.Shared.Helpers;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json;

namespace Nickvision.Parabolic.Shared.Models;

public class Media
{
    public string Url { get; }
    public string Title { get; }
    public int PlaylistPosition { get; }
    public MediaType Type { get; }
    public TimeFrame TimeFrame { get; }
    public List<Format> Formats { get; }
    public List<SubtitleLanguage> Subtitles { get; }
    public string SuggestedSaveFolder { get; }

    private Media(string suggestedSaveFolder, int? playlistPosition)
    {
        Url = string.Empty;
        Title = string.Empty;
        PlaylistPosition = playlistPosition.HasValue ? playlistPosition .Value : -1;
        Type = MediaType.Video;
        TimeFrame = new TimeFrame(TimeSpan.Zero, TimeSpan.Zero);
        Formats = new List<Format>();
        Subtitles = new List<SubtitleLanguage>();
        SuggestedSaveFolder = suggestedSaveFolder;
    }

    public Media(JsonElement ytdlp, ITranslationService translator, DownloaderOptions options, string suggestedSaveFolder, int? playlistPosition) : this(suggestedSaveFolder, playlistPosition)
    {
        if (playlistPosition.HasValue)
        {
            if (ytdlp.TryGetProperty("url", out var urlProperty))
            {
                Url = urlProperty.GetString() ?? string.Empty;
            }
            if (!string.IsNullOrEmpty(Url) && ytdlp.TryGetProperty("webpage_url", out var webpageUrlProperty))
            {
                Url = webpageUrlProperty.GetString() ?? string.Empty;
            }
        }
        else
        {
            if (ytdlp.TryGetProperty("webpage_url", out var webpageUrlProperty))
            {
                Url = webpageUrlProperty.GetString() ?? string.Empty;
            }
            if (!string.IsNullOrEmpty(Url) && ytdlp.TryGetProperty("url", out var urlProperty))
            {
                Url = urlProperty.GetString() ?? string.Empty;
            }
        }
        if (ytdlp.TryGetProperty("title", out var titleProperty))
        {
            Title = titleProperty.GetString() ?? "Media";
            if (ytdlp.TryGetProperty("display_id", out var displayIdProperty))
            {
                var displayId = displayIdProperty.GetString() ?? string.Empty;
                if (options.IncludeMediaIdInTitle && !string.IsNullOrEmpty(displayId))
                {
                    Title += $" [{displayId}]";
                }
            }
            Title = Title.SanitizeForFilename(options.LimitCharacters);
        }
        if (ytdlp.TryGetProperty("duration", out var durationProperty))
        {
            if (durationProperty.TryGetDouble(out var durationDouble))
            {
                TimeFrame = new TimeFrame(TimeSpan.Zero, TimeSpan.FromSeconds(durationDouble));
            }
            else if (durationProperty.TryGetInt64(out var durationInt))
            {
                TimeFrame = new TimeFrame(TimeSpan.Zero, TimeSpan.FromSeconds(durationInt));
            }
        }
        var hasVideoFormats = false;
        var hasAudioFormats = false;
        if (ytdlp.TryGetProperty("formats", out var formatsProperty))
        {
            var skippedFormats = new List<Format>();
            foreach (var formatObject in formatsProperty.EnumerateArray())
            {
                var format = new Format(formatObject, translator);
                if (format.Type != MediaType.Image)
                {
                    if (format.Type == MediaType.Video)
                    {
                        hasVideoFormats = true;
                    }
                    else if (format.Type == MediaType.Audio)
                    {
                        hasAudioFormats = true;
                    }
                    if ((format.VideoCodec.HasValue && options.PreferredVideoCodec != VideoCodec.Any && format.VideoCodec.Value != options.PreferredVideoCodec) ||
                       (format.AudioCodec.HasValue && options.PreferredAudioCodec != AudioCodec.Any && format.AudioCodec.Value != options.PreferredAudioCodec))
                    {
                        skippedFormats.Add(format);
                        continue;
                    }
                    Formats.Add(format);
                }
            }
            if(Formats.Count == 0 && skippedFormats.Count > 0)
            {
                Formats.AddRange(skippedFormats);
            }
            else if(!Formats.HasFormats(MediaType.Video) && skippedFormats.HasFormats(MediaType.Video))
            {
                foreach(var format in skippedFormats.Where(f => f.Type == MediaType.Video))
                {
                    Formats.Add(format);
                }
            }
            else if (!Formats.HasFormats(MediaType.Audio) && skippedFormats.HasFormats(MediaType.Audio))
            {
                foreach (var format in skippedFormats.Where(f => f.Type == MediaType.Audio))
                {
                    Formats.Add(format);
                }
            }
            Formats.Sort();
        }
        if(options.IncludeAutoGeneratedSubtitles && ytdlp.TryGetProperty("automatic_captions", out var autoCaptionsProperty))
        {
            foreach(var captionPair in autoCaptionsProperty.EnumerateObject())
            {
                Subtitles.Add(new SubtitleLanguage(captionPair.Name, true));
            }
        }
        if(ytdlp.TryGetProperty("subtitles", out var subtitlesProperty))
        {
            foreach (var subtitlePair in subtitlesProperty.EnumerateObject())
            {
                if(subtitlePair.Name == "live_chat")
                {
                    continue;
                }
                Subtitles.Add(new SubtitleLanguage(subtitlePair.Name, false));
            }
        }
        Subtitles.Sort();
        if(hasVideoFormats && hasAudioFormats)
        {
            Type = MediaType.Video;
            Formats.Insert(0, Format.WorstAudio);
            Formats.Insert(0, Format.BestAudio);
            Formats.Insert(0, Format.WorstVideo);
            Formats.Insert(0, Format.BestVideo);
        }
        else if(hasAudioFormats)
        {
            Type = MediaType.Audio;
            Formats.Insert(0, Format.WorstAudio);
            Formats.Insert(0, Format.BestAudio);
        }
        else
        {
            Type = MediaType.Video;
            Formats.Insert(0, Format.WorstVideo);
            Formats.Insert(0, Format.BestVideo);
        }
        Formats.Insert(0, Format.NoneVideo);
        Formats.Insert(0, Format.NoneAudio);
    }
}
