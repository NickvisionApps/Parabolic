using NickvisionTubeConverter.Shared.Helpers;
using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using YoutubeDLSharp;

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
    private Quality _quality;
    private Subtitle _subtitle;
    private dynamic _ytdlp;

    /// <summary>
    /// The url of the video
    /// </summary>
    public string VideoUrl { get; init; }
    /// <summary>
    /// The save folder for the download
    /// </summary>
    public string SaveFolder { get; init; }
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
    public Download(string videoUrl, MediaFileType fileType, string saveFolder, string saveFilename, Quality quality = Quality.Best, Subtitle subtitle = Subtitle.None)
    {
        _cancellationToken = null;
        _fileType = fileType;
        _quality = quality;
        _subtitle = subtitle;
        _ytdlp = Python.Runtime.Py.Import("yt_dlp");
        VideoUrl = videoUrl;
        SaveFolder = saveFolder;
        Filename = saveFilename;
        IsDone = false;
    }

    /// <summary>
    /// Gets whether or not a video url is valid
    /// </summary>
    /// <param name="url">The video url to check</param>
    /// <returns>True if valid, else false</returns>
    public static async Task<bool> GetIsValidVideoUrlAsync(string url)
    {
        return await Task.Run(() =>
        {
            try
            {
                using (Python.Runtime.Py.GIL())
                {
                    dynamic ytdlp = Python.Runtime.Py.Import("yt_dlp");
                    var ytOpt = new Dictionary<string, dynamic>() { { "quiet", true } };
                    ytdlp.YoutubeDL(ytOpt).extract_info(url, false);
                }
                return true;
            }
            catch(Exception e)
            {
                return false;
            }
        });
    }

    /// <summary>
    /// Gets video title from metadata
    /// </summary>
    /// <param name="url">The video url to check</param>
    /// <returns>Title string</returns>
    public static async Task<string> GetVideoTitleAsync(string url)
    {
        return await Task.Run(() =>
        {
            var title = "";
            try
            {
                using (Python.Runtime.Py.GIL())
                {
                    dynamic ytdlp = Python.Runtime.Py.Import("yt_dlp");
                    var ytOpt = new Dictionary<string, dynamic>() { { "quiet", true } };
                    title = ytdlp.YoutubeDL(ytOpt).extract_info(url, false)["title"];
                }
            }
            catch(Exception e)
            {
                title = "";
            }
            return title;
        });
        
    }

    /// <summary>
    /// Runs the download
    /// </summary>
    /// <param name="embedMetadata">Whether or not to embed video metadata in the downloaded file</param>
    /// <param name="progressCallback">A callback function for DownloadProgresss</param>
    /// <returns>True if successful, else false</returns>
    public async Task<bool> RunAsync(bool embedMetadata, Progress<DownloadProgress>? progressCallback = null)
    {
        IsDone = false;
        _cancellationToken = new CancellationTokenSource();
        /*
        var ytdlp = new YoutubeDL()
        {
            YoutubeDLPath = "",
            FFmpegPath = DependencyManager.Ffmpeg,
            OutputFolder = SaveFolder,
            OutputFileTemplate = Filename
        };
        var embedThumbnail = embedMetadata && _fileType.GetSupportsThumbnails();
        RunResult<string>? result = null;
        if (_filetype.getisaudio())
        {
            try
            {
                result = await ytdlp.runaudiodownload(videourl, _filetype switch
                {
                    mediafiletype.mp3 => audioconversionformat.mp3,
                    mediafiletype.opus => audioconversionformat.opus,
                    mediafiletype.flac => audioconversionformat.flac,
                    mediafiletype.wav => audioconversionformat.wav,
                    _ => audioconversionformat.best
                }, _cancellationtoken.token, progresscallback, null, new optionset()
                {
                    embedmetadata = embedmetadata,
                    embedthumbnail = embedthumbnail,
                    audioquality = _quality == quality.best ? (byte)0 : (_quality == quality.good ? (byte)5 : (byte)10)
                });
            }
            catch (taskcanceledexception e) { }
        }
        else if (_filetype.getisvideo())
        {
            try
            {
                result = await ytdlp.runvideodownload(videourl, _quality == quality.best ? "bv*+ba/b" : (_quality == quality.good ? "bv*[height<=720]+ba/b[height<=720]" : "wv*+wa/w"), downloadmergeformat.unspecified, _filetype switch
                {
                    mediafiletype.mp4 => videorecodeformat.mp4,
                    mediafiletype.webm => videorecodeformat.webm,
                    _ => videorecodeformat.none
                }, _cancellationtoken.token, progresscallback, null, new optionset()
                {
                    embedmetadata = embedmetadata,
                    embedthumbnail = embedthumbnail,
                    embedsubs = _subtitle != subtitle.none,
                    writeautosubs = _subtitle != subtitle.none && (await ytdlp.runvideodatafetch(videourl)).data.subtitles.count == 0,
                    subformat = _subtitle == subtitle.none ? "" : (_subtitle == subtitle.srt ? "srt" : "vtt"),
                    sublangs = _subtitle == subtitle.none ? "" : "all"
                });
            }
            catch (taskcanceledexception e) { }
        }
        isdone = true;
        _cancellationtoken.dispose();
        _cancellationtoken = null;
        if (result != null)
        {
            return result.success;
        }
        */
        return false;
    }

    /// <summary>
    /// Stops the download
    /// </summary>
    public void Stop() => _cancellationToken?.Cancel();
}
