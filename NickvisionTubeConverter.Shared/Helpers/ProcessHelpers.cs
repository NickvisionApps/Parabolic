using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Management;
using System.Runtime.InteropServices;

namespace NickvisionTubeConverter.Shared.Helpers;

/// <summary>
/// Helper methods for working with processes
/// </summary>
public static class ProcessHelpers
{
    /// <summary>
    /// Gets all of the child processes belonging to a parent process
    /// </summary>
    /// <param name="ppid">The parent process id</param>
    /// <returns>The list of found child processes</returns>
    public static IEnumerable<Process> GetChildProcesses(int ppid)
    {
        if(RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
        {
            using var searcher = new ManagementObjectSearcher($"Select * From Win32_Process Where ParentProcessID={ppid}");
            return searcher.Get().Cast<ManagementObject>().Select(mo => Process.GetProcessById(Convert.ToInt32(mo["ProcessID"])));
        }
        else if(RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
        {
            var children = new List<Process>();
            foreach (var process in Process.GetProcesses())
            {
                try
                {
                    using var reader = new StreamReader($"/proc/{process.Id}/stat");
                    var line = reader.ReadLine() ?? "";
                    var parts = line.Split(' ', 5);
                    if (parts.Length >= 4)
                    {
                        if (ppid == int.Parse(parts[3]))
                        {
                            children.Add(process);
                        }
                    }
                }
                catch { }
            }
            return children;
        }
        return new List<Process>();
    }
}
