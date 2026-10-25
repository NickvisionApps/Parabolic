#include "configuration.h"
#include <charconv>
#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>
#include <optional>
#include <sago/platform_folders.h>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <utility>
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

    void from_json(const nlohmann::json& json, Configuration& config)
    {
        if (json.contains("allowPreviewUpdates") && json["allowPreviewUpdates"].is_boolean())
        {
            config.m_allow_preview_updates = json["allowPreviewUpdates"].get<bool>();
        }
        else if (json.contains("AllowPreviewUpdates") && json["AllowPreviewUpdates"].is_boolean())
        {
            config.m_allow_preview_updates = json["AllowPreviewUpdates"].get<bool>();
        }
        if (json.contains("ariaMaxConnectionsPerServer") && json["ariaMaxConnectionsPerServer"].is_number_integer())
        {
            config.m_aria_max_connections_per_server = json["ariaMaxConnectionsPerServer"].get<int>();
        }
        else if (json.contains("AriaMaxConnectionsPerServer") &&
                 json["AriaMaxConnectionsPerServer"].is_number_integer())
        {
            config.m_aria_max_connections_per_server = json["AriaMaxConnectionsPerServer"].get<int>();
        }
        if (json.contains("ariaMinSplitSize") && json["ariaMinSplitSize"].is_number_integer())
        {
            config.m_aria_min_split_size = json["ariaMinSplitSize"].get<int>();
        }
        else if (json.contains("AriaMinSplitSize") && json["AriaMinSplitSize"].is_number_integer())
        {
            config.m_aria_min_split_size = json["AriaMinSplitSize"].get<int>();
        }
        if (json.contains("automaticallyCheckForUpdates") && json["automaticallyCheckForUpdates"].is_boolean())
        {
            config.m_automatically_check_for_updates = json["automaticallyCheckForUpdates"].get<bool>();
        }
        else if (json.contains("AutomaticallyCheckForUpdates") && json["AutomaticallyCheckForUpdates"].is_boolean())
        {
            config.m_automatically_check_for_updates = json["AutomaticallyCheckForUpdates"].get<bool>();
        }
        if (json.contains("cookiesBrowser") && json["cookiesBrowser"].is_number_integer())
        {
            config.m_cookies_browser = static_cast<browser::Browser>(json["cookiesBrowser"].get<int>());
        }
        else if (json.contains("CookiesBrowser") && json["CookiesBrowser"].is_number_integer())
        {
            config.m_cookies_browser = static_cast<browser::Browser>(json["CookiesBrowser"].get<int>());
        }
        if (json.contains("cookiesPath") && json["cookiesPath"].is_string())
        {
            config.m_cookies_path = json["cookiesPath"].get<std::string>();
        }
        else if (json.contains("CookiesPath") && json["CookiesPath"].is_string())
        {
            config.m_cookies_path = json["CookiesPath"].get<std::string>();
        }
        if (json.contains("cropAudioThumbnails") && json["cropAudioThumbnails"].is_boolean())
        {
            config.m_crop_audio_thumbnails = json["cropAudioThumbnails"].get<bool>();
        }
        else if (json.contains("CropAudioThumbnails") && json["CropAudioThumbnails"].is_boolean())
        {
            config.m_crop_audio_thumbnails = json["CropAudioThumbnails"].get<bool>();
        }
        if (json.contains("embedChapters") && json["embedChapters"].is_boolean())
        {
            config.m_embed_chapters = json["embedChapters"].get<bool>();
        }
        else if (json.contains("EmbedChapters") && json["EmbedChapters"].is_boolean())
        {
            config.m_embed_chapters = json["EmbedChapters"].get<bool>();
        }
        if (json.contains("embedMetadata") && json["embedMetadata"].is_boolean())
        {
            config.m_embed_metadata = json["embedMetadata"].get<bool>();
        }
        else if (json.contains("EmbedMetadata") && json["EmbedMetadata"].is_boolean())
        {
            config.m_embed_metadata = json["EmbedMetadata"].get<bool>();
        }
        if (json.contains("embedSubtitles") && json["embedSubtitles"].is_boolean())
        {
            config.m_embed_subtitles = json["embedSubtitles"].get<bool>();
        }
        else if (json.contains("EmbedSubtitles") && json["EmbedSubtitles"].is_boolean())
        {
            config.m_embed_subtitles = json["EmbedSubtitles"].get<bool>();
        }
        if (json.contains("embedThumbnails") && json["embedThumbnails"].is_boolean())
        {
            config.m_embed_thumbnails = json["embedThumbnails"].get<bool>();
        }
        else if (json.contains("EmbedThumbnails") && json["EmbedThumbnails"].is_boolean())
        {
            config.m_embed_thumbnails = json["EmbedThumbnails"].get<bool>();
        }
        if (json.contains("historyLength") && json["historyLength"].is_number_integer())
        {
            config.m_history_length = static_cast<history::Length>(json["historyLength"].get<int>());
        }
        else if (json.contains("HistoryLength") && json["HistoryLength"].is_number_integer())
        {
            config.m_history_length = static_cast<history::Length>(json["HistoryLength"].get<int>());
        }
        if (json.contains("historySortNewestFirst") && json["historySortNewestFirst"].is_boolean())
        {
            config.m_history_sort_newest = json["historySortNewestFirst"].get<bool>();
        }
        else if (json.contains("HistorySortNewestFirst") && json["HistorySortNewestFirst"].is_boolean())
        {
            config.m_history_sort_newest = json["HistorySortNewestFirst"].get<bool>();
        }
        if (json.contains("includeAutoGeneratedSubtitles") && json["includeAutoGeneratedSubtitles"].is_boolean())
        {
            config.m_include_auto_generated_subtitles = json["includeAutoGeneratedSubtitles"].get<bool>();
        }
        else if (json.contains("IncludeAutoGeneratedSubtitles") && json["IncludeAutoGeneratedSubtitles"].is_boolean())
        {
            config.m_include_auto_generated_subtitles = json["IncludeAutoGeneratedSubtitles"].get<bool>();
        }
        if (json.contains("includeMediaIdInTitle") && json["includeMediaIdInTitle"].is_boolean())
        {
            config.m_include_media_id_in_title = json["includeMediaIdInTitle"].get<bool>();
        }
        else if (json.contains("IncludeMediaIdInTitle") && json["IncludeMediaIdInTitle"].is_boolean())
        {
            config.m_include_media_id_in_title = json["IncludeMediaIdInTitle"].get<bool>();
        }
        if (json.contains("includeSuperResolutions") && json["includeSuperResolutions"].is_boolean())
        {
            config.m_include_super_resolutions = json["includeSuperResolutions"].get<bool>();
        }
        else if (json.contains("IncludeSuperResolutions") && json["IncludeSuperResolutions"].is_boolean())
        {
            config.m_include_super_resolutions = json["IncludeSuperResolutions"].get<bool>();
        }
        if (json.contains("limitCharacters") && json["limitCharacters"].is_boolean())
        {
            config.m_limit_characters = json["limitCharacters"].get<bool>();
        }
        else if (json.contains("LimitCharacters") && json["LimitCharacters"].is_boolean())
        {
            config.m_limit_characters = json["LimitCharacters"].get<bool>();
        }
        if (json.contains("maxNumberOfActiveDownloads") && json["maxNumberOfActiveDownloads"].is_number_integer())
        {
            config.m_max_number_of_active_downloads = json["maxNumberOfActiveDownloads"].get<int>();
        }
        else if (json.contains("MaxNumberOfActiveDownloads") && json["MaxNumberOfActiveDownloads"].is_number_integer())
        {
            config.m_max_number_of_active_downloads = json["MaxNumberOfActiveDownloads"].get<int>();
        }
        if (json.contains("overwriteExistingFiles") && json["overwriteExistingFiles"].is_boolean())
        {
            config.m_overwrite_existing_files = json["overwriteExistingFiles"].get<bool>();
        }
        else if (json.contains("OverwriteExistingFiles") && json["OverwriteExistingFiles"].is_boolean())
        {
            config.m_overwrite_existing_files = json["OverwriteExistingFiles"].get<bool>();
        }
        if (json.contains("postprocessingArguments") && json["postprocessingArguments"].is_array())
        {
            config.m_postprocessing_arguments.clear();
            for (const auto& item: json["postprocessingArguments"])
            {
                post_processor::Argument argument{ "", post_processor::Step::None, post_processor::Executable::None,
                                                   "" };
                from_json(item, argument);
                config.m_postprocessing_arguments.push_back(argument);
            }
        }
        if (json.contains("postprocessingThreads") && json["postprocessingThreads"].is_number_integer())
        {
            config.m_postprocessing_threads = json["postprocessingThreads"].get<int>();
        }
        else if (json.contains("PostprocessingThreads") && json["PostprocessingThreads"].is_number_integer())
        {
            config.m_postprocessing_threads = json["PostprocessingThreads"].get<int>();
        }
        if (json.contains("preferredAudioCodec") && json["preferredAudioCodec"].is_number_integer())
        {
            config.m_preferred_audio_codec = static_cast<media::AudioCodec>(json["preferredAudioCodec"].get<int>());
        }
        else if (json.contains("PreferredAudioCodec") && json["PreferredAudioCodec"].is_number_integer())
        {
            config.m_preferred_audio_codec = static_cast<media::AudioCodec>(json["PreferredAudioCodec"].get<int>());
        }
        if (json.contains("preferredFrameRate") && json["preferredFrameRate"].is_number_integer())
        {
            config.m_preferred_frame_rate = static_cast<media::FrameRate>(json["preferredFrameRate"].get<int>());
        }
        else if (json.contains("PreferredFrameRate") && json["PreferredFrameRate"].is_number_integer())
        {
            config.m_preferred_frame_rate = static_cast<media::FrameRate>(json["PreferredFrameRate"].get<int>());
        }
        if (json.contains("preferredSubtitleFormat") && json["preferredSubtitleFormat"].is_number_integer())
        {
            config.m_preferred_subtitle_format =
                    static_cast<subtitle::Format>(json["preferredSubtitleFormat"].get<int>());
        }
        else if (json.contains("PreferredSubtitleFormat") && json["PreferredSubtitleFormat"].is_number_integer())
        {
            config.m_preferred_subtitle_format =
                    static_cast<subtitle::Format>(json["PreferredSubtitleFormat"].get<int>());
        }
        if (json.contains("preferredVideoCodec") && json["preferredVideoCodec"].is_number_integer())
        {
            config.m_preferred_video_codec = static_cast<media::VideoCodec>(json["preferredVideoCodec"].get<int>());
        }
        else if (json.contains("PreferredVideoCodec") && json["PreferredVideoCodec"].is_number_integer())
        {
            config.m_preferred_video_codec = static_cast<media::VideoCodec>(json["PreferredVideoCodec"].get<int>());
        }
        if (json.contains("preventSuspend") && json["preventSuspend"].is_boolean())
        {
            config.m_prevent_suspend = json["preventSuspend"].get<bool>();
        }
        else if (json.contains("PreventSuspend") && json["PreventSuspend"].is_boolean())
        {
            config.m_prevent_suspend = json["PreventSuspend"].get<bool>();
        }
        if (json.contains("proxyUrl") && json["proxyUrl"].is_string())
        {
            config.m_proxy_url = json["proxyUrl"].get<std::string>();
        }
        else if (json.contains("ProxyUrl") && json["ProxyUrl"].is_string())
        {
            config.m_proxy_url = json["ProxyUrl"].get<std::string>();
        }
        if (json.contains("removeSourceData") && json["removeSourceData"].is_boolean())
        {
            config.m_remove_source_data = json["removeSourceData"].get<bool>();
        }
        else if (json.contains("RemoveSourceData") && json["RemoveSourceData"].is_boolean())
        {
            config.m_remove_source_data = json["RemoveSourceData"].get<bool>();
        }
        if (json.contains("showDisclaimerOnStartup") && json["showDisclaimerOnStartup"].is_boolean())
        {
            config.m_show_disclaimer_on_startup = json["showDisclaimerOnStartup"].get<bool>();
        }
        else if (json.contains("ShowDisclaimerOnStartup") && json["ShowDisclaimerOnStartup"].is_boolean())
        {
            config.m_show_disclaimer_on_startup = json["ShowDisclaimerOnStartup"].get<bool>();
        }
        if (json.contains("speedLimit"))
        {
            if (json["speedLimit"].is_number_integer())
            {
                config.m_speed_limit = json["speedLimit"].get<int>();
            }
            else if (json["speedLimit"].is_null())
            {
                config.m_speed_limit = std::nullopt;
            }
        }
        if (json.contains("theme") && json["theme"].is_number_integer())
        {
            config.m_theme = static_cast<Theme>(json["theme"].get<int>());
        }
        else if (json.contains("Theme") && json["Theme"].is_number_integer())
        {
            config.m_theme = static_cast<Theme>(json["Theme"].get<int>());
        }
        if (json.contains("translateMetadataAndChapters") && json["translateMetadataAndChapters"].is_boolean())
        {
            config.m_translate_metadata_and_chapters = json["translateMetadataAndChapters"].get<bool>();
        }
        else if (json.contains("TranslateMetadataAndChapters") && json["TranslateMetadataAndChapters"].is_boolean())
        {
            config.m_translate_metadata_and_chapters = json["TranslateMetadataAndChapters"].get<bool>();
        }
        if (json.contains("translationLanguage") && json["translationLanguage"].is_string())
        {
            config.m_translation_language = json["translationLanguage"].get<std::string>();
        }
        else if (json.contains("TranslationLanguage") && json["TranslationLanguage"].is_string())
        {
            config.m_translation_language = json["TranslationLanguage"].get<std::string>();
        }
        if (json.contains("useAria") && json["useAria"].is_boolean())
        {
            config.m_use_aria = json["useAria"].get<bool>();
        }
        else if (json.contains("UseAria") && json["UseAria"].is_boolean())
        {
            config.m_use_aria = json["UseAria"].get<bool>();
        }
        if (json.contains("usePartFiles") && json["usePartFiles"].is_boolean())
        {
            config.m_use_part_files = json["usePartFiles"].get<bool>();
        }
        else if (json.contains("UsePartFiles") && json["UsePartFiles"].is_boolean())
        {
            config.m_use_part_files = json["UsePartFiles"].get<bool>();
        }
        if (json.contains("windowGeometry") && json["windowGeometry"].is_object())
        {
            config.m_window_geometry = json["windowGeometry"].get<WindowGeometry>();
        }
        if (json.contains("youTubeSponsorBlock") && json["youTubeSponsorBlock"].is_boolean())
        {
            config.m_youtube_sponsor_block = json["youTubeSponsorBlock"].get<bool>();
        }
        else if (json.contains("YouTubeSponsorBlock") && json["YouTubeSponsorBlock"].is_boolean())
        {
            config.m_youtube_sponsor_block = json["YouTubeSponsorBlock"].get<bool>();
        }
        if (json.contains("ytdlpDiscoveryArgs") && json["ytdlpDiscoveryArgs"].is_string())
        {
            config.m_ytdlp_discovery_args = json["ytdlpDiscoveryArgs"].get<std::string>();
        }
        else if (json.contains("YtdlpDiscoveryArgs") && json["YtdlpDiscoveryArgs"].is_string())
        {
            config.m_ytdlp_discovery_args = json["YtdlpDiscoveryArgs"].get<std::string>();
        }
        if (json.contains("ytdlpDownloadArgs") && json["ytdlpDownloadArgs"].is_string())
        {
            config.m_ytdlp_download_args = json["ytdlpDownloadArgs"].get<std::string>();
        }
        else if (json.contains("YtdlpDownloadArgs") && json["YtdlpDownloadArgs"].is_string())
        {
            config.m_ytdlp_download_args = json["YtdlpDownloadArgs"].get<std::string>();
        }
        from_json(json, config.m_previous_download_options);
    }

    void from_json(const nlohmann::json& json, PreviousDownloadOptions& options)
    {
        if (json.contains("audioBitrate") && json["audioBitrate"].is_number())
        {
            options.m_audio_bitrate = json["audioBitrate"].get<double>();
        }
        else if (json.contains("AudioBitrate") && json["AudioBitrate"].is_number())
        {
            options.m_audio_bitrate = json["AudioBitrate"].get<double>();
        }
        if (json.contains("audioFormatIds") && json["audioFormatIds"].is_object())
        {
            for (const auto& item: json["audioFormatIds"].items())
            {
                int type{};
                if (item.value().is_string() &&
                    std::from_chars(item.key().data(), item.key().data() + item.key().size(), type).ec == std::errc{})
                {
                    options.m_audio_format_ids[static_cast<file::Type>(type)] = item.value().get<std::string>();
                }
            }
        }
        if (json.contains("audioOnlyFileType") && json["audioOnlyFileType"].is_number_integer())
        {
            options.m_audio_only_file_type = static_cast<file::Type>(json["audioOnlyFileType"].get<int>());
        }
        else if (json.contains("AudioOnlyFileType") && json["AudioOnlyFileType"].is_number_integer())
        {
            options.m_audio_only_file_type = static_cast<file::Type>(json["AudioOnlyFileType"].get<int>());
        }
        if (json.contains("downloadImmediatelyAsAudio") && json["downloadImmediatelyAsAudio"].is_boolean())
        {
            options.m_download_immediately_as_audio = json["downloadImmediatelyAsAudio"].get<bool>();
        }
        else if (json.contains("DownloadImmediatelyAsAudio") && json["DownloadImmediatelyAsAudio"].is_boolean())
        {
            options.m_download_immediately_as_audio = json["DownloadImmediatelyAsAudio"].get<bool>();
        }
        if (json.contains("downloadImmediatelyAsVideo") && json["downloadImmediatelyAsVideo"].is_boolean())
        {
            options.m_download_immediately_as_video = json["downloadImmediatelyAsVideo"].get<bool>();
        }
        else if (json.contains("DownloadImmediatelyAsVideo") && json["DownloadImmediatelyAsVideo"].is_boolean())
        {
            options.m_download_immediately_as_video = json["DownloadImmediatelyAsVideo"].get<bool>();
        }
        if (json.contains("exportDescription") && json["exportDescription"].is_boolean())
        {
            options.m_export_description = json["exportDescription"].get<bool>();
        }
        else if (json.contains("ExportDescription") && json["ExportDescription"].is_boolean())
        {
            options.m_export_description = json["ExportDescription"].get<bool>();
        }
        if (json.contains("exportM3U") && json["exportM3U"].is_boolean())
        {
            options.m_export_m3u = json["exportM3U"].get<bool>();
        }
        else if (json.contains("ExportM3U") && json["ExportM3U"].is_boolean())
        {
            options.m_export_m3u = json["ExportM3U"].get<bool>();
        }
        if (json.contains("fullFileType") && json["fullFileType"].is_number_integer())
        {
            options.m_full_file_type = static_cast<file::Type>(json["fullFileType"].get<int>());
        }
        else if (json.contains("FullFileType") && json["FullFileType"].is_number_integer())
        {
            options.m_full_file_type = static_cast<file::Type>(json["FullFileType"].get<int>());
        }
        if (json.contains("numberTitles") && json["numberTitles"].is_boolean())
        {
            options.m_number_titles = json["numberTitles"].get<bool>();
        }
        else if (json.contains("NumberTitles") && json["NumberTitles"].is_boolean())
        {
            options.m_number_titles = json["NumberTitles"].get<bool>();
        }
        if (json.contains("postProcessorArgumentName") && json["postProcessorArgumentName"].is_string())
        {
            options.m_post_processor_argument_name = json["postProcessorArgumentName"].get<std::string>();
        }
        else if (json.contains("PostProcessorArgumentName") && json["PostProcessorArgumentName"].is_string())
        {
            options.m_post_processor_argument_name = json["PostProcessorArgumentName"].get<std::string>();
        }
        if (json.contains("reverseDownloadOrder") && json["reverseDownloadOrder"].is_boolean())
        {
            options.m_reverse_download_order = json["reverseDownloadOrder"].get<bool>();
        }
        else if (json.contains("ReverseDownloadOrder") && json["ReverseDownloadOrder"].is_boolean())
        {
            options.m_reverse_download_order = json["ReverseDownloadOrder"].get<bool>();
        }
        if (json.contains("saveFolder") && json["saveFolder"].is_string())
        {
            options.m_save_folder = std::filesystem::path{ json["saveFolder"].get<std::string>() };
        }
        else if (json.contains("SaveFolder") && json["SaveFolder"].is_string())
        {
            options.m_save_folder = std::filesystem::path{ json["SaveFolder"].get<std::string>() };
        }
        if (json.contains("splitChapters") && json["splitChapters"].is_boolean())
        {
            options.m_split_chapters = json["splitChapters"].get<bool>();
        }
        else if (json.contains("SplitChapters") && json["SplitChapters"].is_boolean())
        {
            options.m_split_chapters = json["SplitChapters"].get<bool>();
        }
        if (json.contains("subtitleLanguages") && json["subtitleLanguages"].is_array())
        {
            options.m_subtitle_languages.clear();
            for (const auto& item: json["subtitleLanguages"])
            {
                subtitle::Subtitle subtitle{ "", false };
                from_json(item, subtitle);
                options.m_subtitle_languages.push_back(subtitle);
            }
        }
        if (json.contains("videoFormatIds") && json["videoFormatIds"].is_object())
        {
            for (const auto& item: json["videoFormatIds"].items())
            {
                int type{};
                if (item.value().is_string() &&
                    std::from_chars(item.key().data(), item.key().data() + item.key().size(), type).ec == std::errc{})
                {
                    options.m_video_format_ids[static_cast<file::Type>(type)] = item.value().get<std::string>();
                }
            }
        }
        if (json.contains("videoOnlyFileType") && json["videoOnlyFileType"].is_number_integer())
        {
            options.m_video_only_file_type = static_cast<file::Type>(json["videoOnlyFileType"].get<int>());
        }
        else if (json.contains("VideoOnlyFileType") && json["VideoOnlyFileType"].is_number_integer())
        {
            options.m_video_only_file_type = static_cast<file::Type>(json["VideoOnlyFileType"].get<int>());
        }
        if (json.contains("videoResolution") && json["videoResolution"].is_object())
        {
            options.m_video_resolution = json["videoResolution"].get<media::VideoResolution>();
        }
    }

    void from_json(const nlohmann::json& json, WindowGeometry& geometry)
    {
        if (json.contains("height") && json["height"].is_number_integer())
        {
            geometry.m_height = json["height"].get<int>();
        }
        if (json.contains("isMaximized") && json["isMaximized"].is_boolean())
        {
            geometry.m_is_maximized = json["isMaximized"].get<bool>();
        }
        if (json.contains("width") && json["width"].is_number_integer())
        {
            geometry.m_width = json["width"].get<int>();
        }
        if (json.contains("x") && json["x"].is_number_integer())
        {
            geometry.m_x = json["x"].get<int>();
        }
        if (json.contains("y") && json["y"].is_number_integer())
        {
            geometry.m_y = json["y"].get<int>();
        }
    }

    void to_json(nlohmann::json& json, const Configuration& config)
    {
        json = nlohmann::json{
            { "allowPreviewUpdates", config.m_allow_preview_updates },
            { "ariaMaxConnectionsPerServer", config.m_aria_max_connections_per_server },
            { "ariaMinSplitSize", config.m_aria_min_split_size },
            { "automaticallyCheckForUpdates", config.m_automatically_check_for_updates },
            { "cookiesBrowser", static_cast<int>(config.m_cookies_browser) },
            { "cookiesPath", config.m_cookies_path },
            { "cropAudioThumbnails", config.m_crop_audio_thumbnails },
            { "embedChapters", config.m_embed_chapters },
            { "embedMetadata", config.m_embed_metadata },
            { "embedSubtitles", config.m_embed_subtitles },
            { "embedThumbnails", config.m_embed_thumbnails },
            { "historyLength", static_cast<int>(config.m_history_length) },
            { "historySortNewestFirst", config.m_history_sort_newest },
            { "includeAutoGeneratedSubtitles", config.m_include_auto_generated_subtitles },
            { "includeMediaIdInTitle", config.m_include_media_id_in_title },
            { "includeSuperResolutions", config.m_include_super_resolutions },
            { "limitCharacters", config.m_limit_characters },
            { "maxNumberOfActiveDownloads", config.m_max_number_of_active_downloads },
            { "overwriteExistingFiles", config.m_overwrite_existing_files },
            { "postprocessingArguments", nlohmann::json::array() },
            { "postprocessingThreads", config.m_postprocessing_threads },
            { "preferredAudioCodec", static_cast<int>(config.m_preferred_audio_codec) },
            { "preferredFrameRate", static_cast<int>(config.m_preferred_frame_rate) },
            { "preferredSubtitleFormat", static_cast<int>(config.m_preferred_subtitle_format) },
            { "preferredVideoCodec", static_cast<int>(config.m_preferred_video_codec) },
            { "preventSuspend", config.m_prevent_suspend },
            { "proxyUrl", config.m_proxy_url },
            { "removeSourceData", config.m_remove_source_data },
            { "showDisclaimerOnStartup", config.m_show_disclaimer_on_startup },
            {
                    "speedLimit",
                    config.m_speed_limit.has_value() ? nlohmann::json(config.m_speed_limit.value())
                                                     : nlohmann::json(nullptr),
            },
            { "theme", static_cast<int>(config.m_theme) },
            { "translateMetadataAndChapters", config.m_translate_metadata_and_chapters },
            { "translationLanguage", config.m_translation_language },
            { "useAria", config.m_use_aria },
            { "usePartFiles", config.m_use_part_files },
            { "windowGeometry", config.m_window_geometry },
            { "youTubeSponsorBlock", config.m_youtube_sponsor_block },
            { "ytdlpDiscoveryArgs", config.m_ytdlp_discovery_args },
            { "ytdlpDownloadArgs", config.m_ytdlp_download_args },
        };
        for (const auto& item: config.m_postprocessing_arguments)
        {
            json["postprocessingArguments"].push_back(item);
        }
        json.update(nlohmann::json(config.m_previous_download_options));
    }

    void to_json(nlohmann::json& json, const PreviousDownloadOptions& options)
    {
        json = nlohmann::json{
            { "audioBitrate", options.m_audio_bitrate },
            { "audioFormatIds", nlohmann::json::object() },
            { "audioOnlyFileType", static_cast<int>(options.m_audio_only_file_type) },
            { "downloadImmediatelyAsAudio", options.m_download_immediately_as_audio },
            { "downloadImmediatelyAsVideo", options.m_download_immediately_as_video },
            { "exportDescription", options.m_export_description },
            { "exportM3U", options.m_export_m3u },
            { "fullFileType", static_cast<int>(options.m_full_file_type) },
            { "numberTitles", options.m_number_titles },
            { "postProcessorArgumentName", options.m_post_processor_argument_name },
            { "reverseDownloadOrder", options.m_reverse_download_order },
            { "saveFolder", options.m_save_folder.string() },
            { "splitChapters", options.m_split_chapters },
            { "subtitleLanguages", nlohmann::json::array() },
            { "videoFormatIds", nlohmann::json::object() },
            { "videoOnlyFileType", static_cast<int>(options.m_video_only_file_type) },
            { "videoResolution", options.m_video_resolution },
        };
        for (const std::pair<const file::Type, std::string>& pair: options.m_audio_format_ids)
        {
            json["audioFormatIds"][std::to_string(static_cast<int>(pair.first))] = pair.second;
        }
        for (const auto& item: options.m_subtitle_languages)
        {
            json["subtitleLanguages"].push_back(item);
        }
        for (const std::pair<const file::Type, std::string>& pair: options.m_video_format_ids)
        {
            json["videoFormatIds"][std::to_string(static_cast<int>(pair.first))] = pair.second;
        }
    }

    void to_json(nlohmann::json& json, const WindowGeometry& geometry)
    {
        json = nlohmann::json{
            { "height", geometry.m_height }, { "isMaximized", geometry.m_is_maximized },
            { "width", geometry.m_width },   { "x", geometry.m_x },
            { "y", geometry.m_y },
        };
    }
}
