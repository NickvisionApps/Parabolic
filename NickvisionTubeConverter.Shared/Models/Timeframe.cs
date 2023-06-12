using System;
using System.Globalization;

namespace NickvisionTubeConverter.Shared.Models;

/// <summary>
/// A model of a timeframe
/// </summary>
public class Timeframe
{
    /// <summary>
    /// The start of the timeframe
    /// </summary>
    public TimeSpan Start { get; init; }
    /// <summary>
    /// The end of the timeframe
    /// </summary>
    public TimeSpan End { get; init; }
    
    /// <summary>
    /// Constructs a timeframe
    /// </summary>
    /// <param name="startTime">The start TimeSpan</param>
    /// <param name="endTime">The end TimeSpan</param>
    public Timeframe(TimeSpan start, TimeSpan end)
    {
        Start = start;
        End = end;
    }
    
    /// <summary>
    /// Parses a Timeframe object
    /// </summary>
    /// <param name="start">The start time</param>
    /// <param name="start">The end time</param>
    /// <returns>The Timeframe object</returns>
    public static Timeframe Parse(string start, string end, double duration)
    {
        TimeSpan startSpan;
        TimeSpan endSpan;
        try
        {
            startSpan = TimeSpan.Parse(start, CultureInfo.CurrentCulture);
        }
        catch
        {
            throw new ArgumentException("Unable to parse start time.");
        }
        try
        {
            endSpan = TimeSpan.Parse(end, CultureInfo.CurrentCulture);
        }
        catch
        {
            throw new ArgumentException("Unable to parse emd time.");
        }
        if(startSpan < TimeSpan.FromSeconds(0))
        {
            throw new ArgumentException("Invalid start time.");
        }
        if(endSpan < startSpan + TimeSpan.FromSeconds(1) || endSpan > TimeSpan.FromSeconds(duration))
        {
            throw new ArgumentException("Invalid end time");
        }
        return new Timeframe(startSpan, endSpan);
    }
}