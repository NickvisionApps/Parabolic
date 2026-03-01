using Microsoft.Extensions.Logging;
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

public class DenoExecutableService : IDenoExecutableService
{
    private static readonly AppVersion _bundledVersion;
    private static readonly string _assetName;

    private readonly ILogger<DenoExecutableService> _logger;
    private readonly IJsonFileService _jsonFileService;
    private readonly IUpdaterService _stableUpdaterService;
    private AppVersion? _latestStableVersion;

    public AppVersion BundledVersion => _bundledVersion;

    static DenoExecutableService()
    {
        if (OperatingSystem.IsLinux())
        {
            _bundledVersion = new AppVersion(Desktop.System.Environment.DeploymentMode == DeploymentMode.Local ? "0.0.0" : "2.7.1");
        }
        else
        {
            _bundledVersion = new AppVersion("2.7.1");
        }
        if (OperatingSystem.IsWindows())
        {
            _assetName = RuntimeInformation.OSArchitecture == Architecture.Arm64 ? "deno-aarch64-pc-windows-msvc.zip" : "deno-x86_64-pc-windows-msvc.zip";
        }
        else if (OperatingSystem.IsLinux())
        {
            _assetName = RuntimeInformation.OSArchitecture == Architecture.Arm64 ? "deno-aarch64-unknown-linux-gnu.zip" : "deno-x86_64-unknown-linux-gnu.zip";
        }
        else if (OperatingSystem.IsMacOS())
        {
            _assetName = RuntimeInformation.OSArchitecture == Architecture.Arm64 ? "deno-aarch64-apple-darwin.zip" : "deno-x86_64-apple-darwin.zip";
        }
        else
        {
            _assetName = string.Empty;
        }
    }

    public DenoExecutableService(ILogger<DenoExecutableService> logger, IJsonFileService jsonFileService, HttpClient httpClient)
    {
        _logger = logger;
        _jsonFileService = jsonFileService;
        _stableUpdaterService = new UpdaterService("denoland", "deno", httpClient);
        _latestStableVersion = null;
    }

    public string? ExecutablePath
    {
        get
        {
            if (!string.IsNullOrEmpty(field))
            {
                return field;
            }
            _logger.LogInformation("Searching for deno executable...");
            var config = _jsonFileService.Load<Configuration>(Configuration.Key);
            if (config.InstalledDenoAppVersion > _bundledVersion)
            {
                var local = Desktop.System.Environment.FindDependency("deno", DependencySearchOption.Local);
                if (!string.IsNullOrEmpty(local) && File.Exists(local))
                {
                    _logger.LogInformation($"Found updated deno executable: {local}");
                    field = local;
                    return field;
                }
                else
                {
                    config.InstalledDenoAppVersion = new AppVersion("0.0.0");
                    _jsonFileService.Save(config, Configuration.Key);
                }
            }
            field = Desktop.System.Environment.FindDependency("deno", DependencySearchOption.Global);
            _logger.LogInformation($"Found bundled deno executable: {field}");
            return field;
        }
    }

    public async Task<bool> DownloadUpdateAsync(AppVersion version, IProgress<DownloadProgress>? progress = null)
    {
        var path = OperatingSystem.IsWindows() ? Path.Combine(UserDirectories.LocalData, "deno.exe") : Path.Combine(UserDirectories.LocalData, "deno");
        var res = await _stableUpdaterService.DownloadReleaseAssetAsync(version, path, _assetName, true, progress);
        if (res)
        {
            var config = await _jsonFileService.LoadAsync<Configuration>(Configuration.Key);
            config.InstalledDenoAppVersion = version;
            await _jsonFileService.SaveAsync(config, Configuration.Key);
            if (!OperatingSystem.IsWindows())
            {
                using var process = new Process()
                {
                    StartInfo = new ProcessStartInfo("chmod", ["0755", path])
                    {
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
        if (process.ExitCode == 0 && AppVersion.TryParse(output.Substring(5, output.IndexOf('(') - 5).Trim(), out var version))
        {
            return version;
        }
        return null;
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
