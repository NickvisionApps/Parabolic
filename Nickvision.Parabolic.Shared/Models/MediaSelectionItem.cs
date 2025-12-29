namespace Nickvision.Parabolic.Shared.Models;

public class MediaSelectionItem : SelectionItem<int>
{
    public string Filename { get; set; }
    public string StartTime { get; set; }
    public string EndTime { get; set; }

    public MediaSelectionItem(int index, string filename, string startTime, string endTime) : base(index, filename, true)
    {
        Filename = filename;
        StartTime = startTime;
        EndTime = endTime;
    }
}
