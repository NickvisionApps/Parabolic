using Nickvision.Avalonia.MVVM;
using System;
using System.IO;
using System.Threading.Tasks;
using VideoLibrary;
using Xabe.FFmpeg;

namespace NickvisionTubeConverter.Models
{
    public class Download : ObservableObject
    {
        private string _saveFolder;
        private string _fullFilename;
        private string _status;
        private int _progress;

        public string VideoLink { get; private set; }
        public string NewFilename { get; private set; }

        public Download(string videoLink, string saveFolder, string newFilename)
        {
            VideoLink = videoLink;
            _saveFolder = saveFolder;
            NewFilename = newFilename;
            FullFilename = newFilename;
            Status = "";
            Progress = 0;
            FFmpeg.ExecutablesPath = $"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}{Path.DirectorySeparatorChar}Nickvision{Path.DirectorySeparatorChar}NickvisionTubeConverter";
        }

        public string FullFilename
        {
            get => _fullFilename;

            set => SetProperty(ref _fullFilename, value);
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

        private async Task<string> DownloadVideoAsync()
        {
            Status = "Downloading...";
            Progress = 3;
            var video = await YouTube.Default.GetVideoAsync(VideoLink);
            var videoBytes = await video.GetBytesAsync();
            var videoFilePath = $"{_saveFolder}/{NewFilename}{video.FileExtension}";
            await File.WriteAllBytesAsync(videoFilePath, videoBytes);
            Progress = 6;
            return videoFilePath;
        }

        public async Task<string> DownloadAsVideoAsync(string extension)
        {
            //Download Video
            var videoFilePath = await DownloadVideoAsync();
            FullFilename = $"{NewFilename}.{extension.ToLower()}";
            //Convert to different video format if the provided video format doesn't match the already downloaded video file
            if (!videoFilePath.ToLower().Contains($".{extension.ToLower()}"))
            {
                var newVideoFilePath = $"{_saveFolder}/{NewFilename}.{extension.ToLower()}";
                Progress = 8;
                await Conversion.ToMp4(videoFilePath, newVideoFilePath).SetOverwriteOutput(true).Start();
                File.Delete(videoFilePath);
                Status = "Completed";
                Progress = 10;
                return newVideoFilePath;
            }
            Status = "Completed";
            Progress = 10;
            return videoFilePath;
        }

        public async Task<string> DownloadAsAudioAsync(string extension)
        {
            var videoFilePath = await DownloadVideoAsync();
            var audioFilePath = $"{_saveFolder}/{NewFilename}.{extension.ToLower()}";
            FullFilename = $"{NewFilename}.{extension.ToLower()}";
            //Convert to audio file
            Progress = 8;
            await Conversion.ExtractAudio(videoFilePath, audioFilePath).SetOverwriteOutput(true).Start();
            File.Delete(videoFilePath);
            Status = "Completed";
            Progress = 10;
            return audioFilePath;
        }
    }
}