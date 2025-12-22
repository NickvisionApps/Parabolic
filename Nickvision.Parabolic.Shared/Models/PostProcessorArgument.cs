using Nickvision.Parabolic.Shared.Helpers;
using System;

namespace Nickvision.Parabolic.Shared.Models;

public class PostProcessorArgument : IComparable<PostProcessorArgument>, IEquatable<PostProcessorArgument>
{
    public string Name { get; set; }
    public PostProcessor PostProcessor { get; set; }
    public Executable Executable { get; set; }
    public string Args { get; set; }

    public PostProcessorArgument(string name, PostProcessor postProcessor, Executable executable, string args)
    {
        Name = name;
        PostProcessor = postProcessor;
        Executable = executable;
        Args = args;
    }

    public int CompareTo(PostProcessorArgument? other) => other is null ? 1 : Name.CompareTo(other.Name);

    public override bool Equals(object? obj)
    {
        if (obj is PostProcessorArgument other)
        {
            return Equals(other);
        }
        return false;
    }

    public bool Equals(PostProcessorArgument? other) => other is not null && Name == other.Name && PostProcessor == other.PostProcessor && Executable == other.Executable && Args == other.Args;

    public override int GetHashCode() => HashCode.Combine(Name, PostProcessor, Executable, Args);

    public override string ToString()
    {
        if (PostProcessor == PostProcessor.None && Executable == Executable.None)
        {
            return Args;
        }
        else if (PostProcessor != PostProcessor.None && Executable != Executable.None)
        {
            return $"{PostProcessor.ToYtdlpString()}+{Executable.ToYtdlpString()}:{Args}";
        }
        else if (PostProcessor != PostProcessor.None)
        {
            return $"{PostProcessor.ToYtdlpString()}:{Args}";
        }
        else if (Executable != Executable.None)
        {
            return $"{Executable.ToYtdlpString()}:{Args}";
        }
        return string.Empty;
    }

    public static bool operator >(PostProcessorArgument left, PostProcessorArgument right) => left.CompareTo(right) > 0;

    public static bool operator <(PostProcessorArgument left, PostProcessorArgument right) => left.CompareTo(right) < 0;

    public static bool operator >=(PostProcessorArgument left, PostProcessorArgument right) => left.CompareTo(right) >= 0;

    public static bool operator <=(PostProcessorArgument left, PostProcessorArgument right) => left.CompareTo(right) <= 0;

    public static bool operator ==(PostProcessorArgument left, PostProcessorArgument right) => left.Equals(right);

    public static bool operator !=(PostProcessorArgument left, PostProcessorArgument right) => !left.Equals(right);
}
