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
    private MediaFileType _fileType;
    private string _saveFolder;
    private string _newFilename;
    private Quality _quality;
    private Subtitle _subtitle;

    /// <summary>
    /// The url of the video
    /// </summary>
    public string VideoUrl { get; init; }
    /// <summary>
    /// The filename of the download
    /// </summary>
    public string Filename { get; private set; }
    /// <summary>
    /// Whether or not the download has completed
    /// </summary>
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
        _fileType = fileType;
        _saveFolder = saveFolder;
        _newFilename = newFilename;
        _quality = quality;
        _subtitle = subtitle;
        VideoUrl = videoUrl;
        Filename = $"{newFilename}";
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
    /// <returns>True if successful, else false</returns>
    public async Task<bool> RunAsync(bool embedMetadata, Progress<DownloadProgress>? progressCallback = null)
    {
        if(!IsDone)
        {
            _cancellationToken = new CancellationTokenSource();
            var ytdlp = new YoutubeDL()
            {
                YoutubeDLPath = DependencyManager.YtdlpPath,
                FFmpegPath = DependencyManager.Ffmpeg,
            };
            RunResult<string>? result = null;
            if (string.IsNullOrEmpty(Filename))
            {
                _newFilename = (await ytdlp.RunVideoDataFetch(VideoUrl)).Data.Title;
                Filename += _newFilename;
            }
            Filename += _fileType.GetDotExtension();
            ytdlp.OutputFolder = _saveFolder;
            ytdlp.OutputFileTemplate = $"{_newFilename}.%(ext)s";
            if (_fileType.GetIsAudio())
            {
                try
                {
                    result = await ytdlp.RunAudioDownload(VideoUrl, _fileType switch
                    {
                        MediaFileType.MP3 => AudioConversionFormat.Mp3,
                        MediaFileType.OPUS => AudioConversionFormat.Opus,
                        MediaFileType.FLAC => AudioConversionFormat.Flac,
                        MediaFileType.WAV => AudioConversionFormat.Wav,
                        _ => AudioConversionFormat.Best
                    }, _cancellationToken.Token, progressCallback, null, new OptionSet()
                    {
                        EmbedMetadata = embedMetadata,
                        AudioQuality = _quality == Quality.Best ? (byte)0 : (_quality == Quality.Good ? (byte)5 : (byte)10)
                    });
                }
                catch (TaskCanceledException e) { }
            }
            else if(_fileType.GetIsVideo())
            {
                try
                {
                    result = await ytdlp.RunVideoDownload(VideoUrl, _quality == Quality.Best ? "bv*+ba/b" : (_quality == Quality.Good ? "bv*[height<=720]+ba/b[height<=720]" : "wv*+wa/w"), DownloadMergeFormat.Unspecified, _fileType switch
                    {
                        MediaFileType.MP4 => VideoRecodeFormat.Mp4,
                        MediaFileType.WEBM => VideoRecodeFormat.Webm,
                        _ => VideoRecodeFormat.None
                    }, _cancellationToken.Token, progressCallback, null, new OptionSet()
                    {
                        EmbedMetadata = embedMetadata,
                        EmbedSubs = true,
                        WriteAutoSubs = (await ytdlp.RunVideoDataFetch(VideoUrl)).Data.Subtitles.Count == 0,
                        SubFormat = _subtitle == Subtitle.None ? "" : (_subtitle == Subtitle.SRT ? "srt" : "vtt"),
                        SubLangs = "all"
                    });
                }
                catch (TaskCanceledException e) { }
            }
            IsDone = true;
            _cancellationToken.Dispose();
            _cancellationToken = null;
            if(result != null)
            {
                return result.Success;
            }
            return false;
        }
        return false;
    }

    /// <summary>
    /// Stops the download
    /// </summary>
    public void Stop() => _cancellationToken?.Cancel();
}
