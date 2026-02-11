using Microsoft.VisualStudio.TestTools.UnitTesting;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.IO;
using System.Net.Http;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Tests;

[TestClass]
public class DownloadTests
{
    private static string? _downloadDirectory;
    private static DownloaderOptions? _downloaderOptions;
    private static HttpClient? _httpClient;
    private static IJsonFileService? _jsonFileService;
    private static IYtdlpExecutableService? _ytdlpExecutableService;

    [ClassInitialize]
    public static void ClassInitialize(TestContext context)
    {
        var appInfo = new AppInfo("org.nickvision.tubeconverter.download.tests", "Nickvision Parabolic Download Tests", "Parabolic Download Tests")
        {
            Version = new AppVersion("2026.2.3")
        };
        _downloadDirectory = Path.Combine(UserDirectories.Cache, "Nickvision Parabolic Download Tests");
        _downloaderOptions = new DownloaderOptions();
        _httpClient = new HttpClient();
        _jsonFileService = new JsonFileService(appInfo);
        _ytdlpExecutableService = new YtdlpExecutableService(_jsonFileService, _httpClient);
        if (Directory.Exists(_downloadDirectory))
        {
            Directory.Delete(_downloadDirectory, true);
        }
        Directory.CreateDirectory(_downloadDirectory);
    }

    [ClassCleanup]
    public static void ClassCleanup()
    {
        Directory.Delete(Path.Combine(UserDirectories.Config, "Nickvision Parabolic Download Tests"), true);
        if (Directory.Exists(_downloadDirectory))
        {
            Directory.Delete(_downloadDirectory, true);
        }
    }

    [TestMethod]
    public async Task Case001_YouTube_LostSky_MP4()
    {
        var done = false;
        using var download = new Download(new DownloadOptions(new Uri("https://www.youtube.com/watch?v=L7kF4MXXCoA"))
        {
            FileType = MediaFileType.MP4,
            VideoFormat = Format.BestVideo,
            AudioFormat = Format.BestAudio,
            SaveFolder = _downloadDirectory!,
            SaveFilename = "1"
        }, null);
        Assert.AreEqual(DownloadStatus.Queued, download.Status);
        Assert.AreEqual(Path.Combine(_downloadDirectory!, "1.mp4"), download.FilePath);
        download.Completed += (_, _) => done = true;
        download.Start(_ytdlpExecutableService!.ExecutablePath ?? "yt-dlp", _downloaderOptions!);
        Assert.AreEqual(DownloadStatus.Running, download.Status);
        while (!done)
        {
            await Task.Delay(500);
        }
        Assert.AreEqual(DownloadStatus.Success, download.Status, download.Log);
        Assert.IsFalse(string.IsNullOrEmpty(download.Log));
        Assert.IsTrue(File.Exists(Path.Combine(_downloadDirectory!, "1.mp4")));
    }

    [TestMethod]
    public async Task Case002_YouTube_Grateful_WEBM()
    {
        var done = false;
        using var download = new Download(new DownloadOptions(new Uri("https://www.youtube.com/watch?v=83RUhxsfLWs"))
        {
            FileType = MediaFileType.WEBM,
            VideoFormat = Format.BestVideo,
            AudioFormat = Format.BestAudio,
            SaveFolder = _downloadDirectory!,
            SaveFilename = "2"
        }, null);
        Assert.AreEqual(DownloadStatus.Queued, download.Status);
        Assert.AreEqual(Path.Combine(_downloadDirectory!, "2.webm"), download.FilePath);
        download.Completed += (_, _) => done = true;
        download.Start(_ytdlpExecutableService!.ExecutablePath ?? "yt-dlp", _downloaderOptions!);
        Assert.AreEqual(DownloadStatus.Running, download.Status);
        while (!done)
        {
            await Task.Delay(500);
        }
        Assert.AreEqual(DownloadStatus.Success, download.Status, download.Log);
        Assert.IsFalse(string.IsNullOrEmpty(download.Log));
        Assert.IsTrue(File.Exists(Path.Combine(_downloadDirectory!, "2.webm")));
    }

