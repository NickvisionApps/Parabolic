using Nickvision.Desktop.Application;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Helpers;
using Nickvision.Parabolic.Shared.Helpers;
using System;
using System.Collections.Generic;
using System.Text.Json;

namespace Nickvision.Parabolic.Shared.Models;

public class Media
{
    public Uri Url { get; }
    public string Title { get; }
    public int PlaylistPosition { get; }
    public bool RequiresPlaylistItems { get; set; }
    public MediaType Type { get; }
    public TimeFrame TimeFrame { get; }
    public List<Format> Formats { get; }
    public List<SubtitleLanguage> Subtitles { get; }
    public Uri ThumbnailUrl { get; }
    public string SuggestedSaveFolder { get; }

    private Media(string suggestedSaveFolder, string suggestedSaveFilename)
    {
        Url = Uri.Empty;
        Title = suggestedSaveFilename;
        PlaylistPosition = -1;
        RequiresPlaylistItems = false;
        Type = MediaType.Video;
        TimeFrame = new TimeFrame(TimeSpan.Zero, TimeSpan.Zero);
        Formats = new List<Format>();
        Subtitles = new List<SubtitleLanguage>();
        ThumbnailUrl = Uri.Empty;
        SuggestedSaveFolder = suggestedSaveFolder;
    }

