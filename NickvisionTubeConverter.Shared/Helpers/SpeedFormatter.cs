using System;

namespace NickvisionTubeConverter.Shared.Helpers;

public static class SpeedFormatter
{
    private static Localizer _localizer;

    static SpeedFormatter()
    {
        _localizer = new Localizer();
    }

    public static string GetString(double speed)
    {
        if (speed > Math.Pow(1024, 3))
        {
            return string.Format(_localizer["Speed", "GiBps"], speed / Math.Pow(1024, 3));
        }
        else if (speed > Math.Pow(1024, 2))
        {
            return string.Format(_localizer["Speed", "MiBps"], speed / Math.Pow(1024, 2));
        }
        else if (speed > 1024)
        {
            return string.Format(_localizer["Speed", "KiBps"], speed / 1024.0);
        }
        else
        {
            return string.Format(_localizer["Speed", "Bps"], speed);
        }
    }
}