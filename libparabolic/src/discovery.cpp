#include "discovery.h"
#include <algorithm>
#include <chrono>
#include <compare>
#include <cstddef>
#include <expected>
#include <filesystem>
#include <format>
#include <map>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <sago/platform_folders.h>
#include <stop_token>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "audio.h"
#include "batch.h"
#include "cookies.h"
#include "deno.h"
#include "extractor.h"
#include "ffmpeg.h"
#include "file.h"
#include "keyring.h"
#include "string_manip.h"
#include "subtitle.h"
#include "url.h"
#include "video.h"
#include "ytdlp.h"

namespace
{
    bool is_preferred(const parabolic::file::Format& format, parabolic::audio::Codec preferred)
    {
        return !format.audio_codec() || preferred == parabolic::audio::Codec::Any || *format.audio_codec() == preferred;
    }

    bool is_preferred(const parabolic::file::Format& format, parabolic::video::FrameRate preferred)
    {
        return !format.frame_rate() || preferred == parabolic::video::FrameRate::Any ||
               *format.frame_rate() == preferred;
    }

    bool is_preferred(const parabolic::file::Format& format, parabolic::video::Codec preferred)
    {
        return !format.video_codec() || preferred == parabolic::video::Codec::Any || *format.video_codec() == preferred;
    }
}

namespace parabolic::discovery
{
    bool Options::include_auto_generated_subtitles() const
    {
        return m_include_auto_generated_subtitles;
    }

    void Options::set_include_auto_generated_subtitles(bool value)
    {
        m_include_auto_generated_subtitles = value;
    }

    bool Options::include_media_id_in_title() const
    {
        return m_include_media_id_in_title;
    }

    void Options::set_include_media_id_in_title(bool value)
    {
        m_include_media_id_in_title = value;
    }

    bool Options::include_super_resolutions() const
    {
        return m_include_super_resolutions;
    }

    void Options::set_include_super_resolutions(bool value)
    {
        m_include_super_resolutions = value;
    }

    bool Options::limit_characters() const
    {
        return m_limit_characters;
    }

    void Options::set_limit_characters(bool value)
    {
        m_limit_characters = value;
    }

    audio::Codec Options::preferred_audio_codec() const
    {
        return m_preferred_audio_codec;
    }

    void Options::set_preferred_audio_codec(audio::Codec value)
    {
        m_preferred_audio_codec = value;
    }

    video::FrameRate Options::preferred_frame_rate() const
    {
        return m_preferred_frame_rate;
    }

    void Options::set_preferred_frame_rate(video::FrameRate value)
    {
        m_preferred_frame_rate = value;
    }

    subtitle::Type Options::preferred_subtitle_format() const
    {
        return m_preferred_subtitle_format;
    }

    void Options::set_preferred_subtitle_format(subtitle::Type value)
    {
        m_preferred_subtitle_format = value;
    }

    video::Codec Options::preferred_video_codec() const
    {
        return m_preferred_video_codec;
    }

    void Options::set_preferred_video_codec(video::Codec value)
    {
        m_preferred_video_codec = value;
    }

    const std::string& Options::ytdlp_arguments() const
    {
        return m_ytdlp_arguments;
    }

    void Options::set_ytdlp_arguments(const std::string& value)
    {
        m_ytdlp_arguments = value;
    }

