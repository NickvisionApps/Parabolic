using Nickvision.Desktop.Globalization;
using System;
using System.Text.Json;

namespace Nickvision.Parabolic.Shared.Models;

public class Format : IComparable<Format>, IEquatable<Format>
{
    private static readonly string Separator;

    public static Format BestVideo { get; }
    public static Format BestAudio { get; }
    public static Format WorstVideo { get; }
    public static Format WorstAudio { get; }
    public static Format NoneVideo { get; }
    public static Format NoneAudio { get; }

    public string Id { get; }
    public string Protocol { get; }
    public string Extension { get; }
    public ulong Bytes { get; }
    public MediaType Type { get; }
    public double? Bitrate { get; }
    public string? AudioLanguage { get; }
    public bool HasAudioDescription { get; }
    public VideoCodec? VideoCodec { get; }
    public AudioCodec? AudioCodec { get; }
    public VideoResolution? VideoResolution { get; }

    static Format()
    {
        Separator = " | ";
        BestVideo = new Format("BEST", "BEST", MediaType.Video);
        BestAudio = new Format("BEST", "BEST", MediaType.Audio);
        WorstVideo = new Format("WORST", "WORST", MediaType.Video);
        WorstAudio = new Format("WORST", "WORST", MediaType.Audio);
        NoneVideo = new Format("NONE", "NONE", MediaType.Video);
        NoneAudio = new Format("NONE", "NONE", MediaType.Audio);
    }

    private Format(string id, string protocol, MediaType type)
    {
        Id = id;
        Protocol = protocol;
        Extension = string.Empty;
        Bytes = 0u;
        Type = type;
        Bitrate = null;
        AudioLanguage = null;
        HasAudioDescription = false;
        VideoCodec = null;
        AudioCodec = null;
        VideoResolution = null;
    }

    public Format(JsonElement ytdlp, ITranslationService translator) : this(string.Empty, string.Empty, MediaType.Video)
    {
        if (ytdlp.TryGetProperty("format_id", out var idProperty))
        {
            Id = idProperty.GetString() ?? string.Empty;
        }
        if (ytdlp.TryGetProperty("protocol", out var protocolProperty))
        {
            Protocol = protocolProperty.GetString() ?? string.Empty;
        }
        if (ytdlp.TryGetProperty("ext", out var extensionProperty))
        {
            Extension = extensionProperty.GetString() ?? string.Empty;
        }
        if (ytdlp.TryGetProperty("filesize", out var filesizeProprty) && filesizeProprty.TryGetUInt64(out var bytes))
        {
            Bytes = bytes;
        }
        if (ytdlp.TryGetProperty("tbr", out var bitrateProperty) && bitrateProperty.TryGetDouble(out var bitrate))
        {
            Bitrate = bitrate;
        }
        var note = string.Empty;
        var resolution = string.Empty;
        if (ytdlp.TryGetProperty("format_note", out var noteProperty))
        {
            note = noteProperty.GetString() ?? string.Empty;
        }
        if (ytdlp.TryGetProperty("resolution", out var resolutionProprety))
        {
            resolution = resolutionProprety.GetString() ?? string.Empty;
        }
        if (resolution == "audio only")
        {
            Type = MediaType.Audio;
            var language = string.Empty;
            if (ytdlp.TryGetProperty("language", out var languageProperty))
            {
                language = languageProperty.GetString() ?? string.Empty;
            }
            if (!string.IsNullOrEmpty(language))
            {
                AudioLanguage = language;
                if (Id.ToLower().Contains("audiodesc"))
                {
                    HasAudioDescription = true;
                }
            }
            if (ytdlp.TryGetProperty("acodec", out var audioCodecProperty))
            {
                AudioCodec = (audioCodecProperty.GetString()?.ToLower() ?? string.Empty) switch
                {
                    var x when x.Contains("flac") || x.Contains("alac") => Models.AudioCodec.FLAC,
                    var x when x.Contains("wav") || x.Contains("aiff") => Models.AudioCodec.WAV,
                    var x when x.Contains("opus") => Models.AudioCodec.OPUS,
                    var x when x.Contains("aac") => Models.AudioCodec.AAC,
                    var x when x.Contains("mp4a") => Models.AudioCodec.MP4A,
                    var x when x.Contains("mp3") => Models.AudioCodec.MP3,
                    "none" => null,
                    _ => null
                };
            }
        }
        else if (note == "storyboard")
        {
            Type = MediaType.Image;
            VideoResolution = VideoResolution.Parse(resolution, translator);
        }
        else
        {
            Type = MediaType.Video;
            if (ytdlp.TryGetProperty("vcodec", out var videoCodecProprety))
            {
                VideoCodec = (videoCodecProprety.GetString()?.ToLower() ?? string.Empty) switch
                {
                    var x when x.Contains("vp09") || x.Contains("vp9") => Models.VideoCodec.VP9,
                    var x when x.Contains("av01") => Models.VideoCodec.AV01,
                    var x when x.Contains("avc1") || x.Contains("h264") => Models.VideoCodec.H264,
                    var x when x.Contains("hevc") || x.Contains("h265") => Models.VideoCodec.H265,
                    "none" => null,
                    _ => null
                };
            }
            if (ytdlp.TryGetProperty("acodec", out var audioCodecProperty))
            {
                AudioCodec = (audioCodecProperty.GetString()?.ToLower() ?? string.Empty) switch
                {
                    var x when x.Contains("flac") || x.Contains("alac") => Models.AudioCodec.FLAC,
                    var x when x.Contains("wav") || x.Contains("aiff") => Models.AudioCodec.WAV,
                    var x when x.Contains("opus") => Models.AudioCodec.OPUS,
                    var x when x.Contains("aac") => Models.AudioCodec.AAC,
                    var x when x.Contains("mp4a") => Models.AudioCodec.MP4A,
                    var x when x.Contains("mp3") => Models.AudioCodec.MP3,
                    "none" => null,
                    _ => null
                };
            }
            VideoResolution = VideoResolution.Parse(resolution, translator);
        }
    }

