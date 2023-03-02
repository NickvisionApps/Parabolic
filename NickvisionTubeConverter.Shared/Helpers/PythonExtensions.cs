using System.Text.RegularExpressions;

namespace NickvisionTubeConverter.Shared.Helpers;

/// <summary>
/// Extension methods for python engine
/// </summary>
public static class PythonExtensions
{
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
