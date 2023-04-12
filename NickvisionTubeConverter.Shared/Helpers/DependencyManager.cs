using NickvisionTubeConverter.Shared.Models;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Net.Http;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;

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
                return $"{Configuration.ConfigDir}{Path.DirectorySeparatorChar}ffmpeg{Path.DirectorySeparatorChar}ffmpeg.exe";
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
    /// The path for aria2
    /// </summary>
    public static string Aria2
    {
        get
        {
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                return $"{Configuration.ConfigDir}{Path.DirectorySeparatorChar}aria2{Path.DirectorySeparatorChar}aria2.exe";
            }
            var prefixes = new List<string>() {
                Directory.GetParent(Directory.GetParent(Path.GetFullPath(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)!))!.FullName)!.FullName,
                Directory.GetParent(Path.GetFullPath(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)!))!.FullName,
                "/usr"
            };
            foreach (var prefix in prefixes)
            {
                var path = $"{prefix}/bin/aria2";
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
                using var httpClient = new HttpClient()
                {
                    Timeout = Timeout.InfiniteTimeSpan,
                };
                //Python
                await PythonHelpers.DeployEmbeddedAsync(new Version("3.11.2"));
                //Ffmpeg
                var ffmpegVer = new Version(6, 0, 0);
                if (!File.Exists(Ffmpeg) || Configuration.Current.WinUIFfmpegVersion != ffmpegVer)
                {
                    var ffmpegDir = $"{Configuration.ConfigDir}{Path.DirectorySeparatorChar}ffmpeg{Path.DirectorySeparatorChar}";
                    if (!Directory.Exists(ffmpegDir))
                    {
                        Directory.CreateDirectory(ffmpegDir);
                    }
                    //Download and Extract Binary Zip
                    var ffmpegZip = $"{ffmpegDir}ffmpeg.zip";
                    var bytes = await httpClient.GetByteArrayAsync("https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-essentials.zip");
                    File.WriteAllBytes(ffmpegZip, bytes);
                    ZipFile.ExtractToDirectory(ffmpegZip, ffmpegDir);
                    File.Delete(ffmpegZip);
                    //Move Binaries
                    var ffmpegBinaryFolder = $"{Directory.GetDirectories(ffmpegDir).First(x => x.Contains("-essentials_build"))}{Path.DirectorySeparatorChar}bin{Path.DirectorySeparatorChar}";
                    File.Move($"{ffmpegBinaryFolder}ffmpeg.exe", $"{ffmpegDir}ffmpeg.exe", true);
                    File.Move($"{ffmpegBinaryFolder}ffplay.exe", $"{ffmpegDir}ffplay.exe", true);
                    File.Move($"{ffmpegBinaryFolder}ffprobe.exe", $"{ffmpegDir}ffprobe.exe", true);
                    Directory.Delete(Directory.GetDirectories(ffmpegDir).First(x => x.Contains("-essentials_build")), true);
                    //Update Config
                    Configuration.Current.WinUIFfmpegVersion = ffmpegVer;
                    Configuration.Current.Save();
                }
                //Aria2
                var ariaVer = new Version(1, 36, 0);
                if (!File.Exists(Aria2) || Configuration.Current.WinUIAriaVersion != ariaVer)
                {
                    var ariaDir = $"{Configuration.ConfigDir}{Path.DirectorySeparatorChar}aria2{Path.DirectorySeparatorChar}";
                    if (!Directory.Exists(ariaDir))
                    {
                        Directory.CreateDirectory(ariaDir);
                    }
                    //Download and Extract Binary Zip
                    var ariaZip = $"{ariaDir}aria.zip";
                    var bytes = await httpClient.GetByteArrayAsync("https://github.com/aria2/aria2/releases/download/release-1.36.0/aria2-1.36.0-win-64bit-build1.zip");
                    File.WriteAllBytes(ariaZip, bytes);
                    ZipFile.ExtractToDirectory(ariaZip, ariaDir);
                    File.Delete(ariaZip);
                    //Move Binaries
                    var ariaBinaryFolder = $"{Directory.GetDirectories(ariaDir).First(x => x.Contains("-1.36.0"))}{Path.DirectorySeparatorChar}";
                    File.Move($"{ariaBinaryFolder}aria2c.exe", $"{ariaDir}aria2.exe", true);
                    Directory.Delete(ariaBinaryFolder, true);
                    //Update Config
                    Configuration.Current.WinUIAriaVersion = ariaVer;
                    Configuration.Current.Save();
                }

            }
            else if (File.Exists(Environment.GetEnvironmentVariable("TC_PYTHON_SO")))
            {
                Python.Runtime.Runtime.PythonDLL = Environment.GetEnvironmentVariable("TC_PYTHON_SO");
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
            var pluginPath = $"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}{Path.DirectorySeparatorChar}yt-dlp{Path.DirectorySeparatorChar}plugins{Path.DirectorySeparatorChar}tubeconverter{Path.DirectorySeparatorChar}yt_dlp_plugins{Path.DirectorySeparatorChar}postprocessor{Path.DirectorySeparatorChar}tubeconverter.py";
            Directory.CreateDirectory(pluginPath.Substring(0, pluginPath.LastIndexOf(Path.DirectorySeparatorChar)));
            using var pluginResource = Assembly.GetExecutingAssembly().GetManifestResourceStream("NickvisionTubeConverter.Shared.Resources.tubeconverter.py")!;
            using var pluginFile = new FileStream(pluginPath, FileMode.Create, FileAccess.Write);
            pluginResource.CopyTo(pluginFile);
            return true;
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
            return false;
        }
    }
}
