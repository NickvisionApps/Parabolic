using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using Xabe.FFmpeg.Downloader;

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
        try
        {
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                var pythonDirPath = $"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}{Path.DirectorySeparatorChar}Nickvision{Path.DirectorySeparatorChar}{AppInfo.Current.Name}{Path.DirectorySeparatorChar}Python{Path.DirectorySeparatorChar}";
                var pythonType = RuntimeInformation.OSArchitecture switch
                {
                    Architecture.X86 => "python-3.11.2-embed-win32",
                    Architecture.Arm => "python-3.11.2-embed-win32",
                    Architecture.Arm64 => "python-3.11.2-embed-arm64",
                    _ => "python-3.11.2-embed-amd64"
                };
                var pythonDllPath = $"{pythonDirPath}{pythonType}{Path.DirectorySeparatorChar}python311.dll";
                var pythonLibPath = $"{pythonDirPath}{pythonType}{Path.DirectorySeparatorChar}Lib{Path.DirectorySeparatorChar}";
                if (!File.Exists(Ffmpeg))
                {
                    await FFmpegDownloader.GetLatestVersion(FFmpegVersion.Official, Ffmpeg.Remove(Ffmpeg.IndexOf("ffmpeg.exe")));
                }
                if (!File.Exists(pythonDllPath) || !Directory.Exists(pythonLibPath) || Configuration.Current.WinUIPythonVersion != new Version(3, 11, 2) || (Configuration.Current.WinUIYtdlpVersion != new Version(2023, 3, 4)))
                {
                    Python.Deployment.Installer.InstallPath = pythonDirPath;
                    Python.Deployment.Installer.Source = new Python.Deployment.Installer.DownloadInstallationSource() { DownloadUrl = $"https://www.python.org/ftp/python/3.11.2/{pythonType}.zip" };
                    Python.Runtime.Runtime.PythonDLL = pythonDllPath;
                    if(Directory.Exists(pythonLibPath))
                    {
                        Directory.Delete(pythonLibPath, true);
                    }
                    await Python.Deployment.Installer.SetupPython(true);
                    await Python.Deployment.Installer.InstallWheel(typeof(MainWindowController).Assembly, "yt_dlp-any.whl", true);
                    await Python.Deployment.Installer.SetupPython();
                    Configuration.Current.WinUIPythonVersion = new Version(3, 11, 2);
                    Configuration.Current.WinUIYtdlpVersion = new Version(2023, 3, 4);
                    Configuration.Current.Save();
                }
                else
                {
                    Python.Runtime.Runtime.PythonDLL = pythonDllPath;
                }
            }
            else
            {
                var process = new Process
                {
                    StartInfo = new ProcessStartInfo
                    {
                        FileName = "python3",
                        Arguments = "-c \"import sysconfig; print('/'.join(sysconfig.get_config_vars('LIBDIR', 'INSTSONAME')))\"",
                        UseShellExecute = false,
                        RedirectStandardOutput = true
                    }
                };
                process.Start();
                Python.Runtime.Runtime.PythonDLL = process.StandardOutput.ReadToEnd().Trim();
                process.WaitForExit();
            }
            return true;
        }
        catch(Exception e)
        {
            Console.WriteLine(e);
            return false;
        }
    }
}
