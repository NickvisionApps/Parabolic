using Nickvision.Desktop;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Network;
using System;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public interface IYtdlpExecutableService : IService
{
    AppVersion BundledVersion { get; }
    string? ExecutablePath { get; }

    Task<bool> DownloadUpdateAsync(AppVersion version, IProgress<DownloadProgress>? progress = null);
    Task<AppVersion?> GetExecutableVersionAsync();
    Task<AppVersion?> GetLatestPreviewVersionAsync();
    Task<AppVersion?> GetLatestStableVersionAsync();
}
