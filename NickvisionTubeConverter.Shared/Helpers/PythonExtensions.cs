using System.Text.RegularExpressions;

namespace NickvisionTubeConverter.Shared.Helpers;

public static class PythonExtensions
{
    public static void SetConsoleOutputFilePath(string path)
    {
        using (Python.Runtime.Py.GIL())
        {
            dynamic sys = Python.Runtime.Py.Import("sys");
            sys.stdout = Python.Runtime.PythonEngine.Eval($"open(\"{Regex.Replace(path, @"\\", @"\\")}\", \"w\")");
        }
    }
}