    Entry::Entry(const nlohmann::json& json, const Options& options, std::filesystem::path suggested_save_folder,
                 std::string suggested_filename)
        : m_title{ std::move(suggested_filename) },
          m_suggested_save_folder{ std::move(suggested_save_folder) }
    {
        if (json.contains("playlist_index") && json["playlist_index"].is_number_integer())
        {
            m_playlist_index = json.at("playlist_index").get<int>();
        }
        std::string url_value;
        std::string webpage_url_value;
        if (json.contains("url") && json["url"].is_string())
        {
            url_value = json.at("url").get<std::string>();
        }
        if (json.contains("webpage_url") && json["webpage_url"].is_string())
        {
            webpage_url_value = json.at("webpage_url").get<std::string>();
        }
        if (m_playlist_index != -1)
        {
            if (!url_value.empty() && url::is_valid(url_value) && !url_value.contains(".m3u8") &&
                !url_value.contains(".mpd") && !url_value.contains(".f4m") && !url_value.contains(".ism"))
            {
                m_url = url_value;
            }
            if (m_url.empty() && !webpage_url_value.empty() && url::is_valid(webpage_url_value))
            {
                m_url = webpage_url_value;
            }
        }
        else
        {
            if (!webpage_url_value.empty() && url::is_valid(webpage_url_value))
            {
                m_url = webpage_url_value;
            }
            if (m_url.empty() && !url_value.empty() && url::is_valid(url_value))
            {
                m_url = url_value;
            }
        }
        if (m_title.empty() && json.contains("title") && json["title"].is_string())
        {
            m_title = json.at("title").get<std::string>();
        }
        if (options.include_media_id_in_title() && json.contains("display_id") && json["display_id"].is_string())
        {
            std::string display_id{ json.at("display_id").get<std::string>() };
            if (!display_id.empty())
            {
                m_title += std::format(" [{}]", display_id);
            }
        }
        m_title = string_manip::sanitize_for_filename(m_title, options.limit_characters());
        if (json.contains("duration") && json["duration"].is_number())
        {
            m_time_frame = { std::chrono::seconds::zero(),
                             std::chrono::seconds(static_cast<long long>(json.at("duration").get<double>())) };
        }
        bool has_video_formats{ false };
        bool has_audio_formats{ false };
        if (json.contains("formats") && json["formats"].is_array())
        {
            std::vector<file::Format> skipped_formats;
            for (const nlohmann::json& format_json : json["formats"])
            {
                file::Format format{ format_json.get<file::Format>() };
                if (format.kind() == file::Kind::Image)
                {
                    continue;
                }
                if (!options.include_super_resolutions() && string_manip::lower(format.id()).ends_with("-sr"))
                {
                    continue;
                }
                if (format.kind() == file::Kind::Video)
                {
                    has_video_formats = true;
                }
                else if (format.kind() == file::Kind::Audio)
                {
                    has_audio_formats = true;
                }
                if (!is_preferred(format, options.preferred_video_codec()) ||
                    !is_preferred(format, options.preferred_audio_codec()) ||
                    !is_preferred(format, options.preferred_frame_rate()))
                {
                    skipped_formats.push_back(std::move(format));
                    continue;
                }
                m_formats.push_back(std::move(format));
            }
            if (m_formats.empty() && !skipped_formats.empty())
            {
                std::vector<file::Format> preferred_codec_formats;
                for (const file::Format& format : skipped_formats)
                {
                    if (is_preferred(format, options.preferred_video_codec()) &&
                        is_preferred(format, options.preferred_audio_codec()))
                    {
                        preferred_codec_formats.push_back(format);
                    }
                }
                m_formats.append_range(!preferred_codec_formats.empty() ? preferred_codec_formats : skipped_formats);
            }
            else
            {
                bool has_selected_video_format{ false };
                bool has_selected_audio_format{ false };
                bool has_skipped_video_format{ false };
                bool has_skipped_audio_format{ false };
                for (const file::Format& format : m_formats)
                {
                    if (format == file::Format::none_video() || format == file::Format::none_audio())
                    {
                        continue;
                    }
                    if (format.kind() == file::Kind::Video)
                    {
                        has_selected_video_format = true;
                    }
                    else if (format.kind() == file::Kind::Audio)
                    {
                        has_selected_audio_format = true;
                    }
                }
                for (const file::Format& format : skipped_formats)
                {
                    if (format.kind() == file::Kind::Video)
                    {
                        has_skipped_video_format = true;
                    }
                    else if (format.kind() == file::Kind::Audio)
                    {
                        has_skipped_audio_format = true;
                    }
                }
                if (!has_selected_video_format && has_skipped_video_format)
                {
                    std::vector<file::Format> preferred_codec_formats;
                    for (const file::Format& format : skipped_formats)
                    {
                        if (format.kind() == file::Kind::Video && is_preferred(format, options.preferred_video_codec()))
                        {
                            preferred_codec_formats.push_back(format);
                        }
                    }
                    if (!preferred_codec_formats.empty())
                    {
                        m_formats.append_range(preferred_codec_formats);
                    }
                    else
                    {
                        for (const file::Format& format : skipped_formats)
                        {
                            if (format.kind() == file::Kind::Video)
                            {
                                m_formats.push_back(format);
                            }
                        }
                    }
                }
                else if (!has_selected_audio_format && has_skipped_audio_format)
                {
                    std::vector<file::Format> preferred_codec_formats;
                    for (const file::Format& format : skipped_formats)
                    {
                        if (format.kind() == file::Kind::Audio && is_preferred(format, options.preferred_audio_codec()))
                        {
                            preferred_codec_formats.push_back(format);
                        }
                    }
                    if (!preferred_codec_formats.empty())
                    {
                        m_formats.append_range(preferred_codec_formats);
                    }
                    else
                    {
                        for (const file::Format& format : skipped_formats)
                        {
                            if (format.kind() == file::Kind::Audio)
                            {
                                m_formats.push_back(format);
                            }
                        }
                    }
                }
            }
            std::ranges::sort(m_formats);
        }
        if (options.include_auto_generated_subtitles() && json.contains("automatic_captions") &&
            json["automatic_captions"].is_object())
        {
            for (const std::pair<const std::string, nlohmann::json>& pair :
                 json["automatic_captions"].get<std::map<std::string, nlohmann::json>>())
            {
                m_subtitle_languages.emplace_back(pair.first, true);
            }
        }
        if (json.contains("subtitles") && json["subtitles"].is_object())
        {
            for (const std::pair<const std::string, nlohmann::json>& pair :
                 json["subtitles"].get<std::map<std::string, nlohmann::json>>())
            {
                if (pair.first == "live_chat")
                {
                    continue;
                }
                m_subtitle_languages.emplace_back(pair.first, false);
            }
        }
        std::ranges::sort(m_subtitle_languages);
        if (json.contains("thumbnail") && json["thumbnail"].is_string())
        {
            std::string thumbnail_url{ json.at("thumbnail").get<std::string>() };
            if (!thumbnail_url.empty() && url::is_valid(thumbnail_url))
            {
                m_thumbnail_url = thumbnail_url;
            }
        }
        std::vector<file::Format> built_in_formats;
        if (has_video_formats && has_audio_formats)
        {
            m_kind = file::Kind::Video;
            built_in_formats = {
                file::Format::none_audio(),  file::Format::none_video(), file::Format::best_video(),
                file::Format::worst_video(), file::Format::best_audio(), file::Format::worst_audio(),
            };
        }
        else if (has_audio_formats)
        {
            m_kind = file::Kind::Audio;
            built_in_formats = {
                file::Format::none_audio(),
                file::Format::none_video(),
                file::Format::best_audio(),
                file::Format::worst_audio(),
            };
        }
        else
        {
            m_kind = file::Kind::Video;
            built_in_formats = {
                file::Format::none_audio(),
                file::Format::none_video(),
                file::Format::best_video(),
                file::Format::worst_video(),
            };
        }
        m_formats.append_range(built_in_formats);
    }

