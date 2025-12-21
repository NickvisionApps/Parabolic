using Microsoft.VisualStudio.TestTools.UnitTesting;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
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
            Version = new AppVersion("2025.12.0-next")
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
        FileType = MediaFileType.MP4,
        SaveFolder = "/home/user/Videos",
        SaveFilename = "example_video.mp4",
        ExportDescription = true
    })));

    [TestMethod]
    public async Task Case004_EnsureOne()
    {
        var all = await _recoveryService!.GetAllAsync();
        Assert.HasCount(1, all);
        var download = all.ElementAt(0);
        Assert.AreEqual(1, download.Id);
        Assert.AreEqual(new Uri("https://example.com/video"), download.Options.Url);
        Assert.AreEqual(MediaFileType.MP4, download.Options.FileType);
        Assert.AreEqual("/home/user/Videos", download.Options.SaveFolder);
        Assert.AreEqual("example_video.mp4", download.Options.SaveFilename);
        Assert.IsTrue(download.Options.ExportDescription);
        Assert.IsFalse(download.CredentialRequired);
    }

    [TestMethod]
    public async Task Case005_RemoveOne()
    {
        var allBefore = await _recoveryService!.GetAllAsync();
        Assert.HasCount(1, allBefore);
        await _recoveryService.RemoveAsync(allBefore.ElementAt(0).Id);
        var allAfter = await _recoveryService.GetAllAsync();
        Assert.HasCount(0, allAfter);
    }

    [TestMethod]
    public async Task Case006_AddMany()
    {
        var many = new List<RecoverableDownload>();
        many.EnsureCapacity(1000);
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
