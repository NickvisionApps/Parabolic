using Nickvision.Desktop;
using Nickvision.Desktop.Keyring;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public interface IDiscoveryService : IService
{
    Task<UrlInfo?> GetForBatchFileAsync(string path, Credential? credential = null, CancellationToken cancellationToken = default);
    Task<UrlInfo?> GetForUrlAsync(Uri url, Credential? credential = null, CancellationToken cancellationToken = default);
}
