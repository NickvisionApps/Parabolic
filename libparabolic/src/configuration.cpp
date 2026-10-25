#include "configuration.h"
#include <algorithm>
#include <cctype>
#include <charconv>
#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <limits>
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <sago/platform_folders.h>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <type_traits>
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

namespace
{
    template<typename T>
    void read_value(const nlohmann::json& json, std::string_view key, T& member)
    {
        std::string pascal{ key };
        pascal[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(pascal[0])));
        for (const std::string& name: { std::string{ key }, pascal })
        {
            const auto it = json.find(name);
            if (it == json.end())
            {
                continue;
            }
            if constexpr (std::is_same_v<T, bool>)
            {
                if (it->is_boolean())
                {
                    member = it->get<bool>();
                    return;
                }
            }
            else if constexpr (std::is_enum_v<T>)
            {
                if (it->is_number_integer())
                {
                    member = static_cast<T>(it->get<int>());
                    return;
                }
            }
            else if constexpr (std::is_integral_v<T>)
            {
                if (it->is_number_integer())
                {
                    member = it->get<T>();
                    return;
                }
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                if (it->is_number())
                {
                    member = it->get<T>();
                    return;
                }
            }
            else if constexpr (std::is_same_v<T, std::filesystem::path>)
            {
                if (it->is_string())
                {
                    member = std::filesystem::path{ it->get<std::string>() };
                    return;
                }
            }
            else if (it->is_string())
            {
                member = it->get<std::string>();
                return;
            }
        }
    }

    template<typename T>
    void read_value(const nlohmann::json& json, std::string_view key, T& member, T lo, T hi)
    {
        if constexpr (std::is_enum_v<T>)
        {
            int raw = static_cast<int>(member);
            read_value(json, key, raw);
            if (raw >= static_cast<int>(lo) && raw <= static_cast<int>(hi))
            {
                member = static_cast<T>(raw);
            }
        }
        else
        {
            T value = member;
            read_value(json, key, value);
            member = std::clamp(value, lo, hi);
        }
    }
}

namespace parabolic::configuration
{
    int Aria2Options::max_connections_per_server() const
    {
        return m_max_connections_per_server;
    }

    void Aria2Options::set_max_connections_per_server(int value)
    {
        m_max_connections_per_server = value;
    }

    int Aria2Options::min_split_size() const
    {
        return m_min_split_size;
    }

    void Aria2Options::set_min_split_size(int value)
    {
        m_min_split_size = value;
    }

    bool Aria2Options::use_aria() const
    {
        return m_use_aria;
    }

    void Aria2Options::set_use_aria(bool value)
    {
        m_use_aria = value;
    }

    bool FormatOptions::include_auto_generated_subtitles() const
    {
        return m_include_auto_generated_subtitles;
    }

    void FormatOptions::set_include_auto_generated_subtitles(bool value)
    {
        m_include_auto_generated_subtitles = value;
    }

    bool FormatOptions::include_super_resolutions() const
    {
        return m_include_super_resolutions;
    }

    void FormatOptions::set_include_super_resolutions(bool value)
    {
        m_include_super_resolutions = value;
    }

    media::AudioCodec FormatOptions::preferred_audio_codec() const
    {
        return m_preferred_audio_codec;
    }

    void FormatOptions::set_preferred_audio_codec(media::AudioCodec value)
    {
        m_preferred_audio_codec = value;
    }

    media::FrameRate FormatOptions::preferred_frame_rate() const
    {
        return m_preferred_frame_rate;
    }

    void FormatOptions::set_preferred_frame_rate(media::FrameRate value)
    {
        m_preferred_frame_rate = value;
    }

    subtitle::Format FormatOptions::preferred_subtitle_format() const
    {
        return m_preferred_subtitle_format;
    }

    void FormatOptions::set_preferred_subtitle_format(subtitle::Format value)
    {
        m_preferred_subtitle_format = value;
    }

    media::VideoCodec FormatOptions::preferred_video_codec() const
    {
        return m_preferred_video_codec;
    }

    void FormatOptions::set_preferred_video_codec(media::VideoCodec value)
    {
        m_preferred_video_codec = value;
    }

    history::Length HistoryOptions::length() const
    {
        return m_length;
    }

    void HistoryOptions::set_length(history::Length value)
    {
        m_length = value;
    }

