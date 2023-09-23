using Python.Runtime;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using Nickvision.Aura;
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
            foreach (var dir in SystemDirectories.Path)
            {
                var path = $"{dir}{Path.DirectorySeparatorChar}python3";
                if (File.Exists(path))
                {
                    return path;
                }
            }
            throw new Exception("python3 not in path");
        }
    }

    /// <summary>
    /// The path for ffmpeg
    /// </summary>
    public static string FfmpegPath
    {
        get
        { 
            foreach (var dir in SystemDirectories.Path)
            {
                var path = $"{dir}{Path.DirectorySeparatorChar}ffmpeg";
                if (File.Exists(path))
                {
                    return path;
                }
            }
            throw new Exception("ffmpeg not found in path");
        }
    }

    /// <summary>
    /// The path for aria2
    /// </summary>
    public static string Aria2Path
    {
        get
        {
            foreach (var dir in SystemDirectories.Path)
            {
                var path = $"{dir}{Path.DirectorySeparatorChar}aria2c";
                if (File.Exists(path))
                {
                    return path;
                }
            }
            throw new Exception("aria2c not found in path");
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
