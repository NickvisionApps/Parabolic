using Nickvision.Desktop.Keyring;
using System;
using System.Collections.Generic;

namespace Nickvision.Parabolic.Shared.Models;

public class DownloadOptions
{
    public Uri Url { get; set; }
    public Credential? Credential { get; set; }
    public string SaveFilename { get; set; }
    public string SaveFolder { get; set; }
    public MediaFileType FileType { get; set; }
    public int PlaylistPosition { get; set; }
    public Format? VideoFormat { get; set; }
    public Format? AudioFormat { get; set; }
    public IEnumerable<SubtitleLanguage> SubtitleLanguages { get; set; }
    public bool SplitChapters { get; set; }
    public bool ExportDescription { get; set; }
    public PostProcessorArgument? PostProcessorArgument { get; set; }
    public TimeFrame? TimeFrame { get; set; }

    public DownloadOptions(Uri url)
    {
        Url = url;
        Credential = null;
        SaveFilename = string.Empty;
        SaveFolder = string.Empty;
        FileType = MediaFileType.MP4;
        PlaylistPosition = -1;
        VideoFormat = null;
        AudioFormat = null;
        SubtitleLanguages = new List<SubtitleLanguage>();
        SplitChapters = false;
        ExportDescription = false;
        PostProcessorArgument = null;
        TimeFrame = null;
    }

    public override int GetHashCode() => HashCode.Combine(
        HashCode.Combine(Url, Credential, FileType, PlaylistPosition, VideoFormat, AudioFormat, SaveFolder, SaveFilename),
        SubtitleLanguages, SplitChapters, ExportDescription, PostProcessorArgument, TimeFrame);
}
