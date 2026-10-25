#include "configuration.h"
#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <map>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <sago/platform_folders.h>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <vector>
#include "browser.h"
#include "environment.h"
#include "file.h"
#include "history.h"
#include "media.h"
#include "post_processor.h"
#include "subtitle.h"
#include "translator.h"
#ifdef _WIN32
#include "string_manip.h"
#endif

using namespace parabolic::translator;

namespace parabolic::configuration
{
    PreviousDownloadOptions::PreviousDownloadOptions()
        : m_audio_format_ids{
              { file::Type::VideoGeneric, media::Format::best_audio().id() },
              { file::Type::Mp4, media::Format::best_audio().id() },
              { file::Type::Mkv, media::Format::best_audio().id() },
              { file::Type::Webm, media::Format::best_audio().id() },
              { file::Type::Mov, media::Format::best_audio().id() },
              { file::Type::Avi, media::Format::best_audio().id() },
              { file::Type::AudioGeneric, media::Format::best_audio().id() },
              { file::Type::Mp3, media::Format::best_audio().id() },
              { file::Type::M4a, media::Format::best_audio().id() },
              { file::Type::Opus, media::Format::best_audio().id() },
              { file::Type::Flac, media::Format::best_audio().id() },
              { file::Type::Wav, media::Format::best_audio().id() },
              { file::Type::Ogg, media::Format::best_audio().id() },
          },
#ifdef _WIN32
          m_save_folder{ std::filesystem::path(string_manip::wstr(sago::getDownloadFolder())) },
#else
          m_save_folder{ sago::getDownloadFolder() },
#endif
          m_video_format_ids{
              { file::Type::VideoGeneric, media::Format::best_video().id() },
              { file::Type::Mp4, media::Format::best_video().id() },
              { file::Type::Mkv, media::Format::best_video().id() },
              { file::Type::Webm, media::Format::best_video().id() },
              { file::Type::Mov, media::Format::best_video().id() },
              { file::Type::Avi, media::Format::best_video().id() },
              { file::Type::AudioGeneric, media::Format::none_video().id() },
              { file::Type::Mp3, media::Format::none_video().id() },
              { file::Type::M4a, media::Format::none_video().id() },
              { file::Type::Opus, media::Format::none_video().id() },
              { file::Type::Flac, media::Format::none_video().id() },
              { file::Type::Wav, media::Format::none_video().id() },
              { file::Type::Ogg, media::Format::none_video().id() },
          }
    {
    }

    double PreviousDownloadOptions::audio_bitrate() const
    {
        return m_audio_bitrate;
    }

    void PreviousDownloadOptions::set_audio_bitrate(double value)
    {
        m_audio_bitrate = value;
    }

    const std::map<file::Type, std::string>& PreviousDownloadOptions::audio_format_ids() const
    {
        return m_audio_format_ids;
    }

    void PreviousDownloadOptions::set_audio_format_ids(const std::map<file::Type, std::string>& value)
    {
        m_audio_format_ids = value;
    }

    file::Type PreviousDownloadOptions::audio_only_file_type() const
    {
        return m_audio_only_file_type;
    }

    void PreviousDownloadOptions::set_audio_only_file_type(file::Type value)
    {
        m_audio_only_file_type = value;
    }

    bool PreviousDownloadOptions::download_immediately_as_audio() const
    {
        return m_download_immediately_as_audio;
    }

    void PreviousDownloadOptions::set_download_immediately_as_audio(bool value)
    {
        m_download_immediately_as_audio = value;
    }

    bool PreviousDownloadOptions::download_immediately_as_video() const
    {
        return m_download_immediately_as_video;
    }

    void PreviousDownloadOptions::set_download_immediately_as_video(bool value)
    {
        m_download_immediately_as_video = value;
    }

    bool PreviousDownloadOptions::export_description() const
    {
        return m_export_description;
    }

    void PreviousDownloadOptions::set_export_description(bool value)
    {
        m_export_description = value;
    }

