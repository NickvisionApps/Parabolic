using Nickvision.Desktop.Globalization;
using System;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace Nickvision.Parabolic.Shared.Models;

public class Format : IComparable<Format>, IEquatable<Format>
{
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
    public FrameRate? FrameRate { get; }

    public bool ContainsAudio => Type == MediaType.Audio || Bitrate.HasValue || !string.IsNullOrEmpty(AudioLanguage) || HasAudioDescription || AudioCodec.HasValue;

    static Format()
    {
        BestVideo = new Format("BEST_VIDEO", "BEST", MediaType.Video);
        BestAudio = new Format("BEST_AUDIO", "BEST", MediaType.Audio);
        WorstVideo = new Format("WORST_VIDEO", "WORST", MediaType.Video);
        WorstAudio = new Format("WORST_AUDIO", "WORST", MediaType.Audio);
        NoneVideo = new Format("NONE_VIDEO", "NONE", MediaType.Video);
        NoneAudio = new Format("NONE_AUDIO", "NONE", MediaType.Audio);
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
        FrameRate = null;
    }

    public Format(JsonElement ytdlp, ITranslationService translator) : this(string.Empty, string.Empty, MediaType.Video)
    {
        if (ytdlp.TryGetProperty("format_id", out var idProperty) && idProperty.ValueKind != JsonValueKind.Null)
        {
            Id = idProperty.GetString() ?? string.Empty;
        }
        if (ytdlp.TryGetProperty("protocol", out var protocolProperty) && protocolProperty.ValueKind != JsonValueKind.Null)
        {
            Protocol = protocolProperty.GetString() ?? string.Empty;
        }
        if (ytdlp.TryGetProperty("ext", out var extensionProperty) && extensionProperty.ValueKind != JsonValueKind.Null)
        {
            Extension = extensionProperty.GetString() ?? string.Empty;
        }
        if (ytdlp.TryGetProperty("filesize", out var filesizeProprty) && filesizeProprty.ValueKind != JsonValueKind.Null && filesizeProprty.TryGetUInt64(out var bytes))
        {
            Bytes = bytes;
        }
        if (ytdlp.TryGetProperty("tbr", out var bitrateProperty) && bitrateProperty.ValueKind != JsonValueKind.Null && bitrateProperty.TryGetDouble(out var bitrate))
        {
            Bitrate = bitrate;
        }
        var note = string.Empty;
        var resolution = string.Empty;
        if (ytdlp.TryGetProperty("format_note", out var noteProperty) && noteProperty.ValueKind != JsonValueKind.Null)
        {
            note = noteProperty.GetString() ?? string.Empty;
        }
        if (ytdlp.TryGetProperty("resolution", out var resolutionProprety) && resolutionProprety.ValueKind != JsonValueKind.Null)
        {
            resolution = resolutionProprety.GetString() ?? string.Empty;
        }
        if (resolution == "audio only")
        {
            Type = MediaType.Audio;
        }
        else
        {
            Type = note == "storyboard" ? MediaType.Image : MediaType.Video;
            VideoResolution = VideoResolution.Parse(resolution, translator);
        }
        var language = string.Empty;
        if (ytdlp.TryGetProperty("language", out var languageProperty) && languageProperty.ValueKind != JsonValueKind.Null)
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
        if (ytdlp.TryGetProperty("vcodec", out var videoCodecProprety) && videoCodecProprety.ValueKind != JsonValueKind.Null)
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
        if (ytdlp.TryGetProperty("acodec", out var audioCodecProperty) && audioCodecProperty.ValueKind != JsonValueKind.Null)
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
        if (ytdlp.TryGetProperty("fps", out var fpsProperty) && fpsProperty.ValueKind != JsonValueKind.Null && fpsProperty.TryGetDouble(out var fps))
        {
            FrameRate = fps switch
            {
                24.0 => Models.FrameRate.Fps24,
                30.0 => Models.FrameRate.Fps30,
                60.0 => Models.FrameRate.Fps60,
                _ => null
            };
        }
    }

    [JsonConstructor]
    internal Format(string id, string protocol, string extension, ulong bytes, MediaType type, double? bitrate, string? audioLanguage, bool hasAudioDescription, VideoCodec? videoCodec, AudioCodec? audioCodec, VideoResolution? videoResolution, FrameRate? frameRate)
    {
        Id = id;
        Protocol = protocol;
        Extension = extension;
        Bytes = bytes;
        Type = type;
        Bitrate = bitrate;
        AudioLanguage = audioLanguage;
        HasAudioDescription = hasAudioDescription;
        VideoCodec = videoCodec;
        AudioCodec = audioCodec;
        VideoResolution = videoResolution;
        FrameRate = frameRate;
    }

    public int CompareTo(Format? other)
    {
        if (other is null)
        {
            return 1;
        }
        var resolutionCompare = VideoResolution?.CompareTo(other.VideoResolution) ?? 0;
        var languageCompare = string.Compare(AudioLanguage, other.AudioLanguage, StringComparison.OrdinalIgnoreCase);
        var bitrateCompare = Bitrate.HasValue && other.Bitrate.HasValue ? Bitrate.Value.CompareTo(other.Bitrate.Value) : 0;
        if (resolutionCompare != 0)
        {
            return resolutionCompare;
        }
        else
        {
            if (Type == MediaType.Video)
            {
                if (languageCompare != 0)
                {
                    return languageCompare;
                }
                else if (bitrateCompare != 0)
                {
                    return bitrateCompare;
                }
                else
                {
                    return Id.CompareTo(other.Id);
                }
            }
            else
            {
                if (bitrateCompare != 0)
                {
                    return bitrateCompare;
                }
                else if (languageCompare != 0)
                {
                    return languageCompare;
                }
                else
                {
                    return Id.CompareTo(other.Id);
                }
            }
        }
    }

