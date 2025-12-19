using Nickvision.Desktop.Keyring;
using System;
using System.Collections.Generic;

namespace Nickvision.Parabolic.Shared.Models;

public class DownloadOptions
{
    public Uri Url { get; set; }
    public Credential? Credential { get; set; }
    public MediaFileType FileType { get; set; }
    public int PlaylistPosition { get; set; }
    public List<Format> VideoFormats { get; set; }
    public List<Format> AudioFormats { get; set; }
    public string SaveFolder { get; set; }
    public string SaveFilename { get; set; }
    public List<SubtitleLanguage> SubtitleLanguages { get; set; }
    public bool SplitChapters { get; set; }
    public bool ExportDescription { get; set; }
    public PostProcessorArgument? PostProcessorArgument { get; set; }
    public TimeFrame? TimeFrame { get; set; }

    public DownloadOptions(Uri url)
    {
        Url = url;
        Credential = null;
        FileType = MediaFileType.MP4;
        PlaylistPosition = -1;
        VideoFormats = new List<Format>();
        AudioFormats = new List<Format>();
        SaveFolder = string.Empty;
        SaveFilename = string.Empty;
        SubtitleLanguages = new List<SubtitleLanguage>();
        SplitChapters = false;
        ExportDescription = false;
        PostProcessorArgument = null;
        TimeFrame = null;
    }
}
