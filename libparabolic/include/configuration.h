#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <cstdint>
#include <filesystem>
#include <limits>
#include <map>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include <vector>
#include "aria2.h"
#include "discovery.h"
#include "extractor.h"
#include "file.h"
#include "history.h"
#include "postprocessing.h"
#include "subtitle.h"
#include "translation.h"
#include "updates.h"
#include "video.h"

namespace parabolic::configuration
{
    enum class Theme : std::uint8_t
    {
        Light,
        Dark,
        System,
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
        [[nodiscard]] const std::vector<std::string>& postprocessing_argument_names() const;
        void set_postprocessing_argument_names(const std::vector<std::string>& value);
        [[nodiscard]] bool reverse_download_order() const;
        void set_reverse_download_order(bool value);
        [[nodiscard]] std::filesystem::path save_folder() const;
        void set_save_folder(const std::filesystem::path& value);
        [[nodiscard]] bool split_chapters() const;
        void set_split_chapters(bool value);
        [[nodiscard]] const std::vector<subtitle::Language>& subtitle_languages() const;
        void set_subtitle_languages(const std::vector<subtitle::Language>& value);
        [[nodiscard]] const std::map<file::Type, std::string>& video_format_ids() const;
        void set_video_format_ids(const std::map<file::Type, std::string>& value);
        [[nodiscard]] file::Type video_only_file_type() const;
        void set_video_only_file_type(file::Type value);
        [[nodiscard]] const video::Resolution& video_resolution() const;
        void set_video_resolution(const video::Resolution& value);
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
        std::vector<std::string> m_postprocessing_argument_names;
        bool m_reverse_download_order{ false };
        std::filesystem::path m_save_folder;
        bool m_split_chapters{ false };
        std::vector<subtitle::Language> m_subtitle_languages;
        std::map<file::Type, std::string> m_video_format_ids;
        file::Type m_video_only_file_type{ file::Type::Mp4 };
        video::Resolution m_video_resolution{ video::Resolution::best() };
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

    class Configuration
    {
    public:
        Configuration(std::string_view app_name);
        [[nodiscard]] aria2::Options aria2_options() const;
        void set_aria2_options(const aria2::Options& value);
        [[nodiscard]] bool automatically_check_for_updates() const;
        void set_automatically_check_for_updates(bool value);
        [[nodiscard]] discovery::Options discovery_options() const;
        void set_discovery_options(const discovery::Options& value);
        [[nodiscard]] extractor::Options extractor_options() const;
        void set_extractor_options(const extractor::Options& value);
        [[nodiscard]] history::Options history_options() const;
        void set_history_options(const history::Options& value);
        [[nodiscard]] bool include_media_id_in_title() const;
        void set_include_media_id_in_title(bool value);
        [[nodiscard]] bool limit_characters() const;
        void set_limit_characters(bool value);
        [[nodiscard]] int max_number_of_active_downloads() const;
        void set_max_number_of_active_downloads(int value);
        [[nodiscard]] bool overwrite_existing_files() const;
        void set_overwrite_existing_files(bool value);
        [[nodiscard]] postprocessing::Options postprocessing_options() const;
        void set_postprocessing_options(const postprocessing::Options& value);
        [[nodiscard]] bool prevent_suspend() const;
        void set_prevent_suspend(bool value);
        [[nodiscard]] PreviousDownloadOptions previous_download_options() const;
        void set_previous_download_options(const PreviousDownloadOptions& value);
        [[nodiscard]] bool show_disclaimer_on_startup() const;
        void set_show_disclaimer_on_startup(bool value);
        [[nodiscard]] subtitle::Options subtitle_options() const;
        void set_subtitle_options(const subtitle::Options& value);
        [[nodiscard]] Theme theme() const;
        void set_theme(Theme value);
        [[nodiscard]] translation::Options translation_options() const;
        void set_translation_options(const translation::Options& value);
        [[nodiscard]] updates::Options updates_options() const;
        void set_updates_options(const updates::Options& value);
        [[nodiscard]] WindowGeometry window_geometry() const;
        void set_window_geometry(const WindowGeometry& value);
        [[nodiscard]] std::string str() const;
        friend void from_json(const nlohmann::json& json, Configuration& config);
        friend void to_json(nlohmann::json& json, const Configuration& config);

    private:
        void save() const;
        mutable std::mutex m_mutex;
        aria2::Options m_aria2_options;
        bool m_automatically_check_for_updates{ true };
        discovery::Options m_discovery_options;
        extractor::Options m_extractor_options;
        history::Options m_history_options;
        bool m_include_media_id_in_title{ true };
        bool m_limit_characters{ false };
        int m_max_number_of_active_downloads{ 5 };
        bool m_overwrite_existing_files{ true };
        std::filesystem::path m_path;
        postprocessing::Options m_postprocessing_options;
        bool m_prevent_suspend{ false };
        PreviousDownloadOptions m_previous_download_options;
        bool m_show_disclaimer_on_startup{ true };
        subtitle::Options m_subtitle_options;
        Theme m_theme{ Theme::System };
        translation::Options m_translation_options;
        updates::Options m_updates_options;
        WindowGeometry m_window_geometry;
    };
}

#endif
