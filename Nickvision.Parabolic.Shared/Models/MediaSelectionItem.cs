using Nickvision.Desktop.Application;
using Nickvision.Desktop.Globalization;
using System;

namespace Nickvision.Parabolic.Shared.Models;

public class MediaSelectionItem : SelectionItem<int>
{
    private TimeFrame _timeFrame;

    public string StartTimeHeader { get; }
    public string EndTimeHeader { get; }

    public TimeSpan Duration => _timeFrame.Duration;

    public MediaSelectionItem(int index, Media media, ITranslationService translator) : base(index, media.Title, true)
    {
        _timeFrame = media.TimeFrame;
        StartTimeHeader = translator._("Start Time");
        EndTimeHeader = translator._("End Time");
        Filename = media.Title;
        StartTime = _timeFrame.StartString;
        EndTime = _timeFrame.EndString;
    }

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
}