    public override bool Equals(object? obj) => obj is Format other && Equals(other);

    public bool Equals(Format? other) => other is not null && Id == other.Id;

    public override int GetHashCode() => Id.GetHashCode();

    public override string ToString() => ToString(null);

    public string ToString(ITranslationService? translator)
    {
        var result = new StringBuilder();
        void AppendSection(string value)
        {
            if (string.IsNullOrEmpty(value))
            {
                return;
            }
            if (result.Length > 0)
            {
                result.Append(" | ");
            }
            result.Append(value);
        }
        if (Type == MediaType.Video)
        {
            if (VideoResolution is not null)
            {
                AppendSection(VideoResolution.ToString(translator));
            }
            if (!string.IsNullOrEmpty(AudioLanguage))
            {
                var language = AudioLanguage;
                if (HasAudioDescription)
                {
                    language += $" ({translator?._("Audio Description") ?? "Audio Description"})";
                }
                AppendSection(language);
            }
            if (VideoCodec.HasValue)
            {
                AppendSection(VideoCodec.Value switch
                {
                    Models.VideoCodec.VP9 => "VP9",
                    Models.VideoCodec.AV01 => "AV1",
                    Models.VideoCodec.H264 => "H.264",
                    Models.VideoCodec.H265 => "H.265",
                    _ => string.Empty
                });
            }
            if (FrameRate.HasValue)
            {
                AppendSection(FrameRate.Value switch
                {
                    Models.FrameRate.Fps24 => translator?._("{0} FPS", 24) ?? "24 FPS",
                    Models.FrameRate.Fps30 => translator?._("{0} FPS", 30) ?? "30 FPS",
                    Models.FrameRate.Fps60 => translator?._("{0} FPS", 60) ?? "60 FPS",
                    _ => string.Empty
                });
            }
            if (AudioCodec.HasValue)
            {
                AppendSection(AudioCodec.Value switch
                {
                    Models.AudioCodec.FLAC => "FLAC",
                    Models.AudioCodec.WAV => "WAV",
                    Models.AudioCodec.OPUS => "OPUS",
                    Models.AudioCodec.AAC => "AAC",
                    Models.AudioCodec.MP4A => "MP4A",
                    Models.AudioCodec.MP3 => "MP3",
                    _ => string.Empty
                });
            }
            if (Bitrate.HasValue)
            {
                AppendSection($"{Bitrate.Value}k");
            }
        }
        else if (Type == MediaType.Audio)
        {
            if (Bitrate.HasValue)
            {
                AppendSection($"{Bitrate.Value}k");
            }
            if (!string.IsNullOrEmpty(AudioLanguage))
            {
                var language = AudioLanguage;
                if (HasAudioDescription)
                {
                    language += $" ({translator?._("Audio Description") ?? "Audio Description"})";
                }
                AppendSection(language);
            }
            if (AudioCodec.HasValue)
            {
                AppendSection(AudioCodec.Value switch
                {
                    Models.AudioCodec.FLAC => "FLAC",
                    Models.AudioCodec.WAV => "WAV",
                    Models.AudioCodec.OPUS => "OPUS",
                    Models.AudioCodec.AAC => "AAC",
                    Models.AudioCodec.MP4A => "MP4A",
                    Models.AudioCodec.MP3 => "MP3",
                    _ => string.Empty
                });
            }
        }
        else if (Type == MediaType.Image)
        {
            if (VideoResolution is not null)
            {
                AppendSection(VideoResolution.ToString(translator));
            }
        }
        if (Bytes > 0)
        {
            const double gib = 1024d * 1024d * 1024d;
            const double mib = 1024d * 1024d;
            if (Bytes > gib)
            {
                AppendSection(translator?._("{0:0.00} GiB", Bytes / gib) ?? string.Format("{0:0.00} GiB", Bytes / gib));
            }
            else if (Bytes > mib)
            {
                AppendSection(translator?._("{0:0.00} MiB", Bytes / mib) ?? string.Format("{0:0.00} MiB", Bytes / mib));
            }
            else if (Bytes > 1024)
            {
                AppendSection(translator?._("{0:0.00} KiB", Bytes / 1024.0) ?? string.Format("{0:0.00} KiB", Bytes / 1024.0));
            }
            else
            {
                AppendSection(translator?._("{0:0.00} B", Bytes) ?? string.Format("{0:0.00} B", Bytes));
            }
        }
        var idLabel = Id switch
        {
            "BEST_VIDEO" => translator?._("Best") ?? "Best",
            "BEST_AUDIO" => translator?._("Best") ?? "Best",
            "WORST_VIDEO" => translator?._("Worst") ?? "Worst",
            "WORST_AUDIO" => translator?._("Worst") ?? "Worst",
            "NONE_VIDEO" => translator?._("None") ?? "None",
            "NONE_AUDIO" => translator?._("None") ?? "None",
            _ => Id
        };
        if (result.Length == 0)
        {
            return idLabel;
        }
        result.Append(" (");
        result.Append(idLabel);
        result.Append(')');
        return result.ToString();
    }

    public static bool operator >(Format left, Format right) => left.CompareTo(right) > 0;

    public static bool operator <(Format left, Format right) => left.CompareTo(right) < 0;

    public static bool operator >=(Format left, Format right) => left.CompareTo(right) >= 0;

    public static bool operator <=(Format left, Format right) => left.CompareTo(right) <= 0;

    public static bool operator ==(Format left, Format right) => left.Equals(right);

    public static bool operator !=(Format left, Format right) => !left.Equals(right);
}
