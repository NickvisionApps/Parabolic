#include "postprocessing.h"
#include <algorithm>
#include <compare>
#include <format>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace parabolic::postprocessing
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

    void Argument::set_name(const std::string& value)
    {
        m_name = value;
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

    void Argument::set_args(const std::string& value)
    {
        m_args = value;
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

    Options::Options()
        : m_threads{ static_cast<int>(std::max(1U, std::thread::hardware_concurrency())) }
    {
    }

    const std::vector<Argument>& Options::arguments() const
    {
        return m_arguments;
    }

    void Options::set_arguments(const std::vector<Argument>& value)
    {
        m_arguments = value;
    }

    bool Options::crop_audio_thumbnails() const
    {
        return m_crop_audio_thumbnails;
    }

    void Options::set_crop_audio_thumbnails(bool value)
    {
        m_crop_audio_thumbnails = value;
    }

    bool Options::embed_chapters() const
    {
        return m_embed_chapters;
    }

    void Options::set_embed_chapters(bool value)
    {
        m_embed_chapters = value;
    }

    bool Options::embed_metadata() const
    {
        return m_embed_metadata;
    }

    void Options::set_embed_metadata(bool value)
    {
        m_embed_metadata = value;
    }

    bool Options::embed_subtitles() const
    {
        return m_embed_subtitles;
    }

    void Options::set_embed_subtitles(bool value)
    {
        m_embed_subtitles = value;
    }

    bool Options::embed_thumbnails() const
    {
        return m_embed_thumbnails;
    }

    void Options::set_embed_thumbnails(bool value)
    {
        m_embed_thumbnails = value;
    }

    bool Options::remove_source_data() const
    {
        return m_remove_source_data;
    }

    void Options::set_remove_source_data(bool value)
    {
        m_remove_source_data = value;
    }

    int Options::threads() const
    {
        return m_threads;
    }

    void Options::set_threads(int value)
    {
        m_threads = value;
    }

    void from_json(const nlohmann::json& json, Argument& argument)
    {
        if (json.contains("name") && json["name"].is_string())
        {
            argument.m_name = json.at("name").get<std::string>();
        }
        else if (json.contains("Name") && json["Name"].is_string())
        {
            argument.m_name = json.at("Name").get<std::string>();
        }
        if (json.contains("step") && json["step"].is_number_integer())
        {
            argument.m_step = static_cast<Step>(json.at("step").get<int>());
        }
        else if (json.contains("Step") && json["Step"].is_number_integer())
        {
            argument.m_step = static_cast<Step>(json.at("Step").get<int>());
        }
        else if (json.contains("postProcessor") && json["postProcessor"].is_number_integer())
        {
            argument.m_step = static_cast<Step>(json.at("postProcessor").get<int>());
        }
        else if (json.contains("PostProcessor") && json["PostProcessor"].is_number_integer())
        {
            argument.m_step = static_cast<Step>(json.at("PostProcessor").get<int>());
        }
        if (json.contains("executable") && json["executable"].is_number_integer())
        {
            argument.m_executable = static_cast<Executable>(json.at("executable").get<int>());
        }
        else if (json.contains("Executable") && json["Executable"].is_number_integer())
        {
            argument.m_executable = static_cast<Executable>(json.at("Executable").get<int>());
        }
        if (json.contains("args") && json["args"].is_string())
        {
            argument.m_args = json.at("args").get<std::string>();
        }
        else if (json.contains("Args") && json["Args"].is_string())
        {
            argument.m_args = json.at("Args").get<std::string>();
        }
    }

    void from_json(const nlohmann::json& json, Options& options)
    {
        if (json.contains("cropAudioThumbnails") && json["cropAudioThumbnails"].is_boolean())
        {
            options.m_crop_audio_thumbnails = json.at("cropAudioThumbnails").get<bool>();
        }
        else if (json.contains("CropAudioThumbnails") && json["CropAudioThumbnails"].is_boolean())
        {
            options.m_crop_audio_thumbnails = json.at("CropAudioThumbnails").get<bool>();
        }
        if (json.contains("embedChapters") && json["embedChapters"].is_boolean())
        {
            options.m_embed_chapters = json.at("embedChapters").get<bool>();
        }
        else if (json.contains("EmbedChapters") && json["EmbedChapters"].is_boolean())
        {
            options.m_embed_chapters = json.at("EmbedChapters").get<bool>();
        }
        if (json.contains("embedMetadata") && json["embedMetadata"].is_boolean())
        {
            options.m_embed_metadata = json.at("embedMetadata").get<bool>();
        }
        else if (json.contains("EmbedMetadata") && json["EmbedMetadata"].is_boolean())
        {
            options.m_embed_metadata = json.at("EmbedMetadata").get<bool>();
        }
        if (json.contains("embedSubtitles") && json["embedSubtitles"].is_boolean())
        {
            options.m_embed_subtitles = json.at("embedSubtitles").get<bool>();
        }
        else if (json.contains("EmbedSubtitles") && json["EmbedSubtitles"].is_boolean())
        {
            options.m_embed_subtitles = json.at("EmbedSubtitles").get<bool>();
        }
        if (json.contains("embedThumbnails") && json["embedThumbnails"].is_boolean())
        {
            options.m_embed_thumbnails = json.at("embedThumbnails").get<bool>();
        }
        else if (json.contains("EmbedThumbnails") && json["EmbedThumbnails"].is_boolean())
        {
            options.m_embed_thumbnails = json.at("EmbedThumbnails").get<bool>();
        }
        if (json.contains("postprocessingArguments") && json["postprocessingArguments"].is_array())
        {
            options.m_arguments.clear();
            for (const nlohmann::json& item : json["postprocessingArguments"])
            {
                Argument argument{ "", Step::None, Executable::None, "" };
                from_json(item, argument);
                options.m_arguments.push_back(argument);
            }
        }
        if (json.contains("postprocessingThreads") && json["postprocessingThreads"].is_number_integer())
        {
            options.m_threads = std::clamp(json.at("postprocessingThreads").get<int>(), 1,
                                           static_cast<int>(std::max(1U, std::thread::hardware_concurrency())));
        }
        else if (json.contains("PostprocessingThreads") && json["PostprocessingThreads"].is_number_integer())
        {
            options.m_threads = std::clamp(json.at("PostprocessingThreads").get<int>(), 1,
                                           static_cast<int>(std::max(1U, std::thread::hardware_concurrency())));
        }
        if (json.contains("removeSourceData") && json["removeSourceData"].is_boolean())
        {
            options.m_remove_source_data = json.at("removeSourceData").get<bool>();
        }
        else if (json.contains("RemoveSourceData") && json["RemoveSourceData"].is_boolean())
        {
            options.m_remove_source_data = json.at("RemoveSourceData").get<bool>();
        }
    }

    void to_json(nlohmann::json& json, const Argument& argument)
    {
        json = nlohmann::json{
            { "name", argument.m_name },
            { "step", static_cast<int>(argument.m_step) },
            { "executable", static_cast<int>(argument.m_executable) },
            { "args", argument.m_args },
        };
    }

    void to_json(nlohmann::json& json, const Options& options)
    {
        json = nlohmann::json{
            { "cropAudioThumbnails", options.m_crop_audio_thumbnails },
            { "embedChapters", options.m_embed_chapters },
            { "embedMetadata", options.m_embed_metadata },
            { "embedSubtitles", options.m_embed_subtitles },
            { "embedThumbnails", options.m_embed_thumbnails },
            { "postprocessingArguments", nlohmann::json::array() },
            { "postprocessingThreads", options.m_threads },
            { "removeSourceData", options.m_remove_source_data },
        };
        for (const Argument& item : options.m_arguments)
        {
            json["postprocessingArguments"].push_back(item);
        }
    }
}
