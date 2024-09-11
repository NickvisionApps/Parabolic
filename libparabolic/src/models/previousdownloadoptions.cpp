#include "models/previousdownloadoptions.h"
#include <libnick/filesystem/userdirectories.h>

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
            return path;
        }
        return UserDirectories::get(UserDirectory::Downloads);
    }

    void PreviousDownloadOptions::setSaveFolder(const std::filesystem::path& previousSaveFolder)
    {
        if(std::filesystem::exists(previousSaveFolder))
        {
            m_json["SaveFolder"] = previousSaveFolder.string();
        }
        else
        {
            m_json["SaveFolder"] = UserDirectories::get(UserDirectory::Downloads).string();
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

    bool PreviousDownloadOptions::getDownloadSubtitles() const
    {
        return m_json["DownloadSubtitles"].is_bool() ? m_json["DownloadSubtitles"].as_bool() : false;
    }

    void PreviousDownloadOptions::setDownloadSubtitles(bool previousSubtitleState)
    {
        m_json["DownloadSubtitles"] = previousSubtitleState;
    }

    VideoCodec PreviousDownloadOptions::getVideoCodec() const
    {
        return m_json["VideoCodec"].is_int64() ? static_cast<VideoCodec>(m_json["VideoCodec"].as_int64()) : VideoCodec::VP9;
    }

    void PreviousDownloadOptions::setVideoCodec(VideoCodec codec)
    {
        m_json["VideoCodec"] = static_cast<int>(codec);
    }

    bool PreviousDownloadOptions::getSplitChapters() const
    {
        return m_json["SplitChapters"].is_bool() ? m_json["SplitChapters"].as_bool() : false;
    }

    void PreviousDownloadOptions::setSplitChapters(bool splitChapters)
    {
        m_json["SplitChapters"] = splitChapters;
    }

    bool PreviousDownloadOptions::getLimitSpeed() const
    {
        return m_json["LimitSpeed"].is_bool() ? m_json["LimitSpeed"].as_bool() : false;
    }

    void PreviousDownloadOptions::setLimitSpeed(bool limitSpeed)
    {
        m_json["LimitSpeed"] = limitSpeed;
    }

    bool PreviousDownloadOptions::getNumberTitles() const
    {
        return m_json["NumberTitles"].is_bool() ? m_json["NumberTitles"].as_bool() : false;
    }

    void PreviousDownloadOptions::setNumberTitles(bool numberTitles)
    {
        m_json["NumberTitles"] = numberTitles;
    }
}