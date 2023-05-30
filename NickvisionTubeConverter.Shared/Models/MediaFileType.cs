using System;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// A model for media file types
/// </summary>
public enum MediaFileType
{
    MP4 = 0,
    WEBM,
    MP3,
    M4A,
    OPUS,
    FLAC,
    WAV,
    Video,
    Audio
}

/// <summary>
/// Helper functions for media file types
/// </summary>
public static class MediaFileTypeHelpers
{
    /// <summary>
    /// Parse a MediaFileType from a string
    /// </summary>
    /// <param name="s">The string to parse</param>
    /// <returns>The MediaFileType or null if failed</returns>
    public static MediaFileType? Parse(string s)
    {
        if (s.IndexOf('.') != -1)
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

    /// <summary>
    /// Gets the dot extension of the MediaFileType
    /// </summary>
    /// <param name="type">The MediaFileType</param>
    /// <returns>The dot extension of the MediaFileType</returns>
    public static string GetDotExtension(this MediaFileType type) => $".{type.ToString().ToLower()}";

    /// <summary>
    /// Gets whether or not the MediaFileType is an audio file type
    /// </summary>
    /// <param name="type">The MediaFileType</param>
    /// <returns>True if audio file type, else false</returns>
    public static bool GetIsAudio(this MediaFileType type) => type switch
    {
        MediaFileType.MP4 => false,
        MediaFileType.WEBM => false,
        MediaFileType.MP3 => true,
        MediaFileType.M4A => true,
        MediaFileType.OPUS => true,
        MediaFileType.FLAC => true,
        MediaFileType.WAV => true,
        MediaFileType.Video => false,
        MediaFileType.Audio => true,
        _ => false
    };

    /// <summary>
    /// Gets whether or not the MediaFileType is a media file type
    /// </summary>
    /// <param name="type">The MediaFileType</param>
    /// <returns>True if media file type, else false</returns>
    public static bool GetIsVideo(this MediaFileType type) => type switch
    {
        MediaFileType.MP4 => true,
        MediaFileType.WEBM => true,
        MediaFileType.MP3 => false,
        MediaFileType.M4A => false,
        MediaFileType.OPUS => false,
        MediaFileType.FLAC => false,
        MediaFileType.WAV => false,
        MediaFileType.Video => true,
        MediaFileType.Audio => false,
        _ => false
    };

    /// <summary>
    /// Gets whether or not the MediaFileType supports embedding thumbnails
    /// </summary>
    /// <param name="type">The MediaFileType</param>
    /// <returns>True if supported, else false</returns>
    public static bool GetSupportsThumbnails(this MediaFileType type) => type switch
    {
        MediaFileType.MP4 => true,
        MediaFileType.WEBM => false,
        MediaFileType.MP3 => true,
        MediaFileType.M4A => true,
        MediaFileType.OPUS => true,
        MediaFileType.FLAC => true,
        MediaFileType.WAV => false,
        MediaFileType.Video => false,
        MediaFileType.Audio => false,
        _ => false
    };

    /// <summary>
    /// Gets whether or not the MediaFileType is generic
    /// </summary>
    /// <param name="type">The MediaFileType</param>
    /// <returns>True if generic, else false</returns>
    public static bool GetIsGeneric(this MediaFileType type) => type switch
    {
        MediaFileType.Video => true,
        MediaFileType.Audio => true,
        _ => false
    };
}
