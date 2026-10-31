#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <filesystem>
#include <limits>
#include <map>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <vector>
#include "file.h"
#include "subtitle.h"
#include "video.h"

namespace parabolic::download
{
    class Options
    {
    public:
        [[nodiscard]] int max_number_of_active_downloads() const;
        void set_max_number_of_active_downloads(int value);
        [[nodiscard]] bool overwrite_existing_files() const;
        void set_overwrite_existing_files(bool value);
        [[nodiscard]] bool prevent_suspend() const;
        void set_prevent_suspend(bool value);
        bool operator==(const Options& rhs) const = default;
        friend void from_json(const nlohmann::json& json, Options& options);
        friend void to_json(nlohmann::json& json, const Options& options);

    private:
        int m_max_number_of_active_downloads{ 5 };
        bool m_overwrite_existing_files{ true };
        bool m_prevent_suspend{ false };
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

    class Manager
    {
    public:
        Manager() = default;
        void set_options(const Options& options);

    private:
        Options m_options;
    };
}

#endif
