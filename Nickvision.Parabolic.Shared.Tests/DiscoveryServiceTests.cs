using Microsoft.VisualStudio.TestTools.UnitTesting;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Globalization;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.IO;
using System.Net.Http;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Tests;

[TestClass]
public sealed class DiscoveryServiceTests
{
    private static string? _batchTestFilePath;
    private static HttpClient? _httpClient;
    private static IJsonFileService? _jsonFileService;
    private static ITranslationService? _translationService;
    private static IYtdlpExecutableService? _ytdlpExecutableService;
    private static IDiscoveryService? _discoveryService;

    [ClassInitialize]
    public static void ClassInitialize(TestContext context)
    {
        var appInfo = new AppInfo("org.nickvision.tubeconverter.discovery.tests", "Nickvision Parabolic Discovery Tests", "Parabolic Discovery Tests")
        {
            Version = new AppVersion("2026.1.0-next")
        };
        _batchTestFilePath = Path.Combine(UserDirectories.Cache, "batch-test.txt");
        _httpClient = new HttpClient();
        _jsonFileService = new JsonFileService(appInfo);
        _translationService = new GettextTranslationService(appInfo);
        _ytdlpExecutableService = new YtdlpExecutableService(_jsonFileService, _httpClient);
        _discoveryService = new DiscoveryService(_jsonFileService, _translationService, _ytdlpExecutableService);
    }

    [ClassCleanup]
    public static void ClassCleanup()
    {
        _httpClient?.Dispose();
        if (File.Exists(_batchTestFilePath))
        {
            File.Delete(_batchTestFilePath);
        }
        Directory.Delete(Path.Combine(UserDirectories.Config, "Nickvision Parabolic Discovery Tests"), true);
    }

    [TestMethod]
    public void Case001_InitalizeCheck() => Assert.IsNotNull(_discoveryService);

    [TestMethod]
    public async Task Case002_YouTube_BatchFile()
    {
        if (File.Exists(_batchTestFilePath))
        {
            File.Delete(_batchTestFilePath);
        }
        File.WriteAllText(_batchTestFilePath!, """
        https://www.youtube.com/watch?v=83RUhxsfLWs
        https://www.youtube.com/watch?v=K4DyBUG242c | ~\Downloads\org.nickvision.tubeconverter.tests
        https://www.youtube.com/watch?v=TW9d8vYrVFQ | ~\Downloads\org.nickvision.tubeconverter.tests | a
        """);
        var result = await _discoveryService!.GetForBatchFileAsync(_batchTestFilePath!);
        Assert.IsNotNull(result);
        Assert.AreEqual(new Uri($"file://{_batchTestFilePath}"), result.Url);
        Assert.AreEqual(Path.GetFileNameWithoutExtension(_batchTestFilePath), result.Title);
        Assert.IsTrue(result.HasSuggestedSaveFolder);
        Assert.HasCount(3, result.Media);
        Assert.IsTrue(result.IsPlaylist);
        var media1 = result.Media[0];
        Assert.AreEqual(new Uri("https://www.youtube.com/watch?v=83RUhxsfLWs"), media1.Url);
        Assert.AreEqual("NEFFEX - Grateful [Copyright Free] No.54 [83RUhxsfLWs]", media1.Title);
        Assert.AreEqual(0, media1.PlaylistPosition);
        Assert.AreEqual(MediaType.Video, media1.Type);
        Assert.AreEqual(TimeFrame.Parse("00:00:00", "00:03:02", TimeSpan.FromSeconds(182))!, media1.TimeFrame);
        Assert.IsGreaterThan(0, media1.Formats.Count);
        Assert.IsGreaterThan(0, media1.Subtitles.Count);
        Assert.IsTrue(string.IsNullOrEmpty(media1.SuggestedSaveFolder));
        var media2 = result.Media[1];
        Assert.AreEqual(new Uri("https://www.youtube.com/watch?v=K4DyBUG242c"), media2.Url);
        Assert.AreEqual("Cartoon, Jéja - On & On (feat. Daniel Levi) | Electronic Pop | NCS - Copyright Free Music [K4DyBUG242c]", media2.Title);
        Assert.AreEqual(1, media2.PlaylistPosition);
        Assert.AreEqual(MediaType.Video, media2.Type);
        Assert.AreEqual(TimeFrame.Parse("00:00:00", "00:03:28", TimeSpan.FromSeconds(208))!, media2.TimeFrame);
        Assert.IsGreaterThan(0, media2.Formats.Count);
        Assert.IsGreaterThan(0, media2.Subtitles.Count);
        Assert.IsFalse(string.IsNullOrEmpty(media2.SuggestedSaveFolder));
        var media3 = result.Media[2];
        Assert.AreEqual(new Uri("https://www.youtube.com/watch?v=TW9d8vYrVFQ"), media3.Url);
        Assert.AreEqual("a [TW9d8vYrVFQ]", media3.Title);
        Assert.AreEqual(2, media3.PlaylistPosition);
        Assert.AreEqual(MediaType.Video, media3.Type);
        Assert.AreEqual(TimeFrame.Parse("00:00:00", "00:03:58", TimeSpan.FromSeconds(238))!, media3.TimeFrame);
        Assert.IsGreaterThan(0, media3.Formats.Count);
        Assert.IsGreaterThan(0, media3.Subtitles.Count);
        Assert.IsFalse(string.IsNullOrEmpty(media3.SuggestedSaveFolder));
        File.Delete(_batchTestFilePath!);
    }