    const std::string& Entry::url() const
    {
        return m_url;
    }

    const std::string& Entry::title() const
    {
        return m_title;
    }

    int Entry::playlist_index() const
    {
        return m_playlist_index;
    }

    bool Entry::requires_playlist_items_option() const
    {
        return m_requires_playlist_items_option;
    }

    void Entry::set_requires_playlist_items_option(bool value)
    {
        m_requires_playlist_items_option = value;
    }

    file::Kind Entry::kind() const
    {
        return m_kind;
    }

    const file::TimeFrame& Entry::time_frame() const
    {
        return m_time_frame;
    }

    const std::vector<file::Format>& Entry::formats() const
    {
        return m_formats;
    }

    const std::vector<subtitle::Language>& Entry::subtitle_languages() const
    {
        return m_subtitle_languages;
    }

    const std::string& Entry::thumbnail_url() const
    {
        return m_thumbnail_url;
    }

    const std::filesystem::path& Entry::suggested_save_folder() const
    {
        return m_suggested_save_folder;
    }

    std::strong_ordering Entry::operator<=>(const Entry& rhs) const
    {
        return m_title <=> rhs.m_title;
    }

    bool Entry::operator==(const Entry& rhs) const
    {
        return m_url == rhs.m_url;
    }

    Result::Result(std::string url, std::string title)
        : m_url{ std::move(url) },
          m_title{ std::move(title) }
    {
        static unsigned int next_id{ 1 };
        m_id = next_id++;
    }

