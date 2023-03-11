using System.Threading.Tasks;

namespace NickvisionTubeConverter.Shared.Helpers;

public static class AsyncExtensions
{
    public static async void FireAndForget(this Task task)
    {
        try
        {
            await task;
        }
        catch { }
    }
}
