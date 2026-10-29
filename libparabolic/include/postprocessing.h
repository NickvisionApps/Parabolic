#ifndef POSTPROCESSING_H
#define POSTPROCESSING_H

#include <compare>
#include <cstdint>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <vector>

namespace parabolic::postprocessing
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

    class Options
    {
    public:
        Options();
        [[nodiscard]] const std::vector<Argument>& arguments() const;
        void set_arguments(const std::vector<Argument>& value);
        [[nodiscard]] bool crop_audio_thumbnails() const;
        void set_crop_audio_thumbnails(bool value);
        [[nodiscard]] bool embed_chapters() const;
        void set_embed_chapters(bool value);
        [[nodiscard]] bool embed_metadata() const;
        void set_embed_metadata(bool value);
        [[nodiscard]] bool embed_subtitles() const;
        void set_embed_subtitles(bool value);
        [[nodiscard]] bool embed_thumbnails() const;
        void set_embed_thumbnails(bool value);
        [[nodiscard]] bool remove_source_data() const;
        void set_remove_source_data(bool value);
        [[nodiscard]] int threads() const;
        void set_threads(int value);
        bool operator==(const Options& rhs) const = default;
        friend void from_json(const nlohmann::json& json, Options& options);
        friend void to_json(nlohmann::json& json, const Options& options);

    private:
        std::vector<Argument> m_arguments;
        bool m_crop_audio_thumbnails{ false };
        bool m_embed_chapters{ false };
        bool m_embed_metadata{ true };
        bool m_embed_subtitles{ true };
        bool m_embed_thumbnails{ true };
        bool m_remove_source_data{ false };
        int m_threads;
    };
}

#endif
