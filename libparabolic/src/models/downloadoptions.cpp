#include "models/downloadoptions.h"
#include <libnick/helpers/stringhelpers.h>
#include <libnick/system/environment.h>

using namespace Nickvision::Helpers;
using namespace Nickvision::Keyring;
using namespace Nickvision::System;

namespace Nickvision::TubeConverter::Shared::Models
{
    DownloadOptions::DownloadOptions()
        : m_fileType{ MediaFileType::MP4 },
        m_limitSpeed{ false },
        m_splitChapters{ false }
    {

    }

    DownloadOptions::DownloadOptions(const std::string& url)
        : m_url{ url },
        m_fileType{ MediaFileType::MP4 },
        m_limitSpeed{ false },
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

    const std::vector<Format>& DownloadOptions::getAvailableFormats() const
    {
        return m_availableFormats;
    }

    void DownloadOptions::setAvailableFormats(const std::vector<Format>& availableFormats)
    {
        m_availableFormats = availableFormats;
    }

    const std::optional<Format>& DownloadOptions::getVideoFormat() const
    {
        return m_videoFormat;
    }

    void DownloadOptions::setVideoFormat(const std::optional<Format>& videoFormat)
    {
        m_videoFormat = videoFormat;
    }

    const std::optional<Format>& DownloadOptions::getAudioFormat() const
    {
        return m_audioFormat;
    }

