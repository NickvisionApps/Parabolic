using Nickvision.Desktop.Globalization;
using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace Nickvision.Parabolic.Shared.Models;

public class MediaSelectionItem : SelectionItem<int>, INotifyPropertyChanged
{
    private static string? _startTimeHeader;
    private static string? _endTimeHeader;

    public string? StartTimeHeader => _startTimeHeader;
    public string? EndTimeHeader => _endTimeHeader;

    static MediaSelectionItem()
    {
        _startTimeHeader = null;
        _endTimeHeader = null;
    }

    public MediaSelectionItem(int index, string filename, string startTime, string endTime, ITranslationService translator) : base(index, filename, true)
    {
        Filename = filename;
        StartTime = startTime;
        EndTime = endTime;
        if (_startTimeHeader is null)
        {
            _startTimeHeader = translator._("Start Time");
        }
        if (_endTimeHeader is null)
        {
            _endTimeHeader = translator._("End Time");
        }
    }

    public event PropertyChangedEventHandler? PropertyChanged;

    public string Filename
    {
        get => field;

        set
        {
            field = value;
            OnPropertyChanged();
        }
    }

    public string StartTime
    {
        get => field;

        set
        {
            field = value;
            OnPropertyChanged();
        }
    }

    public string EndTime
    {
        get => field;

        set
        {
            field = value;
            OnPropertyChanged();
        }
    }

    protected void OnPropertyChanged([CallerMemberName] string? propertyName = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
}
