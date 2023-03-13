using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.Shared.Helpers;

/// <summary>
/// Extension methods for python engine
/// </summary>
public static class PythonExtensions
{
    /// <summary>
    /// Downloads and installs an embedded version of Python (Windows Only)
    /// </summary>
    /// <param name="version">The version of python to download</param>
    public static async Task DeployEmbeddedAsync(Version version)
    {
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
        {
            var pythonDirPath = $"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}{Path.DirectorySeparatorChar}Nickvision{Path.DirectorySeparatorChar}{AppInfo.Current.Name}{Path.DirectorySeparatorChar}Python{Path.DirectorySeparatorChar}";
            var pythonType = RuntimeInformation.OSArchitecture switch
            {
                Architecture.X86 => $"python-{version}-embed-win32",
                Architecture.Arm => $"python-{version}-embed-win32",
                Architecture.Arm64 => $"python-{version}-embed-amd64",
                _ => $"python-{version}-embed-amd64"
            };
            var pythonDllPath = $"{pythonDirPath}{pythonType}{Path.DirectorySeparatorChar}python{version.Major}{version.Minor}.dll";
            var pythonLibPath = $"{pythonDirPath}{pythonType}{Path.DirectorySeparatorChar}Lib{Path.DirectorySeparatorChar}";
            if (!File.Exists(pythonDllPath) || !Directory.Exists(pythonLibPath) || Configuration.Current.WinUIPythonVersion != version || (Configuration.Current.WinUIYtdlpVersion != new Version(2023, 3, 4)))
            {
                Python.Deployment.Installer.InstallPath = pythonDirPath;
                Python.Deployment.Installer.Source = new Python.Deployment.Installer.DownloadInstallationSource() { DownloadUrl = $"https://www.python.org/ftp/python/3.11.2/{pythonType}.zip" };
                Python.Runtime.Runtime.PythonDLL = pythonDllPath;
                if (Directory.Exists(pythonLibPath))
                {
                    Directory.Delete(pythonLibPath, true);
                }
                await Python.Deployment.Installer.SetupPython(true);
                await Python.Deployment.Installer.InstallWheel(typeof(MainWindowController).Assembly, "yt_dlp-any.whl", true);
                await Python.Deployment.Installer.SetupPython();
                Configuration.Current.WinUIPythonVersion = version;
                Configuration.Current.WinUIYtdlpVersion = new Version(2023, 3, 4);
                Configuration.Current.Save();
            }
            else
            {
                Python.Runtime.Runtime.PythonDLL = pythonDllPath;
            }
        }
    }

    /// <summary>
    /// Sets the console output of python to a file
    /// </summary>
    /// <param name="path">The path of the file</param>
    /// <returns>The file handle object</returns>
    public static dynamic SetConsoleOutputFilePath(string path)
    {
        using (Python.Runtime.Py.GIL())
        {
            dynamic sys = Python.Runtime.Py.Import("sys");
            dynamic file = Python.Runtime.PythonEngine.Eval($"open(\"{Regex.Replace(path, @"\\", @"\\")}\", \"w\")");
            sys.stdout = file;
            sys.stderr = file;
            return file;
        }
    }
}