    void DownloadOptions::setAudioFormat(const std::optional<Format>& audioFormat)
    {
        m_audioFormat = audioFormat;
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

    const std::vector<SubtitleLanguage>& DownloadOptions::getSubtitleLanguages() const
    {
        return m_subtitleLanguages;
    }

    void DownloadOptions::setSubtitleLanguages(const std::vector<SubtitleLanguage>& subtitleLanguages)
    {
        m_subtitleLanguages = subtitleLanguages;
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

    std::vector<std::string> DownloadOptions::toArgumentVector(const DownloaderOptions& downloaderOptions) const
    {
        std::vector<std::string> arguments;
        arguments.push_back(m_url);
        arguments.push_back("--xff");
        arguments.push_back("default");
        arguments.push_back("--no-warnings");
        arguments.push_back("--progress");
        arguments.push_back("--newline");;
        arguments.push_back("--progress-template");
        arguments.push_back("[download] PROGRESS;%(progress.status)s;%(progress.downloaded_bytes)s;%(progress.total_bytes)s;%(progress.total_bytes_estimate)s;%(progress.speed)s");
        arguments.push_back("--no-mtime");
        arguments.push_back("--ffmpeg-location");
        arguments.push_back(Environment::findDependency("ffmpeg").string());
        if(downloaderOptions.getOverwriteExistingFiles() && !shouldDownloadResume())
        {
            arguments.push_back("--force-overwrites");
        }
        if(downloaderOptions.getLimitCharacters())
        {
            arguments.push_back("--windows-filenames");
        }
        if(m_credential)
        {
            if(!m_credential->getUsername().empty() && !m_credential->getPassword().empty())
            {
                arguments.push_back("--username");
                arguments.push_back(m_credential->getUsername());
                arguments.push_back("--password");
                arguments.push_back(m_credential->getPassword());
            }
            else if(!m_credential->getPassword().empty())
            {
                arguments.push_back("--video-password");
                arguments.push_back(m_credential->getPassword());
            }
        }
        if(downloaderOptions.getUseAria())
        {
            arguments.push_back("--downloader");
            arguments.push_back(Environment::findDependency("aria2c").string());
            arguments.push_back("--downloader-args");
            arguments.push_back("aria2c:-x " + std::to_string(downloaderOptions.getAriaMaxConnectionsPerServer()) + " -k " + std::to_string(downloaderOptions.getAriaMinSplitSize()) + "M");
        }
        else
        {
            arguments.push_back("--downloader");
            arguments.push_back("native");
        }
        if(!downloaderOptions.getProxyUrl().empty())
        {
            arguments.push_back("--proxy");
            arguments.push_back(downloaderOptions.getProxyUrl());
        }
        if(downloaderOptions.getCookiesBrowser() != Browser::None && Environment::getDeploymentMode() == DeploymentMode::Local)
        {
            arguments.push_back("--cookies-from-browser");
            switch(downloaderOptions.getCookiesBrowser())
            {
            case Browser::Brave:
                arguments.push_back("brave");
                break;
            case Browser::Chrome:
                arguments.push_back("chrome");
                break;
            case Browser::Chromium:
                arguments.push_back("chromium");
                break;
            case Browser::Edge:
                arguments.push_back("edge");
                break;
            case Browser::Firefox:
                arguments.push_back("firefox");
                break;
            case Browser::Opera:
                arguments.push_back("opera");
                break;
            case Browser::Vivaldi:
                arguments.push_back("vivaldi");
                break;
            case Browser::Whale:
                arguments.push_back("whale");
                break;
            default:
                break;
            }
        }
        else if(std::filesystem::exists(downloaderOptions.getCookiesPath()))
        {
            arguments.push_back("--cookies");
            arguments.push_back(downloaderOptions.getCookiesPath().string());
        }
        if(downloaderOptions.getYouTubeSponsorBlock())
        {
            arguments.push_back("--sponsorblock-remove");
            arguments.push_back("default");
        }
        if(downloaderOptions.getEmbedMetadata())
        {
            arguments.push_back("--embed-metadata");
            if(m_fileType.supportsThumbnails())
            {
                arguments.push_back("--embed-thumbnail");
            }
            else
            {
                arguments.push_back("--write-thumbnail");
            }
            arguments.push_back("--convert-thumbnails");
            arguments.push_back("jpg");
            if(downloaderOptions.getCropAudioThumbnails())
            {
                arguments.push_back("--postprocessor-args");
                arguments.push_back("ThumbnailsConvertor:-vf crop=ih:ih");
            }
            if(downloaderOptions.getRemoveSourceData())
            {
                arguments.push_back("--parse-metadata");
                arguments.push_back(":(?P<meta_comment>):(?P<meta_description>):(?P<meta_synopsis>):(?P<meta_purl>)");
            }
        }
        if(downloaderOptions.getEmbedChapters())
        {
            arguments.push_back("--embed-chapters");
        }
        if(m_fileType.isAudio())
        {
            arguments.push_back("--extract-audio");
            arguments.push_back("--audio-format");
            arguments.push_back(StringHelpers::lower(m_fileType.str()));
        }
        else if(m_fileType.isVideo())
        {
            arguments.push_back("--remux-video");
            arguments.push_back(StringHelpers::lower(m_fileType.str()));
        }
        //Force preferred video codec sorting for playlist downloads to use as format selection is not available
        if(downloaderOptions.getPreferredVideoCodec() != VideoCodec::Any)
        {
            std::string vcodec{ "vcodec:" };
            switch (downloaderOptions.getPreferredVideoCodec())
            {
            case VideoCodec::VP9:
                vcodec += "vp9";
                break;
            case VideoCodec::AV01:
                vcodec += "av01";
                break;
            case VideoCodec::H264:
                vcodec += "h264";
                break;            
            }
            arguments.push_back("--format-sort");
            arguments.push_back(vcodec);
            arguments.push_back("--format-sort-force");
        }
        if(m_videoFormat && m_audioFormat)
        {
            arguments.push_back("--format");
            arguments.push_back(m_videoFormat->getId() + "+" + m_audioFormat->getId());
        }
        else if(m_videoFormat)
        {
            arguments.push_back("--format");
            arguments.push_back(m_videoFormat->getId() + "+ba");
        }
        else if(m_audioFormat)
        {
            arguments.push_back("--format");
            if(m_fileType.isVideo())
            {
                arguments.push_back("bv+" + m_audioFormat->getId());
            }
            else
            {
                arguments.push_back(m_audioFormat->getId());
            }
        }
        if(!std::filesystem::exists(m_saveFolder))
        {
            std::filesystem::create_directories(m_saveFolder);
        }
        arguments.push_back("--paths");
        arguments.push_back(m_saveFolder.string());
        arguments.push_back("--output");
        arguments.push_back(m_saveFilename + ".%(ext)s");
        arguments.push_back("--output");
        arguments.push_back("chapter:%(section_number)03d - " + m_saveFilename + ".%(ext)s");
        if(!m_subtitleLanguages.empty())
        {
            std::string languages;
            for(const SubtitleLanguage& language : m_subtitleLanguages)
            {
                languages += language.getLanguage() + ",";
            }
            languages += "-live_chat";
            if(downloaderOptions.getEmbedSubtitles())
            {
                arguments.push_back("--embed-subs");
            }
            arguments.push_back("--write-subs");
            if(downloaderOptions.getIncludeAutoGeneratedSubtitles())
            {
                arguments.push_back("--write-auto-subs");
            }
            arguments.push_back("--sub-langs");
            arguments.push_back(languages);
        }
        if(m_limitSpeed)
        {
            arguments.push_back("--limit-rate");
            arguments.push_back(std::to_string(downloaderOptions.getSpeedLimit()) + "K");
        }
        if(m_splitChapters)
        {
            arguments.push_back("--split-chapters");
        }
        if(m_timeFrame.has_value())
        {
            arguments.push_back("--download-sections");
            arguments.push_back("*" + m_timeFrame->str());
        }
        arguments.push_back("--postprocessor-args");
        arguments.push_back("-threads " + std::to_string(downloaderOptions.getPostprocessingThreads()));
        return arguments;
    }

    bool DownloadOptions::shouldDownloadResume() const
    {
        if(std::filesystem::exists(m_saveFolder / (m_saveFilename + ".part")))
        {
            return true;
        }
        for(const Format& format : m_availableFormats)
        {
            if(std::filesystem::exists(m_saveFolder / (m_saveFilename + ".f" + format.getId() + "." + format.getExtension() + ".part")))
            {
                return true;
            }
        }
        return false;
    }
}