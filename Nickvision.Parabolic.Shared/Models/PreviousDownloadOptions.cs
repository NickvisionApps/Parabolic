using Nickvision.Desktop.Filesystem;
using System.Collections.Generic;

namespace Nickvision.Parabolic.Shared.Models;

public class PreviousDownloadOptions
{
    public static readonly string Key;

    public bool DownloadImmediately { get; set; }
    public string SaveFolder { get; set; }
    public MediaFileType FileType { get; set; }
    public Dictionary<MediaFileType, string> VideoFormatIds { get; set; }
    public Dictionary<MediaFileType, string> AudioFormatIds { get; set; }
    public bool SplitChapters { get; set; }
    public bool ExportDescription { get; set; }
    public string PostProcessorArgumentName { get; set; }
    public bool WritePlaylistFile { get; set; }
    public bool NumberTitles { get; set; }
    public IEnumerable<string> SubtitleLanguageStrings { get; set; }

    static PreviousDownloadOptions()
    {
        Key = "previous";
    }

    public PreviousDownloadOptions()
    {
        DownloadImmediately = false;
        SaveFolder = UserDirectories.Downloads;
        FileType = MediaFileType.MP4;
        VideoFormatIds = new Dictionary<MediaFileType, string>()
        {
            { MediaFileType.Video, Format.BestVideo.Id },
            { MediaFileType.MP4, Format.BestVideo.Id },
            { MediaFileType.MKV, Format.BestVideo.Id },
            { MediaFileType.WEBM, Format.BestVideo.Id },
            { MediaFileType.MOV, Format.BestVideo.Id },
            { MediaFileType.AVI, Format.BestVideo.Id },
            { MediaFileType.Audio, Format.NoneVideo.Id },
            { MediaFileType.MP3, Format.NoneVideo.Id },
            { MediaFileType.M4A, Format.NoneVideo.Id },
            { MediaFileType.OPUS, Format.NoneVideo.Id },
            { MediaFileType.FLAC, Format.NoneVideo.Id },
            { MediaFileType.WAV, Format.NoneVideo.Id },
            { MediaFileType.OGG, Format.NoneVideo.Id },
        };
        AudioFormatIds = new Dictionary<MediaFileType, string>()
        {
            { MediaFileType.Video, Format.BestAudio.Id },
            { MediaFileType.MP4, Format.BestAudio.Id },
            { MediaFileType.MKV, Format.BestAudio.Id },
            { MediaFileType.WEBM, Format.BestAudio.Id },
            { MediaFileType.MOV, Format.BestAudio.Id },
            { MediaFileType.AVI, Format.BestAudio.Id },
            { MediaFileType.Audio, Format.BestAudio.Id },
            { MediaFileType.MP3, Format.BestAudio.Id },
            { MediaFileType.M4A, Format.BestAudio.Id },
            { MediaFileType.OPUS, Format.BestAudio.Id },
            { MediaFileType.FLAC, Format.BestAudio.Id },
            { MediaFileType.WAV, Format.BestAudio.Id },
            { MediaFileType.OGG, Format.BestAudio.Id },
        };
        SplitChapters = false;
        ExportDescription = false;
        PostProcessorArgumentName = string.Empty;
        WritePlaylistFile = false;
        NumberTitles = false;
        SubtitleLanguageStrings = new List<string>();
    }
}