    bool PreviousDownloadOptions::export_m3u() const
    {
        return m_export_m3u;
    }

    void PreviousDownloadOptions::set_export_m3u(bool value)
    {
        m_export_m3u = value;
    }

    file::Type PreviousDownloadOptions::full_file_type() const
    {
        return m_full_file_type;
    }

    void PreviousDownloadOptions::set_full_file_type(file::Type value)
    {
        m_full_file_type = value;
    }

    bool PreviousDownloadOptions::number_titles() const
    {
        return m_number_titles;
    }

    void PreviousDownloadOptions::set_number_titles(bool value)
    {
        m_number_titles = value;
    }

    const std::string& PreviousDownloadOptions::post_processor_argument_name() const
    {
        return m_post_processor_argument_name;
    }

    void PreviousDownloadOptions::set_post_processor_argument_name(const std::string& value)
    {
        m_post_processor_argument_name = value;
    }

    bool PreviousDownloadOptions::reverse_download_order() const
    {
        return m_reverse_download_order;
    }

    void PreviousDownloadOptions::set_reverse_download_order(bool value)
    {
        m_reverse_download_order = value;
    }

    std::filesystem::path PreviousDownloadOptions::save_folder() const
    {
        std::error_code error;
        if (!std::filesystem::exists(m_save_folder, error))
        {
#ifdef _WIN32
            return std::filesystem::path(string_manip::wstr(sago::getDownloadFolder()));
#else
            return sago::getDownloadFolder();
#endif
        }
        return m_save_folder;
    }

    void PreviousDownloadOptions::set_save_folder(const std::filesystem::path& value)
    {
        m_save_folder = value;
    }

    bool PreviousDownloadOptions::split_chapters() const
    {
        return m_split_chapters;
    }

    void PreviousDownloadOptions::set_split_chapters(bool value)
    {
        m_split_chapters = value;
    }

    const std::vector<subtitle::Subtitle>& PreviousDownloadOptions::subtitle_languages() const
    {
        return m_subtitle_languages;
    }

    void PreviousDownloadOptions::set_subtitle_languages(const std::vector<subtitle::Subtitle>& value)
    {
        m_subtitle_languages = value;
    }

    const std::map<file::Type, std::string>& PreviousDownloadOptions::video_format_ids() const
    {
        return m_video_format_ids;
    }

    void PreviousDownloadOptions::set_video_format_ids(const std::map<file::Type, std::string>& value)
    {
        m_video_format_ids = value;
    }

    file::Type PreviousDownloadOptions::video_only_file_type() const
    {
        return m_video_only_file_type;
    }

    void PreviousDownloadOptions::set_video_only_file_type(file::Type value)
    {
        m_video_only_file_type = value;
    }

    const media::VideoResolution& PreviousDownloadOptions::video_resolution() const
    {
        return m_video_resolution;
    }

    void PreviousDownloadOptions::set_video_resolution(const media::VideoResolution& value)
    {
        m_video_resolution = value;
    }

    WindowGeometry::WindowGeometry(int width, int height, bool is_maximized)
        : m_height{ height },
          m_is_maximized{ is_maximized },
          m_width{ width }
    {
    }

    WindowGeometry::WindowGeometry(int width, int height, bool is_maximized, int x, int y)
        : m_height{ height },
          m_is_maximized{ is_maximized },
          m_width{ width },
          m_x{ x },
          m_y{ y }
    {
    }

    int WindowGeometry::height() const
    {
        return m_height;
    }

    void WindowGeometry::set_height(int value)
    {
        m_height = value;
    }

    bool WindowGeometry::is_maximized() const
    {
        return m_is_maximized;
    }

    void WindowGeometry::set_is_maximized(bool value)
    {
        m_is_maximized = value;
    }

    int WindowGeometry::width() const
    {
        return m_width;
    }

    void WindowGeometry::set_width(int value)
    {
        m_width = value;
    }

    int WindowGeometry::x() const
    {
        return m_x;
    }

