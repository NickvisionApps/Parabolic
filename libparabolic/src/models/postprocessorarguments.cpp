#include "models/postprocessorarguments.h"
#include <format>

namespace Nickvision::TubeConverter::Shared::Models
{
    static std::string postProcessorToString(PostProcessor postProcessor)
    {
        switch(postProcessor)
        {
        case PostProcessor::Merger:
            return "Merger";
        case PostProcessor::ModifyChapters:
            return "ModifyChapters";
        case PostProcessor::SplitChapters:
            return "SplitChapters";
        case PostProcessor::ExtractAudio:
            return "ExtractAudio";
        case PostProcessor::VideoRemuxer:
            return "VideoRemuxer";
        case PostProcessor::VideoConverter:
            return "VideoConverter";
        case PostProcessor::Metadata:
            return "Metadata";
        case PostProcessor::EmbedSubtitle:
            return "EmbedSubtitle";
        case PostProcessor::EmbedThumbnail:
            return "EmbedThumbnail";
        case PostProcessor::SubtitlesConverter:
            return "SubtitlesConverter";
        case PostProcessor::ThumbnailsConverter:
            return "ThumbnailsConverter";
        case PostProcessor::FixupStretched:
            return "FixupStretched";
        case PostProcessor::FixupM4a:
            return "FixupM4a";
        case PostProcessor::FixupM3u8:
            return "FixupM3u8";
        case PostProcessor::FixupTimestamp:
            return "FixupTimestamp";
        case PostProcessor::FixupDuration:
            return "FixupDuration";
        }
        return "";
    }

    static std::string executableToString(Executable executable)
    {
        switch(executable)
        {
        case Executable::AtomicParsley:
            return "AtomicParsley";
        case Executable::FFmpeg:
            return "ffmpeg";
        case Executable::FFprobe:
            return "ffprobe";
        }
        return "";
    }

    PostProcessorArguments::PostProcessorArguments(const std::string& name, PostProcessor postProcessor, Executable executable, const std::string& args)
        : m_name{ name },
        m_postProcessor{ postProcessor },
        m_executable{ executable },
        m_args{ args }
    {

    }

    PostProcessorArguments::PostProcessorArguments(boost::json::object json)
        : m_name{ json["Name"].is_string() ? json["Name"].as_string() : "" },
        m_postProcessor{ json["PostProcessor"].is_int64() ? static_cast<PostProcessor>(json["PostProcessor"].as_int64()) : PostProcessor::None },
        m_executable{ json["Executable"].is_int64() ? static_cast<Executable>(json["Executable"].as_int64()) : Executable::None },
        m_args{ json["Args"].is_string() ? json["Args"].as_string() : "" }
    {

    }

    const std::string& PostProcessorArguments::getName() const
    {
        return m_name;
    }

    void PostProcessorArguments::setName(const std::string& name)
    {
        m_name = name;
    }

    PostProcessor PostProcessorArguments::getPostProcessor() const
    {
        return m_postProcessor;
    }

    void PostProcessorArguments::setPostProcessor(PostProcessor postProcessor)
    {
        m_postProcessor = postProcessor;
    }

    Executable PostProcessorArguments::getExecutable() const
    {
        return m_executable;
    }

    void PostProcessorArguments::setExecutable(Executable executable)
    {
        m_executable = executable;
    }

    const std::string& PostProcessorArguments::getArgs() const
    {
       return m_args;
    }

    void PostProcessorArguments::setArgs(const std::string& args)
    {
       m_args = args;
    }

    std::string PostProcessorArguments::str() const
    {
        if(m_postProcessor == PostProcessor::None && m_executable == Executable::None)
        {
            return m_args;
        }
        else if(m_postProcessor != PostProcessor::None && m_executable != Executable::None)
        {
            return std::format("{}+{}:{}", postProcessorToString(m_postProcessor), executableToString(m_executable), m_args);
        }
        else if(m_postProcessor != PostProcessor::None)
        {
            return std::format("{}:{}", postProcessorToString(m_postProcessor), m_args);
        }
        else if(m_executable != Executable::None)
        {
            return std::format("{}:{}", executableToString(m_executable), m_args);
        }
        return "";
    }

    boost::json::object PostProcessorArguments::toJson() const
    {
        boost::json::object json;
        json["Name"] = m_name;
        json["PostProcessor"] = static_cast<int>(m_postProcessor);
        json["Executable"] = static_cast<int>(m_executable);
        json["Args"] = m_args;
        return json;
    }

    bool PostProcessorArguments::operator==(const PostProcessorArguments& other) const
    {
        if(m_name == other.m_name)
        {
            return true;
        }
        return m_postProcessor == other.m_postProcessor && m_executable == other.m_executable && m_args == other.m_args;
    }

    bool PostProcessorArguments::operator!=(const PostProcessorArguments& other) const
    {
        return !(operator==(other));
    }
}
