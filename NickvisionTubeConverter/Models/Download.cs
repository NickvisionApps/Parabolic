using Nickvision.Avalonia.MVVM;
using NickvisionTubeConverter.Extensions;
using System;
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
    private FileFormat? _fileFormat;
    private string _status;
    private int _progress;

    public string VideoLink { get; init; }
    public string FullFilename => $"{_newFilename}{_fileFormat.ToDotExtension()}";

    public Download(string videoLink, string saveFolder, string newFilename, FileFormat fileFormat)
    {
        VideoLink = videoLink;
        _saveFolder = saveFolder;
        _newFilename = newFilename;
        _fileFormat = fileFormat;
        _status = "";
        _progress = 0;
    }

    public string Status
    {
        get => _status;

        set => SetProperty(ref _status, value);
    }

    public int Progress
    {
        get => _progress;

        set => SetProperty(ref _progress, value);
    }

    public async Task<string> DownloadAsync(CancellationTokenSource cancellationSource)
    {
        Status = "Downloading...";
        Progress = 3;
        var video = await YouTube.Default.GetVideoAsync(VideoLink);
        var videoBytes = await video.GetBytesAsync();
        var videoFilePath = $"{_saveFolder}{Path.DirectorySeparatorChar}{_newFilename}{video.FileExtension}";
        var desiredfilePath = $"{_saveFolder}{Path.DirectorySeparatorChar}{FullFilename}";
        await File.WriteAllBytesAsync(videoFilePath, videoBytes);
        Progress = 6;
        if (videoFilePath != desiredfilePath)
        {
            Status = "Converting...";
            Progress = 8;
            var mediaInfo = await FFmpeg.GetMediaInfo(videoFilePath);
            await FFmpeg.Conversions.New().AddStream(mediaInfo.VideoStreams.FirstOrDefault()).AddStream(mediaInfo.AudioStreams.FirstOrDefault()).SetOutput(desiredfilePath).SetOverwriteOutput(true).Start(cancellationSource.Token);
            File.Delete(videoFilePath);
            Status = "Completed";
            Progress = 10;
            return desiredfilePath;
        }
        Status = "Completed";
        Progress = 10;
        return videoFilePath;
    }
}
