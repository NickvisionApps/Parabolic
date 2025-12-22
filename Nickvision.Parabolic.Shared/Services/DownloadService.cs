using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Globalization;
using Nickvision.Parabolic.Shared.Events;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Services;

public class DownloadService : IDownloadService
{
    private readonly IJsonFileService _jsonFileService;
    private readonly ITranslationService _translationService;
    private readonly IYtdlpExecutableService _ytdlpService;
    private readonly IHistoryService _historyService;
    private readonly IRecoveryService _recoveryService;
    private readonly Dictionary<int, IEnumerable<string>> _downloadArgumentsCache;

    public event EventHandler<DownloadAddedEventArgs>? DownloadAdded;

    public DownloadService(IJsonFileService jsonFileService, ITranslationService translationService, IYtdlpExecutableService ytdlpService, IHistoryService historyService, IRecoveryService recoveryService)
    {
        _jsonFileService = jsonFileService;
        _translationService = translationService;
        _ytdlpService = ytdlpService;
        _historyService = historyService;
        _recoveryService = recoveryService;
        _downloadArgumentsCache = new Dictionary<int, IEnumerable<string>>();
    }

    public async Task AddDownloadAsync(DownloadOptions options, bool excludeFromHistory)
    {

    }

    public async Task AddDownloadsAsync(IEnumerable<DownloadOptions> options, bool exlucdeFromHistory)
    {

    }
}
