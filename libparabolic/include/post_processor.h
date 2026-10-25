#pragma once

#include <compare>
#include <nlohmann/json.hpp>
#include <string>

namespace parabolic
{
    enum class PostProcessorStep
    {
        None,
        Merger,
        ModifyChapters,
        SplitChapters,
        ExtractAudio,
        VideoRemuxer,
        VideoConverter,
        Metadata,
        EmbedSubtitle,
        EmbedThumbnail,
        SubtitlesConverter,
        ThumbnailsConverter,
        FixupStretched,
        FixupM4a,
        FixupM3u8,
        FixupTimestamp,
        FixupDuration
    };

    enum class PostProcessorExecutable
    {
        None,
        AtomicParsley,
        FFmpeg,
        FFprobe
    };

    class PostProcessorArgument
    {
    public:
        PostProcessorArgument(std::string name, PostProcessorStep step, PostProcessorExecutable executable, std::string args);
        [[nodiscard]] const std::string& name() const;
        void set_name(std::string value);
        [[nodiscard]] PostProcessorStep step() const;
        void set_step(PostProcessorStep value);
        [[nodiscard]] PostProcessorExecutable executable() const;
        void set_executable(PostProcessorExecutable value);
        [[nodiscard]] const std::string& args() const;
        void set_args(std::string value);
        [[nodiscard]] std::string str() const;
        std::strong_ordering operator<=>(const PostProcessorArgument& rhs) const;
        bool operator==(const PostProcessorArgument& rhs) const;
        friend void from_json(const nlohmann::json& json, PostProcessorArgument& argument);
        friend void to_json(nlohmann::json& json, const PostProcessorArgument& argument);

    private:
        std::string m_name;
        PostProcessorStep m_step;
        PostProcessorExecutable m_executable;
        std::string m_args;
    };

    inline void from_json(const nlohmann::json& json, PostProcessorArgument& argument)
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
            argument.m_step = static_cast<PostProcessorStep>(json.at("step").get<int>());
        }
        else if (json.contains("Step") && json["Step"].is_number_integer())
        {
            argument.m_step = static_cast<PostProcessorStep>(json.at("Step").get<int>());
        }
        else if (json.contains("postProcessor") && json["postProcessor"].is_number_integer())
        {
            argument.m_step = static_cast<PostProcessorStep>(json.at("postProcessor").get<int>());
        }
        else if (json.contains("PostProcessor") && json["PostProcessor"].is_number_integer())
        {
            argument.m_step = static_cast<PostProcessorStep>(json.at("PostProcessor").get<int>());
        }
        if (json.contains("executable") && json["executable"].is_number_integer())
        {
            argument.m_executable = static_cast<PostProcessorExecutable>(json.at("executable").get<int>());
        }
        else if (json.contains("Executable") && json["Executable"].is_number_integer())
        {
            argument.m_executable = static_cast<PostProcessorExecutable>(json.at("Executable").get<int>());
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

    inline void to_json(nlohmann::json& json, const PostProcessorArgument& argument)
    {
        json = nlohmann::json{
            {"name", argument.m_name},
            {"step", static_cast<int>(argument.m_step)},
            {"executable", static_cast<int>(argument.m_executable)},
            {"args", argument.m_args}
        };
    }
}