    [TestMethod]
    public async Task Case003_YouTube_SkyHigh_MKV()
    {
        var done = false;
        using var download = new Download(new DownloadOptions(new Uri("https://www.youtube.com/watch?v=TW9d8vYrVFQ"))
        {
            FileType = MediaFileType.MKV,
            VideoFormat = Format.BestVideo,
            AudioFormat = Format.BestAudio,
            SaveFolder = _downloadDirectory!,
            SaveFilename = "3"
        }, null);
        Assert.AreEqual(DownloadStatus.Queued, download.Status);
        Assert.AreEqual(Path.Combine(_downloadDirectory!, "3.mkv"), download.FilePath);
        download.Completed += (_, _) => done = true;
        download.Start(_ytdlpExecutableService!.ExecutablePath ?? "yt-dlp", _downloaderOptions!);
        Assert.AreEqual(DownloadStatus.Running, download.Status);
        while (!done)
        {
            await Task.Delay(500);
        }
        Assert.AreEqual(DownloadStatus.Success, download.Status, download.Log);
        Assert.IsFalse(string.IsNullOrEmpty(download.Log));
        Assert.IsTrue(File.Exists(Path.Combine(_downloadDirectory!, "3.mkv")));
    }

    [TestMethod]
    public async Task Case004_YouTube_OnOn_MOV()
    {
        var done = false;
        using var download = new Download(new DownloadOptions(new Uri("https://www.youtube.com/watch?v=K4DyBUG242c"))
        {
            FileType = MediaFileType.MOV,
            VideoFormat = Format.BestVideo,
            AudioFormat = Format.BestAudio,
            SaveFolder = _downloadDirectory!,
            SaveFilename = "4"
        }, null);
        Assert.AreEqual(DownloadStatus.Queued, download.Status);
        Assert.AreEqual(Path.Combine(_downloadDirectory!, "4.mov"), download.FilePath);
        download.Completed += (_, _) => done = true;
        download.Start(_ytdlpExecutableService!.ExecutablePath ?? "yt-dlp", _downloaderOptions!);
        Assert.AreEqual(DownloadStatus.Running, download.Status);
        while (!done)
        {
            await Task.Delay(500);
        }
        Assert.AreEqual(DownloadStatus.Success, download.Status, download.Log);
        Assert.IsFalse(string.IsNullOrEmpty(download.Log));
        Assert.IsTrue(File.Exists(Path.Combine(_downloadDirectory!, "4.mov")));
    }

    [TestMethod]
    public async Task Case005_YouTube_LostSky_AVI()
    {
        var done = false;
        using var download = new Download(new DownloadOptions(new Uri("https://www.youtube.com/watch?v=L7kF4MXXCoA"))
        {
            FileType = MediaFileType.AVI,
            VideoFormat = Format.BestVideo,
            AudioFormat = Format.BestAudio,
            SaveFolder = _downloadDirectory!,
            SaveFilename = "5"
        }, null);
        Assert.AreEqual(DownloadStatus.Queued, download.Status);
        Assert.AreEqual(Path.Combine(_downloadDirectory!, "5.avi"), download.FilePath);
        download.Completed += (_, _) => done = true;
        download.Start(_ytdlpExecutableService!.ExecutablePath ?? "yt-dlp", _downloaderOptions!);
        Assert.AreEqual(DownloadStatus.Running, download.Status);
        while (!done)
        {
            await Task.Delay(500);
        }
        Assert.AreEqual(DownloadStatus.Success, download.Status, download.Log);
        Assert.IsFalse(string.IsNullOrEmpty(download.Log));
        Assert.IsTrue(File.Exists(Path.Combine(_downloadDirectory!, "5.avi")));
    }

    [TestMethod]
    public async Task Case006_YouTube_LostSky_MP3()
    {
        var done = false;
        using var download = new Download(new DownloadOptions(new Uri("https://www.youtube.com/watch?v=L7kF4MXXCoA"))
        {
            FileType = MediaFileType.MP3,
            VideoFormat = Format.NoneVideo,
            AudioFormat = Format.BestAudio,
            SaveFolder = _downloadDirectory!,
            SaveFilename = "6"
        }, null);
        Assert.AreEqual(DownloadStatus.Queued, download.Status);
        Assert.AreEqual(Path.Combine(_downloadDirectory!, "6.mp3"), download.FilePath);
        download.Completed += (_, _) => done = true;
        download.Start(_ytdlpExecutableService!.ExecutablePath ?? "yt-dlp", _downloaderOptions!);
        Assert.AreEqual(DownloadStatus.Running, download.Status);
        while (!done)
        {
            await Task.Delay(500);
        }
        Assert.AreEqual(DownloadStatus.Success, download.Status, download.Log);
        Assert.IsFalse(string.IsNullOrEmpty(download.Log));
        Assert.IsTrue(File.Exists(Path.Combine(_downloadDirectory!, "6.mp3")));
    }

