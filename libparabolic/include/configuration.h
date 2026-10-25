#pragma once

#include <cstdint>
#include <filesystem>
#include <limits>
#include <map>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "browser.h"
#include "file.h"
#include "history.h"
#include "media.h"
#include "post_processor.h"
#include "subtitle.h"

namespace parabolic::configuration
{
    enum class Theme : std::uint8_t
    {
        Light,
        Dark,
        System,
    };

    class Aria2Options
    {
    public:
        [[nodiscard]] int max_connections_per_server() const;
        void set_max_connections_per_server(int value);
        [[nodiscard]] int min_split_size() const;
        void set_min_split_size(int value);
        [[nodiscard]] bool use_aria() const;
        void set_use_aria(bool value);
        bool operator==(const Aria2Options& rhs) const = default;
        friend void from_json(const nlohmann::json& json, Aria2Options& options);
        friend void to_json(nlohmann::json& json, const Aria2Options& options);

    private:
        int m_max_connections_per_server{ 16 };
        int m_min_split_size{ 20 };
        bool m_use_aria{ false };
    };

    class FormatOptions
    {
    public:
        [[nodiscard]] bool include_auto_generated_subtitles() const;
        void set_include_auto_generated_subtitles(bool value);
        [[nodiscard]] bool include_super_resolutions() const;
        void set_include_super_resolutions(bool value);
        [[nodiscard]] media::AudioCodec preferred_audio_codec() const;
        void set_preferred_audio_codec(media::AudioCodec value);
        [[nodiscard]] media::FrameRate preferred_frame_rate() const;
        void set_preferred_frame_rate(media::FrameRate value);
        [[nodiscard]] subtitle::Format preferred_subtitle_format() const;
        void set_preferred_subtitle_format(subtitle::Format value);
        [[nodiscard]] media::VideoCodec preferred_video_codec() const;
        void set_preferred_video_codec(media::VideoCodec value);
        bool operator==(const FormatOptions& rhs) const = default;
        friend void from_json(const nlohmann::json& json, FormatOptions& options);
        friend void to_json(nlohmann::json& json, const FormatOptions& options);

    private:
        bool m_include_auto_generated_subtitles{ true };
        bool m_include_super_resolutions{ true };
        media::AudioCodec m_preferred_audio_codec{ media::AudioCodec::Any };
        media::FrameRate m_preferred_frame_rate{ media::FrameRate::Any };
        subtitle::Format m_preferred_subtitle_format{ subtitle::Format::Any };
        media::VideoCodec m_preferred_video_codec{ media::VideoCodec::Any };
    };

    class HistoryOptions
    {
    public:
        [[nodiscard]] history::Length length() const;
        void set_length(history::Length value);
        [[nodiscard]] bool sort_newest() const;
        void set_sort_newest(bool value);
        bool operator==(const HistoryOptions& rhs) const = default;
        friend void from_json(const nlohmann::json& json, HistoryOptions& options);
        friend void to_json(nlohmann::json& json, const HistoryOptions& options);

    private:
        history::Length m_length{ history::Length::OneWeek };
        bool m_sort_newest{ true };
    };

    class PostprocessingOptions
    {
    public:
        PostprocessingOptions();
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
        [[nodiscard]] const std::vector<post_processor::Argument>& postprocessing_arguments() const;
        void set_postprocessing_arguments(const std::vector<post_processor::Argument>& value);
        [[nodiscard]] int postprocessing_threads() const;
        void set_postprocessing_threads(int value);
        [[nodiscard]] bool remove_source_data() const;
        void set_remove_source_data(bool value);
        [[nodiscard]] bool translate_metadata_and_chapters() const;
        void set_translate_metadata_and_chapters(bool value);
        bool operator==(const PostprocessingOptions& rhs) const = default;
        friend void from_json(const nlohmann::json& json, PostprocessingOptions& options);
        friend void to_json(nlohmann::json& json, const PostprocessingOptions& options);

    private:
        bool m_crop_audio_thumbnails{ false };
        bool m_embed_chapters{ false };
        bool m_embed_metadata{ true };
        bool m_embed_subtitles{ true };
        bool m_embed_thumbnails{ true };
        std::vector<post_processor::Argument> m_postprocessing_arguments;
        int m_postprocessing_threads;
        bool m_remove_source_data{ false };
        bool m_translate_metadata_and_chapters{ true };
    };

