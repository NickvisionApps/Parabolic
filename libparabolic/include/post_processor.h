#pragma once

#include <compare>
#include <cstdint>
#include <nlohmann/json_fwd.hpp>
#include <string>

namespace parabolic::post_processor
{
    enum class Step : std::uint8_t
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
        FixupDuration,
    };

    enum class Executable : std::uint8_t
    {
        None,
        AtomicParsley,
        FFmpeg,
        FFprobe,
    };

    class Argument
    {
    public:
        Argument(std::string name, Step step, Executable executable, std::string args);
        [[nodiscard]] const std::string& name() const;
        void set_name(std::string value);
        [[nodiscard]] Step step() const;
        void set_step(Step value);
        [[nodiscard]] Executable executable() const;
        void set_executable(Executable value);
        [[nodiscard]] const std::string& args() const;
        void set_args(std::string value);
        [[nodiscard]] std::string str() const;
        std::strong_ordering operator<=>(const Argument& rhs) const;
        bool operator==(const Argument& rhs) const;
        friend void from_json(const nlohmann::json& json, Argument& argument);
        friend void to_json(nlohmann::json& json, const Argument& argument);

    private:
        std::string m_name;
        Step m_step;
        Executable m_executable;
        std::string m_args;
    };
}