    [TestMethod]
    public async Task Case007_YouTube_Grateful_M4A()
    {
        var done = false;
        using var download = new Download(new DownloadOptions(new Uri("https://www.youtube.com/watch?v=83RUhxsfLWs"))
        {
            FileType = MediaFileType.M4A,
            VideoFormat = Format.NoneVideo,
            AudioFormat = Format.BestAudio,
            SaveFolder = _downloadDirectory!,
            SaveFilename = "7"
        }, null);
        Assert.AreEqual(DownloadStatus.Queued, download.Status);
        Assert.AreEqual(Path.Combine(_downloadDirectory!, "7.m4a"), download.FilePath);
        download.Completed += (_, _) => done = true;
        download.Start(_ytdlpExecutableService!.ExecutablePath ?? "yt-dlp", _downloaderOptions!);
        Assert.AreEqual(DownloadStatus.Running, download.Status);
        while (!done)
        {
            await Task.Delay(500);
        }
        Assert.AreEqual(DownloadStatus.Success, download.Status, download.Log);
        Assert.IsFalse(string.IsNullOrEmpty(download.Log));
        Assert.IsTrue(File.Exists(Path.Combine(_downloadDirectory!, "7.m4a")));
    }

    [TestMethod]
    public async Task Case008_YouTube_OnOn_OPUS()
    {
        var done = false;
        using var download = new Download(new DownloadOptions(new Uri("https://www.youtube.com/watch?v=K4DyBUG242c"))
        {
            FileType = MediaFileType.OPUS,
            VideoFormat = Format.NoneVideo,
            AudioFormat = Format.BestAudio,
            SaveFolder = _downloadDirectory!,
            SaveFilename = "8"
        }, null);
        Assert.AreEqual(DownloadStatus.Queued, download.Status);
        Assert.AreEqual(Path.Combine(_downloadDirectory!, "8.opus"), download.FilePath);
        download.Completed += (_, _) => done = true;
        download.Start(_ytdlpExecutableService!.ExecutablePath ?? "yt-dlp", _downloaderOptions!);
        Assert.AreEqual(DownloadStatus.Running, download.Status);
        while (!done)
        {
            await Task.Delay(500);
        }
        Assert.AreEqual(DownloadStatus.Success, download.Status, download.Log);
        Assert.IsFalse(string.IsNullOrEmpty(download.Log));
        Assert.IsTrue(File.Exists(Path.Combine(_downloadDirectory!, "8.opus")));
    }

    [TestMethod]
    public async Task Case009_YouTube_SkyHigh_FLAC()
    {
        var done = false;
        using var download = new Download(new DownloadOptions(new Uri("https://www.youtube.com/watch?v=TW9d8vYrVFQ"))
        {
            FileType = MediaFileType.FLAC,
            VideoFormat = Format.NoneVideo,
            AudioFormat = Format.BestAudio,
            SaveFolder = _downloadDirectory!,
            SaveFilename = "9"
        }, null);
        Assert.AreEqual(DownloadStatus.Queued, download.Status);
        Assert.AreEqual(Path.Combine(_downloadDirectory!, "9.flac"), download.FilePath);
        download.Completed += (_, _) => done = true;
        download.Start(_ytdlpExecutableService!.ExecutablePath ?? "yt-dlp", _downloaderOptions!);
        Assert.AreEqual(DownloadStatus.Running, download.Status);
        while (!done)
        {
            await Task.Delay(500);
        }
        Assert.AreEqual(DownloadStatus.Success, download.Status, download.Log);
        Assert.IsFalse(string.IsNullOrEmpty(download.Log));
        Assert.IsTrue(File.Exists(Path.Combine(_downloadDirectory!, "9.flac")));
    }

