using System;

namespace NickvisionTubeConverter.Shared.Helpers;

/// <summary>
/// Format methods for speeds
/// </summary>
public static class SpeedFormatter
{
    /// <summary>
    /// Gets a string representation of a speed
    /// </summary>
    /// <param name="speed">The speed</param>
    /// <param name="localizer">Localizer</param>
    /// <returns>The string representation of the speed</returns>
    public static string GetString(double speed, Localizer localizer)
    {
        if (speed > Math.Pow(1024, 3))
        {
            return string.Format(localizer["Speed", "GiBps"], speed / Math.Pow(1024, 3));
        }
        else if (speed > Math.Pow(1024, 2))
        {
            return string.Format(localizer["Speed", "MiBps"], speed / Math.Pow(1024, 2));
        }
        else if (speed > 1024)
        {
            return string.Format(localizer["Speed", "KiBps"], speed / 1024.0);
        }
        else
        {
            return string.Format(localizer["Speed", "Bps"], speed);
        }
    }
}
