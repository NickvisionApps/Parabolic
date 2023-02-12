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
    OPUS,
    FLAC,
    WAV
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
        MediaFileType.OPUS => true,
        MediaFileType.FLAC => true,
        MediaFileType.WAV => true,
        _ => false
    };

    /// <summary>
    /// Gets whether or not the MediaFileType is a video file type
    /// </summary>
    /// <param name="type">The MediaFileType</param>
    /// <returns>True if video file type, else false</returns>
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
