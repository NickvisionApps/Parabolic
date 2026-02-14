using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Network;
using Nickvision.Desktop.System;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Diagnostics;
using System.IO;
using System.Net.Http;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class YtdlpExecutableService : IYtdlpExecutableService
{
    private static readonly AppVersion _bundledVersion;
    private static readonly string _assetName;

    private readonly IJsonFileService _jsonFileService;
    private readonly IUpdaterService _stableUpdaterService;
    private readonly IUpdaterService _previewUpdaterService;
    private AppVersion? _latestPreviewVersion;
    private AppVersion? _latestStableVersion;

    public AppVersion BundledVersion => _bundledVersion;

    static YtdlpExecutableService()
    {
        if (OperatingSystem.IsLinux())
        {
            _bundledVersion = new AppVersion(Desktop.System.Environment.DeploymentMode == DeploymentMode.Local ? "0.0.0" : "2026.02.04");
        }
        else
        {
            _bundledVersion = new AppVersion("2026.02.04");
        }
        if (OperatingSystem.IsWindows())
        {
            _assetName = RuntimeInformation.OSArchitecture == Architecture.Arm64 ? "yt-dlp_arm64.exe" : "yt-dlp.exe";
        }
        else if (OperatingSystem.IsLinux())
        {
            _assetName = RuntimeInformation.OSArchitecture == Architecture.Arm64 ? "yt-dlp_linux_aarch64" : "yt-dlp_linux";
        }
        else if (OperatingSystem.IsMacOS())
        {
            _assetName = "yt-dlp_macos";
        }
        else
        {
            _assetName = "yt-dlp";
        }
    }

    public YtdlpExecutableService(IJsonFileService jsonFileService, HttpClient httpClient)
    {
        _jsonFileService = jsonFileService;
        _stableUpdaterService = new GitHubUpdaterService("yt-dlp", "yt-dlp", httpClient);
        _previewUpdaterService = new GitHubUpdaterService("yt-dlp", "yt-dlp-nightly-builds", httpClient);
        _latestPreviewVersion = null;
        _latestStableVersion = null;
    }

    public string? ExecutablePath
    {
        get
        {
            var config = _jsonFileService.Load<Configuration>(Configuration.Key);
            if (config.InstalledYtdlpAppVersion > _bundledVersion)
            {
                var local = Desktop.System.Environment.FindDependency("yt-dlp", DependencySearchOption.Local);
                if (!string.IsNullOrEmpty(local) && File.Exists(local))
                {
                    return local;
                }
                else
                {
                    config.InstalledYtdlpAppVersion = new AppVersion("0.0.0");
                    _jsonFileService.Save(config, Configuration.Key);
                }
            }
            return Desktop.System.Environment.FindDependency("yt-dlp", DependencySearchOption.Global);
        }
    }

    public async Task<bool> DownloadUpdateAsync(AppVersion version, IProgress<DownloadProgress>? progress = null)
    {
        var path = OperatingSystem.IsWindows() ? Path.Combine(UserDirectories.LocalData, "yt-dlp.exe") : Path.Combine(UserDirectories.LocalData, "yt-dlp");
        var res = version.BaseVersion.Revision > 0 ? await _previewUpdaterService.DownloadReleaseAssetAsync(version, path, _assetName, true, progress) : await _stableUpdaterService.DownloadReleaseAssetAsync(version, path, _assetName, true, progress);
        if (res)
        {
            var config = await _jsonFileService.LoadAsync<Configuration>(Configuration.Key);
            config.InstalledYtdlpAppVersion = version;
            await _jsonFileService.SaveAsync(config, Configuration.Key);
            if (!OperatingSystem.IsWindows())
            {
                using var process = new Process()
                {
                    StartInfo = new ProcessStartInfo
                    {
                        FileName = "chmod",
                        ArgumentList = [
                          "0755",
                          path
                        ],
                        UseShellExecute = false,
                        CreateNoWindow = true,
                    }
                };
                process.Start();
                await process.WaitForExitAsync();
                return process.ExitCode == 0;
            }
        }
        return res;
    }

    public async Task<AppVersion?> GetExecutableVersionAsync()
    {
        using var process = new Process()
        {
            StartInfo = new ProcessStartInfo(ExecutablePath ?? string.Empty, "--version")
            {
                RedirectStandardOutput = true,
                UseShellExecute = false,
                CreateNoWindow = true,
            }
        };
        process.Start();
        var outputTask = process.StandardOutput.ReadToEndAsync();
        await process.WaitForExitAsync();
        var output = await outputTask;
        if (process.ExitCode == 0 && AppVersion.TryParse(output.Trim(), out var version))
        {
            return version;
        }
        return null;
    }

    public async Task<AppVersion?> GetLatestPreviewVersionAsync()
    {
        if (_latestPreviewVersion is null)
        {
            var _ = ExecutablePath;
            _latestPreviewVersion = await _previewUpdaterService.GetLatestStableVersionAsync();
        }
        return _latestPreviewVersion;
    }

    public async Task<AppVersion?> GetLatestStableVersionAsync()
    {
        if (_latestStableVersion is null)
        {
            var _ = ExecutablePath;
            return await _stableUpdaterService.GetLatestStableVersionAsync();
        }
        return _latestStableVersion;
    }
}
