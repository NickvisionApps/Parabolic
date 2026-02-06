using System;
using System.Diagnostics;

namespace Nickvision.Parabolic.Shared.Helpers;

public static partial class ProcessExtensions
{
    extension(Process p)
    {

        public void Suspend(bool entireProcessTree = false)
        {
            if (OperatingSystem.IsWindows())
            {
                WindowsProcessHelpers.Suspend(p, entireProcessTree);
            }
            else
            {
                UnixProcessHelpers.Suspend(p, entireProcessTree);
            }
        }

        public void Resume(bool entireProcessTree = false)
        {
            if (OperatingSystem.IsWindows())
            {
                WindowsProcessHelpers.Resume(p, entireProcessTree);
            }
            else
            {
                UnixProcessHelpers.Resume(p, entireProcessTree);
            }
        }
    }
}
