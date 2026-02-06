using System.Diagnostics;
using System.Runtime.InteropServices;

namespace Nickvision.Parabolic.Shared.Helpers;

public static partial class UnixProcessHelpers
{
    [LibraryImport("libc")]
    private static partial int kill(int pid, int sig);

    public static void Suspend(Process p, bool entireProcessTree = false)
    {
        if (entireProcessTree)
        {
            kill(-p.Id, 19);
        }
        kill(p.Id, 19);
    }

    public static void Resume(Process p, bool entireProcessTree = false)
    {
        if (entireProcessTree)
        {
            kill(-p.Id, 18);
        }
        kill(p.Id, 18);
    }
}
