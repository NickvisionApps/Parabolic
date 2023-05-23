using Python.Runtime;
using System.Text.RegularExpressions;

namespace NickvisionTubeConverter.Shared.Helpers;

/// <summary>
/// Helper methods for python engine
/// </summary>
public static class PythonHelpers
{
    /// <summary>
    /// Sets the console output of python to a file
    /// </summary>
    /// <param name="path">The path of the file</param>
    /// <returns>The file handle object</returns>
    public static dynamic SetConsoleOutputFilePath(string path)
    {
        using (Py.GIL())
        {
            dynamic sys = Py.Import("sys");
            dynamic file = PythonEngine.Eval($"open(\"{Regex.Replace(path, @"\\", @"\\")}\", \"w\")");
            sys.stdout = file;
            sys.stderr = file;
            return file;
        }
    }
}