    public Media(JsonElement ytdlp, IConfigurationService configurationService, ITranslationService translator, string suggestedSaveFolder, string suggestedSaveFilename) : this(suggestedSaveFolder, suggestedSaveFilename)
    {
        if (ytdlp.TryGetProperty("playlist_index", out var playlistIndexProperty) && playlistIndexProperty.ValueKind != JsonValueKind.Null)
        {
            PlaylistPosition = playlistIndexProperty.GetInt32();
        }
        if (PlaylistPosition != -1)
        {
            if (ytdlp.TryGetProperty("url", out var urlProperty) && urlProperty.ValueKind != JsonValueKind.Null)
            {
                if (Uri.TryCreate(urlProperty.GetString() ?? string.Empty, UriKind.Absolute, out var uri))
                {
                    Url = uri;
                }
            }
            if (Url.IsEmpty && ytdlp.TryGetProperty("webpage_url", out var webpageUrlProperty) && webpageUrlProperty.ValueKind != JsonValueKind.Null)
            {
                if (Uri.TryCreate(webpageUrlProperty.GetString() ?? string.Empty, UriKind.Absolute, out var uri))
                {
                    Url = uri;
                }
            }
        }
        else
        {
            if (ytdlp.TryGetProperty("webpage_url", out var webpageUrlProperty) && webpageUrlProperty.ValueKind != JsonValueKind.Null)
            {
                if (Uri.TryCreate(webpageUrlProperty.GetString() ?? string.Empty, UriKind.Absolute, out var uri))
                {
                    Url = uri;
                }
            }
            if (Url.IsEmpty && ytdlp.TryGetProperty("url", out var urlProperty) && urlProperty.ValueKind != JsonValueKind.Null)
            {
                if (Uri.TryCreate(urlProperty.GetString() ?? string.Empty, UriKind.Absolute, out var uri))
                {
                    Url = uri;
                }
            }
        }
        if (string.IsNullOrEmpty(Title) && ytdlp.TryGetProperty("title", out var titleProperty) && titleProperty.ValueKind != JsonValueKind.Null)
        {
            Title = titleProperty.GetString() ?? "Media";
        }
        if (configurationService.IncludeMediaIdInTitle && ytdlp.TryGetProperty("display_id", out var displayIdProperty) && displayIdProperty.ValueKind != JsonValueKind.Null)
        {
            var displayId = displayIdProperty.GetString() ?? string.Empty;
            if (!string.IsNullOrEmpty(displayId))
            {
                Title += $" [{displayId}]";
            }
        }
        Title = Title.SanitizeForFilename(configurationService.LimitCharacters);
        if (ytdlp.TryGetProperty("duration", out var durationProperty) && durationProperty.ValueKind != JsonValueKind.Null)
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
        if (ytdlp.TryGetProperty("formats", out var formatsProperty) && formatsProperty.ValueKind != JsonValueKind.Null)
        {
            var skippedFormats = new List<Format>();
            foreach (var formatObject in formatsProperty.EnumerateArray())
            {
                var format = new Format(formatObject, translator);
                if (format.Type == MediaType.Image)
                {
                    continue;
                }
                else if (!configurationService.IncludeSuperResolutions && format.Id.EndsWith("-sr", StringComparison.InvariantCultureIgnoreCase))
                {
                    continue;
                }
                if (format.Type == MediaType.Video)
                {
                    hasVideoFormats = true;
                }
                else if (format.Type == MediaType.Audio)
                {
                    hasAudioFormats = true;
                }
                if ((format.VideoCodec.HasValue && configurationService.PreferredVideoCodec != VideoCodec.Any && format.VideoCodec.Value != configurationService.PreferredVideoCodec) ||
                    (format.AudioCodec.HasValue && configurationService.PreferredAudioCodec != AudioCodec.Any && format.AudioCodec.Value != configurationService.PreferredAudioCodec) ||
                    (format.FrameRate.HasValue && configurationService.PreferredFrameRate != FrameRate.Any && format.FrameRate.Value != configurationService.PreferredFrameRate))
                {
                    skippedFormats.Add(format);
                    continue;
                }
                Formats.Add(format);
            }
            if (Formats.Count == 0 && skippedFormats.Count > 0)
            {
                var preferredCodecFormats = new List<Format>(skippedFormats.Count);
                foreach (var skippedFormat in skippedFormats)
                {
                    if ((!skippedFormat.VideoCodec.HasValue || configurationService.PreferredVideoCodec == VideoCodec.Any || skippedFormat.VideoCodec.Value == configurationService.PreferredVideoCodec) &&
                        (!skippedFormat.AudioCodec.HasValue || configurationService.PreferredAudioCodec == AudioCodec.Any || skippedFormat.AudioCodec.Value == configurationService.PreferredAudioCodec))
                    {
                        preferredCodecFormats.Add(skippedFormat);
                    }
                }
                Formats.AddRange(preferredCodecFormats.Count > 0 ? preferredCodecFormats : skippedFormats);
            }
            else
            {
                var hasSelectedVideoFormat = false;
                var hasSelectedAudioFormat = false;
                var hasSkippedVideoFormat = false;
                var hasSkippedAudioFormat = false;
                foreach (var selectedFormat in Formats)
                {
                    if (selectedFormat == Format.NoneVideo || selectedFormat == Format.NoneAudio)
                    {
                        continue;
                    }
                    if (selectedFormat.Type == MediaType.Video)
                    {
                        hasSelectedVideoFormat = true;
                    }
                    else if (selectedFormat.Type == MediaType.Audio)
                    {
                        hasSelectedAudioFormat = true;
                    }
                }
                foreach (var skippedFormat in skippedFormats)
                {
                    if (skippedFormat.Type == MediaType.Video)
                    {
                        hasSkippedVideoFormat = true;
                    }
                    else if (skippedFormat.Type == MediaType.Audio)
                    {
                        hasSkippedAudioFormat = true;
                    }
                }
                if (!hasSelectedVideoFormat && hasSkippedVideoFormat)
                {
                    var preferredCodecVideoFormats = new List<Format>();
                    foreach (var skippedFormat in skippedFormats)
                    {
                        if (skippedFormat.Type == MediaType.Video &&
                            (!skippedFormat.VideoCodec.HasValue || configurationService.PreferredVideoCodec == VideoCodec.Any || skippedFormat.VideoCodec.Value == configurationService.PreferredVideoCodec))
                        {
                            preferredCodecVideoFormats.Add(skippedFormat);
                        }
                    }
                    if (preferredCodecVideoFormats.Count > 0)
                    {
                        Formats.AddRange(preferredCodecVideoFormats);
                    }
                    else
                    {
                        foreach (var skippedFormat in skippedFormats)
                        {
                            if (skippedFormat.Type == MediaType.Video)
                            {
                                Formats.Add(skippedFormat);
                            }
                        }
                    }
                }
                else if (!hasSelectedAudioFormat && hasSkippedAudioFormat)
                {
                    var preferredCodecAudioFormats = new List<Format>();
                    foreach (var skippedFormat in skippedFormats)
                    {
                        if (skippedFormat.Type == MediaType.Audio &&
                            (!skippedFormat.AudioCodec.HasValue || configurationService.PreferredAudioCodec == AudioCodec.Any || skippedFormat.AudioCodec.Value == configurationService.PreferredAudioCodec))
                        {
                            preferredCodecAudioFormats.Add(skippedFormat);
                        }
                    }
                    if (preferredCodecAudioFormats.Count > 0)
                    {
                        Formats.AddRange(preferredCodecAudioFormats);
                    }
                    else
                    {
                        foreach (var skippedFormat in skippedFormats)
                        {
                            if (skippedFormat.Type == MediaType.Audio)
                            {
                                Formats.Add(skippedFormat);
                            }
                        }
                    }
                }
            }
            Formats.Sort();
        }
        if (configurationService.IncludeAutoGeneratedSubtitles && ytdlp.TryGetProperty("automatic_captions", out var autoCaptionsProperty) && autoCaptionsProperty.ValueKind != JsonValueKind.Null)
        {
            foreach (var captionPair in autoCaptionsProperty.EnumerateObject())
            {
                Subtitles.Add(new SubtitleLanguage(captionPair.Name, true));
            }
        }
        if (ytdlp.TryGetProperty("subtitles", out var subtitlesProperty) && subtitlesProperty.ValueKind != JsonValueKind.Null)
        {
            foreach (var subtitlePair in subtitlesProperty.EnumerateObject())
            {
                if (subtitlePair.Name == "live_chat")
                {
                    continue;
                }
                Subtitles.Add(new SubtitleLanguage(subtitlePair.Name, false));
            }
        }
        Subtitles.Sort();
        if (ytdlp.TryGetProperty("thumbnail", out var thumbnailProperty) && thumbnailProperty.ValueKind != JsonValueKind.Null)
        {
            if (Uri.TryCreate(thumbnailProperty.GetString() ?? string.Empty, UriKind.Absolute, out var uri))
            {
                ThumbnailUrl = uri;
            }
        }
        if (hasVideoFormats && hasAudioFormats)
        {
            Type = MediaType.Video;
            Formats.Insert(0, Format.WorstAudio);
            Formats.Insert(0, Format.BestAudio);
            Formats.Insert(0, Format.WorstVideo);
            Formats.Insert(0, Format.BestVideo);
        }
        else if (hasAudioFormats)
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
