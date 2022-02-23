using Nickvision.Avalonia.MVVM;
using NickvisionTubeConverter.Extensions;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using VideoLibrary;
using Xabe.FFmpeg;

namespace NickvisionTubeConverter.Models;

public class Download : ObservableObject
{
    private string _saveFolder;
    private string _newFilename;
    private FileFormat _fileFormat;
    private Quality _quality;
    private DownloadStatus _status;
    private bool _working;

    public string VideoLink { get; init; }
    public string FullFilename => $"{_newFilename}{_fileFormat.ToDotExtension()}";

    public Download(string videoLink, string saveFolder, string newFilename, FileFormat fileFormat, Quality quality)
    {
        VideoLink = videoLink;
        _saveFolder = saveFolder;
        _newFilename = newFilename;
        _fileFormat = fileFormat;
        _quality = quality;
        _status = DownloadStatus.Waiting;
        _working = false;
    }

    public DownloadStatus Status
    {
        get => _status;

        set => SetProperty(ref _status, value);
    }

    public bool Working
    {
        get => _working;

        set => SetProperty(ref _working, value);
    }

    public async Task<string> DownloadAsync(CancellationTokenSource cancellationSource)
    {
        Working = true;
        Status = DownloadStatus.Finding;
        var streams = await YouTube.Default.GetAllVideosAsync(VideoLink);
        YouTubeVideo? selectedVideo = null;
        if (_fileFormat.IsVideo())
        {
            var videos = streams.Where(x => x.AdaptiveKind == AdaptiveKind.Video && x.AudioFormat != AudioFormat.Unknown);
            selectedVideo = _quality switch
            {
                Quality.Worst => videos.MinBy(x => x.Resolution),
                Quality.Best => videos.MaxBy(x => x.Resolution),
                _ => videos.FirstOrDefault()
            };
        }
        else
        {
            var audios = streams.Where(x => x.AdaptiveKind == AdaptiveKind.Audio);
            selectedVideo = _quality switch
            {
                Quality.Worst => audios.MinBy(x => x.AudioBitrate),
                Quality.Best => audios.MaxBy(x => x.AudioBitrate),
                _ => audios.FirstOrDefault()
            };
        }
        Status = DownloadStatus.Downloading;
        var videoBytes = await selectedVideo!.GetBytesAsync();
        var videoFilePath = $"{_saveFolder}{Path.DirectorySeparatorChar}{_newFilename}{selectedVideo.FileExtension}";
        var desiredfilePath = $"{_saveFolder}{Path.DirectorySeparatorChar}{FullFilename}";
        await File.WriteAllBytesAsync(videoFilePath, videoBytes);
        if (videoFilePath != desiredfilePath)
        {
            Status = DownloadStatus.Converting;
            var mediaInfo = await FFmpeg.GetMediaInfo(videoFilePath, cancellationSource.Token);
            await FFmpeg.Conversions.New().AddStream(mediaInfo.VideoStreams.FirstOrDefault()).AddStream(mediaInfo.AudioStreams.FirstOrDefault()).SetOutput(desiredfilePath).SetOverwriteOutput(true).Start(cancellationSource.Token);
            File.Delete(videoFilePath);
        }
        Status = DownloadStatus.Completed;
        Working = false;
        return desiredfilePath;
    }
}
