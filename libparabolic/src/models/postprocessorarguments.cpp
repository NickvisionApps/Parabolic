#include "models/postprocessorarguments.h"
#include <format>

namespace Nickvision::TubeConverter::Shared::Models
{
    static std::string postProcessorToString(PostProcessor postProcessor)
    {

    }

    static std::string executableToString(Executable executable)
    {

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
    }

    boost::json::object PostProcessorArguments::toJson() const
    {
        boost::json::object json;
        json["Name"] = m_name;
        json["PostProcessor"] = static_cast<int>(m_postProcessor);
        json["Executable"] = static_cast<int>(m_executable);
        json["Args"] = m_args;
    }
}
