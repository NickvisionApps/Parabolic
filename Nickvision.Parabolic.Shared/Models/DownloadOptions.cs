using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Keyring;
using System;
using System.Collections.Generic;
using System.IO;

namespace Nickvision.Parabolic.Shared.Models;

public class DownloadOptions
{
    private const int _maxDotExtensionLength = 15;
    // File systems limit names to 255 bytes; leave room for yt-dlp's ".f<format id>.<ext>.part" names
    private const int _maxFilenameBytes = 200;
    private bool _isEnsuringPath;

    public Uri Url { get; set; }
    public Credential? Credential { get; set; }
    public MediaFileType FileType { get; set; }
    public int PlaylistPosition { get; set; }
    public bool RequiresPlaylistItems { get; set; }
    public Format? VideoFormat { get; set; }
    public Format? AudioFormat { get; set; }
    public IReadOnlyList<SubtitleLanguage> SubtitleLanguages { get; set; }
    public bool SplitChapters { get; set; }
    public bool ExportDescription { get; set; }
    public PostProcessorArgument? PostProcessorArgument { get; set; }
    public TimeFrame? TimeFrame { get; set; }
    public VideoResolution? VideoResolution { get; set; }
    public double? AudioBitrate { get; set; }

    public DownloadOptions(Uri url)
    {
        _isEnsuringPath = false;
        Url = url;
        Credential = null;
        SaveFilename = string.Empty;
        SaveFolder = string.Empty;
        FileType = MediaFileType.MP4;
        PlaylistPosition = -1;
        RequiresPlaylistItems = false;
        VideoFormat = null;
        AudioFormat = null;
        SubtitleLanguages = new List<SubtitleLanguage>();
        SplitChapters = false;
        ExportDescription = false;
        PostProcessorArgument = null;
        TimeFrame = null;
        VideoResolution = null;
        AudioBitrate = null;
    }

    public string SaveFolder
    {
        get => field;

        set
        {
            field = value;
            EnsurePathSize();
        }
    }

    public string SaveFilename
    {
        get => field;

        set
        {
            field = TrimToUtf8Bytes(value, _maxFilenameBytes);
            EnsurePathSize();
        }
    }

    private static string TrimToUtf8Bytes(string value, int maxBytes)
    {
        var bytes = 0;
        var length = 0;
        foreach (var rune in value.EnumerateRunes())
        {
            bytes += rune.Utf8SequenceLength;
            if (bytes > maxBytes)
            {
                break;
            }
            length += rune.Utf16SequenceLength;
        }
        return value.Substring(0, length);
    }

    private void EnsurePathSize()
    {
        if (_isEnsuringPath || string.IsNullOrEmpty(SaveFolder) || string.IsNullOrEmpty(SaveFilename))
        {
            return;
        }
        _isEnsuringPath = true;
        var maxPathSize = OperatingSystem.IsWindows() ? 259 : 4095;
        if (Path.Combine(SaveFolder, SaveFilename).Length + _maxDotExtensionLength > maxPathSize)
        {
            var excessLength = Path.Combine(SaveFolder, SaveFilename).Length + _maxDotExtensionLength - maxPathSize;
            if (SaveFilename.Length > excessLength)
            {
                SaveFilename = SaveFilename.Substring(0, SaveFilename.Length - excessLength);
            }
            else if (SaveFolder.Length > excessLength)
            {
                SaveFolder = UserDirectories.Downloads;
            }
        }
        _isEnsuringPath = false;
    }
}
