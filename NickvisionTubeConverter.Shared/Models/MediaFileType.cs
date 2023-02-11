using System;

namespace NickvisionTubeConverter.Shared.Models;

public enum MediaFileType
{
    MP4 = 0,
    WEBM,
    MP3,
    OPUS,
    FLAC,
    WAV
}

public static class MediaFileTypeHelpers
{
    public static MediaFileType? Parse(string s)
    {
        if(s.IndexOf('.') != -1)
        {
            s = s.Remove(s.IndexOf('.'), 1);
        }
        try
        {
            return (MediaFileType?)Enum.Parse(typeof(MediaFileType), s, true);
        }
        catch
        {
            return null;
        }
    }

    public static string ToDotExtension(this MediaFileType type) => $".{type.ToString().ToLower()}";

    public static bool GetIsAudio(this MediaFileType type) => type switch
    {
        MediaFileType.MP4 => false,
        MediaFileType.WEBM => false,
        MediaFileType.MP3 => true,
        MediaFileType.OPUS => true,
        MediaFileType.FLAC => true,
        MediaFileType.WAV => true,
        _ => false
    };

    public static bool GetIsVideo(this MediaFileType type) => type switch
    {
        MediaFileType.MP4 => true,
        MediaFileType.WEBM => true,
        MediaFileType.MP3 => false,
        MediaFileType.OPUS => false,
        MediaFileType.FLAC => false,
        MediaFileType.WAV => false,
        _ => false
    };
}