    class PreviousDownloadOptions
    {
    public:
        PreviousDownloadOptions();
        [[nodiscard]] double audio_bitrate() const;
        void set_audio_bitrate(double value);
        [[nodiscard]] const std::map<file::Type, std::string>& audio_format_ids() const;
        void set_audio_format_ids(const std::map<file::Type, std::string>& value);
        [[nodiscard]] file::Type audio_only_file_type() const;
        void set_audio_only_file_type(file::Type value);
        [[nodiscard]] bool download_immediately_as_audio() const;
        void set_download_immediately_as_audio(bool value);
        [[nodiscard]] bool download_immediately_as_video() const;
        void set_download_immediately_as_video(bool value);
        [[nodiscard]] bool export_description() const;
        void set_export_description(bool value);
        [[nodiscard]] bool export_m3u() const;
        void set_export_m3u(bool value);
        [[nodiscard]] file::Type full_file_type() const;
        void set_full_file_type(file::Type value);
        [[nodiscard]] bool number_titles() const;
        void set_number_titles(bool value);
        [[nodiscard]] const std::string& post_processor_argument_name() const;
        void set_post_processor_argument_name(const std::string& value);
        [[nodiscard]] bool reverse_download_order() const;
        void set_reverse_download_order(bool value);
        [[nodiscard]] std::filesystem::path save_folder() const;
        void set_save_folder(const std::filesystem::path& value);
        [[nodiscard]] bool split_chapters() const;
        void set_split_chapters(bool value);
        [[nodiscard]] const std::vector<subtitle::Subtitle>& subtitle_languages() const;
        void set_subtitle_languages(const std::vector<subtitle::Subtitle>& value);
        [[nodiscard]] const std::map<file::Type, std::string>& video_format_ids() const;
        void set_video_format_ids(const std::map<file::Type, std::string>& value);
        [[nodiscard]] file::Type video_only_file_type() const;
        void set_video_only_file_type(file::Type value);
        [[nodiscard]] const media::VideoResolution& video_resolution() const;
        void set_video_resolution(const media::VideoResolution& value);
        bool operator==(const PreviousDownloadOptions& rhs) const = default;
        friend void from_json(const nlohmann::json& json, PreviousDownloadOptions& options);
        friend void to_json(nlohmann::json& json, const PreviousDownloadOptions& options);

    private:
        double m_audio_bitrate{ std::numeric_limits<double>::max() };
        std::map<file::Type, std::string> m_audio_format_ids;
        file::Type m_audio_only_file_type{ file::Type::Mp3 };
        bool m_download_immediately_as_audio{ false };
        bool m_download_immediately_as_video{ false };
        bool m_export_description{ false };
        bool m_export_m3u{ false };
        file::Type m_full_file_type{ file::Type::Mp4 };
        bool m_number_titles{ false };
        std::string m_post_processor_argument_name;
        bool m_reverse_download_order{ false };
        std::filesystem::path m_save_folder;
        bool m_split_chapters{ false };
        std::vector<subtitle::Subtitle> m_subtitle_languages;
        std::map<file::Type, std::string> m_video_format_ids;
        file::Type m_video_only_file_type{ file::Type::Mp4 };
        media::VideoResolution m_video_resolution{ media::VideoResolution::best() };
    };

    class WindowGeometry
    {
    public:
        WindowGeometry() = default;
        WindowGeometry(int width, int height, bool is_maximized);
        WindowGeometry(int width, int height, bool is_maximized, int x, int y);
        [[nodiscard]] int height() const;
        void set_height(int value);
        [[nodiscard]] bool is_maximized() const;
        void set_is_maximized(bool value);
        [[nodiscard]] int width() const;
        void set_width(int value);
        [[nodiscard]] int x() const;
        void set_x(int value);
        [[nodiscard]] int y() const;
        void set_y(int value);
        bool operator==(const WindowGeometry& rhs) const = default;
        friend void from_json(const nlohmann::json& json, WindowGeometry& geometry);
        friend void to_json(nlohmann::json& json, const WindowGeometry& geometry);

    private:
        int m_height{ 600 };
        bool m_is_maximized{ false };
        int m_width{ 800 };
        int m_x{ 10 };
        int m_y{ 10 };
    };

