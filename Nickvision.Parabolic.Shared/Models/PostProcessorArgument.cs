using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace Nickvision.Parabolic.Shared.Models;

public class PostProcessorArgument : IComparable<PostProcessorArgument>, IEquatable<PostProcessorArgument>, INotifyPropertyChanged
{
    public event PropertyChangedEventHandler? PropertyChanged;

    public PostProcessorArgument(string name, PostProcessor postProcessor, Executable executable, string args)
    {
        Name = name;
        PostProcessor = postProcessor;
        Executable = executable;
        Args = args;
    }

    public string Name
    {
        get => field;

        set
        {
            field = value;
            OnPropertyChanged();
        }
    }
    public PostProcessor PostProcessor
    {
        get => field;

        set
        {
            field = value;
            OnPropertyChanged();
        }
    }

    public Executable Executable
    {
        get => field;

        set
        {
            field = value;
            OnPropertyChanged();
        }
    }

    public string Args
    {
        get => field;

        set
        {
            field = value;
            OnPropertyChanged();
        }
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
        var ytdlpExecutable = Executable switch
        {
            Executable.AtomicParsley => "AtomicParsley",
            Executable.FFmpeg => "ffmpeg",
            Executable.FFprobe => "ffprobe",
            _ => string.Empty
        };
        var ytdlpPostProcessor = PostProcessor switch
        {
            PostProcessor.Merger => "Merger",
            PostProcessor.ModifyChapters => "ModifyChapters",
            PostProcessor.SplitChapters => "SplitChapters",
            PostProcessor.ExtractAudio => "ExtractAudio",
            PostProcessor.VideoRemuxer => "VideoRemuxer",
            PostProcessor.VideoConverter => "VideoConverter",
            PostProcessor.Metadata => "Metadata",
            PostProcessor.EmbedSubtitle => "EmbedSubtitle",
            PostProcessor.EmbedThumbnail => "EmbedThumbnail",
            PostProcessor.SubtitlesConverter => "SubtitlesConverter",
            PostProcessor.ThumbnailsConverter => "ThumbnailsConverter",
            PostProcessor.FixupStretched => "FixupStretched",
            PostProcessor.FixupM4a => "FixupM4a",
            PostProcessor.FixupM3u8 => "FixupM3u8",
            PostProcessor.FixupTimestamp => "FixupTimestamp",
            PostProcessor.FixupDuration => "FixupDuration",
            _ => string.Empty
        };
        if (PostProcessor == PostProcessor.None && Executable == Executable.None)
        {
            return Args;
        }
        else if (PostProcessor != PostProcessor.None && Executable != Executable.None)
        {
            return $"{ytdlpPostProcessor}+{ytdlpExecutable}:{Args}";
        }
        else if (PostProcessor != PostProcessor.None)
        {
            return $"{ytdlpPostProcessor}:{Args}";
        }
        else if (Executable != Executable.None)
        {
            return $"{ytdlpExecutable}:{Args}";
        }
        return string.Empty;
    }

    public static bool operator >(PostProcessorArgument left, PostProcessorArgument right) => left.CompareTo(right) > 0;

    public static bool operator <(PostProcessorArgument left, PostProcessorArgument right) => left.CompareTo(right) < 0;

    public static bool operator >=(PostProcessorArgument left, PostProcessorArgument right) => left.CompareTo(right) >= 0;

    public static bool operator <=(PostProcessorArgument left, PostProcessorArgument right) => left.CompareTo(right) <= 0;

    public static bool operator ==(PostProcessorArgument left, PostProcessorArgument right) => left.Equals(right);

    public static bool operator !=(PostProcessorArgument left, PostProcessorArgument right) => !left.Equals(right);

    protected void OnPropertyChanged([CallerMemberName] string? propertyName = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
}