    bool HistoryOptions::sort_newest() const
    {
        return m_sort_newest;
    }

    void HistoryOptions::set_sort_newest(bool value)
    {
        m_sort_newest = value;
    }

    PostprocessingOptions::PostprocessingOptions()
        : m_postprocessing_threads{ static_cast<int>(std::thread::hardware_concurrency()) }
    {
    }

    bool PostprocessingOptions::crop_audio_thumbnails() const
    {
        return m_crop_audio_thumbnails;
    }

    void PostprocessingOptions::set_crop_audio_thumbnails(bool value)
    {
        m_crop_audio_thumbnails = value;
    }

    bool PostprocessingOptions::embed_chapters() const
    {
        return m_embed_chapters;
    }

    void PostprocessingOptions::set_embed_chapters(bool value)
    {
        m_embed_chapters = value;
    }

    bool PostprocessingOptions::embed_metadata() const
    {
        return m_embed_metadata;
    }

    void PostprocessingOptions::set_embed_metadata(bool value)
    {
        m_embed_metadata = value;
    }

    bool PostprocessingOptions::embed_subtitles() const
    {
        return m_embed_subtitles;
    }

    void PostprocessingOptions::set_embed_subtitles(bool value)
    {
        m_embed_subtitles = value;
    }

    bool PostprocessingOptions::embed_thumbnails() const
    {
        return m_embed_thumbnails;
    }

    void PostprocessingOptions::set_embed_thumbnails(bool value)
    {
        m_embed_thumbnails = value;
    }

    const std::vector<post_processor::Argument>& PostprocessingOptions::postprocessing_arguments() const
    {
        return m_postprocessing_arguments;
    }

    void PostprocessingOptions::set_postprocessing_arguments(const std::vector<post_processor::Argument>& value)
    {
        m_postprocessing_arguments = value;
    }

    int PostprocessingOptions::postprocessing_threads() const
    {
        return m_postprocessing_threads;
    }

    void PostprocessingOptions::set_postprocessing_threads(int value)
    {
        m_postprocessing_threads = value;
    }

    bool PostprocessingOptions::remove_source_data() const
    {
        return m_remove_source_data;
    }

    void PostprocessingOptions::set_remove_source_data(bool value)
    {
        m_remove_source_data = value;
    }

    bool PostprocessingOptions::translate_metadata_and_chapters() const
    {
        return m_translate_metadata_and_chapters;
    }

    void PostprocessingOptions::set_translate_metadata_and_chapters(bool value)
    {
        m_translate_metadata_and_chapters = value;
    }

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

    browser::Browser YtdlpOptions::cookies_browser() const
    {
        return m_cookies_browser;
    }

    void YtdlpOptions::set_cookies_browser(browser::Browser value)
    {
        m_cookies_browser = value;
    }

    const std::string& YtdlpOptions::cookies_path() const
    {
        return m_cookies_path;
    }

    void YtdlpOptions::set_cookies_path(const std::string& value)
    {
        m_cookies_path = value;
    }

    const std::string& YtdlpOptions::proxy_url() const
    {
        return m_proxy_url;
    }

    void YtdlpOptions::set_proxy_url(const std::string& value)
    {
        m_proxy_url = value;
    }

    std::optional<int> YtdlpOptions::speed_limit() const
    {
        return m_speed_limit;
    }

    void YtdlpOptions::set_speed_limit(std::optional<int> value)
    {
        m_speed_limit = value;
    }

    bool YtdlpOptions::use_part_files() const
    {
        return m_use_part_files;
    }

    void YtdlpOptions::set_use_part_files(bool value)
    {
        m_use_part_files = value;
    }

    bool YtdlpOptions::youtube_sponsor_block() const
    {
        return m_youtube_sponsor_block;
    }

    void YtdlpOptions::set_youtube_sponsor_block(bool value)
    {
        m_youtube_sponsor_block = value;
    }

    const std::string& YtdlpOptions::ytdlp_discovery_args() const
    {
        return m_ytdlp_discovery_args;
    }

    void YtdlpOptions::set_ytdlp_discovery_args(const std::string& value)
    {
        m_ytdlp_discovery_args = value;
    }

    const std::string& YtdlpOptions::ytdlp_download_args() const
    {
        return m_ytdlp_download_args;
    }

