using Nickvision.Parabolic.Shared.Models;

namespace Nickvision.Parabolic.Shared.Helpers;

public static class ExecutableExtensions
{
    extension(Executable e)
    {
        public string ToYtdlpString() => e switch
        {
            Executable.AtomicParsley => "AtomicParsley",
            Executable.FFmpeg => "ffmpeg",
            Executable.FFprobe => "ffprobe",
            _ => string.Empty
        };
    }
}
