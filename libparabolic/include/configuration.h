#pragma once

#include <charconv>
#include <filesystem>
#include <limits>
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
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

    class Configuration
    {
    public:
        Configuration(std::string_view app_name);
        [[nodiscard]] bool allow_preview_updates() const;
        void set_allow_preview_updates(bool value);
        [[nodiscard]] int aria_max_connections_per_server() const;
        void set_aria_max_connections_per_server(int value);
        [[nodiscard]] int aria_min_split_size() const;
        void set_aria_min_split_size(int value);
        [[nodiscard]] bool automatically_check_for_updates() const;
        void set_automatically_check_for_updates(bool value);
        [[nodiscard]] browser::Browser cookies_browser() const;
        void set_cookies_browser(browser::Browser value);
        [[nodiscard]] std::string cookies_path() const;
        void set_cookies_path(const std::string& value);
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
        [[nodiscard]] history::Length history_length() const;
        void set_history_length(history::Length value);
        [[nodiscard]] bool history_sort_newest() const;
        void set_history_sort_newest(bool value);
        [[nodiscard]] bool include_auto_generated_subtitles() const;
        void set_include_auto_generated_subtitles(bool value);
        [[nodiscard]] bool include_media_id_in_title() const;
        void set_include_media_id_in_title(bool value);
        [[nodiscard]] bool include_super_resolutions() const;
        void set_include_super_resolutions(bool value);
        [[nodiscard]] bool limit_characters() const;
        void set_limit_characters(bool value);
        [[nodiscard]] int max_number_of_active_downloads() const;
        void set_max_number_of_active_downloads(int value);
        [[nodiscard]] bool overwrite_existing_files() const;
        void set_overwrite_existing_files(bool value);
        [[nodiscard]] std::vector<post_processor::Argument> postprocessing_arguments() const;
        void set_postprocessing_arguments(const std::vector<post_processor::Argument>& value);
        [[nodiscard]] int postprocessing_threads() const;
        void set_postprocessing_threads(int value);
        [[nodiscard]] media::AudioCodec preferred_audio_codec() const;
        void set_preferred_audio_codec(media::AudioCodec value);
        [[nodiscard]] media::FrameRate preferred_frame_rate() const;
        void set_preferred_frame_rate(media::FrameRate value);
        [[nodiscard]] subtitle::Format preferred_subtitle_format() const;
        void set_preferred_subtitle_format(subtitle::Format value);
        [[nodiscard]] media::VideoCodec preferred_video_codec() const;
        void set_preferred_video_codec(media::VideoCodec value);
        [[nodiscard]] bool prevent_suspend() const;
        void set_prevent_suspend(bool value);
        [[nodiscard]] PreviousDownloadOptions previous_download_options() const;
        void set_previous_download_options(const PreviousDownloadOptions& value);
        [[nodiscard]] std::string proxy_url() const;
        void set_proxy_url(const std::string& value);
        [[nodiscard]] bool remove_source_data() const;
        void set_remove_source_data(bool value);
        [[nodiscard]] bool show_disclaimer_on_startup() const;
        void set_show_disclaimer_on_startup(bool value);
        [[nodiscard]] std::optional<int> speed_limit() const;
        void set_speed_limit(std::optional<int> value);
        [[nodiscard]] Theme theme() const;
        void set_theme(Theme value);
        [[nodiscard]] bool translate_metadata_and_chapters() const;
        void set_translate_metadata_and_chapters(bool value);
        [[nodiscard]] std::string translation_language() const;
        void set_translation_language(const std::string& value);
        [[nodiscard]] bool use_aria() const;
        void set_use_aria(bool value);
        [[nodiscard]] bool use_part_files() const;
        void set_use_part_files(bool value);
        [[nodiscard]] WindowGeometry window_geometry() const;
        void set_window_geometry(const WindowGeometry& value);
        [[nodiscard]] bool youtube_sponsor_block() const;
        void set_youtube_sponsor_block(bool value);
        [[nodiscard]] std::string ytdlp_discovery_args() const;
        void set_ytdlp_discovery_args(const std::string& value);
        [[nodiscard]] std::string ytdlp_download_args() const;
        void set_ytdlp_download_args(const std::string& value);
        [[nodiscard]] std::string str() const;

    private:
        void save() const;
        std::filesystem::path m_path;
        std::string m_cookies_path;
        std::vector<post_processor::Argument> m_postprocessing_arguments;
        std::string m_proxy_url;
        std::string m_translation_language;
        std::string m_ytdlp_discovery_args;
        std::string m_ytdlp_download_args;
        mutable std::mutex m_mutex;
        PreviousDownloadOptions m_previous_download_options;
        int m_aria_max_connections_per_server{ 16 };
        int m_aria_min_split_size{ 20 };
        int m_max_number_of_active_downloads{ 5 };
        int m_postprocessing_threads;
        std::optional<int> m_speed_limit;
        WindowGeometry m_window_geometry;
        history::Length m_history_length{ history::Length::OneWeek };
        bool m_allow_preview_updates{ false };
        bool m_automatically_check_for_updates{ true };
        browser::Browser m_cookies_browser{ browser::Browser::None };
        bool m_crop_audio_thumbnails{ false };
        bool m_embed_chapters{ false };
        bool m_embed_metadata{ true };
        bool m_embed_subtitles{ true };
        bool m_embed_thumbnails{ true };
        bool m_history_sort_newest{ true };
        bool m_include_auto_generated_subtitles{ true };
        bool m_include_media_id_in_title{ true };
        bool m_include_super_resolutions{ true };
        bool m_limit_characters{ false };
        bool m_overwrite_existing_files{ true };
        media::AudioCodec m_preferred_audio_codec{ media::AudioCodec::Any };
        media::FrameRate m_preferred_frame_rate{ media::FrameRate::Any };
        subtitle::Format m_preferred_subtitle_format{ subtitle::Format::Any };
        media::VideoCodec m_preferred_video_codec{ media::VideoCodec::Any };
        bool m_prevent_suspend{ false };
        bool m_remove_source_data{ false };
        bool m_show_disclaimer_on_startup{ true };
        Theme m_theme{ Theme::System };
        bool m_translate_metadata_and_chapters{ true };
        bool m_use_aria{ false };
        bool m_use_part_files{ true };
        bool m_youtube_sponsor_block{ false };
        friend void from_json(const nlohmann::json& json, Configuration& config);
        friend void to_json(nlohmann::json& json, const Configuration& config);
    };

    inline void from_json(const nlohmann::json& json, Configuration& config)
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

    inline void from_json(const nlohmann::json& json, PreviousDownloadOptions& options)
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

    inline void from_json(const nlohmann::json& json, WindowGeometry& geometry)
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

    inline void to_json(nlohmann::json& json, const Configuration& config)
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

    inline void to_json(nlohmann::json& json, const PreviousDownloadOptions& options)
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

    inline void to_json(nlohmann::json& json, const WindowGeometry& geometry)
    {
        json = nlohmann::json{
            { "height", geometry.m_height }, { "isMaximized", geometry.m_is_maximized },
            { "width", geometry.m_width },   { "x", geometry.m_x },
            { "y", geometry.m_y },
        };
    }
}
