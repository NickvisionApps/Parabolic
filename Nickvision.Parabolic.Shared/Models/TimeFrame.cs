using System;

namespace Nickvision.Parabolic.Shared.Models;

public class TimeFrame : IEquatable<TimeFrame>
{
    public TimeSpan Start { get; init; }
    public TimeSpan End { get; init; }

    public string StartString => $"{Start:c}";
    public string EndString => $"{End:c}";
    public TimeSpan Duration => End - Start;

    public TimeFrame(TimeSpan start, TimeSpan end)
    {
        Start = start;
        End = end;
        if (End < Start)
        {
            throw new ArgumentException("End time must be greater than or equal to start time.");
        }
    }

    public static TimeFrame? Parse(string start, string end, TimeSpan duration)
    {
        if (string.IsNullOrEmpty(start) || string.IsNullOrEmpty(end) || duration.TotalSeconds <= 0)
        {
            return null;
        }
        var startParts = start.Split(':');
        var endParts = end.Split(':');
        if (startParts.Length != 3 || endParts.Length != 3)
        {
            return null;
        }
        try
        {
            var startTimeSpan = new TimeSpan(int.Parse(startParts[0]), int.Parse(startParts[1]), int.Parse(startParts[2]));
            var endTimeSpan = new TimeSpan(int.Parse(endParts[0]), int.Parse(endParts[1]), int.Parse(endParts[2]));
            if (startTimeSpan < TimeSpan.Zero || endTimeSpan <= startTimeSpan || endTimeSpan > duration)
            {
                return null;
            }
            return new TimeFrame(startTimeSpan, endTimeSpan);
        }
        catch
        {
            return null;
        }
    }

    public override bool Equals(object? obj) => obj is TimeFrame other && Equals(other);

    public bool Equals(TimeFrame? other) => other is not null && Start == other.Start && End == other.End;

    public override int GetHashCode() => HashCode.Combine(Start, End);

    public override string ToString() => $"{Start:c}-{End:c}";

    public static bool operator ==(TimeFrame left, TimeFrame right) => left.Equals(right);

    public static bool operator !=(TimeFrame left, TimeFrame right) => !left.Equals(right);
}
