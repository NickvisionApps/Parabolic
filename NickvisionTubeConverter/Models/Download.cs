using Nickvision.Avalonia.MVVM;
using NickvisionTubeConverter.Extensions;
using System;
using System.IO;
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
        FFmpeg.ExecutablesPath = $"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}{Path.DirectorySeparatorChar}Nickvision{Path.DirectorySeparatorChar}NickvisionTubeConverter";
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

    public async Task<string> DownloadAsync()
    {
        Status = "Downloading...";
        Progress = 3;
        var video = await YouTube.Default.GetVideoAsync(VideoLink);
        var videoBytes = await video.GetBytesAsync();
        var videoFilePath = $"{_saveFolder}{Path.DirectorySeparatorChar}{_newFilename}{video.FileExtension}";
        var filePath = $"{_saveFolder}{Path.DirectorySeparatorChar}{FullFilename}";
        await File.WriteAllBytesAsync(videoFilePath, videoBytes);
        Progress = 6;
        if(_fileFormat.IsAudio())
        {
            Progress = 8;
            await Conversion.ExtractAudio(videoFilePath, filePath).SetOverwriteOutput(true).Start();
            File.Delete(videoFilePath);
            Status = "Completed";
            Progress = 10;
            return filePath;
        }
        else
        {
            if(videoFilePath != filePath)
            {
                Progress = 8;
                await Conversion.ToMp4(videoFilePath, filePath).SetOverwriteOutput(true).Start();
                File.Delete(videoFilePath);
                Status = "Completed";
                Progress = 10;
                return filePath;
            }
            Status = "Completed";
            Progress = 10;
            return videoFilePath;
        }
    }
}
