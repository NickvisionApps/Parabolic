#include "models/previousdownloadoptions.h"

using namespace Nickvision::App;

namespace Nickvision::TubeConverter::Shared::Models
{
    PreviousDownloadOptions::PreviousDownloadOptions(const std::string& key, const std::string& appName)
        : DataFileBase{ key, appName }
    {
        
    }

    std::filesystem::path PreviousDownloadOptions::getSaveFolder() const
    {
        std::filesystem::path path{ m_json.get("SaveFolder", "").asString() };
        if(std::filesystem::exists(path))
        {
            return path;
        }
        return {};
    }

    void PreviousDownloadOptions::setSaveFolder(const std::filesystem::path& previousSaveFolder)
    {
        if(std::filesystem::exists(previousSaveFolder))
        {
            m_json["SaveFolder"] = previousSaveFolder.string();
        }
        else
        {
            m_json["SaveFolder"] = "";
        }
    }

    MediaFileType PreviousDownloadOptions::getFileType() const
    {
        return { static_cast<MediaFileType::MediaFileTypeValue>(m_json.get("FileType", static_cast<int>(MediaFileType::MP4)).asInt()) };
    }

    void PreviousDownloadOptions::setFileType(const MediaFileType& previousMediaFileType)
    {
        m_json["FileType"] = static_cast<int>(previousMediaFileType);
    }

    VideoResolution PreviousDownloadOptions::getVideoResolution() const
    {
        std::optional<VideoResolution> resolution{ VideoResolution::parse(m_json.get("VideoResolution", "").asString()) };
        if(resolution)
        {
            return resolution.value();
        }
        return VideoResolution{ -1, -1 };
    }

    void PreviousDownloadOptions::setVideoResolution(const VideoResolution& previousVideoResolution)
    {
        m_json["VideoResolution"] = previousVideoResolution.str();
    }

    bool PreviousDownloadOptions::getDownloadSubtitles() const
    {
        return m_json.get("DownloadSubtitles", false).asBool();
    }

    void PreviousDownloadOptions::setDownloadSubtitles(bool previousSubtitleState)
    {
        m_json["DownloadSubtitles"] = previousSubtitleState;
    }

    bool PreviousDownloadOptions::getPreferAV1State() const
    {
        return m_json.get("PreferAV1State", false).asBool();
    }

    void PreviousDownloadOptions::setPreferAV1State(bool previousPreferAV1State)
    {
        m_json["PreferAV1State"] = previousPreferAV1State;
    }

    bool PreviousDownloadOptions::getNumberTitles() const
    {
        return m_json.get("NumberTitles", false).asBool();
    }

    void PreviousDownloadOptions::setNumberTitles(bool numberTitles)
    {
        m_json["NumberTitles"] = numberTitles;
    }
}