    public int CompareTo(Format? other)
    {
        if (other is null)
        {
            return 1;
        }
        return Id.CompareTo(other.Id);
    }

    public override bool Equals(object? obj) => obj is Format other && Equals(other);

    public bool Equals(Format? other) => other is not null && Id == other.Id;

    public override int GetHashCode() => Id.GetHashCode();

    public override string ToString() => ToString(null);

    public string ToString(ITranslationService? translator)
    {
        var result = string.Empty;
        if (Type == MediaType.Video)
        {
            if (VideoResolution is not null)
            {
                result += $"{Separator}{VideoResolution.ToString(translator)}";
            }
            if (Bitrate.HasValue)
            {
                result += $"{Separator}{Bitrate.Value}k";
            }
            if (VideoCodec.HasValue)
            {
                result += $"{Separator}{VideoCodec.Value switch
                {
                    Models.VideoCodec.VP9 => "VP9",
                    Models.VideoCodec.AV01 => "AV1",
                    Models.VideoCodec.H264 => "H.264",
                    Models.VideoCodec.H265 => "H.265",
                    _ => string.Empty
                }}";
            }
            if (AudioCodec.HasValue)
            {
                result += $"{Separator}{AudioCodec.Value switch
                {
                    Models.AudioCodec.FLAC => "FLAC",
                    Models.AudioCodec.WAV => "WAV",
                    Models.AudioCodec.OPUS => "OPUS",
                    Models.AudioCodec.AAC => "AAC",
                    Models.AudioCodec.MP4A => "MP4A",
                    Models.AudioCodec.MP3 => "MP3",
                    _ => string.Empty
                }}";
            }
        }
        else if (Type == MediaType.Audio)
        {
            if (Bitrate.HasValue)
            {
                result += $"{Separator}{Bitrate.Value}k";
            }
            if (!string.IsNullOrEmpty(AudioLanguage))
            {
                result += $"{Separator}{AudioLanguage}";
                if (HasAudioDescription)
                {
                    result += $" ({translator?._("Audio Description") ?? "Audio Description"})";
                }
            }
            if (AudioCodec.HasValue)
            {
                result += $"{Separator}{AudioCodec.Value switch
                {
                    Models.AudioCodec.FLAC => "FLAC",
                    Models.AudioCodec.WAV => "WAV",
                    Models.AudioCodec.OPUS => "OPUS",
                    Models.AudioCodec.AAC => "AAC",
                    Models.AudioCodec.MP4A => "MP4A",
                    Models.AudioCodec.MP3 => "MP3",
                    _ => string.Empty
                }}";
            }
        }
        else if (Type == MediaType.Image)
        {
            if (VideoResolution is not null)
            {
                result += $"{Separator}{VideoResolution.ToString(translator)}";
            }
        }
        if (Bytes > 0)
        {
            var pow2 = Math.Pow(1024, 2);
            var pow3 = Math.Pow(1024, 3);
            result += Separator;
            if (Bytes > pow3)
            {
                result += translator?._("{0:0.00} GiB", Bytes / pow3) ?? string.Format("{0:0.00} GiB", Bytes / pow3);
            }
            else if (Bytes > pow2)
            {
                result += translator?._("{0:0.00} MiB", Bytes / pow2) ?? string.Format("{0:0.00} MiB", Bytes / pow2);
            }
            else if (Bytes > 1024)
            {
                result += translator?._("{0:0.00} KiB", Bytes / 1024.0) ?? string.Format("{0:0.00} KiB", Bytes / 1024.0);
            }
            else
            {
                result += translator?._("{0:0.00} B", Bytes) ?? string.Format("{0:0.00} B", Bytes);
            }
        }
        result += $" ({Id switch
        {
            "BEST" => translator?._("Best") ?? "Best",
            "WORST" => translator?._("Worst") ?? "Worst",
            "NONE" => translator?._("None") ?? "None",
            _ => Id
        }})";
        if (result[1] == '|')
        {
            return result.Substring(3);
        }
        else if (result[0] == ' ')
        {
            if (result[1] == '(' && result[^1] == ')')
            {
                return result.Substring(2, result.Length - 3);
            }
            return result.Substring(1);
        }
        return result;
    }

    public static bool operator >(Format left, Format right) => left.CompareTo(right) > 0;

    public static bool operator <(Format left, Format right) => left.CompareTo(right) < 0;

    public static bool operator >=(Format left, Format right) => left.CompareTo(right) >= 0;

    public static bool operator <=(Format left, Format right) => left.CompareTo(right) <= 0;

    public static bool operator ==(Format left, Format right) => left.Equals(right);

    public static bool operator !=(Format left, Format right) => !left.Equals(right);
}
