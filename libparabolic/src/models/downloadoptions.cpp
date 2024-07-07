#include "models/downloadoptions.h"

using namespace Nickvision::Keyring;

namespace Nickvision::TubeConverter::Shared::Models
{
    DownloadOptions::DownloadOptions()
        : m_fileType{ MediaFileType::MP4 },
        m_downloadSubtitles{ false },
        m_limitSpeed{ false },
        m_preferAV1{ false },
        m_splitChapters{ false }
    {

    }

    DownloadOptions::DownloadOptions(const std::string& url)
        : m_url{ url },
        m_fileType{ MediaFileType::MP4 },
        m_downloadSubtitles{ false },
        m_limitSpeed{ false },
        m_preferAV1{ false },
        m_splitChapters{ false }
    {

    }

    const std::string& DownloadOptions::getUrl() const
    {
        return m_url;
    }

    void DownloadOptions::setUrl(const std::string& url)
    {
        m_url = url;
    }

    const std::optional<Credential>& DownloadOptions::getCredential() const
    {
        return m_credential;
    }

    void DownloadOptions::setCredential(const std::optional<Credential>& credential)
    {
        m_credential = credential;
    }

    const MediaFileType& DownloadOptions::getFileType() const
    {
        return m_fileType;
    }

    void DownloadOptions::setFileType(const MediaFileType& fileType)
    {
        m_fileType = fileType;
    }

    const std::variant<Quality, VideoResolution>& DownloadOptions::getResolution() const
    {
        return m_resolution;
    }

    void DownloadOptions::setResolution(const std::variant<Quality, VideoResolution>& resolution)
    {
        m_resolution = resolution;
    }

    const std::filesystem::path& DownloadOptions::getSaveFolder() const
    {
        return m_saveFolder;
    }

    void DownloadOptions::setSaveFolder(const std::filesystem::path& saveFolder)
    {
        m_saveFolder = saveFolder;
    }

    const std::string& DownloadOptions::getSaveFilename() const
    {
        return m_saveFilename;
    }

    void DownloadOptions::setSaveFilename(const std::string& saveFilename)
    {
        m_saveFilename = saveFilename;
    }

    const std::string& DownloadOptions::getAudioLanguage() const
    {
        return m_audioLanguage;
    }

    void DownloadOptions::setAudioLanguage(const std::string& audioLanguage)
    {
        m_audioLanguage = audioLanguage;
    }

    bool DownloadOptions::getDownloadSubtitles() const
    {
        return m_downloadSubtitles;
    }

    void DownloadOptions::setDownloadSubtitles(bool downloadSubtitles)
    {
        m_downloadSubtitles = downloadSubtitles;
    }

    const std::optional<unsigned int>& DownloadOptions::getPlaylistPosition() const
    {
        return m_playlistPosition;
    }

    void DownloadOptions::setPlaylistPosition(const std::optional<unsigned int>& playlistPosition)
    {
        m_playlistPosition = playlistPosition;
    }

    bool DownloadOptions::getLimitSpeed() const
    {
        return m_limitSpeed;
    }

    void DownloadOptions::setLimitSpeed(bool limitSpeed)
    {
        if(limitSpeed && m_timeFrame.has_value())
        {
            return;
        }
        m_limitSpeed = limitSpeed;
    }

    bool DownloadOptions::getPreferAV1() const
    {
        return m_preferAV1;
    }

    void DownloadOptions::setPreferAV1(bool preferAV1)
    {
        m_preferAV1 = preferAV1;
    }

    bool DownloadOptions::getSplitChapters() const
    {
        return m_splitChapters;
    }

    void DownloadOptions::setSplitChapters(bool splitChapters)
    {
        m_splitChapters = splitChapters;
    }

    const std::optional<TimeFrame>& DownloadOptions::getTimeFrame() const
    {
        return m_timeFrame;
    }

    void DownloadOptions::setTimeFrame(const std::optional<TimeFrame>& timeFrame)
    {
        if(timeFrame && m_limitSpeed)
        {
            return;
        }
        m_timeFrame = timeFrame;
    }
}