using Nickvision.Desktop.Globalization;
using System;

namespace Nickvision.Parabolic.Shared.Models;

public class VideoResolution : IComparable<VideoResolution>, IEquatable<VideoResolution>
{
    public static VideoResolution Best { get; }
    public static VideoResolution Worst { get; }

    public int Width { get; }
    public int Height { get; }

    static VideoResolution()
    {
        Best = new VideoResolution(int.MaxValue, int.MaxValue);
        Worst = new VideoResolution(-1, -1);
    }

    public VideoResolution(int width, int height)
    {
        Width = width;
        Height = height;
    }

    public static VideoResolution? Parse(string value, ITranslationService translator)
    {
        if (value == "Best" || value == translator._("Best"))
        {
            return Best;
        }
        else if (value == "Worst" || value == translator._("Worst"))
        {
            return Worst;
        }
        var parts = value.Split('x');
        if (parts.Length != 2)
        {
            return null;
        }
        if (int.TryParse(parts[0], out int width) && int.TryParse(parts[1], out int height))
        {
            return new VideoResolution(width, height);
        }
        return null;
    }

    public int CompareTo(VideoResolution? other) => other is null ? 1 : (Width * Height).CompareTo(other.Width * other.Height);

    public override bool Equals(object? obj) => obj is VideoResolution other && Equals(other);

    public bool Equals(VideoResolution? other) => other is not null && Width == other.Width && Height == other.Height;

    public override int GetHashCode() => HashCode.Combine(Width, Height);

    public override string ToString() => ToString(null);

    public string ToString(ITranslationService? translator)
    {
        if(Width == int.MaxValue && Height == int.MaxValue)
        {
            return translator?._("Best") ?? "Best";
        }
        else if(Width == -1 && Height == -1)
        {
            return translator?._("Worst") ?? "Worst";
        }
        return $"{Width}x{Height}";
    }

    public static bool operator >(VideoResolution left, VideoResolution right) => left.CompareTo(right) > 0;

    public static bool operator <(VideoResolution left, VideoResolution right) => left.CompareTo(right) < 0;

    public static bool operator >=(VideoResolution left, VideoResolution right) => left.CompareTo(right) >= 0;

    public static bool operator <=(VideoResolution left, VideoResolution right) => left.CompareTo(right) <= 0;

    public static bool operator ==(VideoResolution left, VideoResolution right) => left.Equals(right);

    public static bool operator !=(VideoResolution left, VideoResolution right) => !left.Equals(right);
}
