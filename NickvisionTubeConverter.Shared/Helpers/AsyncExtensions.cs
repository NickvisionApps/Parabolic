using System.Threading.Tasks;

namespace NickvisionTubeConverter.Shared.Helpers;

/// <summary>
/// Extension methods for working with System.Threading.Tasks.Task
/// </summary>
public static class AsyncExtensions
{
    /// <summary>
    /// Fires an async method and forgets about it's return
    /// </summary>
    /// <param name="task">The async Task</param>
    public static async void FireAndForget(this Task task)
    {
        try
        {
            await task;
        }
        catch { }
    }
}
