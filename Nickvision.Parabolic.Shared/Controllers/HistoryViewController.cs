using Nickvision.Desktop.Application;
using Nickvision.Desktop.Globalization;
using Nickvision.Parabolic.Shared.Events;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Controllers;

public class HistoryViewController
{
    private readonly IHistoryService _historyService;

    public ITranslationService Translator { get; }
    public IReadOnlyList<SelectionItem<HistoryLength>> Lengths { get; }

    public event EventHandler<DownloadRequestedEventArgs>? DownloadRequested;

    public HistoryViewController(ITranslationService translationService, IHistoryService historyService)
    {
        _historyService = historyService;
        Translator = translationService;
        var selectedLength = _historyService.Length;
        Lengths = new List<SelectionItem<HistoryLength>>
        {
            new SelectionItem<HistoryLength>(HistoryLength.Never, Translator._("Never"), selectedLength == HistoryLength.Never),
            new SelectionItem<HistoryLength>(HistoryLength.OneDay, Translator._("1 Day"), selectedLength == HistoryLength.OneDay),
            new SelectionItem<HistoryLength>(HistoryLength.OneWeek, Translator._("1 Week"), selectedLength == HistoryLength.OneWeek),
            new SelectionItem<HistoryLength>(HistoryLength.OneMonth, Translator._("1 Month"), selectedLength == HistoryLength.OneMonth),
            new SelectionItem<HistoryLength>(HistoryLength.ThreeMonths, Translator._("3 Months"), selectedLength == HistoryLength.ThreeMonths),
            new SelectionItem<HistoryLength>(HistoryLength.SixMonths, Translator._("6 Months"), selectedLength == HistoryLength.SixMonths),
            new SelectionItem<HistoryLength>(HistoryLength.OneYear, Translator._("1 Year"), selectedLength == HistoryLength.OneYear),
            new SelectionItem<HistoryLength>(HistoryLength.Forever, Translator._("Forever"), selectedLength == HistoryLength.Forever)
        };
    }

    public bool SortNewest
    {
        get => _historyService.SortNewest;

        set => _historyService.SortNewest = value;
    }

    public HistoryLength Length
    {
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

    public async Task RemoveAsync(Uri url) => await _historyService.RemoveAsync(url);

    public void RequestDownload(Uri url) => DownloadRequested?.Invoke(this, new DownloadRequestedEventArgs(url));
}
