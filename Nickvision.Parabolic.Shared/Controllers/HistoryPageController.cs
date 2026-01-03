using Nickvision.Desktop.Application;
using Nickvision.Desktop.Globalization;
using Nickvision.Parabolic.Shared.Events;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Controllers;

public class HistoryPageController
{
    private readonly IHistoryService _historyService;

    public ITranslationService Translator { get; }

    public event EventHandler<DownloadRequestedEventArgs>? DownloadRequested;

    public HistoryPageController(ITranslationService translationService, IHistoryService historyService)
    {
        _historyService = historyService;
        Translator = translationService;
    }

    public bool SortNewest
    {
        get => _historyService.SortNewest;

        set => _historyService.SortNewest = value;
    }

    public HistoryLength Length
    {
        get => _historyService.Length;

        set => _historyService.Length = value;
    }

    public async Task ClearAllAsync() => await _historyService.ClearAsync();

    public async Task<IReadOnlyList<SelectionItem<HistoricDownload>>> GetAllAsync()
    {
        var result = new List<SelectionItem<HistoricDownload>>();
        foreach (var download in await _historyService.GetAllAsync())
        {
            result.Add(new SelectionItem<HistoricDownload>(download, download.Title, false));
        }
        return result;
    }

    public async Task RemoveAsync(IEnumerable<SelectionItem<HistoricDownload>> list)
    {
        foreach (var item in list)
        {
            await _historyService.RemoveAsync(item.Value);
        }
    }

    public void RequestDownload(Uri url) => DownloadRequested?.Invoke(this, new DownloadRequestedEventArgs(url));
}
