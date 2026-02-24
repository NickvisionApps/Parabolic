using System;
using System.Diagnostics;

namespace Nickvision.Parabolic.Shared.Helpers;

public static partial class ProcessExtensions
{
    extension(Process p)
    {
        public void SetAsParentProcess()
        {
            if (OperatingSystem.IsWindows())
            {
                WindowsProcessHelpers.SetAsParentProcess(p);
            }
            else if(OperatingSystem.IsMacOS())
            {
                MacOSProcessHelpers.SetAsParentProcess(p);
            }
            else if(OperatingSystem.IsLinux())
            {
                LinuxProcessHelpers.SetAsParentProcess(p);
            }
        }


        public void Suspend()
        {
            if (OperatingSystem.IsWindows())
            {
                WindowsProcessHelpers.Suspend(p);
            }
            else if(OperatingSystem.IsMacOS())
            {
                MacOSProcessHelpers.Suspend(p);
            }
            else if (OperatingSystem.IsLinux())
            {
                LinuxProcessHelpers.Suspend(p);
            }
        }

        public void Resume()
        {
            if (OperatingSystem.IsWindows())
            {
                WindowsProcessHelpers.Resume(p);
            }
            else if(OperatingSystem.IsMacOS())
            {
                MacOSProcessHelpers.Resume(p);
            }
            else if (OperatingSystem.IsLinux())
            {
                LinuxProcessHelpers.Resume(p);
            }
        }
    }
}
