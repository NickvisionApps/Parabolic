using System;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// A model of an available video resolution
/// </summary>
public class VideoResolution : IComparable<VideoResolution>
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
    /// <param name="width">The resolution height</param>
    public VideoResolution(int width, int height)
    {
        Width = width;
        Height = height;
    }

    /// <summary>
    /// Gets a string representation of a VideoResolution
    /// </summary>
    /// <returns>The string representation of the VideoResolution</returns>
    public override string ToString() => $"{Width}x{Height}";

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
    /// Compares two VideoResolution objects by >
    /// Objects with equal Height are compared by Width
    /// </summary>
    /// <param name="a">The first VideoResolution object</param>
    /// <param name="b">The second VideoResolution object</param>
    /// <returns>True if a > b, else false</returns>
    public static bool operator <(VideoResolution? a, VideoResolution? b) => a?.Width < b?.Width && a?.Height <= b?.Height;

    /// <summary>
    /// Compares two VideoResolution objects by <
    /// Objects with equal Height are compared by Width
    /// </summary>
    /// <param name="a">The first VideoResolution object</param>
    /// <param name="b">The second VideoResolution object</param>
    /// <returns>True if a < b, else false</returns>
    public static bool operator >(VideoResolution? a, VideoResolution? b) => a?.Width > b?.Width && a?.Height >= b?.Height;
}