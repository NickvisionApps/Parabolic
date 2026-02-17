using Microsoft.VisualStudio.TestTools.UnitTesting;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Parabolic.Shared.Services;
using System.IO;
using System.Net.Http;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Tests;

[TestClass]
public sealed class YtdlpExecutableServiceTests
{
    private static HttpClient? _httpClient;
    private static IJsonFileService? _jsonFileService;
    private static IYtdlpExecutableService? _ytdlpExecutableService;

    [ClassInitialize]
    public static void ClassInitialize(TestContext context)
    {
        var appInfo = new AppInfo("org.nickvision.tubeconverter.ytdlp.tests", "Nickvision Parabolic Ytdlp Tests", "Parabolic Ytdlp Tests")
        {
            Version = new AppVersion("2026.2.4")
        };
        _httpClient = new HttpClient();
        _jsonFileService = new JsonFileService(appInfo);
        _ytdlpExecutableService = new YtdlpExecutableService(_jsonFileService, _httpClient);
    }

    [ClassCleanup]
    public static void ClassCleanup()
    {
        _httpClient?.Dispose();
        Directory.Delete(Path.Combine(UserDirectories.Config, "Nickvision Parabolic Ytdlp Tests"), true);
    }

    [TestMethod]
    public void Case001_InitalizeCheck() => Assert.IsNotNull(_ytdlpExecutableService);

    [TestMethod]
    public void Case002_BundledVersionCheck() => Assert.AreEqual(new AppVersion("2026.02.04"), _ytdlpExecutableService!.BundledVersion);

    [TestMethod]
    public void Case003_ExecutablePathCheck() => File.Exists(_ytdlpExecutableService!.ExecutablePath);

    [TestMethod]
    public async Task Case004_ExecutableVersionCheck()
    {
        var version = await _ytdlpExecutableService!.GetExecutableVersionAsync();
        Assert.IsNotNull(version);
        Assert.IsGreaterThanOrEqualTo(_ytdlpExecutableService.BundledVersion, version);
    }

    [TestMethod]
    public async Task Case005_DownloadLatestStableVersion()
    {
#if OS_WINDOWS
        var downloadedYtdlp = Path.Combine(UserDirectories.LocalData, "yt-dlp.exe");
#else
        var downloadedYtdlp = Path.Combine(UserDirectories.LocalData, "yt-dlp");
#endif
        var stable = await _ytdlpExecutableService!.GetLatestStableVersionAsync();
        if (File.Exists(downloadedYtdlp))
        {
            File.Delete(downloadedYtdlp);
        }
        Assert.IsNotNull(stable);
        Assert.AreEqual(new AppVersion("2026.02.04"), stable);
        Assert.IsTrue(await _ytdlpExecutableService.DownloadUpdateAsync(stable));
        Assert.IsTrue(File.Exists(downloadedYtdlp));
        File.Delete(downloadedYtdlp);
    }

    [TestMethod]
    public async Task Case006_DownloadLatestPreviewVersion()
    {
#if OS_WINDOWS
        var downloadedYtdlp = Path.Combine(UserDirectories.LocalData, "yt-dlp.exe");
#else
        var downloadedYtdlp = Path.Combine(UserDirectories.LocalData, "yt-dlp");
#endif
        var preview = await _ytdlpExecutableService!.GetLatestPreviewVersionAsync();
        if (File.Exists(downloadedYtdlp))
        {
            File.Delete(downloadedYtdlp);
        }
        Assert.IsNotNull(preview);
        Assert.IsGreaterThan(new AppVersion("2026.2.4"), preview);
        Assert.IsTrue(await _ytdlpExecutableService.DownloadUpdateAsync(preview));
        Assert.IsTrue(File.Exists(downloadedYtdlp));
        File.Delete(downloadedYtdlp);
    }
}
