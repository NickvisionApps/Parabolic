namespace Nickvision.Parabolic.Shared.Models;

public enum Executable
{
    None,
    AtomicParsley,
    FFmpeg,
    FFprobe
}

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
