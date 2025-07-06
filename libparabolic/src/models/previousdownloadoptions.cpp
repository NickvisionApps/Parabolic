#include "models/previousdownloadoptions.h"
#include <libnick/filesystem/userdirectories.h>
#include "models/format.h"

using namespace Nickvision::App;
using namespace Nickvision::Filesystem;

namespace Nickvision::TubeConverter::Shared::Models
{
    PreviousDownloadOptions::PreviousDownloadOptions(const std::string& key, const std::string& appName)
        : DataFileBase{ key, appName }
    {

    }

    std::filesystem::path PreviousDownloadOptions::getSaveFolder() const
    {
        std::filesystem::path path{ m_json["SaveFolder"].is_string() ? m_json["SaveFolder"].as_string().c_str() : UserDirectories::get(UserDirectory::Downloads).string() };
        if(std::filesystem::exists(path))
        {
            return path.make_preferred();
        }
        return UserDirectories::get(UserDirectory::Downloads).make_preferred();
    }

    void PreviousDownloadOptions::setSaveFolder(std::filesystem::path previousSaveFolder)
    {
        if(std::filesystem::exists(previousSaveFolder))
        {
            m_json["SaveFolder"] = previousSaveFolder.make_preferred().string();
        }
        else
        {
            m_json["SaveFolder"] = UserDirectories::get(UserDirectory::Downloads).make_preferred().string();
        }
    }

    MediaFileType PreviousDownloadOptions::getFileType() const
    {
        return { m_json["FileType"].is_int64() ? static_cast<MediaFileType::MediaFileTypeValue>(m_json["FileType"].as_int64()) : MediaFileType::MP4 };
    }

    void PreviousDownloadOptions::setFileType(const MediaFileType& previousMediaFileType)
    {
        m_json["FileType"] = static_cast<int>(previousMediaFileType);
    }

    std::string PreviousDownloadOptions::getVideoFormatId(const MediaFileType& type) const
    {
        static Format bestVideoFormat{ FormatValue::Best, MediaType::Video };
        static Format noneAudioFormat{ FormatValue::None, MediaType::Audio };
        std::string key{ "VideoFormatId_" + type.str() };
        return m_json[key].is_string() ? m_json[key].as_string().c_str() : (type.isAudio() ? noneAudioFormat.getId() : bestVideoFormat.getId());
    }

    void PreviousDownloadOptions::setVideoFormatId(const MediaFileType& type, const std::string& videoFormatId)
    {
        std::string key{ "VideoFormatId_" + type.str() };
        m_json[key] = videoFormatId;
    }

    std::string PreviousDownloadOptions::getAudioFormatId(const MediaFileType& type) const
    {
        static Format bestAudioFormat{ FormatValue::Best, MediaType::Audio };
        std::string key{ "AudioFormatId_" + type.str() };
        return m_json[key].is_string() ? m_json[key].as_string().c_str() : bestAudioFormat.getId();
    }

    void PreviousDownloadOptions::setAudioFormatId(const MediaFileType& type, const std::string& audioFormatId)
    {
        std::string key{ "AudioFormatId_" + type.str() };
        m_json[key] = audioFormatId;
    }

    bool PreviousDownloadOptions::getSplitChapters() const
    {
        return m_json["SplitChapters"].is_bool() ? m_json["SplitChapters"].as_bool() : false;
    }

    void PreviousDownloadOptions::setSplitChapters(bool splitChapters)
    {
        m_json["SplitChapters"] = splitChapters;
    }

    bool PreviousDownloadOptions::getExportDescription() const
    {
        return m_json["ExportDescription"].is_bool() ? m_json["ExportDescription"].as_bool() : false;
    }

    void PreviousDownloadOptions::setExportDescription(bool exportDescription)
    {
        m_json["ExportDescription"] = exportDescription;
    }

    std::string PreviousDownloadOptions::getPostProcessorArgument() const
    {
        return m_json["PostProcessorArgument"].is_string() ? m_json["PostProcessorArgument"].as_string().c_str() : "";
    }

    void PreviousDownloadOptions::setPostProcessorArgument(const std::string& postProcessorArgument)
    {
        m_json["PostProcessorArgument"] = postProcessorArgument;
    }

    bool PreviousDownloadOptions::getWritePlaylistFile() const
    {
        return m_json["WritePlaylistFile"].is_bool() ? m_json["WritePlaylistFile"].as_bool() : false;
    }

    void PreviousDownloadOptions::setWritePlaylistFile(bool writePlaylistFile)
    {
        m_json["WritePlaylistFile"] = writePlaylistFile;
    }

    bool PreviousDownloadOptions::getNumberTitles() const
    {
        return m_json["NumberTitles"].is_bool() ? m_json["NumberTitles"].as_bool() : false;
    }

    void PreviousDownloadOptions::setNumberTitles(bool numberTitles)
    {
        m_json["NumberTitles"] = numberTitles;
    }

    std::vector<SubtitleLanguage> PreviousDownloadOptions::getSubtitleLanguages() const
    {
        std::vector<SubtitleLanguage> languages;
        if(m_json.contains("SubtitleLanguages") && m_json["SubtitleLanguages"].is_array())
        {
            for(const boost::json::value& language : m_json["SubtitleLanguages"].as_array())
            {
                if(language.is_object())
                {
                    boost::json::object obj = language.as_object();
                    languages.push_back({ obj["Language"].is_string() ? obj["Language"].as_string().c_str() : "", obj["AutoGenerated"].is_bool() ? obj["AutoGenerated"].as_bool() : false });
                }
            }
        }
        return languages;
    }

    void PreviousDownloadOptions::setSubtitleLanguages(const std::vector<SubtitleLanguage>& previousSubtitleLanguages)
    {
        boost::json::array languages;
        for(const SubtitleLanguage& language : previousSubtitleLanguages)
        {
            boost::json::object obj;
            obj["Language"] = language.getLanguage();
            obj["AutoGenerated"] = language.isAutoGenerated();
            languages.push_back(obj);
        }
        m_json["SubtitleLanguages"] = languages;
    }
}
