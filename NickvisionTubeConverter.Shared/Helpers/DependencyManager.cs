using Python.Runtime;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;
namespace NickvisionTubeConverter.Shared.Helpers;

internal static class DependencyManager
{
    /// <summary>
    /// The path for python
    /// </summary>
    public static string PythonPath
    {
        get
        {
            var prefixes = new List<string>() {
                Directory.GetParent(Directory.GetParent(Path.GetFullPath(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)!))!.FullName)!.FullName,
                Directory.GetParent(Path.GetFullPath(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)!))!.FullName,
                "/usr",
                "snap/tube-converter/current/usr"
            };
            foreach (var prefix in prefixes)
            {
                var path = $"{prefix}/bin/python3";
                if (File.Exists(path))
                {
                    return path;
                }
            }
            return "python3";
        }
    }

    /// <summary>
    /// The path for ffmpeg
    /// </summary>
    public static string FfmpegPath
    {
        get
        {
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
            return "ffmpeg";
        }
    }

    /// <summary>
    /// The path for aria2
    /// </summary>
    public static string Aria2Path
    {
        get
        {
            var prefixes = new List<string>() {
                Directory.GetParent(Directory.GetParent(Path.GetFullPath(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)!))!.FullName)!.FullName,
                Directory.GetParent(Path.GetFullPath(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)!))!.FullName,
                "/usr"
            };
            foreach (var prefix in prefixes)
            {
                var path = $"{prefix}/bin/aria2c";
                if (File.Exists(path))
                {
                    return path;
                }
            }
            return "aria2c";
        }
    }

    /// <summary>
    /// Setups dependencies for the application
    /// </summary>
    /// <returns>True if successful, else false.</returns>
    public static bool SetupDependencies()
    {
        try
        {
            if (File.Exists(Environment.GetEnvironmentVariable("TC_PYTHON_SO")))
            {
                Runtime.PythonDLL = Environment.GetEnvironmentVariable("TC_PYTHON_SO");
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
                Runtime.PythonDLL = process.StandardOutput.ReadToEnd().Trim();
                process.WaitForExit();
            }
            // Install yt-dlp plugin
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