    class YtdlpOptions
    {
    public:
        [[nodiscard]] browser::Browser cookies_browser() const;
        void set_cookies_browser(browser::Browser value);
        [[nodiscard]] const std::string& cookies_path() const;
        void set_cookies_path(const std::string& value);
        [[nodiscard]] const std::string& proxy_url() const;
        void set_proxy_url(const std::string& value);
        [[nodiscard]] std::optional<int> speed_limit() const;
        void set_speed_limit(std::optional<int> value);
        [[nodiscard]] bool use_part_files() const;
        void set_use_part_files(bool value);
        [[nodiscard]] bool youtube_sponsor_block() const;
        void set_youtube_sponsor_block(bool value);
        [[nodiscard]] const std::string& ytdlp_discovery_args() const;
        void set_ytdlp_discovery_args(const std::string& value);
        [[nodiscard]] const std::string& ytdlp_download_args() const;
        void set_ytdlp_download_args(const std::string& value);
        bool operator==(const YtdlpOptions& rhs) const = default;
        friend void from_json(const nlohmann::json& json, YtdlpOptions& options);
        friend void to_json(nlohmann::json& json, const YtdlpOptions& options);

    private:
        browser::Browser m_cookies_browser{ browser::Browser::None };
        std::string m_cookies_path;
        std::string m_proxy_url;
        std::optional<int> m_speed_limit;
        bool m_use_part_files{ true };
        bool m_youtube_sponsor_block{ false };
        std::string m_ytdlp_discovery_args;
        std::string m_ytdlp_download_args;
    };

    class Configuration
    {
    public:
        Configuration(std::string_view app_name);
        [[nodiscard]] bool allow_preview_updates() const;
        void set_allow_preview_updates(bool value);
        [[nodiscard]] Aria2Options aria2_options() const;
        void set_aria2_options(const Aria2Options& value);
        [[nodiscard]] bool automatically_check_for_updates() const;
        void set_automatically_check_for_updates(bool value);
        [[nodiscard]] FormatOptions format_options() const;
        void set_format_options(const FormatOptions& value);
        [[nodiscard]] HistoryOptions history_options() const;
        void set_history_options(const HistoryOptions& value);
        [[nodiscard]] bool include_media_id_in_title() const;
        void set_include_media_id_in_title(bool value);
        [[nodiscard]] bool limit_characters() const;
        void set_limit_characters(bool value);
        [[nodiscard]] int max_number_of_active_downloads() const;
        void set_max_number_of_active_downloads(int value);
        [[nodiscard]] bool overwrite_existing_files() const;
        void set_overwrite_existing_files(bool value);
        [[nodiscard]] PostprocessingOptions postprocessing_options() const;
        void set_postprocessing_options(const PostprocessingOptions& value);
        [[nodiscard]] bool prevent_suspend() const;
        void set_prevent_suspend(bool value);
        [[nodiscard]] PreviousDownloadOptions previous_download_options() const;
        void set_previous_download_options(const PreviousDownloadOptions& value);
        [[nodiscard]] bool show_disclaimer_on_startup() const;
        void set_show_disclaimer_on_startup(bool value);
        [[nodiscard]] Theme theme() const;
        void set_theme(Theme value);
        [[nodiscard]] std::string translation_language() const;
        void set_translation_language(const std::string& value);
        [[nodiscard]] WindowGeometry window_geometry() const;
        void set_window_geometry(const WindowGeometry& value);
        [[nodiscard]] YtdlpOptions ytdlp_options() const;
        void set_ytdlp_options(const YtdlpOptions& value);
        [[nodiscard]] std::string str() const;
        friend void from_json(const nlohmann::json& json, Configuration& config);
        friend void to_json(nlohmann::json& json, const Configuration& config);

    private:
        void save() const;
        template<typename T>
        void update_value(T& member, const T& value);
        mutable std::mutex m_mutex;
        bool m_allow_preview_updates{ false };
        Aria2Options m_aria2_options;
        bool m_automatically_check_for_updates{ true };
        FormatOptions m_format_options;
        HistoryOptions m_history_options;
        bool m_include_media_id_in_title{ true };
        bool m_limit_characters{ false };
        int m_max_number_of_active_downloads{ 5 };
        bool m_overwrite_existing_files{ true };
        std::filesystem::path m_path;
        PostprocessingOptions m_postprocessing_options;
        bool m_prevent_suspend{ false };
        PreviousDownloadOptions m_previous_download_options;
        bool m_show_disclaimer_on_startup{ true };
        Theme m_theme{ Theme::System };
        std::string m_translation_language;
        WindowGeometry m_window_geometry;
        YtdlpOptions m_ytdlp_options;
    };
}
