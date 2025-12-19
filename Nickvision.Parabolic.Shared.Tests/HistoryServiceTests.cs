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
public class HistoryServiceTests
{
    private static string? _historyDirectory;
    private static IHistoryService? _historyService;

    [ClassInitialize]
    public static void ClassInitialize(TestContext context)
    {
        var appInfo = new AppInfo("org.nickvision.tubeconverter.history.tests", "Nickvision Parabolic History Tests", "Parabolic History Tests")
        {
            Version = new AppVersion("2025.12.0-next")
        };
        _historyDirectory = Path.Combine(UserDirectories.Config, "Nickvision Parabolic History Tests");
        if (Directory.Exists(_historyDirectory))
        {
            Directory.Delete(_historyDirectory, true);
        }
        _historyService = new HistoryService(appInfo);
    }

    [ClassCleanup]
    public static void ClassCleanup()
    {
        (_historyService as IDisposable)?.Dispose();
        Directory.Delete(_historyDirectory!, true);
    }

    [TestMethod]
    public void Case001_InitalizeCheck() => Assert.IsNotNull(_historyService);

    [TestMethod]
    public async Task Case002_EnsureEmpty() => Assert.HasCount(0, await _historyService!.GetAllAsync());

    [TestMethod]
    public void Case003_ModifyLength()
    {
        Assert.AreEqual(HistoryLength.OneWeek, _historyService!.Length);
        _historyService.Length = HistoryLength.OneMonth;
        Assert.AreEqual(HistoryLength.OneMonth, _historyService.Length);
    }

    [TestMethod]
    public async Task Case004_AddOne() => Assert.IsTrue(await _historyService!.AddAsync(new HistoricDownload(new Uri("https://www.example.com/video"))
    {
        Title = "Example Video",
        Path = "/path/to/video.mp4"
    }));

    [TestMethod]
    public async Task Case005_EnsureOne()
    {
        var all = await _historyService!.GetAllAsync();
        Assert.HasCount(1, all);
        var one = all.ElementAt(0);
        Assert.AreEqual(new Uri("https://www.example.com/video"), one.Url);
        Assert.AreEqual("Example Video", one.Title);
        Assert.AreEqual("/path/to/video.mp4", one.Path);
    }

    [TestMethod]
    public async Task Case006_UpdateOne()
    {
        var updatedDownload = new HistoricDownload(new Uri("https://www.example.com/video"))
        {
            Title = "Updated Example Video",
            Path = "/new/path/to/video.mp4"
        };
        Assert.IsTrue(await _historyService!.AddAsync(updatedDownload));
        var all = await _historyService.GetAllAsync();
        Assert.HasCount(1, all);
        var one = all.ElementAt(0);
        Assert.AreEqual(new Uri("https://www.example.com/video"), one.Url);
        Assert.AreEqual("Updated Example Video", one.Title);
        Assert.AreEqual("/new/path/to/video.mp4", one.Path);
        updatedDownload.Title = "Final Example Video";
        Assert.IsTrue(await _historyService.UpdateAsync(updatedDownload));
        all = await _historyService.GetAllAsync();
        Assert.HasCount(1, all);
        one = all.ElementAt(0);
        Assert.AreEqual(new Uri("https://www.example.com/video"), one.Url);
        Assert.AreEqual("Final Example Video", one.Title);
        Assert.AreEqual("/new/path/to/video.mp4", one.Path);
    }

    [TestMethod]
    public async Task Case007_RemoveOne()
    {
        Assert.IsTrue(await _historyService!.RemoveAsync(new Uri("https://www.example.com/video")));
        Assert.HasCount(0, await _historyService.GetAllAsync());
    }

    [TestMethod]
    public async Task Case008_AddMany()
    {
        var many = new List<HistoricDownload>();
        many.EnsureCapacity(1000);
        for (var i = 0; i < 1000; i++)
        {
            many.Add(new HistoricDownload(new Uri($"https://www.example.com/video{i}"))
            {
                Title = $"Example Video {i}",
                Path = $"/path/to/video{i}.mp4"
            });
        }
        Assert.IsTrue(await _historyService!.AddAsync(many));
    }

    [TestMethod]
    public async Task Case009_EnsureMany() => Assert.HasCount(1000, await _historyService!.GetAllAsync());

    [TestMethod]
    public async Task Case010_ClearAll()
    {
        Assert.IsTrue(await _historyService!.ClearAsync());
        Assert.HasCount(0, await _historyService.GetAllAsync());
    }
}