    [TestMethod]
    public async Task Case010_YouTube_LostSky_WAV()
    {
        var done = false;
        using var download = new Download(new DownloadOptions(new Uri("https://www.youtube.com/watch?v=L7kF4MXXCoA"))
        {
            FileType = MediaFileType.WAV,
            VideoFormat = Format.NoneVideo,
            AudioFormat = Format.BestAudio,
            SaveFolder = _downloadDirectory!,
            SaveFilename = "10"
        }, null);
        Assert.AreEqual(DownloadStatus.Queued, download.Status);
        Assert.AreEqual(Path.Combine(_downloadDirectory!, "10.wav"), download.FilePath);
        download.Completed += (_, _) => done = true;
        download.Start(_ytdlpExecutableService!.ExecutablePath ?? "yt-dlp", _downloaderOptions!);
        Assert.AreEqual(DownloadStatus.Running, download.Status);
        while (!done)
        {
            await Task.Delay(500);
        }
        Assert.AreEqual(DownloadStatus.Success, download.Status, download.Log);
        Assert.IsFalse(string.IsNullOrEmpty(download.Log));
        Assert.IsTrue(File.Exists(Path.Combine(_downloadDirectory!, "10.wav")));
    }

    [TestMethod]
    public async Task Case011_YouTube_LostSky_OGG()
    {
        var done = false;
        using var download = new Download(new DownloadOptions(new Uri("https://www.youtube.com/watch?v=L7kF4MXXCoA"))
        {
            FileType = MediaFileType.OGG,
            VideoFormat = Format.NoneVideo,
            AudioFormat = Format.BestAudio,
            SaveFolder = _downloadDirectory!,
            SaveFilename = "11"
        }, null);
        Assert.AreEqual(DownloadStatus.Queued, download.Status);
        Assert.AreEqual(Path.Combine(_downloadDirectory!, "11.ogg"), download.FilePath);
        download.Completed += (_, _) => done = true;
        download.Start(_ytdlpExecutableService!.ExecutablePath ?? "yt-dlp", _downloaderOptions!);
        Assert.AreEqual(DownloadStatus.Running, download.Status);
        while (!done)
        {
            await Task.Delay(500);
        }
        Assert.AreEqual(DownloadStatus.Success, download.Status, download.Log);
        Assert.IsFalse(string.IsNullOrEmpty(download.Log));
        Assert.IsTrue(File.Exists(Path.Combine(_downloadDirectory!, "11.ogg")));
    }

    [TestMethod]
    public async Task Case012_SoundCloud_Dominator_MP3()
    {
        var done = false;
        using var download = new Download(new DownloadOptions(new Uri("https://soundcloud.com/rlgrime/dominator"))
        {
            FileType = MediaFileType.MP3,
            VideoFormat = Format.NoneVideo,
            AudioFormat = Format.BestAudio,
            SaveFolder = _downloadDirectory!,
            SaveFilename = "12"
        }, null);
        Assert.AreEqual(DownloadStatus.Queued, download.Status);
        Assert.AreEqual(Path.Combine(_downloadDirectory!, "12.mp3"), download.FilePath);
        download.Completed += (_, _) => done = true;
        download.Start(_ytdlpExecutableService!.ExecutablePath ?? "yt-dlp", _downloaderOptions!);
        Assert.AreEqual(DownloadStatus.Running, download.Status);
        while (!done)
        {
            await Task.Delay(500);
        }
        Assert.AreEqual(DownloadStatus.Success, download.Status, download.Log);
        Assert.IsFalse(string.IsNullOrEmpty(download.Log));
        Assert.IsTrue(File.Exists(Path.Combine(_downloadDirectory!, "12.mp3")));
    }

    [TestMethod]
    public async Task Case013_YouTube_LostSky_Stop()
    {
        var done = false;
        using var download = new Download(new DownloadOptions(new Uri("https://www.youtube.com/watch?v=L7kF4MXXCoA"))
        {
            FileType = MediaFileType.MP4,
            VideoFormat = Format.BestVideo,
            AudioFormat = Format.BestAudio,
            SaveFolder = _downloadDirectory!,
            SaveFilename = "13"
        }, null);
        Assert.AreEqual(DownloadStatus.Queued, download.Status);
        Assert.AreEqual(Path.Combine(_downloadDirectory!, "13.mp4"), download.FilePath);
        download.Completed += (_, _) => done = true;
        download.Start(_ytdlpExecutableService!.ExecutablePath ?? "yt-dlp", _downloaderOptions!);
        Assert.AreEqual(DownloadStatus.Running, download.Status);
        var iterations = 0;
        while (!done)
        {
            if (iterations == 5)
            {
                download.Stop();
            }
            await Task.Delay(500);
            iterations++;
        }
        Assert.AreEqual(DownloadStatus.Stopped, download.Status, download.Log);
        Assert.IsFalse(string.IsNullOrEmpty(download.Log));
    }

