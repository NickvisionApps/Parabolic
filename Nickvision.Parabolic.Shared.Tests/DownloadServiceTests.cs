using Microsoft.VisualStudio.TestTools.UnitTesting;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Globalization;
using Nickvision.Parabolic.Shared.Events;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.IO;
using System.Net.Http;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Tests;

[TestClass]
public class DownloadServiceTests
{
    private static string? _downloadDirectory;
    private static HttpClient? _httpClient;
    private static IJsonFileService? _jsonFileService;
    private static ITranslationService? _translationService;
    private static IYtdlpExecutableService? _ytdlpExecutableService;
    private static IHistoryService? _historyService;
    private static IRecoveryService? _recoveryService;
    private static IFileMetadataService? _fileMetadataService;
    private static IDownloadService? _downloadService;
    private static DownloadProgressChangedEventArgs? _lastProgressChangedEventArgs;
    private static DownloadCompletedEventArgs? _downloadCompletedEventArgs;

    [ClassInitialize]
    public static void ClassInitialize(TestContext context)
    {
        var appInfo = new AppInfo("org.nickvision.tubeconverter.downloads.tests", "Nickvision Parabolic Downloads Tests", "Parabolic Downloads Tests")
        {
            Version = new AppVersion("2026.2.3")
        };
        _downloadDirectory = Path.Combine(UserDirectories.Cache, "Nickvision Parabolic Downloads Tests");
        _httpClient = new HttpClient();
        _jsonFileService = new JsonFileService(appInfo);
        _translationService = new GettextTranslationService(appInfo);
        _ytdlpExecutableService = new YtdlpExecutableService(_jsonFileService, _httpClient);
        _historyService = new HistoryService(appInfo);
        _recoveryService = new RecoveryService(appInfo);
        _fileMetadataService = new FileMetadataService(_translationService, null);
        _downloadService = new DownloadService(_jsonFileService, _translationService, _ytdlpExecutableService, _historyService, _recoveryService, _fileMetadataService);
        _lastProgressChangedEventArgs = null;
        _downloadCompletedEventArgs = null;
        _downloadService.DownloadProgressChanged += Download_ProgressChanged;
        _downloadService.DownloadCompleted += Download_Completed;
        if (Directory.Exists(_downloadDirectory))
        {
            Directory.Delete(_downloadDirectory, true);
        }
        Directory.CreateDirectory(_downloadDirectory);
    }

    [ClassCleanup]
    public static void ClassCleanup()
    {
        _downloadService?.DownloadProgressChanged -= Download_ProgressChanged;
        _downloadService?.DownloadCompleted -= Download_Completed;
        _httpClient?.Dispose();
        (_historyService as IDisposable)?.Dispose();
        (_recoveryService as IDisposable)?.Dispose();
        (_downloadService as IDisposable)?.Dispose();
        Directory.Delete(Path.Combine(UserDirectories.Config, "Nickvision Parabolic Downloads Tests"), true);
        if (Directory.Exists(_downloadDirectory))
        {
            Directory.Delete(_downloadDirectory, true);
        }
    }

    [TestMethod]
    public void Case001_InitalizeCheck() => Assert.IsNotNull(_downloadService);

    [TestMethod]
    public async Task Case002_AddDownload()
    {
        DownloadAddedEventArgs? args = null;
        var options = new DownloadOptions(new Uri("https://www.youtube.com/watch?v=L7kF4MXXCoA"))
        {
            SaveFilename = "1",
            SaveFolder = _downloadDirectory!,
            FileType = MediaFileType.MP4,
            VideoFormat = Format.BestVideo,
            AudioFormat = Format.BestAudio
        };
        _downloadService!.DownloadAdded += (sender, e) => args = e;
        await _downloadService.AddAsync(options, false);
        Assert.IsNotNull(args);
        Assert.AreEqual(0, args.Id);
        Assert.AreEqual(DownloadStatus.Running, args.Status);
        Assert.HasCount(1, await _historyService!.GetAllAsync());
        Assert.HasCount(1, await _recoveryService!.GetAllAsync());
        Assert.AreEqual(1, _downloadService.DownloadingCount);
        Assert.AreEqual(0, _downloadService.QueuedCount);
        Assert.AreEqual(0, _downloadService.CompletedCount);
    }

    [TestMethod]
    public async Task Case003_DownloadProgress()
    {
        await Task.Delay(1000);
        Assert.IsNotNull(_lastProgressChangedEventArgs);
        Assert.IsFalse(_lastProgressChangedEventArgs.LogChunk.IsEmpty);
    }

    [TestMethod]
    public async Task Case004_DownloadCompletion()
    {
        Assert.IsNotNull(_lastProgressChangedEventArgs);
        Assert.IsNull(_downloadCompletedEventArgs);
        while (_downloadCompletedEventArgs is null)
        {
            await Task.Delay(3000);
        }
        Assert.IsNotNull(_downloadCompletedEventArgs);
        Assert.AreEqual(0, _downloadCompletedEventArgs.Id);
        Assert.AreEqual(DownloadStatus.Success, _downloadCompletedEventArgs.Status);
        Assert.IsTrue(File.Exists(_downloadCompletedEventArgs.Path));
        Assert.HasCount(1, await _historyService!.GetAllAsync());
        Assert.HasCount(0, await _recoveryService!.GetAllAsync());
        Assert.AreEqual(0, _downloadService!.DownloadingCount);
        Assert.AreEqual(0, _downloadService.QueuedCount);
        Assert.AreEqual(1, _downloadService.CompletedCount);
    }

    [TestMethod]
    public void Case005_ClearCompletedDownloads()
    {
        Assert.HasCount(1, _downloadService!.ClearCompleted());
        Assert.AreEqual(0, _downloadService!.DownloadingCount);
        Assert.AreEqual(0, _downloadService.QueuedCount);
        Assert.AreEqual(0, _downloadService.CompletedCount);
    }

    private static void Download_ProgressChanged(object? sender, DownloadProgressChangedEventArgs e) => _lastProgressChangedEventArgs = e;

    private static void Download_Completed(object? sender, DownloadCompletedEventArgs e) => _downloadCompletedEventArgs = e;
}
