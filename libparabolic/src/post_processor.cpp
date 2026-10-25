#include "post_processor.h"
#include <format>
#include <utility>

namespace parabolic
{
    PostProcessorArgument::PostProcessorArgument(std::string name, PostProcessorStep step, PostProcessorExecutable executable, std::string args)
        : m_name{ std::move(name) },
        m_step{ step },
        m_executable{ executable },
        m_args{ std::move(args) }
    {
    }

    const std::string& PostProcessorArgument::name() const
    {
        return m_name;
    }

    void PostProcessorArgument::set_name(std::string value)
    {
        m_name = std::move(value);
    }

    PostProcessorStep PostProcessorArgument::step() const
    {
        return m_step;
    }

    void PostProcessorArgument::set_step(PostProcessorStep value)
    {
        m_step = value;
    }

    PostProcessorExecutable PostProcessorArgument::executable() const
    {
        return m_executable;
    }

    void PostProcessorArgument::set_executable(PostProcessorExecutable value)
    {
        m_executable = value;
    }

    const std::string& PostProcessorArgument::args() const
    {
        return m_args;
    }

    void PostProcessorArgument::set_args(std::string value)
    {
        m_args = std::move(value);
    }

    std::string PostProcessorArgument::str() const
    {
        std::string executable;
        switch (m_executable)
        {
        case PostProcessorExecutable::AtomicParsley:
            executable = "AtomicParsley";
            break;
        case PostProcessorExecutable::FFmpeg:
            executable = "ffmpeg";
            break;
        case PostProcessorExecutable::FFprobe:
            executable = "ffprobe";
            break;
        default:
            break;
        }
        std::string step;
        switch (m_step)
        {
        case PostProcessorStep::Merger:
            step = "Merger";
            break;
        case PostProcessorStep::ModifyChapters:
            step = "ModifyChapters";
            break;
        case PostProcessorStep::SplitChapters:
            step = "SplitChapters";
            break;
        case PostProcessorStep::ExtractAudio:
            step = "ExtractAudio";
            break;
        case PostProcessorStep::VideoRemuxer:
            step = "VideoRemuxer";
            break;
        case PostProcessorStep::VideoConverter:
            step = "VideoConverter";
            break;
        case PostProcessorStep::Metadata:
            step = "Metadata";
            break;
        case PostProcessorStep::EmbedSubtitle:
            step = "EmbedSubtitle";
            break;
        case PostProcessorStep::EmbedThumbnail:
            step = "EmbedThumbnail";
            break;
        case PostProcessorStep::SubtitlesConverter:
            step = "SubtitlesConverter";
            break;
        case PostProcessorStep::ThumbnailsConverter:
            step = "ThumbnailsConverter";
            break;
        case PostProcessorStep::FixupStretched:
            step = "FixupStretched";
            break;
        case PostProcessorStep::FixupM4a:
            step = "FixupM4a";
            break;
        case PostProcessorStep::FixupM3u8:
            step = "FixupM3u8";
            break;
        case PostProcessorStep::FixupTimestamp:
            step = "FixupTimestamp";
            break;
        case PostProcessorStep::FixupDuration:
            step = "FixupDuration";
            break;
        default:
            break;
        }
        if (m_step == PostProcessorStep::None && m_executable == PostProcessorExecutable::None)
        {
            return m_args;
        }
        if (m_step != PostProcessorStep::None && m_executable != PostProcessorExecutable::None)
        {
            return std::format("{}+{}:{}", step, executable, m_args);
        }
        if (m_step != PostProcessorStep::None)
        {
            return std::format("{}:{}", step, m_args);
        }
        return std::format("{}:{}", executable, m_args);
    }

    std::strong_ordering PostProcessorArgument::operator<=>(const PostProcessorArgument& rhs) const
    {
        return m_name <=> rhs.m_name;
    }

    bool PostProcessorArgument::operator==(const PostProcessorArgument& rhs) const
    {
        return m_name == rhs.m_name && m_step == rhs.m_step && m_executable == rhs.m_executable && m_args == rhs.m_args;
    }
}
