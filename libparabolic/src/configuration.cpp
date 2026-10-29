#include "configuration.h"
#include <algorithm>
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
#include <sago/platform_folders.h>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>
#include "aria2.h"
#include "discovery.h"
#include "environment.h"
#include "extractor.h"
#include "file.h"
#include "format.h"
#include "history.h"
#include "postprocessing.h"
#include "subtitle.h"
#include "translation.h"
#include "video.h"
#ifdef _WIN32
#include "string_manip.h"
#endif

namespace parabolic::configuration
{
    PreviousDownloadOptions::PreviousDownloadOptions()
        : m_audio_format_ids{
              { file::Type::VideoGeneric, format::Format::best_audio().id() },
              { file::Type::Mp4, format::Format::best_audio().id() },
              { file::Type::Mkv, format::Format::best_audio().id() },
              { file::Type::Webm, format::Format::best_audio().id() },
              { file::Type::Mov, format::Format::best_audio().id() },
              { file::Type::Avi, format::Format::best_audio().id() },
              { file::Type::AudioGeneric, format::Format::best_audio().id() },
              { file::Type::Mp3, format::Format::best_audio().id() },
              { file::Type::M4a, format::Format::best_audio().id() },
              { file::Type::Opus, format::Format::best_audio().id() },
              { file::Type::Flac, format::Format::best_audio().id() },
              { file::Type::Wav, format::Format::best_audio().id() },
              { file::Type::Ogg, format::Format::best_audio().id() },
          },
#ifdef _WIN32
          m_save_folder{ std::filesystem::path(string_manip::wstr(sago::getDownloadFolder())) },
#else
          m_save_folder{ sago::getDownloadFolder() },
#endif
          m_video_format_ids{
              { file::Type::VideoGeneric, format::Format::best_video().id() },
              { file::Type::Mp4, format::Format::best_video().id() },
              { file::Type::Mkv, format::Format::best_video().id() },
              { file::Type::Webm, format::Format::best_video().id() },
              { file::Type::Mov, format::Format::best_video().id() },
              { file::Type::Avi, format::Format::best_video().id() },
              { file::Type::AudioGeneric, format::Format::none_video().id() },
              { file::Type::Mp3, format::Format::none_video().id() },
              { file::Type::M4a, format::Format::none_video().id() },
              { file::Type::Opus, format::Format::none_video().id() },
              { file::Type::Flac, format::Format::none_video().id() },
              { file::Type::Wav, format::Format::none_video().id() },
              { file::Type::Ogg, format::Format::none_video().id() },
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

    const std::vector<std::string>& PreviousDownloadOptions::postprocessing_argument_names() const
    {
        return m_postprocessing_argument_names;
    }

    void PreviousDownloadOptions::set_postprocessing_argument_names(const std::vector<std::string>& value)
    {
        m_postprocessing_argument_names = value;
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

    const std::vector<subtitle::Language>& PreviousDownloadOptions::subtitle_languages() const
    {
        return m_subtitle_languages;
    }

    void PreviousDownloadOptions::set_subtitle_languages(const std::vector<subtitle::Language>& value)
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

    const video::Resolution& PreviousDownloadOptions::video_resolution() const
    {
        return m_video_resolution;
    }

    void PreviousDownloadOptions::set_video_resolution(const video::Resolution& value)
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
          m_translation_language{ translation::Translator::follow_system_language() }
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

    bool Configuration::allow_preview_updates() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_allow_preview_updates;
    }

    void Configuration::set_allow_preview_updates(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_allow_preview_updates == value)
        {
            return;
        }
        m_allow_preview_updates = value;
        save();
    }

