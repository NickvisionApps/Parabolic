#include "download.h"
#include <algorithm>
#include <charconv>
#include <filesystem>
#include <map>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <sago/platform_folders.h>
#include <string>
#include <system_error>
#include <utility>
#include <vector>
#include "file.h"
#include "subtitle.h"
#include "video.h"
#ifdef _WIN32
#include "string_manip.h"
#endif

namespace parabolic::download
{
    int Options::max_number_of_active_downloads() const
    {
        return m_max_number_of_active_downloads;
    }

    void Options::set_max_number_of_active_downloads(int value)
    {
        m_max_number_of_active_downloads = value;
    }

    bool Options::overwrite_existing_files() const
    {
        return m_overwrite_existing_files;
    }

    void Options::set_overwrite_existing_files(bool value)
    {
        m_overwrite_existing_files = value;
    }

    bool Options::prevent_suspend() const
    {
        return m_prevent_suspend;
    }

    void Options::set_prevent_suspend(bool value)
    {
        m_prevent_suspend = value;
    }

    const std::string& Options::ytdlp_arguments() const
    {
        return m_ytdlp_arguments;
    }

    void Options::set_ytdlp_arguments(const std::string& value)
    {
        m_ytdlp_arguments = value;
    }

    PreviousDownloadOptions::PreviousDownloadOptions()
        : m_audio_format_ids{
              { file::Type::VideoGeneric, file::Format::best_audio().id() },
              { file::Type::Mp4, file::Format::best_audio().id() },
              { file::Type::Mkv, file::Format::best_audio().id() },
              { file::Type::Webm, file::Format::best_audio().id() },
              { file::Type::Mov, file::Format::best_audio().id() },
              { file::Type::Avi, file::Format::best_audio().id() },
              { file::Type::AudioGeneric, file::Format::best_audio().id() },
              { file::Type::Mp3, file::Format::best_audio().id() },
              { file::Type::M4a, file::Format::best_audio().id() },
              { file::Type::Opus, file::Format::best_audio().id() },
              { file::Type::Flac, file::Format::best_audio().id() },
              { file::Type::Wav, file::Format::best_audio().id() },
              { file::Type::Ogg, file::Format::best_audio().id() },
          },
#ifdef _WIN32
          m_save_folder{ std::filesystem::path(string_manip::wstr(sago::getDownloadFolder())) },
#else
          m_save_folder{ sago::getDownloadFolder() },
#endif
          m_video_format_ids{
              { file::Type::VideoGeneric, file::Format::best_video().id() },
              { file::Type::Mp4, file::Format::best_video().id() },
              { file::Type::Mkv, file::Format::best_video().id() },
              { file::Type::Webm, file::Format::best_video().id() },
              { file::Type::Mov, file::Format::best_video().id() },
              { file::Type::Avi, file::Format::best_video().id() },
              { file::Type::AudioGeneric, file::Format::none_video().id() },
              { file::Type::Mp3, file::Format::none_video().id() },
              { file::Type::M4a, file::Format::none_video().id() },
              { file::Type::Opus, file::Format::none_video().id() },
              { file::Type::Flac, file::Format::none_video().id() },
              { file::Type::Wav, file::Format::none_video().id() },
              { file::Type::Ogg, file::Format::none_video().id() },
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

    void Manager::set_options(const Options& options)
    {
        m_options = options;
    }

    void from_json(const nlohmann::json& json, Options& options)
    {
        if (json.contains("maxNumberOfActiveDownloads") && json["maxNumberOfActiveDownloads"].is_number_integer())
        {
            options.m_max_number_of_active_downloads =
                    std::clamp(json.at("maxNumberOfActiveDownloads").get<int>(), 1, 10);
        }
        else if (json.contains("MaxNumberOfActiveDownloads") && json["MaxNumberOfActiveDownloads"].is_number_integer())
        {
            options.m_max_number_of_active_downloads =
                    std::clamp(json.at("MaxNumberOfActiveDownloads").get<int>(), 1, 10);
        }
        if (json.contains("overwriteExistingFiles") && json["overwriteExistingFiles"].is_boolean())
        {
            options.m_overwrite_existing_files = json.at("overwriteExistingFiles").get<bool>();
        }
        else if (json.contains("OverwriteExistingFiles") && json["OverwriteExistingFiles"].is_boolean())
        {
            options.m_overwrite_existing_files = json.at("OverwriteExistingFiles").get<bool>();
        }
        if (json.contains("preventSuspend") && json["preventSuspend"].is_boolean())
        {
            options.m_prevent_suspend = json.at("preventSuspend").get<bool>();
        }
        else if (json.contains("PreventSuspend") && json["PreventSuspend"].is_boolean())
        {
            options.m_prevent_suspend = json.at("PreventSuspend").get<bool>();
        }
        if (json.contains("ytdlpDownloadArgs") && json["ytdlpDownloadArgs"].is_string())
        {
            options.m_ytdlp_arguments = json.at("ytdlpDownloadArgs").get<std::string>();
        }
        else if (json.contains("YtdlpDownloadArgs") && json["YtdlpDownloadArgs"].is_string())
        {
            options.m_ytdlp_arguments = json.at("YtdlpDownloadArgs").get<std::string>();
        }
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
            for (const std::pair<const std::string, nlohmann::json>& pair :
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
            for (const nlohmann::json& item : json["postprocessingArgumentNames"])
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
            for (const nlohmann::json& item : json["subtitleLanguages"])
            {
                subtitle::Language subtitle{ "", false };
                from_json(item, subtitle);
                options.m_subtitle_languages.push_back(subtitle);
            }
        }
        if (json.contains("videoFormatIds") && json["videoFormatIds"].is_object())
        {
            for (const std::pair<const std::string, nlohmann::json>& pair :
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

    void to_json(nlohmann::json& json, const Options& options)
    {
        json = nlohmann::json{
            { "maxNumberOfActiveDownloads", options.m_max_number_of_active_downloads },
            { "overwriteExistingFiles", options.m_overwrite_existing_files },
            { "preventSuspend", options.m_prevent_suspend },
            { "ytdlpDownloadArgs", options.m_ytdlp_arguments },
        };
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
        for (const std::pair<const file::Type, std::string>& pair : options.m_audio_format_ids)
        {
            json["audioFormatIds"][std::to_string(static_cast<int>(pair.first))] = pair.second;
        }
        for (const subtitle::Language& item : options.m_subtitle_languages)
        {
            json["subtitleLanguages"].push_back(item);
        }
        for (const std::pair<const file::Type, std::string>& pair : options.m_video_format_ids)
        {
            json["videoFormatIds"][std::to_string(static_cast<int>(pair.first))] = pair.second;
        }
    }
}
