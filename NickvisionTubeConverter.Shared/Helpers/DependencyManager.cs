using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System.IO;
using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using System.Threading.Tasks;
using YoutubeDLSharp;

namespace NickvisionTubeConverter.Shared.Helpers;

internal static class DependencyManager
{
    /// <summary>
    /// The path for ffmpeg
    /// </summary>
    public static string Ffmpeg
    {
        get
        {
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                return $"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}{Path.DirectorySeparatorChar}Nickvision{Path.DirectorySeparatorChar}{AppInfo.Current.Name}{Path.DirectorySeparatorChar}ffmpeg.exe";
            }
            var prefixes = new List<string>() {
                Directory.GetParent(Directory.GetParent(Path.GetFullPath(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)!))!.FullName)!.FullName,
                Directory.GetParent(Path.GetFullPath(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)!))!.FullName,
                "/usr"
            };
            foreach (var prefix in prefixes)
            {
                var path = $"{prefix}/bin/ffmpeg";
                if (File.Exists(path))
                {
                    return path;
                }
            }
            return "";
        }
    }

    /// <summary>
    /// Setups dependencies for the application
    /// </summary>
    /// <returns>True if successful, else false.</returns>
    public static async Task<bool> SetupDependenciesAsync()
    {
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
        {
            await YoutubeDL.DownloadFFmpegBinary(Ffmpeg.Remove(Ffmpeg.IndexOf("ffmpeg.exe")));
            Python.Deployment.Installer.InstallPath = $"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}{Path.DirectorySeparatorChar}Nickvision{Path.DirectorySeparatorChar}{AppInfo.Current.Name}{Path.DirectorySeparatorChar}Python{Path.DirectorySeparatorChar}";
            await Python.Deployment.Installer.SetupPython();
            await Python.Deployment.Installer.InstallWheel(typeof(MainWindowController).Assembly, "yt_dlp-any.whl");
            Python.Runtime.Runtime.PythonDLL = $"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}{Path.DirectorySeparatorChar}Nickvision{Path.DirectorySeparatorChar}{AppInfo.Current.Name}{Path.DirectorySeparatorChar}Python{Path.DirectorySeparatorChar}python-3.7.3-embed-amd64{Path.DirectorySeparatorChar}python37.dll";
        }
        else
        {
            var prefixes = new List<string>() {
                Directory.GetParent(Directory.GetParent(Path.GetFullPath(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)!))!.FullName)!.FullName,
                Directory.GetParent(Path.GetFullPath(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)!))!.FullName,
                "/usr"
            };
            foreach (var prefix in prefixes)
            {
                var path = $"{prefix}/lib/x86_64-linux-gnu/libpython3.10.so";
                if (File.Exists(path))
                {
                    Python.Runtime.Runtime.PythonDLL = path;
                    break;
                }
            }
        }
        Python.Runtime.PythonEngine.Initialize();
        try
        {
            return true;
        }
        catch
        {
            return false;
        }
    }
}