    aria2::Options Configuration::aria2_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_aria2_options;
    }

    void Configuration::set_aria2_options(const aria2::Options& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_aria2_options == value)
        {
            return;
        }
        m_aria2_options = value;
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
        if (m_automatically_check_for_updates == value)
        {
            return;
        }
        m_automatically_check_for_updates = value;
        save();
    }

    discovery::Options Configuration::discovery_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_discovery_options;
    }

    void Configuration::set_discovery_options(const discovery::Options& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_discovery_options == value)
        {
            return;
        }
        m_discovery_options = value;
        save();
    }

    extractor::Options Configuration::extractor_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_extractor_options;
    }

    void Configuration::set_extractor_options(const extractor::Options& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_extractor_options == value)
        {
            return;
        }
        m_extractor_options = value;
        save();
    }

    history::Options Configuration::history_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_history_options;
    }

    void Configuration::set_history_options(const history::Options& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_history_options == value)
        {
            return;
        }
        m_history_options = value;
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
        if (m_include_media_id_in_title == value)
        {
            return;
        }
        m_include_media_id_in_title = value;
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
        if (m_limit_characters == value)
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
        if (m_max_number_of_active_downloads == value)
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
        if (m_overwrite_existing_files == value)
        {
            return;
        }
        m_overwrite_existing_files = value;
        save();
    }

    postprocessing::Options Configuration::postprocessing_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_postprocessing_options;
    }

    void Configuration::set_postprocessing_options(const postprocessing::Options& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_postprocessing_options == value)
        {
            return;
        }
        m_postprocessing_options = value;
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
        if (m_prevent_suspend == value)
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
        if (m_previous_download_options == value)
        {
            return;
        }
        m_previous_download_options = value;
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
        if (m_show_disclaimer_on_startup == value)
        {
            return;
        }
        m_show_disclaimer_on_startup = value;
        save();
    }

    subtitle::Options Configuration::subtitle_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_subtitle_options;
    }

    void Configuration::set_subtitle_options(const subtitle::Options& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_subtitle_options == value)
        {
            return;
        }
        m_subtitle_options = value;
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
        if (m_theme == value)
        {
            return;
        }
        m_theme = value;
        save();
    }

    std::string Configuration::translation_language() const
    {
        std::scoped_lock lock{ m_mutex };
        if (m_translation_language.empty())
        {
            return translation::Translator::follow_system_language();
        }
        return m_translation_language;
    }

    void Configuration::set_translation_language(const std::string& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_translation_language ||
            (m_translation_language == translation::Translator::follow_system_language() && value.empty()))
        {
            return;
        }
        if (value.empty())
        {
            m_translation_language = translation::Translator::follow_system_language();
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
        std::scoped_lock lock{ m_mutex };
        if (m_window_geometry == value)
        {
            return;
        }
        m_window_geometry = value;
        save();
    }

    std::string Configuration::str() const
    {
        std::scoped_lock lock{ m_mutex };
        nlohmann::json json = *this;
        return json.dump(4);
    }

    void from_json(const nlohmann::json& json, Configuration& config)
    {
        if (json.contains("allowPreviewUpdates") && json["allowPreviewUpdates"].is_boolean())
        {
            config.m_allow_preview_updates = json.at("allowPreviewUpdates").get<bool>();
        }
        else if (json.contains("AllowPreviewUpdates") && json["AllowPreviewUpdates"].is_boolean())
        {
            config.m_allow_preview_updates = json.at("AllowPreviewUpdates").get<bool>();
        }
        if (json.contains("automaticallyCheckForUpdates") && json["automaticallyCheckForUpdates"].is_boolean())
        {
            config.m_automatically_check_for_updates = json.at("automaticallyCheckForUpdates").get<bool>();
        }
        else if (json.contains("AutomaticallyCheckForUpdates") && json["AutomaticallyCheckForUpdates"].is_boolean())
        {
            config.m_automatically_check_for_updates = json.at("AutomaticallyCheckForUpdates").get<bool>();
        }
        if (json.contains("includeMediaIdInTitle") && json["includeMediaIdInTitle"].is_boolean())
        {
            config.m_include_media_id_in_title = json.at("includeMediaIdInTitle").get<bool>();
        }
        else if (json.contains("IncludeMediaIdInTitle") && json["IncludeMediaIdInTitle"].is_boolean())
        {
            config.m_include_media_id_in_title = json.at("IncludeMediaIdInTitle").get<bool>();
        }
        if (json.contains("limitCharacters") && json["limitCharacters"].is_boolean())
        {
            config.m_limit_characters = json.at("limitCharacters").get<bool>();
        }
        else if (json.contains("LimitCharacters") && json["LimitCharacters"].is_boolean())
        {
            config.m_limit_characters = json.at("LimitCharacters").get<bool>();
        }
        if (json.contains("maxNumberOfActiveDownloads") && json["maxNumberOfActiveDownloads"].is_number_integer())
        {
            config.m_max_number_of_active_downloads =
                    std::clamp(json.at("maxNumberOfActiveDownloads").get<int>(), 1, 10);
        }
        else if (json.contains("MaxNumberOfActiveDownloads") && json["MaxNumberOfActiveDownloads"].is_number_integer())
        {
            config.m_max_number_of_active_downloads =
                    std::clamp(json.at("MaxNumberOfActiveDownloads").get<int>(), 1, 10);
        }
        if (json.contains("overwriteExistingFiles") && json["overwriteExistingFiles"].is_boolean())
        {
            config.m_overwrite_existing_files = json.at("overwriteExistingFiles").get<bool>();
        }
        else if (json.contains("OverwriteExistingFiles") && json["OverwriteExistingFiles"].is_boolean())
        {
            config.m_overwrite_existing_files = json.at("OverwriteExistingFiles").get<bool>();
        }
        if (json.contains("preventSuspend") && json["preventSuspend"].is_boolean())
        {
            config.m_prevent_suspend = json.at("preventSuspend").get<bool>();
        }
        else if (json.contains("PreventSuspend") && json["PreventSuspend"].is_boolean())
        {
            config.m_prevent_suspend = json.at("PreventSuspend").get<bool>();
        }
        if (json.contains("showDisclaimerOnStartup") && json["showDisclaimerOnStartup"].is_boolean())
        {
            config.m_show_disclaimer_on_startup = json.at("showDisclaimerOnStartup").get<bool>();
        }
        else if (json.contains("ShowDisclaimerOnStartup") && json["ShowDisclaimerOnStartup"].is_boolean())
        {
            config.m_show_disclaimer_on_startup = json.at("ShowDisclaimerOnStartup").get<bool>();
        }
        if (json.contains("theme") && json["theme"].is_number_integer())
        {
            int value{ json.at("theme").get<int>() };
            if (value >= static_cast<int>(Theme::Light) && value <= static_cast<int>(Theme::System))
            {
                config.m_theme = static_cast<Theme>(value);
            }
        }
        else if (json.contains("Theme") && json["Theme"].is_number_integer())
        {
            int value{ json.at("Theme").get<int>() };
            if (value >= static_cast<int>(Theme::Light) && value <= static_cast<int>(Theme::System))
            {
                config.m_theme = static_cast<Theme>(value);
            }
        }
        if (json.contains("translationLanguage") && json["translationLanguage"].is_string())
        {
            config.m_translation_language = json.at("translationLanguage").get<std::string>();
        }
        else if (json.contains("TranslationLanguage") && json["TranslationLanguage"].is_string())
        {
            config.m_translation_language = json.at("TranslationLanguage").get<std::string>();
        }
        if (json.contains("windowGeometry") && json["windowGeometry"].is_object())
        {
            config.m_window_geometry = json["windowGeometry"].get<WindowGeometry>();
        }
        from_json(json, config.m_aria2_options);
        from_json(json, config.m_discovery_options);
        from_json(json, config.m_extractor_options);
        from_json(json, config.m_history_options);
        from_json(json, config.m_postprocessing_options);
        from_json(json, config.m_previous_download_options);
        from_json(json, config.m_subtitle_options);
    }

    void from_json(const nlohmann::json& json, PreviousDownloadOptions& options)
    {
        if (json.contains("audioBitrate") && json["audioBitrate"].is_number())
        {
            options.m_audio_bitrate = json.at("audioBitrate").get<double>();
        }
        else if (json.contains("AudioBitrate") && json["AudioBitrate"].is_number())
        {
            options.m_audio_bitrate = json.at("AudioBitrate").get<double>();
        }
        if (json.contains("audioFormatIds") && json["audioFormatIds"].is_object())
        {
            for (const std::pair<const std::string, nlohmann::json>& pair:
                 json["audioFormatIds"].get<std::map<std::string, nlohmann::json>>())
            {
                int type{};
                if (pair.second.is_string() &&
                    std::from_chars(pair.first.data(), pair.first.data() + pair.first.size(), type).ec == std::errc{} &&
                    type >= static_cast<int>(file::Type::VideoGeneric) && type <= static_cast<int>(file::Type::Ogg))
                {
                    options.m_audio_format_ids[static_cast<file::Type>(type)] = pair.second.get<std::string>();
                }
            }
        }
        if (json.contains("audioOnlyFileType") && json["audioOnlyFileType"].is_number_integer())
        {
            int value{ json.at("audioOnlyFileType").get<int>() };
            if (value >= static_cast<int>(file::Type::VideoGeneric) && value <= static_cast<int>(file::Type::Ogg))
            {
                options.m_audio_only_file_type = static_cast<file::Type>(value);
            }
        }
        else if (json.contains("AudioOnlyFileType") && json["AudioOnlyFileType"].is_number_integer())
        {
            int value{ json.at("AudioOnlyFileType").get<int>() };
            if (value >= static_cast<int>(file::Type::VideoGeneric) && value <= static_cast<int>(file::Type::Ogg))
            {
                options.m_audio_only_file_type = static_cast<file::Type>(value);
            }
        }
        if (json.contains("downloadImmediatelyAsAudio") && json["downloadImmediatelyAsAudio"].is_boolean())
        {
            options.m_download_immediately_as_audio = json.at("downloadImmediatelyAsAudio").get<bool>();
        }
        else if (json.contains("DownloadImmediatelyAsAudio") && json["DownloadImmediatelyAsAudio"].is_boolean())
        {
            options.m_download_immediately_as_audio = json.at("DownloadImmediatelyAsAudio").get<bool>();
        }
        if (json.contains("downloadImmediatelyAsVideo") && json["downloadImmediatelyAsVideo"].is_boolean())
        {
            options.m_download_immediately_as_video = json.at("downloadImmediatelyAsVideo").get<bool>();
        }
        else if (json.contains("DownloadImmediatelyAsVideo") && json["DownloadImmediatelyAsVideo"].is_boolean())
        {
            options.m_download_immediately_as_video = json.at("DownloadImmediatelyAsVideo").get<bool>();
        }
        if (json.contains("exportDescription") && json["exportDescription"].is_boolean())
        {
            options.m_export_description = json.at("exportDescription").get<bool>();
        }
        else if (json.contains("ExportDescription") && json["ExportDescription"].is_boolean())
        {
            options.m_export_description = json.at("ExportDescription").get<bool>();
        }
        if (json.contains("exportM3U") && json["exportM3U"].is_boolean())
        {
            options.m_export_m3u = json.at("exportM3U").get<bool>();
        }
        else if (json.contains("ExportM3U") && json["ExportM3U"].is_boolean())
        {
            options.m_export_m3u = json.at("ExportM3U").get<bool>();
        }
        if (json.contains("fullFileType") && json["fullFileType"].is_number_integer())
        {
            int value{ json.at("fullFileType").get<int>() };
            if (value >= static_cast<int>(file::Type::VideoGeneric) && value <= static_cast<int>(file::Type::Ogg))
            {
                options.m_full_file_type = static_cast<file::Type>(value);
            }
        }
        else if (json.contains("FullFileType") && json["FullFileType"].is_number_integer())
        {
            int value{ json.at("FullFileType").get<int>() };
            if (value >= static_cast<int>(file::Type::VideoGeneric) && value <= static_cast<int>(file::Type::Ogg))
            {
                options.m_full_file_type = static_cast<file::Type>(value);
            }
        }
        if (json.contains("numberTitles") && json["numberTitles"].is_boolean())
        {
            options.m_number_titles = json.at("numberTitles").get<bool>();
        }
        else if (json.contains("NumberTitles") && json["NumberTitles"].is_boolean())
        {
            options.m_number_titles = json.at("NumberTitles").get<bool>();
        }
        if (json.contains("postprocessingArgumentNames") && json["postprocessingArgumentNames"].is_array())
        {
            options.m_postprocessing_argument_names.clear();
            for (const nlohmann::json& item: json["postprocessingArgumentNames"])
            {
                if (item.is_string())
                {
                    options.m_postprocessing_argument_names.push_back(item.get<std::string>());
                }
            }
        }
        if (json.contains("reverseDownloadOrder") && json["reverseDownloadOrder"].is_boolean())
        {
            options.m_reverse_download_order = json.at("reverseDownloadOrder").get<bool>();
        }
        else if (json.contains("ReverseDownloadOrder") && json["ReverseDownloadOrder"].is_boolean())
        {
            options.m_reverse_download_order = json.at("ReverseDownloadOrder").get<bool>();
        }
        if (json.contains("saveFolder") && json["saveFolder"].is_string())
        {
            options.m_save_folder = std::filesystem::path{ json.at("saveFolder").get<std::string>() };
        }
        else if (json.contains("SaveFolder") && json["SaveFolder"].is_string())
        {
            options.m_save_folder = std::filesystem::path{ json.at("SaveFolder").get<std::string>() };
        }
        if (json.contains("splitChapters") && json["splitChapters"].is_boolean())
        {
            options.m_split_chapters = json.at("splitChapters").get<bool>();
        }
        else if (json.contains("SplitChapters") && json["SplitChapters"].is_boolean())
        {
            options.m_split_chapters = json.at("SplitChapters").get<bool>();
        }
        if (json.contains("subtitleLanguages") && json["subtitleLanguages"].is_array())
        {
            options.m_subtitle_languages.clear();
            for (const nlohmann::json& item: json["subtitleLanguages"])
            {
                subtitle::Language subtitle{ "", false };
                from_json(item, subtitle);
                options.m_subtitle_languages.push_back(subtitle);
            }
        }
        if (json.contains("videoFormatIds") && json["videoFormatIds"].is_object())
        {
            for (const std::pair<const std::string, nlohmann::json>& pair:
                 json["videoFormatIds"].get<std::map<std::string, nlohmann::json>>())
            {
                int type{};
                if (pair.second.is_string() &&
                    std::from_chars(pair.first.data(), pair.first.data() + pair.first.size(), type).ec == std::errc{} &&
                    type >= static_cast<int>(file::Type::VideoGeneric) && type <= static_cast<int>(file::Type::Ogg))
                {
                    options.m_video_format_ids[static_cast<file::Type>(type)] = pair.second.get<std::string>();
                }
            }
        }
        if (json.contains("videoOnlyFileType") && json["videoOnlyFileType"].is_number_integer())
        {
            int value{ json.at("videoOnlyFileType").get<int>() };
            if (value >= static_cast<int>(file::Type::VideoGeneric) && value <= static_cast<int>(file::Type::Ogg))
            {
                options.m_video_only_file_type = static_cast<file::Type>(value);
            }
        }
        else if (json.contains("VideoOnlyFileType") && json["VideoOnlyFileType"].is_number_integer())
        {
            int value{ json.at("VideoOnlyFileType").get<int>() };
            if (value >= static_cast<int>(file::Type::VideoGeneric) && value <= static_cast<int>(file::Type::Ogg))
            {
                options.m_video_only_file_type = static_cast<file::Type>(value);
            }
        }
        if (json.contains("videoResolution") && json["videoResolution"].is_object())
        {
            options.m_video_resolution = json["videoResolution"].get<video::Resolution>();
        }
    }

    void from_json(const nlohmann::json& json, WindowGeometry& geometry)
    {
        if (json.contains("height") && json["height"].is_number_integer())
        {
            geometry.m_height = std::clamp(json.at("height").get<int>(), 1, std::numeric_limits<int>::max());
        }
        else if (json.contains("Height") && json["Height"].is_number_integer())
        {
            geometry.m_height = std::clamp(json.at("Height").get<int>(), 1, std::numeric_limits<int>::max());
        }
        if (json.contains("isMaximized") && json["isMaximized"].is_boolean())
        {
            geometry.m_is_maximized = json.at("isMaximized").get<bool>();
        }
        else if (json.contains("IsMaximized") && json["IsMaximized"].is_boolean())
        {
            geometry.m_is_maximized = json.at("IsMaximized").get<bool>();
        }
        if (json.contains("width") && json["width"].is_number_integer())
        {
            geometry.m_width = std::clamp(json.at("width").get<int>(), 1, std::numeric_limits<int>::max());
        }
        else if (json.contains("Width") && json["Width"].is_number_integer())
        {
            geometry.m_width = std::clamp(json.at("Width").get<int>(), 1, std::numeric_limits<int>::max());
        }
        if (json.contains("x") && json["x"].is_number_integer())
        {
            geometry.m_x = json.at("x").get<int>();
        }
        else if (json.contains("X") && json["X"].is_number_integer())
        {
            geometry.m_x = json.at("X").get<int>();
        }
        if (json.contains("y") && json["y"].is_number_integer())
        {
            geometry.m_y = json.at("y").get<int>();
        }
        else if (json.contains("Y") && json["Y"].is_number_integer())
        {
            geometry.m_y = json.at("Y").get<int>();
        }
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
        json.update(nlohmann::json(config.m_discovery_options));
        json.update(nlohmann::json(config.m_extractor_options));
        json.update(nlohmann::json(config.m_history_options));
        json.update(nlohmann::json(config.m_postprocessing_options));
        json.update(nlohmann::json(config.m_previous_download_options));
        json.update(nlohmann::json(config.m_subtitle_options));
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
            { "postprocessingArgumentNames", options.m_postprocessing_argument_names },
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
        for (const subtitle::Language& item: options.m_subtitle_languages)
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
