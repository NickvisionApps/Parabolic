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
        std::filesystem::path path{ m_json.get("SaveFolder", UserDirectories::get(UserDirectory::Downloads).string()).asString() };
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
        return { static_cast<MediaFileType::MediaFileTypeValue>(m_json.get("FileType", static_cast<int>(MediaFileType::MP4)).asInt()) };
    }

    void PreviousDownloadOptions::setFileType(const MediaFileType& previousMediaFileType)
    {
        m_json["FileType"] = static_cast<int>(previousMediaFileType);
    }

    bool PreviousDownloadOptions::getDownloadSubtitles() const
    {
        return m_json.get("DownloadSubtitles", false).asBool();
    }

    void PreviousDownloadOptions::setDownloadSubtitles(bool previousSubtitleState)
    {
        m_json["DownloadSubtitles"] = previousSubtitleState;
    }

    bool PreviousDownloadOptions::getPreferAV1() const
    {
        return m_json.get("PreferAV1State", false).asBool();
    }

    void PreviousDownloadOptions::setPreferAV1(bool previousPreferAV1State)
    {
        m_json["PreferAV1State"] = previousPreferAV1State;
    }

    bool PreviousDownloadOptions::getSplitChapters() const
    {
        return m_json.get("SplitChapters", false).asBool();
    }

    void PreviousDownloadOptions::setSplitChapters(bool splitChapters)
    {
        m_json["SplitChapters"] = splitChapters;
    }

    bool PreviousDownloadOptions::getLimitSpeed() const
    {
        return m_json.get("LimitSpeed", false).asBool();
    }

    void PreviousDownloadOptions::setLimitSpeed(bool limitSpeed)
    {
        m_json["LimitSpeed"] = limitSpeed;
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