using Nickvision.Parabolic.Shared.Models;
using System;
using System.Linq;

namespace Nickvision.Parabolic.Shared.Helpers;

public static class MediaFileTypeExtensions
{
    extension(MediaFileType type)
    {
        public string DotExtension => type switch
        {
            MediaFileType.Audio => string.Empty,
            MediaFileType.Video => string.Empty,
            _ => $".{type.ToString().ToLower()}"
        };

        public bool IsGeneric => type == MediaFileType.Audio || type == MediaFileType.Video;

        public bool IsAudio => type switch
        {
            MediaFileType.Audio => true,
            MediaFileType.MP3 => true,
            MediaFileType.M4A => true,
            MediaFileType.OPUS => true,
            MediaFileType.FLAC => true,
            MediaFileType.WAV => true,
            MediaFileType.OGG => true,
            _ => false
        };

        public bool IsVideo => type switch
        {
            MediaFileType.Video => true,
            MediaFileType.MP4 => true,
            MediaFileType.WEBM => true,
            MediaFileType.MKV => true,
            MediaFileType.MOV => true,
            MediaFileType.AVI => true,
            _ => false
        };

        public bool SupportsThumbnails => type switch
        {
            MediaFileType.MP4 => true,
            MediaFileType.MKV => true,
            MediaFileType.MOV => true,
            MediaFileType.MP3 => true,
            MediaFileType.M4A => true,
            MediaFileType.OPUS => true,
            MediaFileType.FLAC => true,
            MediaFileType.OGG => true,
            _ => false
        };

        public bool ShouldRecode => type switch
        {
            MediaFileType.WEBM => true,
            MediaFileType.MOV => true,
            MediaFileType.AVI => true,
            _ => false
        };

        public bool GetSupportsSubtitleFormat(SubtitleFormat format) => format switch
        {
            SubtitleFormat.Any => type.IsVideo && type != MediaFileType.AVI,
            SubtitleFormat.VTT => type.IsVideo && type != MediaFileType.AVI,
            SubtitleFormat.SRT => type.IsVideo && type != MediaFileType.WEBM && type != MediaFileType.AVI,
            SubtitleFormat.ASS => type != MediaFileType.MKV,
            SubtitleFormat.LRC => type.IsAudio,
            _ => false
        };
    }

    extension(MediaFileType)
    {
        public static int AudioCount => Enum.GetValues<MediaFileType>().Where(x => x.IsAudio).Count();

        public static int VideoCount => Enum.GetValues<MediaFileType>().Where(x => x.IsVideo).Count();
    }
}
