#include "post_processor.h"
#include <format>
#include <utility>

namespace parabolic::post_processor
{
    Argument::Argument(std::string name, Step step, Executable executable, std::string args)
        : m_name{ std::move(name) },
          m_step{ step },
          m_executable{ executable },
          m_args{ std::move(args) }
    {
    }

    const std::string& Argument::name() const
    {
        return m_name;
    }

    void Argument::set_name(std::string value)
    {
        m_name = std::move(value);
    }

    Step Argument::step() const
    {
        return m_step;
    }

    void Argument::set_step(Step value)
    {
        m_step = value;
    }

    Executable Argument::executable() const
    {
        return m_executable;
    }

    void Argument::set_executable(Executable value)
    {
        m_executable = value;
    }

    const std::string& Argument::args() const
    {
        return m_args;
    }

    void Argument::set_args(std::string value)
    {
        m_args = std::move(value);
    }

    std::string Argument::str() const
    {
        std::string executable;
        switch (m_executable)
        {
        case Executable::AtomicParsley:
            executable = "AtomicParsley";
            break;
        case Executable::FFmpeg:
            executable = "ffmpeg";
            break;
        case Executable::FFprobe:
            executable = "ffprobe";
            break;
        default:
            break;
        }
        std::string step;
        switch (m_step)
        {
        case Step::Merger:
            step = "Merger";
            break;
        case Step::ModifyChapters:
            step = "ModifyChapters";
            break;
        case Step::SplitChapters:
            step = "SplitChapters";
            break;
        case Step::ExtractAudio:
            step = "ExtractAudio";
            break;
        case Step::VideoRemuxer:
            step = "VideoRemuxer";
            break;
        case Step::VideoConverter:
            step = "VideoConverter";
            break;
        case Step::Metadata:
            step = "Metadata";
            break;
        case Step::EmbedSubtitle:
            step = "EmbedSubtitle";
            break;
        case Step::EmbedThumbnail:
            step = "EmbedThumbnail";
            break;
        case Step::SubtitlesConverter:
            step = "SubtitlesConverter";
            break;
        case Step::ThumbnailsConverter:
            step = "ThumbnailsConverter";
            break;
        case Step::FixupStretched:
            step = "FixupStretched";
            break;
        case Step::FixupM4a:
            step = "FixupM4a";
            break;
        case Step::FixupM3u8:
            step = "FixupM3u8";
            break;
        case Step::FixupTimestamp:
            step = "FixupTimestamp";
            break;
        case Step::FixupDuration:
            step = "FixupDuration";
            break;
        default:
            break;
        }
        if (m_step == Step::None && m_executable == Executable::None)
        {
            return m_args;
        }
        if (m_step != Step::None && m_executable != Executable::None)
        {
            return std::format("{}+{}:{}", step, executable, m_args);
        }
        if (m_step != Step::None)
        {
            return std::format("{}:{}", step, m_args);
        }
        return std::format("{}:{}", executable, m_args);
    }

    std::strong_ordering Argument::operator<=>(const Argument& rhs) const
    {
        return m_name <=> rhs.m_name;
    }

    bool Argument::operator==(const Argument& rhs) const
    {
        return m_name == rhs.m_name && m_step == rhs.m_step && m_executable == rhs.m_executable && m_args == rhs.m_args;
    }
}
