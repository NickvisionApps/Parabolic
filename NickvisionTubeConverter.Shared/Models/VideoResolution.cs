using System;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// A model of an available video resolution
/// </summary>
public class VideoResolution : IComparable<VideoResolution>, IEquatable<VideoResolution>
{
    /// <summary>
    /// The resolution width
    /// </summary>
    public int Width { get; init; }
    /// <summary>
    /// The resolution height
    /// </summary>
    public int Height { get; init; }


    /// <summary>
    /// Constructs a VideoResolution
    /// </summary>
    /// <param name="width">The resolution width</param>
    /// <param name="height">The resolution height</param>
    public VideoResolution(int width, int height)
    {
        Width = width;
        Height = height;
    }

    /// <summary>
    /// Parses a VideoResolution from a string
    /// </summary>
    /// <param name="s">The string to parse (Format: WidthxHeight)</param>
    /// <returns>The parsed VideoResolution, null if error</returns>
    public static VideoResolution? Parse(string s)
    {
        var split = s.Split("x");
        if(split.Length == 2)
        {
            try
            {
                return new VideoResolution(int.Parse(split[0]), int.Parse(split[1]));
            }
            catch { }
        }
        return null;
    }

    /// <summary>
    /// Gets a string representation of a VideoResolution
    /// </summary>
    /// <returns>The string representation of the VideoResolution</returns>
    public override string ToString() => Width == 0 && Height == 0 ? _("Default") : $"{Width}x{Height}";

    /// <summary>
    /// Compares this with other
    /// </summary>
    /// <param name="other">The VideoResolution object to compare to</param>
    /// <returns>-1 if this is less than other. 0 if this is equal to other. 1 if this is greater than other</returns>
    /// <exception cref="NullReferenceException">Thrown if other is null</exception>
    public int CompareTo(VideoResolution? other)
    {
        if (other == null)
        {
            throw new NullReferenceException();
        }
        if (this < other)
        {
            return -1;
        }
        else if (this == other)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }

    /// <summary>
    /// Compares two VideoResolution objects by ==
    /// </summary>
    /// <param name="a">The first VideoResolution object</param>
    /// <param name="b">The second VideoResolution object</param>
    /// <returns>True if a == b, else false</returns>
    public static bool operator ==(VideoResolution? a, VideoResolution? b) => a?.Width == b?.Width && a?.Height == b?.Height;

    /// <summary>
    /// Compares two VideoResolution objects by !=
    /// </summary>
    /// <param name="a">The first VideoResolution object</param>
    /// <param name="b">The second VideoResolution object</param>
    /// <returns>True if a != b, else false</returns>
    public static bool operator !=(VideoResolution? a, VideoResolution? b) => a?.Width != b?.Width && a?.Height != b?.Height;

    /// <summary>
    /// Compares two VideoResolution objects by &gt;
    /// Objects with equal Height are compared by Width
    /// </summary>
    /// <param name="a">The first VideoResolution object</param>
    /// <param name="b">The second VideoResolution object</param>
    /// <returns>True if a &gt; b, else false</returns>
    public static bool operator <(VideoResolution? a, VideoResolution? b) => a?.Width < b?.Width && a?.Height <= b?.Height;

    /// <summary>
    /// Compares two VideoResolution objects by &lt;
    /// Objects with equal Height are compared by Width
    /// </summary>
    /// <param name="a">The first VideoResolution object</param>
    /// <param name="b">The second VideoResolution object</param>
    /// <returns>True if a &lt; b, else false</returns>
    public static bool operator >(VideoResolution? a, VideoResolution? b) => a?.Width > b?.Width && a?.Height >= b?.Height;

    /// <summary>
    /// Gets whether or not an object is equal to this VideoResolution
    /// </summary>
    /// <param name="obj">The object to compare</param>
    /// <returns>True if equals, else false</returns>
    public override bool Equals(object? obj)
    {
        if (obj is VideoResolution toCompare)
        {
            return Width == toCompare.Width && Height == toCompare.Height;
        }
        return false;
    }

    /// <summary>
    /// Gets whether or not an object is equal to this VideoResolution
    /// </summary>
    /// <param name="obj">The VideoResolution? object to compare</param>
    /// <returns>True if equals, else false</returns>
    public bool Equals(VideoResolution? obj) => Equals((object?)obj);
}