    Result::Result(const nlohmann::json& json, const Options& options, std::string url,
                   std::filesystem::path suggested_save_folder, std::string suggested_filename)
        : Result{ std::move(url), std::move(suggested_filename) }
    {
        m_has_suggested_save_folder = !suggested_save_folder.empty();
        if (json.contains("title") && json["title"].is_string())
        {
            m_title = json.at("title").get<std::string>();
        }
        if (json.contains("entries") && json["entries"].is_array() && !json["entries"].empty())
        {
            for (const nlohmann::json& entry : json["entries"])
            {
                if (entry.is_object())
                {
                    m_entries.emplace_back(entry, options, suggested_save_folder, "");
                }
            }
            std::unordered_map<std::string, int> url_counts;
            url_counts.reserve(m_entries.size());
            for (const Entry& entry : m_entries)
            {
                if (entry.playlist_index() != -1 && !entry.url().empty())
                {
                    ++url_counts[entry.url()];
                }
            }
            for (Entry& entry : m_entries)
            {
                if (entry.playlist_index() != -1 && !entry.url().empty() && url_counts[entry.url()] > 1)
                {
                    entry.set_requires_playlist_items_option(true);
                }
            }
        }
        else
        {
            m_entries.emplace_back(json, options, suggested_save_folder, suggested_filename);
        }
    }

    Result::Result(std::string url, std::string title, const std::vector<Result>& results)
        : Result{ std::move(url), std::move(title) }
    {
        for (const Result& result : results)
        {
            m_has_suggested_save_folder |= result.m_has_suggested_save_folder;
            m_entries.reserve(m_entries.size() + result.m_entries.size());
            m_entries.append_range(result.m_entries);
        }
    }

    unsigned int Result::id() const
    {
        return m_id;
    }

    const std::string& Result::url() const
    {
        return m_url;
    }

    const std::string& Result::title() const
    {
        return m_title;
    }

    bool Result::has_suggested_save_folder() const
    {
        return m_has_suggested_save_folder;
    }

    const std::vector<Entry>& Result::entries() const
    {
        return m_entries;
    }

    bool Result::is_playlist() const
    {
        return m_entries.size() > 1;
    }

    bool Result::empty() const
    {
        return m_entries.empty();
    }

    size_t Result::size() const
    {
        return m_entries.size();
    }

    std::vector<Entry>::const_iterator Result::begin() const
    {
        return m_entries.begin();
    }

    std::vector<Entry>::const_iterator Result::end() const
    {
        return m_entries.end();
    }

    std::vector<Entry>::const_reverse_iterator Result::rbegin() const
    {
        return m_entries.rbegin();
    }

    std::vector<Entry>::const_reverse_iterator Result::rend() const
    {
        return m_entries.rend();
    }

    std::vector<Entry>::const_iterator Result::cbegin() const
    {
        return m_entries.cbegin();
    }

    std::vector<Entry>::const_iterator Result::cend() const
    {
        return m_entries.cend();
    }

    std::vector<Entry>::const_reverse_iterator Result::crbegin() const
    {
        return m_entries.crbegin();
    }

    std::vector<Entry>::const_reverse_iterator Result::crend() const
    {
        return m_entries.crend();
    }

    void Result::sort()
    {
        std::ranges::sort(m_entries);
    }

    const Entry& Result::operator[](size_t index) const
    {
        return m_entries[index];
    }

    void Manager::set_options(const Options& discovery_options, const extractor::Options& extractor_options)
    {
        m_discovery_options = discovery_options;
        m_extractor_options = extractor_options;
    }

