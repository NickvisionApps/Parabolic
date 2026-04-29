using Microsoft.Extensions.Logging;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.System;
using System;
using System.Diagnostics;
using System.Net.Http;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class DenoExecutableService : DependencyExecutableService, IDenoExecutableService
{
    private static readonly AppVersion DenoBundledVersion;
    private static readonly string DenoAssetName;

    static DenoExecutableService()
    {
        if (OperatingSystem.IsLinux())
        {
            DenoBundledVersion = new AppVersion(Desktop.System.Environment.DeploymentMode == DeploymentMode.Local ? "0.0.0" : "2.7.14");
        }
        else
        {
            DenoBundledVersion = new AppVersion("2.7.14");
        }
        if (OperatingSystem.IsWindows())
        {
            DenoAssetName = RuntimeInformation.OSArchitecture == Architecture.Arm64 ? "deno-aarch64-pc-windows-msvc.zip" : "deno-x86_64-pc-windows-msvc.zip";
        }
        else if (OperatingSystem.IsLinux())
        {
            DenoAssetName = RuntimeInformation.OSArchitecture == Architecture.Arm64 ? "deno-aarch64-unknown-linux-gnu.zip" : "deno-x86_64-unknown-linux-gnu.zip";
        }
        else if (OperatingSystem.IsMacOS())
        {
            DenoAssetName = RuntimeInformation.OSArchitecture == Architecture.Arm64 ? "deno-aarch64-apple-darwin.zip" : "deno-x86_64-apple-darwin.zip";
        }
        else
        {
            DenoAssetName = "deno";
        }
    }

    public DenoExecutableService(ILogger<DenoExecutableService> logger, ILogger<UpdaterService> updaterLogger, IConfigurationService configurationService, IHttpClientFactory httpClientFactory) : base(logger, "deno", DenoBundledVersion, DenoAssetName, configurationService, new UpdaterService(updaterLogger, "denoland", "deno", httpClientFactory.CreateClient()))
    {

    }

    public override async Task<AppVersion?> GetExecutableVersionAsync(string versionArgument = "--version")
    {
        using var process = new Process()
        {
            StartInfo = new ProcessStartInfo(ExecutablePath, versionArgument)
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
}
