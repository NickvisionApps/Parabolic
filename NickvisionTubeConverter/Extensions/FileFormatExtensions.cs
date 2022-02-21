using NickvisionTubeConverter.Models;

namespace NickvisionTubeConverter.Extensions;

public static class FileFormatExtensions
{
    public static string? ToString(this FileFormat fileFormat)
    {
        return fileFormat switch
        {
            FileFormat.MP4 => "MP4",
            FileFormat.MOV => "MOV",
            FileFormat.AVI => "AVI",
            FileFormat.MP3 => "MP3",
            FileFormat.WAV => "WAV",
            FileFormat.WMA => "WMA",
            FileFormat.OGG => "OGG",
            FileFormat.FLAC => "FLAC",
            _ => null
        };
    }

    public static string? ToDotExtension(this FileFormat fileFormat)
    {
        return fileFormat switch
        {
            FileFormat.MP4 => ".mp4",
            FileFormat.MOV => ".mov",
            FileFormat.AVI => ".avi",
            FileFormat.MP3 => ".mp3",
            FileFormat.WAV => ".wav",
            FileFormat.WMA => ".wma",
            FileFormat.OGG => ".ogg",
            FileFormat.FLAC => ".flac",
            _ => null
        };
    }

    public static bool IsAudio(this FileFormat fileFormat)
    {
        return fileFormat switch
        {
            FileFormat.MP4 => false,
            FileFormat.MOV => false,
            FileFormat.AVI => false,
            FileFormat.MP3 => true,
            FileFormat.WAV => true,
            FileFormat.WMA => true,
            FileFormat.OGG => true,
            FileFormat.FLAC => true,
            _ => false
        };
    }

    public static bool IsVideo(this FileFormat fileFormat)
    {
        return fileFormat switch
        {
            FileFormat.MP4 => true,
            FileFormat.MOV => true,
            FileFormat.AVI => true,
            FileFormat.MP3 => false,
            FileFormat.WAV => false,
            FileFormat.WMA => false,
            FileFormat.OGG => false,
            FileFormat.FLAC => false,
            _ => false
        };
    }
}
