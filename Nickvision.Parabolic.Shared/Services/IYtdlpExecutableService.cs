using Nickvision.Desktop.Keyring;
using Nickvision.Desktop.System;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Diagnostics;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public interface IYtdlpExecutableService : IDependencyExecutableService
{
    Task<Process> CreateDiscoveryProcessAsync(Uri url, Credential? credential);
    Task<Process> CreateDownloadProcessAsync(DownloadOptions downloadOptions);
}
