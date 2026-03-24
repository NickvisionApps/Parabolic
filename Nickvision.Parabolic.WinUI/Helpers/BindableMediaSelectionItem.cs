using Nickvision.Parabolic.Shared.Models;
using System;
using System.ComponentModel;
using WinRT;

namespace Nickvision.Parabolic.WinUI.Helpers;

[GeneratedBindableCustomProperty]
public sealed partial class BindableMediaSelectionItem : INotifyPropertyChanged
{
    public MediaSelectionItem SelectionItem { get; init; }

    public event PropertyChangedEventHandler? PropertyChanged;

    public string Label => SelectionItem.Label;
    public string StartTimeHeader => SelectionItem.StartTimeHeader;
    public string EndTimeHeader => SelectionItem.EndTimeHeader;
    public TimeSpan Duration => SelectionItem.Duration;
    public int Value => SelectionItem.Value;

    public BindableMediaSelectionItem(MediaSelectionItem selectionItem)
    {
        SelectionItem = selectionItem;
        SelectionItem.PropertyChanged += (_, e) => PropertyChanged?.Invoke(this, e);
    }

    public bool ShouldSelect
    {
        get => SelectionItem.ShouldSelect;

        set => SelectionItem.ShouldSelect = value;
    }

    public string Filename
    {
        get => SelectionItem.Filename;

        set => SelectionItem.Filename = value;
    }

    public string StartTime
    {
        get => SelectionItem.StartTime;

        set => SelectionItem.StartTime = value;
    }

    public string EndTime
    {
        get => SelectionItem.EndTime;

        set => SelectionItem.EndTime = value;
    }
}