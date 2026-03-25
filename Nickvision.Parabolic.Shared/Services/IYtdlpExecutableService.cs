using Nickvision.Desktop.Application;
using Nickvision.Desktop.Keyring;
using Nickvision.Desktop.Network;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Diagnostics;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public interface IYtdlpExecutableService
{
    AppVersion BundledVersion { get; }
    string? ExecutablePath { get; }

    Task<Process> CreateDiscoveryProcessAsync(Uri url, Credential? credential);
    Task<Process> CreateDownloadProcessAsync(DownloadOptions downloadOptions);
    Task<bool> DownloadUpdateAsync(AppVersion version, IProgress<DownloadProgress>? progress = null);
    Task<AppVersion?> GetExecutableVersionAsync();
    Task<AppVersion?> GetLatestPreviewVersionAsync();
    Task<AppVersion?> GetLatestStableVersionAsync();
}