    std::expected<Result, std::string> Manager::discover_file(const std::filesystem::path& path,
                                                              const std::optional<keyring::Credential>& credential,
                                                              const std::stop_token& stop_token) const
    {
        std::vector<Result> results;
        std::string error{ "No URLs found in the batch file." };
        for (const batch::Item& item : batch::read_file(path))
        {
            if (stop_token.stop_requested())
            {
                return std::unexpected{ "Operation cancelled." };
            }
            std::expected<Result, std::string> result{
                discover(item.url(), item.suggested_save_folder(), item.suggested_filename(), credential, stop_token),
            };
            if (!result)
            {
                error = result.error();
                continue;
            }
            results.push_back(std::move(result.value()));
        }
        if (results.empty())
        {
            return std::unexpected{ error };
        }
#ifdef _WIN32
        return Result{ string_manip::str(path.wstring()), string_manip::str(path.stem().wstring()), results };
#else
        return Result{ path.string(), path.stem().string(), results };
#endif
    }

    std::expected<Result, std::string> Manager::discover_url(const std::string& url,
                                                             const std::optional<keyring::Credential>& credential,
                                                             const std::stop_token& stop_token) const
    {
        return discover(url, "", "", credential, stop_token);
    }

    std::expected<Result, std::string> Manager::discover(const std::string& url,
                                                         const std::filesystem::path& suggested_save_folder,
                                                         const std::string& suggested_filename,
                                                         const std::optional<keyring::Credential>& credential,
                                                         const std::stop_token& stop_token) const
    {
        std::vector<std::string> args{
            url,
            "--ignore-config",
            "--dump-single-json",
            "--skip-download",
            "--ignore-errors",
            "--no-warnings",
            "--ffmpeg-location",
            parabolic::ffmpeg::executable_path(),
            "--js-runtimes",
            std::format("deno:{}", parabolic::deno::executable_path()),
            "--paths",
            std::format("temp:{}", sago::getCacheDir()),
        };
        std::string plugins{ parabolic::ytdlp::plugins_path() };
        if (std::filesystem::exists(plugins))
        {
            args.emplace_back("--plugin-dir");
            args.push_back(plugins);
        }
        if (m_discovery_options.limit_characters())
        {
            args.emplace_back("--windows-filenames");
        }
        if (!m_extractor_options.proxy_url().empty())
        {
            args.emplace_back("--proxy");
            args.push_back(m_extractor_options.proxy_url());
        }
        if (credential)
        {
            if (!credential->username().empty() && !credential->password().empty())
            {
                args.emplace_back("--username");
                args.push_back(credential->username());
                args.emplace_back("--password");
                args.push_back(credential->password());
            }
            else if (!credential->password().empty())
            {
                args.emplace_back("--video-password");
                args.push_back(credential->password());
            }
        }
        if (m_extractor_options.cookies_browser() != cookies::Browser::None)
        {
            args.emplace_back("--cookies-from-browser");
            args.push_back(cookies::to_argument_str(m_extractor_options.cookies_browser()));
        }
        else if (std::filesystem::exists(m_extractor_options.cookies_path()))
        {
            args.emplace_back("--cookies");
            args.push_back(m_extractor_options.cookies_path());
        }
        args.append_range(string_manip::command_line_split(m_discovery_options.ytdlp_arguments()));
        std::expected<std::string, std::string> result{ ytdlp::execute(args, {}, stop_token) };
        if (!result)
        {
            return std::unexpected{ result.error() };
        }
        std::string output{ string_manip::trim(std::move(result.value())) };
        if (!output.starts_with('{'))
        {
            return std::unexpected{ "yt-dlp did not return a valid JSON." };
        }
        nlohmann::json json = nlohmann::json::parse(output, nullptr, false, true);
        if (!json.is_object())
        {
            return std::unexpected{ "Failed to parse JSON from yt-dlp." };
        }
        if (json.contains("entries") && json["entries"].is_array() && !json["entries"].empty())
        {
            std::vector<Result> results;
            for (const nlohmann::json& entry : json["entries"])
            {
                if (entry.is_object() && entry.contains("ie_key") && entry["ie_key"].is_string() &&
                    entry.at("ie_key").get<std::string>() == "YoutubeTab" && entry.contains("url") &&
                    entry["url"].is_string())
                {
                    std::expected<Result, std::string> tab{
                        discover(entry.at("url").get<std::string>(), suggested_save_folder, suggested_filename,
                                 credential, stop_token),
                    };
                    if (!tab)
                    {
                        return std::unexpected{ tab.error() };
                    }
                    results.push_back(std::move(tab.value()));
                }
            }
            if (!results.empty() && json.contains("title") && json["title"].is_string())
            {
                return Result{ url, json.at("title").get<std::string>(), results };
            }
        }
        return Result{ json, m_discovery_options, url, suggested_save_folder, suggested_filename };
    }