    void YtdlpOptions::set_ytdlp_download_args(const std::string& value)
    {
        m_ytdlp_download_args = value;
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
          m_translation_language{ translator::Translator::follow_system_language() }
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

    void Configuration::save() const
    {
        try
        {
            nlohmann::json json = *this;
            std::filesystem::create_directories(m_path.parent_path());
            std::filesystem::path temp{ m_path };
            temp += ".tmp";
            std::ofstream file{ temp, std::ios::out | std::ios::trunc };
            file << json.dump();
            file.close();
            if (file.fail())
            {
                std::filesystem::remove(temp);
                return;
            }
            std::filesystem::rename(temp, m_path);
        }
        catch (const std::exception&)
        {
            return;
        }
    }

    template<typename T>
    void Configuration::update_value(T& member, const T& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (member == value)
        {
            return;
        }
        member = value;
        save();
    }

    bool Configuration::allow_preview_updates() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_allow_preview_updates;
    }

    void Configuration::set_allow_preview_updates(bool value)
    {
        update_value(m_allow_preview_updates, value);
    }

    Aria2Options Configuration::aria2_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_aria2_options;
    }

    void Configuration::set_aria2_options(const Aria2Options& value)
    {
        update_value(m_aria2_options, value);
    }

    bool Configuration::automatically_check_for_updates() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_automatically_check_for_updates;
    }

    void Configuration::set_automatically_check_for_updates(bool value)
    {
        update_value(m_automatically_check_for_updates, value);
    }

    FormatOptions Configuration::format_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_format_options;
    }

    void Configuration::set_format_options(const FormatOptions& value)
    {
        update_value(m_format_options, value);
    }

    HistoryOptions Configuration::history_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_history_options;
    }

    void Configuration::set_history_options(const HistoryOptions& value)
    {
        update_value(m_history_options, value);
    }

    bool Configuration::include_media_id_in_title() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_include_media_id_in_title;
    }

    void Configuration::set_include_media_id_in_title(bool value)
    {
        update_value(m_include_media_id_in_title, value);
    }

    bool Configuration::limit_characters() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_limit_characters;
    }

    void Configuration::set_limit_characters(bool value)
    {
        update_value(m_limit_characters, value);
    }

    int Configuration::max_number_of_active_downloads() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_max_number_of_active_downloads;
    }

    void Configuration::set_max_number_of_active_downloads(int value)
    {
        update_value(m_max_number_of_active_downloads, value);
    }

    bool Configuration::overwrite_existing_files() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_overwrite_existing_files;
    }

    void Configuration::set_overwrite_existing_files(bool value)
    {
        update_value(m_overwrite_existing_files, value);
    }

    PostprocessingOptions Configuration::postprocessing_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_postprocessing_options;
    }

    void Configuration::set_postprocessing_options(const PostprocessingOptions& value)
    {
        update_value(m_postprocessing_options, value);
    }

    bool Configuration::prevent_suspend() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_prevent_suspend;
    }

    void Configuration::set_prevent_suspend(bool value)
    {
        update_value(m_prevent_suspend, value);
    }

    PreviousDownloadOptions Configuration::previous_download_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_previous_download_options;
    }

    void Configuration::set_previous_download_options(const PreviousDownloadOptions& value)
    {
        update_value(m_previous_download_options, value);
    }

    bool Configuration::show_disclaimer_on_startup() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_show_disclaimer_on_startup;
    }

    void Configuration::set_show_disclaimer_on_startup(bool value)
    {
        update_value(m_show_disclaimer_on_startup, value);
    }

    Theme Configuration::theme() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_theme;
    }

    void Configuration::set_theme(Theme value)
    {
        update_value(m_theme, value);
    }

    std::string Configuration::translation_language() const
    {
        std::scoped_lock lock{ m_mutex };
        if (m_translation_language.empty())
        {
            return translator::Translator::follow_system_language();
        }
        return m_translation_language;
    }

    void Configuration::set_translation_language(const std::string& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_translation_language ||
            (m_translation_language == translator::Translator::follow_system_language() && value.empty()))
        {
            return;
        }
        if (value.empty())
        {
            m_translation_language = translator::Translator::follow_system_language();
        }
        else
        {
            m_translation_language = value;
        }
        save();
    }

    WindowGeometry Configuration::window_geometry() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_window_geometry;
    }

    void Configuration::set_window_geometry(const WindowGeometry& value)
    {
        update_value(m_window_geometry, value);
    }

    YtdlpOptions Configuration::ytdlp_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_ytdlp_options;
    }

    void Configuration::set_ytdlp_options(const YtdlpOptions& value)
    {
        update_value(m_ytdlp_options, value);
    }

    std::string Configuration::str() const
    {
        std::scoped_lock lock{ m_mutex };
        nlohmann::json json = *this;
        return json.dump(4);
    }

    void from_json(const nlohmann::json& json, Aria2Options& options)
    {
        read_value(json, "ariaMaxConnectionsPerServer", options.m_max_connections_per_server, 1, 16);
        read_value(json, "ariaMinSplitSize", options.m_min_split_size, 1, 1024);
        read_value(json, "useAria", options.m_use_aria);
    }

    void from_json(const nlohmann::json& json, Configuration& config)
    {
        read_value(json, "allowPreviewUpdates", config.m_allow_preview_updates);
        read_value(json, "automaticallyCheckForUpdates", config.m_automatically_check_for_updates);
        read_value(json, "includeMediaIdInTitle", config.m_include_media_id_in_title);
        read_value(json, "limitCharacters", config.m_limit_characters);
        read_value(json, "maxNumberOfActiveDownloads", config.m_max_number_of_active_downloads, 1, 10);
        read_value(json, "overwriteExistingFiles", config.m_overwrite_existing_files);
        read_value(json, "preventSuspend", config.m_prevent_suspend);
        read_value(json, "showDisclaimerOnStartup", config.m_show_disclaimer_on_startup);
        read_value(json, "theme", config.m_theme, Theme::Light, Theme::System);
        read_value(json, "translationLanguage", config.m_translation_language);
        if (json.contains("windowGeometry") && json["windowGeometry"].is_object())
        {
            config.m_window_geometry = json["windowGeometry"].get<WindowGeometry>();
        }
        from_json(json, config.m_aria2_options);
        from_json(json, config.m_format_options);
        from_json(json, config.m_history_options);
        from_json(json, config.m_postprocessing_options);
        from_json(json, config.m_previous_download_options);
        from_json(json, config.m_ytdlp_options);
    }

    void from_json(const nlohmann::json& json, FormatOptions& options)
    {
        read_value(json, "includeAutoGeneratedSubtitles", options.m_include_auto_generated_subtitles);
        read_value(json, "includeSuperResolutions", options.m_include_super_resolutions);
        read_value(json, "preferredAudioCodec", options.m_preferred_audio_codec, media::AudioCodec::Any,
                   media::AudioCodec::Mp3);
        read_value(json, "preferredFrameRate", options.m_preferred_frame_rate, media::FrameRate::Any,
                   media::FrameRate::Fps60);
        read_value(json, "preferredSubtitleFormat", options.m_preferred_subtitle_format, subtitle::Format::Any,
                   subtitle::Format::Lrc);
        read_value(json, "preferredVideoCodec", options.m_preferred_video_codec, media::VideoCodec::Any,
                   media::VideoCodec::H264);
    }

    void from_json(const nlohmann::json& json, HistoryOptions& options)
    {
        int length = static_cast<int>(options.m_length);
        read_value(json, "historyLength", length);
        switch (length)
        {
        case static_cast<int>(history::Length::Never):
        case static_cast<int>(history::Length::OneDay):
        case static_cast<int>(history::Length::OneWeek):
        case static_cast<int>(history::Length::OneMonth):
        case static_cast<int>(history::Length::ThreeMonths):
        case static_cast<int>(history::Length::SixMonths):
        case static_cast<int>(history::Length::OneYear):
        case static_cast<int>(history::Length::Forever):
            options.m_length = static_cast<history::Length>(length);
            break;
        default:
            break;
        }
        read_value(json, "historySortNewestFirst", options.m_sort_newest);
    }

    void from_json(const nlohmann::json& json, PostprocessingOptions& options)
    {
        read_value(json, "cropAudioThumbnails", options.m_crop_audio_thumbnails);
        read_value(json, "embedChapters", options.m_embed_chapters);
        read_value(json, "embedMetadata", options.m_embed_metadata);
        read_value(json, "embedSubtitles", options.m_embed_subtitles);
        read_value(json, "embedThumbnails", options.m_embed_thumbnails);
        if (json.contains("postprocessingArguments") && json["postprocessingArguments"].is_array())
        {
            options.m_postprocessing_arguments.clear();
            for (const auto& item: json["postprocessingArguments"])
            {
                post_processor::Argument argument{ "", post_processor::Step::None, post_processor::Executable::None,
                                                   "" };
                from_json(item, argument);
                options.m_postprocessing_arguments.push_back(argument);
            }
        }
        read_value(json, "postprocessingThreads", options.m_postprocessing_threads, 1,
                   static_cast<int>(std::max(1U, std::thread::hardware_concurrency())));
        read_value(json, "removeSourceData", options.m_remove_source_data);
        read_value(json, "translateMetadataAndChapters", options.m_translate_metadata_and_chapters);
    }

    void from_json(const nlohmann::json& json, PreviousDownloadOptions& options)
    {
        read_value(json, "audioBitrate", options.m_audio_bitrate);
        if (json.contains("audioFormatIds") && json["audioFormatIds"].is_object())
        {
            for (const auto& item: json["audioFormatIds"].items())
            {
                int type{};
                if (item.value().is_string() &&
                    std::from_chars(item.key().data(), item.key().data() + item.key().size(), type).ec == std::errc{} &&
                    type >= static_cast<int>(file::Type::VideoGeneric) && type <= static_cast<int>(file::Type::Ogg))
                {
                    options.m_audio_format_ids[static_cast<file::Type>(type)] = item.value().get<std::string>();
                }
            }
        }
        read_value(json, "audioOnlyFileType", options.m_audio_only_file_type, file::Type::VideoGeneric,
                   file::Type::Ogg);
        read_value(json, "downloadImmediatelyAsAudio", options.m_download_immediately_as_audio);
        read_value(json, "downloadImmediatelyAsVideo", options.m_download_immediately_as_video);
        read_value(json, "exportDescription", options.m_export_description);
        read_value(json, "exportM3U", options.m_export_m3u);
        read_value(json, "fullFileType", options.m_full_file_type, file::Type::VideoGeneric, file::Type::Ogg);
        read_value(json, "numberTitles", options.m_number_titles);
        read_value(json, "postProcessorArgumentName", options.m_post_processor_argument_name);
        read_value(json, "reverseDownloadOrder", options.m_reverse_download_order);
        read_value(json, "saveFolder", options.m_save_folder);
        read_value(json, "splitChapters", options.m_split_chapters);
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
                    std::from_chars(item.key().data(), item.key().data() + item.key().size(), type).ec == std::errc{} &&
                    type >= static_cast<int>(file::Type::VideoGeneric) && type <= static_cast<int>(file::Type::Ogg))
                {
                    options.m_video_format_ids[static_cast<file::Type>(type)] = item.value().get<std::string>();
                }
            }
        }
        read_value(json, "videoOnlyFileType", options.m_video_only_file_type, file::Type::VideoGeneric,
                   file::Type::Ogg);
        if (json.contains("videoResolution") && json["videoResolution"].is_object())
        {
            options.m_video_resolution = json["videoResolution"].get<media::VideoResolution>();
        }
    }

    void from_json(const nlohmann::json& json, WindowGeometry& geometry)
    {
        read_value(json, "height", geometry.m_height, 1, std::numeric_limits<int>::max());
        read_value(json, "isMaximized", geometry.m_is_maximized);
        read_value(json, "width", geometry.m_width, 1, std::numeric_limits<int>::max());
        read_value(json, "x", geometry.m_x);
        read_value(json, "y", geometry.m_y);
    }

    void from_json(const nlohmann::json& json, YtdlpOptions& options)
    {
        read_value(json, "cookiesBrowser", options.m_cookies_browser, browser::Browser::None, browser::Browser::Whale);
        read_value(json, "cookiesPath", options.m_cookies_path);
        read_value(json, "proxyUrl", options.m_proxy_url);
        if (json.contains("speedLimit"))
        {
            if (json["speedLimit"].is_number_integer())
            {
                options.m_speed_limit = std::clamp(json["speedLimit"].get<int>(), 512, 10240);
            }
            else if (json["speedLimit"].is_null())
            {
                options.m_speed_limit = std::nullopt;
            }
        }
        read_value(json, "usePartFiles", options.m_use_part_files);
        read_value(json, "youTubeSponsorBlock", options.m_youtube_sponsor_block);
        read_value(json, "ytdlpDiscoveryArgs", options.m_ytdlp_discovery_args);
        read_value(json, "ytdlpDownloadArgs", options.m_ytdlp_download_args);
    }

    void to_json(nlohmann::json& json, const Aria2Options& options)
    {
        json = nlohmann::json{
            { "ariaMaxConnectionsPerServer", options.m_max_connections_per_server },
            { "ariaMinSplitSize", options.m_min_split_size },
            { "useAria", options.m_use_aria },
        };
    }

    void to_json(nlohmann::json& json, const Configuration& config)
    {
        json = nlohmann::json{
            { "allowPreviewUpdates", config.m_allow_preview_updates },
            { "automaticallyCheckForUpdates", config.m_automatically_check_for_updates },
            { "includeMediaIdInTitle", config.m_include_media_id_in_title },
            { "limitCharacters", config.m_limit_characters },
            { "maxNumberOfActiveDownloads", config.m_max_number_of_active_downloads },
            { "overwriteExistingFiles", config.m_overwrite_existing_files },
            { "preventSuspend", config.m_prevent_suspend },
            { "showDisclaimerOnStartup", config.m_show_disclaimer_on_startup },
            { "theme", static_cast<int>(config.m_theme) },
            { "translationLanguage", config.m_translation_language },
            { "windowGeometry", config.m_window_geometry },
        };
        json.update(nlohmann::json(config.m_aria2_options));
        json.update(nlohmann::json(config.m_format_options));
        json.update(nlohmann::json(config.m_history_options));
        json.update(nlohmann::json(config.m_postprocessing_options));
        json.update(nlohmann::json(config.m_previous_download_options));
        json.update(nlohmann::json(config.m_ytdlp_options));
    }

    void to_json(nlohmann::json& json, const FormatOptions& options)
    {
        json = nlohmann::json{
            { "includeAutoGeneratedSubtitles", options.m_include_auto_generated_subtitles },
            { "includeSuperResolutions", options.m_include_super_resolutions },
            { "preferredAudioCodec", static_cast<int>(options.m_preferred_audio_codec) },
            { "preferredFrameRate", static_cast<int>(options.m_preferred_frame_rate) },
            { "preferredSubtitleFormat", static_cast<int>(options.m_preferred_subtitle_format) },
            { "preferredVideoCodec", static_cast<int>(options.m_preferred_video_codec) },
        };
    }

    void to_json(nlohmann::json& json, const HistoryOptions& options)
    {
        json = nlohmann::json{
            { "historyLength", static_cast<int>(options.m_length) },
            { "historySortNewestFirst", options.m_sort_newest },
        };
    }

    void to_json(nlohmann::json& json, const PostprocessingOptions& options)
    {
        json = nlohmann::json{
            { "cropAudioThumbnails", options.m_crop_audio_thumbnails },
            { "embedChapters", options.m_embed_chapters },
            { "embedMetadata", options.m_embed_metadata },
            { "embedSubtitles", options.m_embed_subtitles },
            { "embedThumbnails", options.m_embed_thumbnails },
            { "postprocessingArguments", nlohmann::json::array() },
            { "postprocessingThreads", options.m_postprocessing_threads },
            { "removeSourceData", options.m_remove_source_data },
            { "translateMetadataAndChapters", options.m_translate_metadata_and_chapters },
        };
        for (const auto& item: options.m_postprocessing_arguments)
        {
            json["postprocessingArguments"].push_back(item);
        }
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

    void to_json(nlohmann::json& json, const YtdlpOptions& options)
    {
        json = nlohmann::json{
            { "cookiesBrowser", static_cast<int>(options.m_cookies_browser) },
            { "cookiesPath", options.m_cookies_path },
            { "proxyUrl", options.m_proxy_url },
            {
                    "speedLimit",
                    options.m_speed_limit.has_value() ? nlohmann::json(options.m_speed_limit.value())
                                                      : nlohmann::json(nullptr),
            },
            { "usePartFiles", options.m_use_part_files },
            { "youTubeSponsorBlock", options.m_youtube_sponsor_block },
            { "ytdlpDiscoveryArgs", options.m_ytdlp_discovery_args },
            { "ytdlpDownloadArgs", options.m_ytdlp_download_args },
        };
    }
}
