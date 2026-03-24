using Nickvision.Desktop.Application;
using Nickvision.Parabolic.Shared.Models;
using System.ComponentModel;
using WinRT;

namespace Nickvision.Parabolic.WinUI.Helpers;

[GeneratedBindableCustomProperty]
public sealed partial class BindableHistoricDownloadSelectionItem : INotifyPropertyChanged
{
    private readonly SelectionItem<HistoricDownload> _selectionItem;

    public event PropertyChangedEventHandler? PropertyChanged;

    public string Label => _selectionItem.Label;
    public HistoricDownload Value => _selectionItem.Value;

    public BindableHistoricDownloadSelectionItem(SelectionItem<HistoricDownload> selectionItem)
    {
        _selectionItem = selectionItem;
        _selectionItem.PropertyChanged += (_, e) => PropertyChanged?.Invoke(this, e);
    }

    public bool ShouldSelect
    {
        get => _selectionItem.ShouldSelect;

        set => _selectionItem.ShouldSelect = value;
    }
}