    void from_json(const nlohmann::json& json, Options& options)
    {
        if (json.contains("includeAutoGeneratedSubtitles") && json["includeAutoGeneratedSubtitles"].is_boolean())
        {
            options.m_include_auto_generated_subtitles = json.at("includeAutoGeneratedSubtitles").get<bool>();
        }
        else if (json.contains("IncludeAutoGeneratedSubtitles") && json["IncludeAutoGeneratedSubtitles"].is_boolean())
        {
            options.m_include_auto_generated_subtitles = json.at("IncludeAutoGeneratedSubtitles").get<bool>();
        }
        if (json.contains("includeMediaIdInTitle") && json["includeMediaIdInTitle"].is_boolean())
        {
            options.m_include_media_id_in_title = json.at("includeMediaIdInTitle").get<bool>();
        }
        else if (json.contains("IncludeMediaIdInTitle") && json["IncludeMediaIdInTitle"].is_boolean())
        {
            options.m_include_media_id_in_title = json.at("IncludeMediaIdInTitle").get<bool>();
        }
        if (json.contains("includeSuperResolutions") && json["includeSuperResolutions"].is_boolean())
        {
            options.m_include_super_resolutions = json.at("includeSuperResolutions").get<bool>();
        }
        else if (json.contains("IncludeSuperResolutions") && json["IncludeSuperResolutions"].is_boolean())
        {
            options.m_include_super_resolutions = json.at("IncludeSuperResolutions").get<bool>();
        }
        if (json.contains("limitCharacters") && json["limitCharacters"].is_boolean())
        {
            options.m_limit_characters = json.at("limitCharacters").get<bool>();
        }
        else if (json.contains("LimitCharacters") && json["LimitCharacters"].is_boolean())
        {
            options.m_limit_characters = json.at("LimitCharacters").get<bool>();
        }
        if (json.contains("preferredAudioCodec") && json["preferredAudioCodec"].is_number_integer())
        {
            int value{ json.at("preferredAudioCodec").get<int>() };
            if (value >= static_cast<int>(audio::Codec::Any) && value <= static_cast<int>(audio::Codec::Mp3))
            {
                options.m_preferred_audio_codec = static_cast<audio::Codec>(value);
            }
        }
        else if (json.contains("PreferredAudioCodec") && json["PreferredAudioCodec"].is_number_integer())
        {
            int value{ json.at("PreferredAudioCodec").get<int>() };
            if (value >= static_cast<int>(audio::Codec::Any) && value <= static_cast<int>(audio::Codec::Mp3))
            {
                options.m_preferred_audio_codec = static_cast<audio::Codec>(value);
            }
        }
        if (json.contains("preferredFrameRate") && json["preferredFrameRate"].is_number_integer())
        {
            int value{ json.at("preferredFrameRate").get<int>() };
            if (value >= static_cast<int>(video::FrameRate::Any) && value <= static_cast<int>(video::FrameRate::Fps60))
            {
                options.m_preferred_frame_rate = static_cast<video::FrameRate>(value);
            }
        }
        else if (json.contains("PreferredFrameRate") && json["PreferredFrameRate"].is_number_integer())
        {
            int value{ json.at("PreferredFrameRate").get<int>() };
            if (value >= static_cast<int>(video::FrameRate::Any) && value <= static_cast<int>(video::FrameRate::Fps60))
            {
                options.m_preferred_frame_rate = static_cast<video::FrameRate>(value);
            }
        }
        if (json.contains("preferredSubtitleFormat") && json["preferredSubtitleFormat"].is_number_integer())
        {
            int value{ json.at("preferredSubtitleFormat").get<int>() };
            if (value >= static_cast<int>(subtitle::Type::Any) && value <= static_cast<int>(subtitle::Type::Lrc))
            {
                options.m_preferred_subtitle_format = static_cast<subtitle::Type>(value);
            }
        }
        else if (json.contains("PreferredSubtitleFormat") && json["PreferredSubtitleFormat"].is_number_integer())
        {
            int value{ json.at("PreferredSubtitleFormat").get<int>() };
            if (value >= static_cast<int>(subtitle::Type::Any) && value <= static_cast<int>(subtitle::Type::Lrc))
            {
                options.m_preferred_subtitle_format = static_cast<subtitle::Type>(value);
            }
        }
        if (json.contains("preferredVideoCodec") && json["preferredVideoCodec"].is_number_integer())
        {
            int value{ json.at("preferredVideoCodec").get<int>() };
            if (value >= static_cast<int>(video::Codec::Any) && value <= static_cast<int>(video::Codec::H264))
            {
                options.m_preferred_video_codec = static_cast<video::Codec>(value);
            }
        }
        else if (json.contains("PreferredVideoCodec") && json["PreferredVideoCodec"].is_number_integer())
        {
            int value{ json.at("PreferredVideoCodec").get<int>() };
            if (value >= static_cast<int>(video::Codec::Any) && value <= static_cast<int>(video::Codec::H264))
            {
                options.m_preferred_video_codec = static_cast<video::Codec>(value);
            }
        }
        if (json.contains("ytdlpDiscoveryArgs") && json["ytdlpDiscoveryArgs"].is_string())
        {
            options.m_ytdlp_arguments = json.at("ytdlpDiscoveryArgs").get<std::string>();
        }
        else if (json.contains("YtdlpDiscoveryArgs") && json["YtdlpDiscoveryArgs"].is_string())
        {
            options.m_ytdlp_arguments = json.at("YtdlpDiscoveryArgs").get<std::string>();
        }
    }