    void WindowGeometry::set_x(int value)
    {
        m_x = value;
    }

    int WindowGeometry::y() const
    {
        return m_y;
    }

    void WindowGeometry::set_y(int value)
    {
        m_y = value;
    }

    Configuration::Configuration(std::string_view app_name)
#ifdef _WIN32
        : m_path{ environment::is_portable() ? environment::executable_directory() / "config.json"
                                             : std::filesystem::path(string_manip::wstr(sago::getConfigHome())) /
                                                       app_name / "config.json" },
#else
        : m_path{
              environment::is_portable() ? environment::executable_directory() / "config.json"
                                         : std::filesystem::path(sago::getConfigHome()) / app_name / "config.json",
          },
#endif
          m_translation_language{ Translator::follow_system_language() },
          m_postprocessing_threads{ static_cast<int>(std::thread::hardware_concurrency()) }
    {
#ifdef _WIN32
        m_limit_characters = true;
#endif
        try
        {
            std::ifstream file{ m_path };
            if (file)
            {
                nlohmann::json json = nlohmann::json::parse(file, nullptr, false, true);
                if (!json.is_discarded() && json.is_object())
                {
                    from_json(json, *this);
                }
            }
        }
        catch (const std::exception&)
        {
            return;
        }
    }

    bool Configuration::allow_preview_updates() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_allow_preview_updates;
    }

    void Configuration::set_allow_preview_updates(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_allow_preview_updates)
        {
            return;
        }
        m_allow_preview_updates = value;
        save();
    }

    int Configuration::aria_max_connections_per_server() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_aria_max_connections_per_server;
    }

    void Configuration::set_aria_max_connections_per_server(int value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_aria_max_connections_per_server)
        {
            return;
        }
        m_aria_max_connections_per_server = value;
        save();
    }

    int Configuration::aria_min_split_size() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_aria_min_split_size;
    }

    void Configuration::set_aria_min_split_size(int value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_aria_min_split_size)
        {
            return;
        }
        m_aria_min_split_size = value;
        save();
    }

    bool Configuration::automatically_check_for_updates() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_automatically_check_for_updates;
    }

    void Configuration::set_automatically_check_for_updates(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_automatically_check_for_updates)
        {
            return;
        }
        m_automatically_check_for_updates = value;
        save();
    }

    browser::Browser Configuration::cookies_browser() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_cookies_browser;
    }

    void Configuration::set_cookies_browser(browser::Browser value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_cookies_browser)
        {
            return;
        }
        m_cookies_browser = value;
        save();
    }

    std::string Configuration::cookies_path() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_cookies_path;
    }

    void Configuration::set_cookies_path(const std::string& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_cookies_path)
        {
            return;
        }
        m_cookies_path = value;
        save();
    }

    bool Configuration::crop_audio_thumbnails() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_crop_audio_thumbnails;
    }

    void Configuration::set_crop_audio_thumbnails(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_crop_audio_thumbnails)
        {
            return;
        }
        m_crop_audio_thumbnails = value;
        save();
    }

    bool Configuration::embed_chapters() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_embed_chapters;
    }

    void Configuration::set_embed_chapters(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_embed_chapters)
        {
            return;
        }
        m_embed_chapters = value;
        save();
    }

    bool Configuration::embed_metadata() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_embed_metadata;
    }

    void Configuration::set_embed_metadata(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_embed_metadata)
        {
            return;
        }
        m_embed_metadata = value;
        save();
    }

    bool Configuration::embed_subtitles() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_embed_subtitles;
    }

    void Configuration::set_embed_subtitles(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_embed_subtitles)
        {
            return;
        }
        m_embed_subtitles = value;
        save();
    }

    bool Configuration::embed_thumbnails() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_embed_thumbnails;
    }

    void Configuration::set_embed_thumbnails(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_embed_thumbnails)
        {
            return;
        }
        m_embed_thumbnails = value;
        save();
    }

    history::Length Configuration::history_length() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_history_length;
    }

    void Configuration::set_history_length(history::Length value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_history_length)
        {
            return;
        }
        m_history_length = value;
        save();
    }

    bool Configuration::history_sort_newest() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_history_sort_newest;
    }

    void Configuration::set_history_sort_newest(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_history_sort_newest)
        {
            return;
        }
        m_history_sort_newest = value;
        save();
    }

    bool Configuration::include_auto_generated_subtitles() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_include_auto_generated_subtitles;
    }

    void Configuration::set_include_auto_generated_subtitles(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_include_auto_generated_subtitles)
        {
            return;
        }
        m_include_auto_generated_subtitles = value;
        save();
    }

    bool Configuration::include_media_id_in_title() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_include_media_id_in_title;
    }

    void Configuration::set_include_media_id_in_title(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_include_media_id_in_title)
        {
            return;
        }
        m_include_media_id_in_title = value;
        save();
    }

    bool Configuration::include_super_resolutions() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_include_super_resolutions;
    }

    void Configuration::set_include_super_resolutions(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_include_super_resolutions)
        {
            return;
        }
        m_include_super_resolutions = value;
        save();
    }

    bool Configuration::limit_characters() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_limit_characters;
    }

    void Configuration::set_limit_characters(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_limit_characters)
        {
            return;
        }
        m_limit_characters = value;
        save();
    }

    int Configuration::max_number_of_active_downloads() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_max_number_of_active_downloads;
    }

    void Configuration::set_max_number_of_active_downloads(int value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_max_number_of_active_downloads)
        {
            return;
        }
        m_max_number_of_active_downloads = value;
        save();
    }

    bool Configuration::overwrite_existing_files() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_overwrite_existing_files;
    }

    void Configuration::set_overwrite_existing_files(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_overwrite_existing_files)
        {
            return;
        }
        m_overwrite_existing_files = value;
        save();
    }

    std::vector<post_processor::Argument> Configuration::postprocessing_arguments() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_postprocessing_arguments;
    }

    void Configuration::set_postprocessing_arguments(const std::vector<post_processor::Argument>& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_postprocessing_arguments)
        {
            return;
        }
        m_postprocessing_arguments = value;
        save();
    }

    int Configuration::postprocessing_threads() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_postprocessing_threads;
    }

    void Configuration::set_postprocessing_threads(int value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_postprocessing_threads)
        {
            return;
        }
        m_postprocessing_threads = value;
        save();
    }

    media::AudioCodec Configuration::preferred_audio_codec() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_preferred_audio_codec;
    }

    void Configuration::set_preferred_audio_codec(media::AudioCodec value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_preferred_audio_codec)
        {
            return;
        }
        m_preferred_audio_codec = value;
        save();
    }

    media::FrameRate Configuration::preferred_frame_rate() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_preferred_frame_rate;
    }

    void Configuration::set_preferred_frame_rate(media::FrameRate value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_preferred_frame_rate)
        {
            return;
        }
        m_preferred_frame_rate = value;
        save();
    }

    subtitle::Format Configuration::preferred_subtitle_format() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_preferred_subtitle_format;
    }

    void Configuration::set_preferred_subtitle_format(subtitle::Format value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_preferred_subtitle_format)
        {
            return;
        }
        m_preferred_subtitle_format = value;
        save();
    }

    media::VideoCodec Configuration::preferred_video_codec() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_preferred_video_codec;
    }

    void Configuration::set_preferred_video_codec(media::VideoCodec value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_preferred_video_codec)
        {
            return;
        }
        m_preferred_video_codec = value;
        save();
    }

    bool Configuration::prevent_suspend() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_prevent_suspend;
    }

    void Configuration::set_prevent_suspend(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_prevent_suspend)
        {
            return;
        }
        m_prevent_suspend = value;
        save();
    }

    PreviousDownloadOptions Configuration::previous_download_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_previous_download_options;
    }

    void Configuration::set_previous_download_options(const PreviousDownloadOptions& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_previous_download_options)
        {
            return;
        }
        m_previous_download_options = value;
        save();
    }

    std::string Configuration::proxy_url() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_proxy_url;
    }

    void Configuration::set_proxy_url(const std::string& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_proxy_url)
        {
            return;
        }
        m_proxy_url = value;
        save();
    }

    bool Configuration::remove_source_data() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_remove_source_data;
    }

    void Configuration::set_remove_source_data(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_remove_source_data)
        {
            return;
        }
        m_remove_source_data = value;
        save();
    }

    bool Configuration::show_disclaimer_on_startup() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_show_disclaimer_on_startup;
    }

    void Configuration::set_show_disclaimer_on_startup(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_show_disclaimer_on_startup)
        {
            return;
        }
        m_show_disclaimer_on_startup = value;
        save();
    }

    std::optional<int> Configuration::speed_limit() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_speed_limit;
    }

    void Configuration::set_speed_limit(std::optional<int> value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_speed_limit)
        {
            return;
        }
        m_speed_limit = value;
        save();
    }

    Theme Configuration::theme() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_theme;
    }

    void Configuration::set_theme(Theme value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_theme)
        {
            return;
        }
        m_theme = value;
        save();
    }

    bool Configuration::translate_metadata_and_chapters() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_translate_metadata_and_chapters;
    }

    void Configuration::set_translate_metadata_and_chapters(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_translate_metadata_and_chapters)
        {
            return;
        }
        m_translate_metadata_and_chapters = value;
        save();
    }

    std::string Configuration::translation_language() const
    {
        std::scoped_lock lock{ m_mutex };
        if (m_translation_language.empty())
        {
            return Translator::follow_system_language();
        }
        return m_translation_language;
    }

    void Configuration::set_translation_language(const std::string& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_translation_language ||
            (m_translation_language == Translator::follow_system_language() && value.empty()))
        {
            return;
        }
        if (value.empty())
        {
            m_translation_language = Translator::follow_system_language();
        }
        else
        {
            m_translation_language = value;
        }
        save();
    }

    bool Configuration::use_aria() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_use_aria;
    }

    void Configuration::set_use_aria(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_use_aria)
        {
            return;
        }
        m_use_aria = value;
        save();
    }

    bool Configuration::use_part_files() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_use_part_files;
    }

    void Configuration::set_use_part_files(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_use_part_files)
        {
            return;
        }
        m_use_part_files = value;
        save();
    }

    WindowGeometry Configuration::window_geometry() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_window_geometry;
    }

    void Configuration::set_window_geometry(const WindowGeometry& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_window_geometry)
        {
            return;
        }
        m_window_geometry = value;
        save();
    }

    bool Configuration::youtube_sponsor_block() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_youtube_sponsor_block;
    }

    void Configuration::set_youtube_sponsor_block(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_youtube_sponsor_block)
        {
            return;
        }
        m_youtube_sponsor_block = value;
        save();
    }

    std::string Configuration::ytdlp_discovery_args() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_ytdlp_discovery_args;
    }

    void Configuration::set_ytdlp_discovery_args(const std::string& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_ytdlp_discovery_args)
        {
            return;
        }
        m_ytdlp_discovery_args = value;
        save();
    }

    std::string Configuration::ytdlp_download_args() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_ytdlp_download_args;
    }

    void Configuration::set_ytdlp_download_args(const std::string& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_ytdlp_download_args)
        {
            return;
        }
        m_ytdlp_download_args = value;
        save();
    }

    std::string Configuration::str() const
    {
        std::scoped_lock lock{ m_mutex };
        nlohmann::json json = *this;
        return json.dump(4);
    }

    void Configuration::save() const
    {
        try
        {
            std::filesystem::create_directories(m_path.parent_path());
            std::ofstream file{ m_path, std::ios::out | std::ios::trunc };
            if (file)
            {
                nlohmann::json json = *this;
                file << json.dump();
            }
        }
        catch (const std::exception&)
        {
            return;
        }
    }
}
