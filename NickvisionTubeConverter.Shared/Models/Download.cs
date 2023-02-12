using NickvisionTubeConverter.Shared.Helpers;
using System;
using System.Threading;
using System.Threading.Tasks;
using YoutubeDLSharp;
using YoutubeDLSharp.Options;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// Qualities for a Download
/// </summary>
public enum Quality
{
    Best = 0,
    Good,
    Worst
}

/// <summary>
/// Subtitle types for a download
/// </summary>
public enum Subtitle
{
    None = 0,
    VTT,
    SRT
}

/// <summary>
/// A model of a video download
/// </summary>
public class Download
{
    private CancellationTokenSource? _cancellationToken;
    private string _saveFolder;
    private string _newFilename;

    public string VideoUrl { get; init; }
    public MediaFileType FileType { get; init; }
    public string Path { get; private set; }
    public Quality Quality { get; init; }
    public Subtitle Subtitle { get; init; }
    public bool IsDone { get; private set; }

    /// <summary>
    /// Constructs a Download
    /// </summary>
    /// <param name="videoUrl">The url of the video to download</param>
    /// <param name="fileType">The file type to download the video as</param>
    /// <param name="saveFolder">The folder to save the download to</param>
    /// <param name="newFilename">The filename to save the download as</param>
    /// <param name="quality">The quality of the download</param>
    /// <param name="subtitle">The subtitles for the download</param>
    public Download(string videoUrl, MediaFileType fileType, string saveFolder, string newFilename = "", Quality quality = Quality.Best, Subtitle subtitle = Subtitle.None)
    {
        _cancellationToken = null;
        _saveFolder = saveFolder;
        _newFilename = newFilename;
        VideoUrl = videoUrl;
        FileType = fileType;
        Path = $"{saveFolder}{System.IO.Path.DirectorySeparatorChar}{newFilename}";
        Quality = quality;
        Subtitle = subtitle;
        IsDone = false;
    }

    /// <summary>
    /// Gets whether or not a video url is valid
    /// </summary>
    /// <param name="url">The video url to check</param>
    /// <returns>True if valid, else false</returns>
    public static async Task<bool> GetIsValidVideoUrl(string url) => (await new YoutubeDL()
    {
        YoutubeDLPath = DependencyManager.YtdlpPath,
        FFmpegPath = DependencyManager.Ffmpeg,
    }.RunVideoDataFetch(url)).Success;

    /// <summary>
    /// Runs the download
    /// </summary>
    /// <param name="embedMetadata">Whether or not to embed video metadata in the downloaded file</param>
    /// <param name="progressCallback">A callback function for DownloadProgresss</param>
    public async Task RunAsync(bool embedMetadata, Progress<DownloadProgress>? progressCallback = null)
    {
        if(!IsDone)
        {
            _cancellationToken = new CancellationTokenSource();
            var ytdlp = new YoutubeDL()
            {
                YoutubeDLPath = DependencyManager.YtdlpPath,
                FFmpegPath = DependencyManager.Ffmpeg,
            };
            if (string.IsNullOrEmpty(System.IO.Path.GetFileName(Path)))
            {
                _newFilename = (await ytdlp.RunVideoDataFetch(VideoUrl, _cancellationToken.Token)).Data.Title;
                Path += _newFilename;
            }
            Path += FileType.GetDotExtension();
            ytdlp.OutputFolder = _saveFolder;
            ytdlp.OutputFileTemplate = $"{_newFilename}.%(ext)s";
            if (FileType.GetIsAudio())
            {
                await ytdlp.RunAudioDownload(VideoUrl, FileType switch
                {
                    MediaFileType.MP3 => AudioConversionFormat.Mp3,
                    MediaFileType.OPUS => AudioConversionFormat.Opus,
                    MediaFileType.FLAC => AudioConversionFormat.Flac,
                    MediaFileType.WAV => AudioConversionFormat.Wav,
                    _ => AudioConversionFormat.Best
                }, _cancellationToken.Token, progressCallback, null, new OptionSet()
                {
                    EmbedMetadata = embedMetadata,
                    AudioQuality = Quality == Quality.Best ? (byte)0 : (Quality == Quality.Good ? (byte)5 : (byte)10)
                });
            }
            else if(FileType.GetIsVideo())
            {
                var format = Quality == Quality.Best ? "bv*+ba/b" : (Quality == Quality.Good ? "bv*[height<=720]+ba/b[height<=720]" : "wv*+wa/w");
                await ytdlp.RunVideoDownload(VideoUrl, format, DownloadMergeFormat.Unspecified, FileType switch
                {
                    MediaFileType.MP4 => VideoRecodeFormat.Mp4,
                    MediaFileType.WEBM => VideoRecodeFormat.Webm,
                    _ => VideoRecodeFormat.None
                }, _cancellationToken.Token, progressCallback, null, new OptionSet()
                {
                    EmbedMetadata = embedMetadata,
                    EmbedSubs = true,
                    WriteAutoSubs = (await ytdlp.RunVideoDataFetch(VideoUrl)).Data.Subtitles.Count == 0,
                    SubFormat = Subtitle == Subtitle.None ? "" : (Subtitle == Subtitle.SRT ? "srt" : "vtt"),
                    SubLangs = "all"
                });
            }
            IsDone = true;
            _cancellationToken.Dispose();
            _cancellationToken = null;
        }
    }

    /// <summary>
    /// Stops the download
    /// </summary>
    public void Stop() => _cancellationToken?.Cancel();
}