    void to_json(nlohmann::json& json, const Options& options)
    {
        json = nlohmann::json{
            { "includeAutoGeneratedSubtitles", options.m_include_auto_generated_subtitles },
            { "includeMediaIdInTitle", options.m_include_media_id_in_title },
            { "includeSuperResolutions", options.m_include_super_resolutions },
            { "limitCharacters", options.m_limit_characters },
            { "preferredAudioCodec", static_cast<int>(options.m_preferred_audio_codec) },
            { "preferredFrameRate", static_cast<int>(options.m_preferred_frame_rate) },
            { "preferredSubtitleFormat", static_cast<int>(options.m_preferred_subtitle_format) },
            { "preferredVideoCodec", static_cast<int>(options.m_preferred_video_codec) },
            { "ytdlpDiscoveryArgs", options.m_ytdlp_arguments },
        };
    }

    void to_json(nlohmann::json& json, const Entry& entry)
    {
        json = nlohmann::json{
            { "webpage_url", entry.m_url },
            { "url", entry.m_url },
            { "title", entry.m_title },
            { "formats", nlohmann::json::array() },
            { "subtitles", nlohmann::json::object() },
            { "automatic_captions", nlohmann::json::object() },
        };
        if (entry.m_playlist_index != -1)
        {
            json["playlist_index"] = entry.m_playlist_index;
        }
        if (entry.m_time_frame.end() > std::chrono::seconds::zero())
        {
            json["duration"] = entry.m_time_frame.end().count();
        }
        if (!entry.m_thumbnail_url.empty())
        {
            json["thumbnail"] = entry.m_thumbnail_url;
        }
        if (!entry.m_suggested_save_folder.empty())
        {
#ifdef _WIN32
            json["suggestedSaveFolder"] = string_manip::str(entry.m_suggested_save_folder.wstring());
#else
            json["suggestedSaveFolder"] = entry.m_suggested_save_folder.string();
#endif
        }
        for (const file::Format& format : entry.m_formats)
        {
            if (format == file::Format::best_video() || format == file::Format::worst_video() ||
                format == file::Format::none_video() || format == file::Format::best_audio() ||
                format == file::Format::worst_audio() || format == file::Format::none_audio())
            {
                continue;
            }
            json["formats"].push_back(format);
        }
        for (const subtitle::Language& language : entry.m_subtitle_languages)
        {
            json[language.is_auto_generated() ? "automatic_captions" : "subtitles"][language.language()] =
                    nlohmann::json::array();
        }
    }
}
