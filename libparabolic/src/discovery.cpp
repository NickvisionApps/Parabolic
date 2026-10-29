#include "discovery.h"
#include <filesystem>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <ranges>
#include <string>
#include <vector>
#include "audio.h"
#include "batch.h"
#include "keyring.h"
#include "subtitle.h"
#include "video.h"

namespace parabolic::discovery
{
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

    const std::string& Entry::suggested_save_folder() const
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

    const Options& Manager::options() const
    {
        return m_options;
    }

    Result::Result()
    {
        static unsigned int next_id{ 1 };
        m_id = next_id++;
    }

    Result::Result(std::string url, std::string title, const std::vector<Result>& results)
        : m_url{ std::move(url) },
          m_title{ std::move(title) }
    {
        for (const Result& result: results)
        {
            m_has_suggested_save_folder |= result.m_has_suggested_save_folder;
            m_entries.reserve(m_entries.size() + result.m_entries.size());
            m_entries.insert_range(m_entries.end(), result.m_entries);
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

    void Manager::set_options(const Options& options)
    {
        m_options = options;
    }

    Result Manager::discover_file(const std::filesystem::path& path,
                                              const std::optional<keyring::Credential>& credential) const
    {
        std::vector<Result> results;
        for (const batch::Item& item: batch::read_file(path))
        {
            results.push_back(discover(item.url(), item.suggested_save_folder(), item.suggested_filename(), credential));
        }
        return { path, path.stem().string(), results };
    }

    Result Manager::discover_url(const std::string& url,
                                             const std::optional<keyring::Credential>& credential) const
    {
        return discover(url, {}, {}, credential);
    }

    Result Manager::discover(const std::string& url, const std::filesystem::path& suggested_save_folder,
                                         const std::string& suggested_filename,
                                         const std::optional<keyring::Credential>& credential) const
    {
        return { url, suggested_filename, {} };
    }

    void from_json(const nlohmann::json& json, Options& options)
    {
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
    }

    void to_json(nlohmann::json& json, const Options& options)
    {
        json = nlohmann::json{
            { "includeMediaIdInTitle", options.m_include_media_id_in_title },
            { "includeSuperResolutions", options.m_include_super_resolutions },
            { "limitCharacters", options.m_limit_characters },
            { "preferredAudioCodec", static_cast<int>(options.m_preferred_audio_codec) },
            { "preferredFrameRate", static_cast<int>(options.m_preferred_frame_rate) },
            { "preferredSubtitleFormat", static_cast<int>(options.m_preferred_subtitle_format) },
            { "preferredVideoCodec", static_cast<int>(options.m_preferred_video_codec) },
        };
    }
}
