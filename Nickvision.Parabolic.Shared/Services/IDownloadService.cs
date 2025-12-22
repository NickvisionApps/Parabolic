using Nickvision.Desktop;
using Nickvision.Parabolic.Shared.Events;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public interface IDownloadService : IService
{
    event EventHandler<DownloadAddedEventArgs> DownloadAdded;

    Task AddDownloadAsync(DownloadOptions options, bool excludeFromHistory);
    Task AddDownloadsAsync(IEnumerable<DownloadOptions> options, bool excludeFromHistory);
}
