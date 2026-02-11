using Microsoft.VisualStudio.TestTools.UnitTesting;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Keyring;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Tests;

[TestClass]
public class RecoveryServiceTests
{
    private static string? _recoveryDirectory;
    private static IRecoveryService? _recoveryService;

    [ClassInitialize]
    public static void ClassInitialize(TestContext context)
    {
        var appInfo = new AppInfo("org.nickvision.tubeconverter.recovery.tests", "Nickvision Parabolic Recovery Tests", "Parabolic Recovery Tests")
        {
            Version = new AppVersion("2026.2.3")
        };
        _recoveryDirectory = Path.Combine(UserDirectories.Config, "Nickvision Parabolic Recovery Tests");
        if (Directory.Exists(_recoveryDirectory))
        {
            Directory.Delete(_recoveryDirectory, true);
        }
        _recoveryService = new RecoveryService(appInfo);
    }

    [ClassCleanup]
    public static void ClassCleanup()
    {
        (_recoveryService as IDisposable)?.Dispose();
        Directory.Delete(_recoveryDirectory!, true);
    }

    [TestMethod]
    public void Case001_InitalizeCheck() => Assert.IsNotNull(_recoveryDirectory);

    [TestMethod]
    public async Task Case002_EnsureEmpty() => Assert.HasCount(0, await _recoveryService!.GetAllAsync());

    [TestMethod]
    public async Task Case003_AddOne() => Assert.IsTrue(await _recoveryService!.AddAsync(new RecoverableDownload(1, new DownloadOptions(new Uri("https://example.com/video"))
    {
        Credential = new Credential("Example", "abc", "123"),
        FileType = MediaFileType.MP4,
        PlaylistPosition = 2,
        VideoFormat = Format.WorstVideo,
        AudioFormat = Format.WorstAudio,
        SaveFolder = "/home/user/Videos",
        SaveFilename = "example_video.mp4",
        SubtitleLanguages = [new SubtitleLanguage("en", false), new SubtitleLanguage("es", true)],
        SplitChapters = false,
        ExportDescription = true,
        PostProcessorArgument = new PostProcessorArgument("Test", PostProcessor.None, Executable.FFmpeg, "-h 1"),
        TimeFrame = TimeFrame.Parse("00:00:00", "00:01:00", TimeSpan.FromMinutes(1))
    })));

    [TestMethod]
    public async Task Case004_EnsureOne()
    {
        var all = await _recoveryService!.GetAllAsync();
        Assert.HasCount(1, all);
        var download = all[0];
        Assert.AreEqual(1, download.Id);
        Assert.AreEqual(new Uri("https://example.com/video"), download.Options.Url);
        Assert.IsTrue(download.CredentialRequired);
        Assert.IsNull(download.Options.Credential);
        Assert.AreEqual(MediaFileType.MP4, download.Options.FileType);
        Assert.AreEqual(2, download.Options.PlaylistPosition);
        Assert.AreEqual(Format.WorstVideo, download.Options.VideoFormat);
        Assert.AreEqual(Format.WorstAudio, download.Options.AudioFormat);
        Assert.AreEqual("/home/user/Videos", download.Options.SaveFolder);
        Assert.AreEqual("example_video.mp4", download.Options.SaveFilename);
        Assert.HasCount(2, download.Options.SubtitleLanguages);
        Assert.IsFalse(download.Options.SplitChapters);
        Assert.IsTrue(download.Options.ExportDescription);
        Assert.AreEqual(new PostProcessorArgument("Test", PostProcessor.None, Executable.FFmpeg, "-h 1"), download.Options.PostProcessorArgument);
        Assert.AreEqual(TimeFrame.Parse("00:00:00", "00:01:00", TimeSpan.FromMinutes(1)), download.Options.TimeFrame);
    }

    [TestMethod]
    public async Task Case005_RemoveOne()
    {
        var allBefore = await _recoveryService!.GetAllAsync();
        Assert.HasCount(1, allBefore);
        await _recoveryService.RemoveAsync(allBefore[0].Id);
        var allAfter = await _recoveryService.GetAllAsync();
        Assert.HasCount(0, allAfter);
    }

    [TestMethod]
    public async Task Case006_AddMany()
    {
        var many = new List<RecoverableDownload>(1000);
        for (var i = 0; i < 1000; i++)
        {
            many.Add(new RecoverableDownload(i, new DownloadOptions(new Uri($"https://example.com/video{i}"))
            {
                FileType = MediaFileType.MP3,
                SaveFolder = $"/home/user/Music",
                SaveFilename = $"example_audio_{i}.mp3",
                ExportDescription = false
            }));
        }
        Assert.IsTrue(await _recoveryService!.AddAsync(many));
    }

    [TestMethod]
    public async Task Case007_EnsureMany() => Assert.HasCount(1000, await _recoveryService!.GetAllAsync());

    [TestMethod]
    public async Task Case008_ClearAll()
    {
        Assert.IsTrue(await _recoveryService!.ClearAsync());
        Assert.HasCount(0, await _recoveryService.GetAllAsync());
    }
}