    [TestMethod]
    public async Task Case014_YouTube_SkyHigh_Pause()
    {
        var done = false;
        using var download = new Download(new DownloadOptions(new Uri("https://www.youtube.com/watch?v=TW9d8vYrVFQ"))
        {
            FileType = MediaFileType.MP4,
            VideoFormat = Format.BestVideo,
            AudioFormat = Format.BestAudio,
            SaveFolder = _downloadDirectory!,
            SaveFilename = "14"
        }, null);
        Assert.AreEqual(DownloadStatus.Queued, download.Status);
        Assert.AreEqual(Path.Combine(_downloadDirectory!, "14.mp4"), download.FilePath);
        download.Completed += (_, _) => done = true;
        download.Start(_ytdlpExecutableService!.ExecutablePath ?? "yt-dlp", _downloaderOptions!);
        Assert.AreEqual(DownloadStatus.Running, download.Status);
        var iterations = 0;
        while (!done)
        {
            if (iterations == 4)
            {
                download.Pause();
            }
            if (iterations == 8)
            {
                download.Resume();
            }
            await Task.Delay(500);
            iterations++;
        }
        Assert.AreEqual(DownloadStatus.Success, download.Status, download.Log);
        Assert.IsFalse(string.IsNullOrEmpty(download.Log));
        Assert.IsTrue(File.Exists(Path.Combine(_downloadDirectory!, "14.mp4")));
    }

    [TestMethod]
    public async Task Case015_YouTube_LostSky_Playlist()
    {
        var done = false;
        using var download = new Download(new DownloadOptions(new Uri("https://www.youtube.com/watch?v=L7kF4MXXCoA"))
        {
            FileType = MediaFileType.MP4,
            VideoResolution = new VideoResolution(1920, 1080),
            AudioBitrate = double.MaxValue,
            SaveFolder = _downloadDirectory!,
            SaveFilename = "15"
        }, null);
        Assert.AreEqual(DownloadStatus.Queued, download.Status);
        Assert.AreEqual(Path.Combine(_downloadDirectory!, "15.mp4"), download.FilePath);
        download.Completed += (_, _) => done = true;
        download.Start(_ytdlpExecutableService!.ExecutablePath ?? "yt-dlp", _downloaderOptions!);
        Assert.AreEqual(DownloadStatus.Running, download.Status);
        while (!done)
        {
            await Task.Delay(500);
        }
        Assert.AreEqual(DownloadStatus.Success, download.Status, download.Log);
        Assert.IsFalse(string.IsNullOrEmpty(download.Log));
        Assert.IsTrue(File.Exists(Path.Combine(_downloadDirectory!, "15.mp4")));
    }

    [TestMethod]
    public async Task Case016_SoundCloud_Dominator_Playlist()
    {
        var done = false;
        using var download = new Download(new DownloadOptions(new Uri("https://soundcloud.com/rlgrime/dominator"))
        {
            FileType = MediaFileType.MP3,
            VideoResolution = VideoResolution.Best,
            AudioBitrate = double.MaxValue,
            SaveFolder = _downloadDirectory!,
            SaveFilename = "16"
        }, null);
        Assert.AreEqual(DownloadStatus.Queued, download.Status);
        Assert.AreEqual(Path.Combine(_downloadDirectory!, "16.mp3"), download.FilePath);
        download.Completed += (_, _) => done = true;
        download.Start(_ytdlpExecutableService!.ExecutablePath ?? "yt-dlp", _downloaderOptions!);
        Assert.AreEqual(DownloadStatus.Running, download.Status);
        while (!done)
        {
            await Task.Delay(500);
        }
        Assert.AreEqual(DownloadStatus.Success, download.Status, download.Log);
        Assert.IsFalse(string.IsNullOrEmpty(download.Log));
        Assert.IsTrue(File.Exists(Path.Combine(_downloadDirectory!, "16.mp3")));
    }
}