    [TestMethod]
    public async Task Case003_YouTube_LostSky()
    {
        var url = new Uri("https://www.youtube.com/watch?v=L7kF4MXXCoA");
        var result = await _discoveryService!.GetForUrlAsync(url);
        Assert.IsNotNull(result);
        Assert.AreEqual(url, result.Url);
        Assert.AreEqual("Lost Sky - Dreams pt. II (feat. Sara Skinner) | Trap | NCS - Copyright Free Music", result.Title);
        Assert.IsFalse(result.HasSuggestedSaveFolder);
        Assert.HasCount(1, result.Media);
        Assert.IsFalse(result.IsPlaylist);
        var media = result.Media[0];
        Assert.AreEqual(url, media.Url);
        Assert.AreEqual("Lost Sky - Dreams pt. II (feat. Sara Skinner) | Trap | NCS - Copyright Free Music [L7kF4MXXCoA]", media.Title);
        Assert.AreEqual(-1, media.PlaylistPosition);
        Assert.AreEqual(MediaType.Video, media.Type);
        Assert.AreEqual(TimeFrame.Parse("00:00:00", "00:03:35", TimeSpan.FromSeconds(215))!, media.TimeFrame);
        Assert.IsGreaterThan(0, media.Formats.Count);
        Assert.IsGreaterThan(0, media.Subtitles.Count);
        Assert.IsTrue(string.IsNullOrEmpty(media.SuggestedSaveFolder));
    }

    [TestMethod]
    public async Task Case004_YouTube_Playlist()
    {
        var result = await _discoveryService!.GetForUrlAsync(new Uri("https://www.youtube.com/playlist?list=PLXJg25X-OulsVsnvZ7RVtSDW-id9_RzAO"));
        Assert.IsNotNull(result);
        Assert.AreEqual(new Uri("https://www.youtube.com/playlist?list=PLXJg25X-OulsVsnvZ7RVtSDW-id9_RzAO"), result.Url);
        Assert.AreEqual("Top 10 Copyright Free Music Tracks 2025", result.Title);
        Assert.IsFalse(result.HasSuggestedSaveFolder);
        Assert.HasCount(17, result.Media);
        Assert.IsTrue(result.IsPlaylist);
        var media8 = result.Media[7];
        Assert.AreEqual(new Uri("https://www.youtube.com/watch?v=DTUUOewYJkY"), media8.Url);
        Assert.AreEqual("Happy Background Instrumental Royalty Free Music for Summer Videos, Commercials, Adverts, Kids [DTUUOewYJkY]", media8.Title);
        Assert.AreEqual(7, media8.PlaylistPosition);
        Assert.AreEqual(MediaType.Video, media8.Type);
        Assert.AreEqual(TimeFrame.Parse("00:00:00", "00:02:32", TimeSpan.FromSeconds(152))!, media8.TimeFrame);
        Assert.IsGreaterThan(0, media8.Formats.Count);
        Assert.HasCount(0, media8.Subtitles);
        Assert.IsTrue(string.IsNullOrEmpty(media8.SuggestedSaveFolder));
    }

    [TestMethod]
    public async Task Case005_SoundCloud_Control()
    {
        var result = await _discoveryService!.GetForUrlAsync(new Uri("https://soundcloud.com/neffexmusic/control-copyright-free"));
        Assert.IsNotNull(result);
        Assert.AreEqual(new Uri("https://soundcloud.com/neffexmusic/control-copyright-free"), result.Url);
        Assert.AreEqual("Control [Copyright Free]", result.Title);
        Assert.IsFalse(result.HasSuggestedSaveFolder);
        Assert.HasCount(1, result.Media);
        Assert.IsFalse(result.IsPlaylist);
        var media = result.Media[0];
        Assert.AreEqual(new Uri("https://soundcloud.com/neffexmusic/control-copyright-free"), media.Url);
        Assert.AreEqual("Control [Copyright Free] [2175665586]", media.Title);
        Assert.AreEqual(-1, media.PlaylistPosition);
        Assert.AreEqual(MediaType.Audio, media.Type);
        Assert.AreEqual(TimeFrame.Parse("00:00:00", "00:04:11", TimeSpan.FromSeconds(251))!, media.TimeFrame);
        Assert.IsGreaterThan(0, media.Formats.Count);
        Assert.HasCount(0, media.Subtitles);
        Assert.IsTrue(string.IsNullOrEmpty(media.SuggestedSaveFolder));
    }

    [TestMethod]
    public async Task Case006_SoundCloud_Dominator()
    {
        var result = await _discoveryService!.GetForUrlAsync(new Uri("https://soundcloud.com/rlgrime/dominator"));
        Assert.IsNotNull(result);
        Assert.AreEqual(new Uri("https://soundcloud.com/rlgrime/dominator"), result.Url);
        Assert.AreEqual("Dominator", result.Title);
        Assert.IsFalse(result.HasSuggestedSaveFolder);
        Assert.HasCount(1, result.Media);
        Assert.IsFalse(result.IsPlaylist);
        var media = result.Media[0];
        Assert.AreEqual(new Uri("https://soundcloud.com/rlgrime/dominator"), media.Url);
        Assert.AreEqual("Dominator [2189965763]", media.Title);
        Assert.AreEqual(-1, media.PlaylistPosition);
        Assert.AreEqual(MediaType.Audio, media.Type);
        Assert.AreEqual(TimeFrame.Parse("00:00:00", "00:02:53", TimeSpan.FromSeconds(173))!, media.TimeFrame);
        Assert.IsGreaterThan(0, media.Formats.Count);
        Assert.HasCount(0, media.Subtitles);
        Assert.IsTrue(string.IsNullOrEmpty(media.SuggestedSaveFolder));
    }
}
