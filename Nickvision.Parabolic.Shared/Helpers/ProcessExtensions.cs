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
            else
            {
                UnixProcessHelpers.SetAsParentProcess(p);
            }
        }


        public void Suspend()
        {
            if (OperatingSystem.IsWindows())
            {
                WindowsProcessHelpers.Suspend(p);
            }
            else
            {
                UnixProcessHelpers.Suspend(p);
            }
        }

        public void Resume()
        {
            if (OperatingSystem.IsWindows())
            {
                WindowsProcessHelpers.Resume(p);
            }
            else
            {
                UnixProcessHelpers.Resume(p);
            }
        }
    }
}
