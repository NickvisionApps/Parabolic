#ifndef DISCOVERY_H
#define DISCOVERY_H

#include <compare>
#include <cstddef>
#include <filesystem>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <vector>
#include "audio.h"
#include "extractor.h"
#include "file.h"
#include "keyring.h"
#include "subtitle.h"
#include "video.h"

namespace parabolic::discovery
{
    class Options
    {
    public:
        [[nodiscard]] bool include_auto_generated_subtitles() const;
        void set_include_auto_generated_subtitles(bool value);
        [[nodiscard]] bool include_media_id_in_title() const;
        void set_include_media_id_in_title(bool value);
        [[nodiscard]] bool include_super_resolutions() const;
        void set_include_super_resolutions(bool value);
        [[nodiscard]] bool limit_characters() const;
        void set_limit_characters(bool value);
        [[nodiscard]] audio::Codec preferred_audio_codec() const;
        void set_preferred_audio_codec(audio::Codec value);
        [[nodiscard]] video::FrameRate preferred_frame_rate() const;
        void set_preferred_frame_rate(video::FrameRate value);
        [[nodiscard]] subtitle::Type preferred_subtitle_format() const;
        void set_preferred_subtitle_format(subtitle::Type value);
        [[nodiscard]] video::Codec preferred_video_codec() const;
        void set_preferred_video_codec(video::Codec value);
        bool operator==(const Options& rhs) const = default;
        friend void from_json(const nlohmann::json& json, Options& options);
        friend void to_json(nlohmann::json& json, const Options& options);

    private:
        bool m_include_auto_generated_subtitles{ true };
        bool m_include_media_id_in_title{ true };
        bool m_include_super_resolutions{ true };
#ifdef _WIN32
        bool m_limit_characters{ true };
#else
        bool m_limit_characters{ false };
#endif
        audio::Codec m_preferred_audio_codec{ audio::Codec::Any };
        video::FrameRate m_preferred_frame_rate{ video::FrameRate::Any };
        subtitle::Type m_preferred_subtitle_format{ subtitle::Type::Any };
        video::Codec m_preferred_video_codec{ video::Codec::Any };
    };

    class Entry
    {
    public:
        Entry() = default;
        Entry(const nlohmann::json& json, const Options& options, std::string suggested_save_folder,
              std::string suggested_filename);
        [[nodiscard]] const std::string& url() const;
        [[nodiscard]] const std::string& title() const;
        [[nodiscard]] int playlist_index() const;
        [[nodiscard]] bool requires_playlist_items_option() const;
        void set_requires_playlist_items_option(bool value);
        [[nodiscard]] file::Kind kind() const;
        [[nodiscard]] const file::TimeFrame& time_frame() const;
        [[nodiscard]] const std::vector<file::Format>& formats() const;
        [[nodiscard]] const std::vector<subtitle::Language>& subtitle_languages() const;
        [[nodiscard]] const std::string& thumbnail_url() const;
        [[nodiscard]] const std::string& suggested_save_folder() const;
        [[nodiscard]] std::strong_ordering operator<=>(const Entry& rhs) const;
        [[nodiscard]] bool operator==(const Entry& rhs) const;
        friend void to_json(nlohmann::json& json, const Entry& entry);

    private:
        std::string m_url;
        std::string m_title;
        int m_playlist_index{ -1 };
        bool m_requires_playlist_items_option{ false };
        file::Kind m_kind{ file::Kind::Unknown };
        file::TimeFrame m_time_frame;
        std::vector<file::Format> m_formats;
        std::vector<subtitle::Language> m_subtitle_languages;
        std::string m_thumbnail_url;
        std::string m_suggested_save_folder;
    };

    class Result
    {
    public:
        Result();
        Result(std::string url, std::string title, const std::vector<Result>& results);
        [[nodiscard]] unsigned int id() const;
        [[nodiscard]] const std::string& url() const;
        [[nodiscard]] const std::string& title() const;
        [[nodiscard]] bool has_suggested_save_folder() const;
        [[nodiscard]] const std::vector<Entry>& entries() const;
        [[nodiscard]] bool is_playlist() const;
        [[nodiscard]] bool empty() const;
        [[nodiscard]] size_t size() const;
        [[nodiscard]] std::vector<Entry>::const_iterator begin() const;
        [[nodiscard]] std::vector<Entry>::const_iterator end() const;
        [[nodiscard]] std::vector<Entry>::const_reverse_iterator rbegin() const;
        [[nodiscard]] std::vector<Entry>::const_reverse_iterator rend() const;
        [[nodiscard]] std::vector<Entry>::const_iterator cbegin() const;
        [[nodiscard]] std::vector<Entry>::const_iterator cend() const;
        [[nodiscard]] std::vector<Entry>::const_reverse_iterator crbegin() const;
        [[nodiscard]] std::vector<Entry>::const_reverse_iterator crend() const;
        void sort();
        [[nodiscard]] const Entry& operator[](size_t index) const;
        [[nodiscard]] bool operator==(const Result& rhs) const = default;

    private:
        unsigned int m_id{ 0 };
        std::string m_url;
        std::string m_title;
        bool m_has_suggested_save_folder{ false };
        std::vector<Entry> m_entries;
    };

    class Manager
    {
    public:
        Manager() = default;
        void set_options(const Options& discovery_options, const extractor::Options& extractor_options);
        [[nodiscard]] Result discover_file(const std::filesystem::path& path,
                                           const std::optional<keyring::Credential>& credential = std::nullopt) const;
        [[nodiscard]] Result discover_url(const std::string& url,
                                          const std::optional<keyring::Credential>& credential = std::nullopt) const;

    private:
        [[nodiscard]] Result discover(const std::string& url, const std::filesystem::path& suggested_save_folder,
                                      const std::string& suggested_filename,
                                      const std::optional<keyring::Credential>& credential) const;
        Options m_discovery_options;
        extractor::Options m_extractor_options;
    };
}

#endif
