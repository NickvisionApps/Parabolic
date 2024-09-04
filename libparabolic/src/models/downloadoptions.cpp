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

    const std::variant<Quality, VideoResolution>& DownloadOptions::getQuality() const
    {
        return m_quality;
    }

    void DownloadOptions::setQuality(const std::variant<Quality, VideoResolution>& quality)
    {
        m_quality = quality;
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
        arguments.push_back("\"[download] PROGRESS;%(progress.status)s;%(progress.downloaded_bytes)s;%(progress.total_bytes)s;%(progress.total_bytes_estimate)s;%(progress.speed)s\"");
        arguments.push_back("--no-mtime");
        arguments.push_back("--ffmpeg-location");
        arguments.push_back(Environment::findDependency("ffmpeg").string());
        if(downloaderOptions.getOverwriteExistingFiles())
        {
            arguments.push_back("--force-overwrites");
        }
        else
        {
            arguments.push_back("--no-overwrites");
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
            arguments.push_back("aria2c:-x " + std::to_string(downloaderOptions.getAriaMaxConnectionsPerServer()) + " -k " + std::to_string(downloaderOptions.getAriaMinSplitSize()));
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
                arguments.push_back("--convert-thumbnails");
                arguments.push_back("jpg");
            }
            else
            {
                arguments.push_back("--write-thumbnail");
                arguments.push_back("--convert-thumbnails");
                arguments.push_back("jpg");
            }
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
        std::string formatSort;
        if(m_fileType.isAudio())
        {
            arguments.push_back("--extract-audio");
            arguments.push_back("--audio-format");
            arguments.push_back(StringHelpers::lower(m_fileType.str()));
            if(m_fileType != MediaFileType::FLAC)
            {
                formatSort += "aext:" + StringHelpers::lower(m_fileType.str());
            }
            else
            {
                formatSort += "acodec:flac";
            }
            if(m_quality.index() == 0)
            {
                arguments.push_back("--audio-quality");
                switch(std::get<Quality>(m_quality))
                {
                case Quality::Best:
                    arguments.push_back("0");
                    break;
                case Quality::Good:
                    arguments.push_back("5");
                    break;
                case Quality::Worst:
                    arguments.push_back("10");
                    break;
                default:
                    break;
                }
            }
            if(!m_audioLanguage.empty())
            {
                arguments.push_back("--format");
                size_t find{ m_audioLanguage.find(" (audio_description)") };
                if(find == std::string::npos)
                {
                    arguments.push_back("(ba[language=" + m_audioLanguage + "]/b)[format_id!*=?audiodesc]");
                }
                else
                {
                    std::string language{ m_audioLanguage.substr(0, find) };
                    arguments.push_back("(ba[language=" + language + "]/b)[format_id*=?audiodesc]");
                }
            }
        }
        else if(m_fileType.isVideo())
        {
            arguments.push_back("--remux-video");
            arguments.push_back(StringHelpers::lower(m_fileType.str()));
            formatSort += "vext:" + StringHelpers::lower(m_fileType.str());
            if(m_quality.index() == 1)
            {
                VideoResolution resolution{ std::get<VideoResolution>(m_quality) };
                arguments.push_back("--format");
                if(!resolution.isBest())
                {
                    size_t find{ m_audioLanguage.find(" (audio_description)") };
                    if(m_audioLanguage.empty())
                    {
                        arguments.push_back("bv[height<=" + std::to_string(resolution.getHeight()) + "][width<=" + std::to_string(resolution.getWidth()) + "]+ba/b");
                    }
                    else if(find == std::string::npos)
                    {
                        arguments.push_back("(bv[height<=" + std::to_string(resolution.getHeight()) + "][width<=" + std::to_string(resolution.getWidth()) + "]+ba[language= " + m_audioLanguage + "]/b)[format_id!*=?audiodesc]");
                    }
                    else
                    {
                        std::string language{ m_audioLanguage.substr(0, find) };
                        arguments.push_back("(bv[height<=" + std::to_string(resolution.getHeight()) + "][width<=" + std::to_string(resolution.getWidth()) + "]+ba[language= " + language + "]/b)[format_id*=?audiodesc]");
                    }
                }
                else
                {
                    size_t find{ m_audioLanguage.find(" (audio_description)") };
                    if(m_audioLanguage.empty())
                    {
                        arguments.push_back("bv+ba/b");
                    }
                    else if(find == std::string::npos)
                    {
                        arguments.push_back("(bv+ba[language= " + m_audioLanguage + "]/b)[format_id!*=?audiodesc]");
                    }
                    else
                    {
                        std::string language{ m_audioLanguage.substr(0, find) };
                        arguments.push_back("(bv+ba[language= " + language + "]/b)[format_id*=?audiodesc]");
                    }
                }
                
            }
        }
        if(m_preferAV1)
        {
            formatSort += ",vcodec:av01";
        }
        arguments.push_back("--format-sort");
        arguments.push_back(formatSort);
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
        if(m_downloadSubtitles)
        {
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
            arguments.push_back("all,-live_chat");
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
        return arguments;
    }
}