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
    /// Setups dependencies for the application
    /// </summary>
    /// <returns>True if successful, else false.</returns>
    public static async Task<bool> SetupDependenciesAsync()
    {
        try
        {
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                //Python
                await PythonHelpers.DeployEmbeddedAsync(new Version("3.11.2"));
                //Ffmpeg
                var ffmpegVer = new Version(6, 0, 0);
                if (!File.Exists(Ffmpeg) || Configuration.Current.WinUIFfmpegVersion != ffmpegVer)
                {
                    var httpClient = new HttpClient()
                    {
                        Timeout = Timeout.InfiniteTimeSpan
                    };
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
            return true;
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
            return false;
        }
    }
}
