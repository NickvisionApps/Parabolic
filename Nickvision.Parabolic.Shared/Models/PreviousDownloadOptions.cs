using Nickvision.Desktop.Filesystem;
using System.Collections.Generic;

namespace Nickvision.Parabolic.Shared.Models;

public class PreviousDownloadOptions
{
    public static readonly string Key;

    public bool DownloadImmediately { get; set; }
    public string SaveFolder { get; set; }
    public MediaFileType FileType { get; set; }
    public string VideoFormatId { get; set; }
    public string AudioFormatId { get; set; }
    public bool SplitChapters { get; set; }
    public bool ExportDescription { get; set; }
    public string PostProcessorArgumentName { get; set; }
    public bool WritePlaylistFile { get; set; }
    public bool NumberTitles { get; set; }
    public List<string> SubtitleLanguages { get; set; }

    static PreviousDownloadOptions()
    {
        Key = "previous";
    }

    public PreviousDownloadOptions()
    {
        DownloadImmediately = false;
        SaveFolder = UserDirectories.Downloads;
        FileType = MediaFileType.MP4;
        VideoFormatId = "BEST_VIDEO";
        AudioFormatId = "BEST_AUDIO";
        SplitChapters = false;
        ExportDescription = false;
        PostProcessorArgumentName = string.Empty;
        WritePlaylistFile = false;
        NumberTitles = false;
        SubtitleLanguages = new List<string>();
    }
}
