using System;
using static Nickvision.Aura.Localization.Gettext;

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
    /// <returns>The string representation of the speed</returns>
    public static string GetSpeedString(this double speed)
    {
        if (speed > Math.Pow(1024, 3))
        {
            return _("{0:f1} GiB/s", speed / Math.Pow(1024, 3));
        }
        else if (speed > Math.Pow(1024, 2))
        {
            return _("{0:f1} MiB/s", speed / Math.Pow(1024, 2));
        }
        else if (speed > 1024)
        {
            return _("{0:f1} KiB/s", speed / 1024.0);
        }
        else
        {
            return _("{0:f1} B/s", speed);
        }
    }
}
