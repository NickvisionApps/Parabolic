#include "models/downloadoptions.h"
#include <libnick/helpers/stringhelpers.h>
#include <libnick/system/environment.h>
#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <linux/limits.h>
#else
#include <sys/syslimits.h>
#endif

using namespace Nickvision::Helpers;
using namespace Nickvision::Keyring;
using namespace Nickvision::System;

namespace Nickvision::TubeConverter::Shared::Models
{
    DownloadOptions::DownloadOptions()
        : m_fileType{ MediaFileType::MP4 },
        m_splitChapters{ false },
        m_exportDescription{ false },
        m_playlistPosition{ -1 }
    {

    }

    DownloadOptions::DownloadOptions(const std::string& url)
        : m_url{ url },
        m_fileType{ MediaFileType::MP4 },
        m_splitChapters{ false },
        m_exportDescription{ false },
        m_playlistPosition{ -1 }
    {

    }

    DownloadOptions::DownloadOptions(boost::json::object json)
        : m_url{ json["Url"].is_string() ? json["Url"].as_string().c_str() : "" },
        m_fileType{ json["FileType"].is_int64() ? static_cast<MediaFileType::MediaFileTypeValue>(json["FileType"].as_int64()) : MediaFileType::MP4 },
        m_saveFolder{ json["SaveFolder"].is_string() ? json["SaveFolder"].as_string().c_str() : "" },
        m_saveFilename{ json["SaveFilename"].is_string() ? json["SaveFilename"].as_string().c_str() : "" },
        m_splitChapters{ json["SplitChapters"].is_bool() ? json["SplitChapters"].as_bool() : false },
        m_exportDescription{ json["ExportDescription"].is_bool() ? json["ExportDescription"].as_bool() : false },
        m_playlistPosition{ json["PlaylistPosition"].is_int64() ? static_cast<int>(json["PlaylistPosition"].as_int64()) : -1 }
    {
        if(json["Credential"].is_object())
        {
            boost::json::object credential = json["Credential"].as_object();
            m_credential = Credential{ "", "", credential["Username"].is_string() ? credential["Username"].as_string().c_str() : "", credential["Password"].is_string() ? credential["Password"].as_string().c_str() : "" };
        }
        if(json["AvailableFormats"].is_array())
        {
            boost::json::array availableFormats = json["AvailableFormats"].as_array();
            for(const boost::json::value& value : availableFormats)
            {
                m_availableFormats.push_back(Format(value.as_object(), false));
            }
        }
        if(json["VideoFormat"].is_object())
        {
            m_videoFormat = Format(json["VideoFormat"].as_object(), false);
        }
        if(json["AudioFormat"].is_object())
        {
            m_audioFormat = Format(json["AudioFormat"].as_object(), false);
        }
        if(json["SubtitleLanguages"].is_array())
        {
            boost::json::array subtitleLanguages = json["SubtitleLanguages"].as_array();
            for(const boost::json::value& value : subtitleLanguages)
            {
                m_subtitleLanguages.push_back(SubtitleLanguage(value.as_object()));
            }
        }
        if(json["PostProcessorArgument"].is_object())
        {
            m_postProcessorArgument = PostProcessorArgument(json["PostProcessorArgument"].as_object());
        }
        if(json["TimeFrame"].is_object())
        {
            m_timeFrame = TimeFrame(json["TimeFrame"].as_object());
        }
        validateFileNamesAndPaths();
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
        validateFileNamesAndPaths();
    }

    const std::vector<Format>& DownloadOptions::getAvailableFormats() const
    {
        return m_availableFormats;
    }

    void DownloadOptions::setAvailableFormats(const std::vector<Format>& availableFormats)
    {
        m_availableFormats = availableFormats;
        validateFileNamesAndPaths();
    }

    const std::optional<Format>& DownloadOptions::getVideoFormat() const
    {
        return m_videoFormat;
    }

    void DownloadOptions::setVideoFormat(const std::optional<Format>& videoFormat)
    {
        m_videoFormat = videoFormat;
        if(m_videoFormat && m_fileType.isGeneric() && m_fileType.isVideo())
        {
            std::optional<MediaFileType> newFileType{ MediaFileType::parse(m_videoFormat->getExtension()) };
            if(newFileType)
            {
                m_fileType = *newFileType;
            }
        }
    }

    const std::optional<Format>& DownloadOptions::getAudioFormat() const
    {
        return m_audioFormat;
    }

    void DownloadOptions::setAudioFormat(const std::optional<Format>& audioFormat)
    {
        m_audioFormat = audioFormat;
        if(m_audioFormat && m_fileType.isGeneric() && m_fileType.isAudio())
        {
            std::optional<MediaFileType> newFileType{ MediaFileType::parse(m_audioFormat->getExtension()) };
            if(newFileType)
            {
                m_fileType = *newFileType;
            }
        }
    }

    const std::filesystem::path& DownloadOptions::getSaveFolder() const
    {
        return m_saveFolder;
    }

    void DownloadOptions::setSaveFolder(const std::filesystem::path& saveFolder)
    {
        m_saveFolder = saveFolder;
        validateFileNamesAndPaths();
    }

    const std::string& DownloadOptions::getSaveFilename() const
    {
        return m_saveFilename;
    }

    void DownloadOptions::setSaveFilename(const std::string& saveFilename)
    {
        m_saveFilename = m_saveFilename.find(std::string("›")) != std::string::npos ? StringHelpers::normalizeForFilename(saveFilename, true) : saveFilename;
        validateFileNamesAndPaths();
    }

    const std::vector<SubtitleLanguage>& DownloadOptions::getSubtitleLanguages() const
    {
        return m_subtitleLanguages;
    }

    void DownloadOptions::setSubtitleLanguages(const std::vector<SubtitleLanguage>& subtitleLanguages)
    {
        m_subtitleLanguages = subtitleLanguages;
    }

    bool DownloadOptions::getSplitChapters() const
    {
        return m_splitChapters;
    }

    void DownloadOptions::setSplitChapters(bool splitChapters)
    {
        m_splitChapters = splitChapters;
    }

    bool DownloadOptions::getExportDescription() const
    {
        return m_exportDescription;
    }

    void DownloadOptions::setExportDescription(bool exportDescription)
    {
        m_exportDescription = exportDescription;
    }

    const std::optional<PostProcessorArgument>& DownloadOptions::getPostProcessorArgument() const
    {
        return m_postProcessorArgument;
    }

    void DownloadOptions::setPostProcessorArgument(const std::optional<PostProcessorArgument>& postProcessorArgument)
    {
        m_postProcessorArgument = postProcessorArgument;
    }

    const std::optional<TimeFrame>& DownloadOptions::getTimeFrame() const
    {
        return m_timeFrame;
    }

    void DownloadOptions::setTimeFrame(const std::optional<TimeFrame>& timeFrame)
    {
        m_timeFrame = timeFrame;
    }

    int DownloadOptions::getPlaylistPosition() const
    {
        return m_playlistPosition;
    }

    void DownloadOptions::setPlaylistPosition(int position)
    {
        if(position <= 0)
        {
            position = -1;
        }
        m_playlistPosition = position;
    }

    std::vector<std::string> DownloadOptions::toArgumentVector(const DownloaderOptions& downloaderOptions) const
    {
        std::vector<std::string> arguments;
        //Basic Options
        arguments.push_back(m_url);
        arguments.push_back("--ignore-config");
        arguments.push_back("--verbose");
        arguments.push_back("--xff");
        arguments.push_back("default");
        arguments.push_back("--no-warnings");
        arguments.push_back("--progress");
        arguments.push_back("--newline");;
        arguments.push_back("--progress-template");
        arguments.push_back("[download] PROGRESS;%(progress.status)s;%(progress.downloaded_bytes)s;%(progress.total_bytes)s;%(progress.total_bytes_estimate)s;%(progress.speed)s;%(progress.eta)s");
        arguments.push_back("--progress-delta");
        arguments.push_back(".25");
        arguments.push_back("--sleep-requests");
        arguments.push_back("1");
        arguments.push_back("--no-mtime");
        arguments.push_back("--no-embed-info-json");
        arguments.push_back("--ffmpeg-location");
        arguments.push_back(Environment::findDependency("ffmpeg").string());
        arguments.push_back("--js-runtimes");
        arguments.push_back("deno:" + Environment::findDependency("deno").string());
        arguments.push_back("--plugin-dir");
        arguments.push_back((Environment::getExecutableDirectory() / "plugins").string());
        //Downloader Options
        if(downloaderOptions.getOverwriteExistingFiles() && !shouldDownloadResume())
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
        std::string formatSort{ m_timeFrame ? "proto:https" : "" };
        if(downloaderOptions.getPreferredVideoCodec() != VideoCodec::Any)
        {
            if(!formatSort.empty())
            {
                formatSort += ",";
            }
            formatSort += "+vcodec:";
            switch (downloaderOptions.getPreferredVideoCodec())
            {
            case VideoCodec::VP9:
                formatSort += "vp9";
                break;
            case VideoCodec::AV01:
                formatSort += "av01";
                break;
            case VideoCodec::H264:
                formatSort += "h264";
                break;
            case VideoCodec::H265:
                formatSort += "h265";
                break;
            }
            formatSort += ",res";
        }
        if(downloaderOptions.getPreferredAudioCodec() != AudioCodec::Any)
        {
            if(!formatSort.empty())
            {
                formatSort += ",";
            }
            formatSort += "+acodec:";
            switch (downloaderOptions.getPreferredAudioCodec())
            {
            case AudioCodec::FLAC:
                formatSort += "flac";
                break;
            case AudioCodec::WAV:
                formatSort += "wav";
                break;
            case AudioCodec::OPUS:
                formatSort += "opus";
                break;
            case AudioCodec::AAC:
                formatSort += "aac";
                break;
            case AudioCodec::MP4A:
                formatSort += "mp4a";
                break;
            case AudioCodec::MP3:
                formatSort += "mp3";
                break;
            }
            formatSort += ",quality";
        }
        if(downloaderOptions.getPreferredAudioCodec() == AudioCodec::Any && Environment::getOperatingSystem() == OperatingSystem::Windows && m_audioFormat && m_audioFormat->isFormatValue(FormatValue::Best))
        {
            if(!formatSort.empty())
            {
                formatSort += ",";
            }
            formatSort += "+acodec:mp4a";
        }
        if(!formatSort.empty())
        {
            arguments.push_back("--format-sort");
            arguments.push_back(formatSort);
        }
        if(!downloaderOptions.getUsePartFiles())
        {
            arguments.push_back("--no-part");
        }
        if(downloaderOptions.getYouTubeSponsorBlock())
        {
            arguments.push_back("--sponsorblock-remove");
            arguments.push_back("default");
        }
        if(downloaderOptions.getSpeedLimit() && !m_timeFrame)
        {
            arguments.push_back("--limit-rate");
            arguments.push_back(std::to_string(*downloaderOptions.getSpeedLimit()) + "K");
        }
        if(!downloaderOptions.getProxyUrl().empty())
        {
            arguments.push_back("--proxy");
            arguments.push_back(downloaderOptions.getProxyUrl());
        }
        if(downloaderOptions.getCookiesBrowser() != Browser::None)
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
        if(downloaderOptions.getEmbedMetadata())
        {
            arguments.push_back("--embed-metadata");
            if(downloaderOptions.getRemoveSourceData())
            {
                arguments.push_back("--postprocessor-args");
                arguments.push_back("Metadata+ffmpeg:-metadata comment= -metadata description= -metadata synopsis= -metadata purl= ");
            }
            if(m_playlistPosition != -1)
            {
                arguments.push_back("--postprocessor-args");
                arguments.push_back("Metadata+ffmpeg:-metadata track=" + std::to_string(m_playlistPosition));
            }
        }
        if(downloaderOptions.getEmbedThumbnails())
        {
            if(m_fileType.supportsThumbnails())
            {
                arguments.push_back("--embed-thumbnail");
            }
            else
            {
                arguments.push_back("--write-thumbnail");
            }
            arguments.push_back("--convert-thumbnails");
            arguments.push_back("png>png/jpg");
            if(downloaderOptions.getCropAudioThumbnails() && m_fileType.isAudio())
            {
                arguments.push_back("--postprocessor-args");
                arguments.push_back("ThumbnailsConvertor:-vf crop=ih:ih");
            }
        }
        if(downloaderOptions.getEmbedChapters())
        {
            arguments.push_back("--embed-chapters");
        }
        if(downloaderOptions.getUseAria())
        {
            arguments.push_back("--downloader");
            arguments.push_back(Environment::findDependency("aria2c").string());
            arguments.push_back("--downloader-args");
            arguments.push_back("aria2c:--summary-interval=" + std::string(Environment::getOperatingSystem() == OperatingSystem::Windows ? "0" : "1") + " --enable-color=false -x " + std::to_string(downloaderOptions.getAriaMaxConnectionsPerServer()) + " -k " + std::to_string(downloaderOptions.getAriaMinSplitSize()) + "M");
            arguments.push_back("--concurrent-fragments");
            arguments.push_back("8");
        }
        //Download Options
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
        if(m_fileType.isAudio())
        {
            arguments.push_back("--extract-audio");
            arguments.push_back("--audio-quality");
            arguments.push_back("0");
            if(!m_fileType.isGeneric())
            {
                arguments.push_back("--audio-format");
                if(m_fileType == MediaFileType::OGG)
                {
                    arguments.push_back("vorbis");
                }
                else
                {
                    arguments.push_back(StringHelpers::lower(m_fileType.str()));
                }
            }
        }
        else if(m_fileType.isVideo())
        {
            if(!m_fileType.isGeneric())
            {
                arguments.push_back("--remux-video");
                arguments.push_back(StringHelpers::lower(m_fileType.str()));
                if(m_fileType.shouldRecode())
                {
                    arguments.push_back("--recode-video");
                    arguments.push_back(StringHelpers::lower(m_fileType.str()));
                }
            }
        }
        std::string formatString;
        if(m_videoFormat && !m_videoFormat->isFormatValue(FormatValue::None))
        {
            if(m_videoFormat->isFormatValue(FormatValue::Best))
            {
                formatString += "bv";
                if(m_audioFormat && m_audioFormat->isFormatValue(FormatValue::None))
                {
                    formatString += "*";
                }
            }
            else if(m_videoFormat->isFormatValue(FormatValue::Worst))
            {
                formatString += "wv";
                if(m_audioFormat && m_audioFormat->isFormatValue(FormatValue::None))
                {
                    formatString += "*";
                }
            }
            else
            {
                formatString += m_videoFormat->getId();
            }
        }
        if(m_audioFormat && !m_audioFormat->isFormatValue(FormatValue::None))
        {
            if(!formatString.empty())
            {
                formatString += "+";
            }
            if(m_audioFormat->isFormatValue(FormatValue::Best))
            {
                formatString += "ba";
            }
            else if(m_audioFormat->isFormatValue(FormatValue::Worst))
            {
                formatString += "wa";
            }
            else
            {
                formatString += m_audioFormat->getId();
            }
        }
        if(formatString == "bv*")
        {
            if(m_fileType.isAudio())
            {
                formatString += "+ba";
            }
        }
        else if(formatString == "wv*" && m_fileType.isAudio())
        {
            if(m_fileType.isAudio())
            {
                formatString += "+wa";
            }
        }
        if(formatString == "bv+ba" || formatString == "bv*+ba" || formatString == "bv*" || formatString == "ba")
        {
            formatString += "/b";
        }
        else if(formatString == "wv+wa" || formatString == "wv*+wa" || formatString == "wv*" || formatString == "wa")
        {
            formatString += "/w";
        }
        if(!formatString.empty())
        {
            arguments.push_back("--format");
            arguments.push_back(formatString);
        }
        if(!std::filesystem::exists(m_saveFolder))
        {
            std::filesystem::create_directories(m_saveFolder);
        }
        arguments.push_back("--paths");
        arguments.push_back(m_saveFolder.string());
        arguments.push_back("--paths");
        arguments.push_back("temp:" + m_saveFolder.string());
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
            arguments.push_back("--sub-langs");
            arguments.push_back(languages);
            arguments.push_back("--sleep-subtitles");
            arguments.push_back("60");
            arguments.push_back("--write-subs");
            if(downloaderOptions.getIncludeAutoGeneratedSubtitles())
            {
                arguments.push_back("--write-auto-subs");
            }
            arguments.push_back("--sub-format");
            switch(downloaderOptions.getPreferredSubtitleFormat())
            {
            case SubtitleFormat::SRT:
                arguments.push_back("srt/best");
                break;
            case SubtitleFormat::ASS:
                arguments.push_back("ass/best");
                break;
            case SubtitleFormat::LRC:
                arguments.push_back("lrc/best");
                break;
            default:
                arguments.push_back("vtt/best");
                break;
            }
            arguments.push_back("--convert-subs");
            switch(downloaderOptions.getPreferredSubtitleFormat())
            {
            case SubtitleFormat::SRT:
                arguments.push_back("srt");
                arguments.push_back("--use-postprocessor");
                arguments.push_back("srt_fix");
                break;
            case SubtitleFormat::ASS:
                arguments.push_back("ass");
                break;
            case SubtitleFormat::LRC:
                arguments.push_back("lrc");
                break;
            default:
                arguments.push_back("vtt");
                break;
            }
            if(downloaderOptions.getEmbedSubtitles() && m_fileType.supportsSubtitleFormat(downloaderOptions.getPreferredSubtitleFormat()))
            {
                arguments.push_back("--embed-subs");
                arguments.push_back("--compat-options");
                arguments.push_back("no-keep-subs");
            }
        }
        if(m_splitChapters)
        {
            std::string args{ "SplitChapters:-map_metadata 0 -map_chapters -1" };
            if(m_fileType == MediaFileType::FLAC)
            {
                args += " -c:a flac";
            }
            arguments.push_back("--split-chapters");
            arguments.push_back("--postprocessor-args");
            arguments.push_back(args);
        }
        if(m_exportDescription)
        {
            arguments.push_back("--write-description");
        }
        if(m_postProcessorArgument)
        {
            arguments.push_back("--postprocessor-args");
            if(m_postProcessorArgument->getExecutable() == Executable::FFmpeg && m_postProcessorArgument->getPostProcessor() == PostProcessor::None)
            {
                arguments.push_back(m_postProcessorArgument->str() + " -threads " + std::to_string(downloaderOptions.getPostprocessingThreads()));
            }
            else
            {
                arguments.push_back(m_postProcessorArgument->str());
            }
        }
        else
        {
            arguments.push_back("--postprocessor-args");
            arguments.push_back("ffmpeg:-threads " + std::to_string(downloaderOptions.getPostprocessingThreads()));
        }
        if(m_timeFrame)
        {
            arguments.push_back("--download-sections");
            arguments.push_back("*" + m_timeFrame->str());
            arguments.push_back("--force-keyframes-at-cuts");
        }
        arguments.push_back("--print");
        arguments.push_back("after_move:filepath");
        return arguments;
    }

    boost::json::object DownloadOptions::toJson(bool includeCredential) const
    {
        boost::json::object json;
        json["Url"] = m_url;
        if(m_credential)
        {
            if(includeCredential)
            {
                boost::json::object credential;
                credential["Username"] = m_credential->getUsername();
                credential["Password"] = m_credential->getPassword();
                json["Credential"] = credential;
            }
            else
            {
                json["Credential"] = "Hidden";
            }
        }
        json["FileType"] = static_cast<int>(m_fileType);
        boost::json::array availableFormats;
        for(const Format& format : m_availableFormats)
        {
            availableFormats.push_back(format.toJson());
        }
        json["AvailableFormats"] = availableFormats;
        if(m_videoFormat)
        {
            json["VideoFormat"] = m_videoFormat->toJson();
        }
        if(m_audioFormat)
        {
            json["AudioFormat"] = m_audioFormat->toJson();
        }
        json["SaveFolder"] = m_saveFolder.string();
        json["SaveFilename"] = m_saveFilename;
        boost::json::array subtitleLanguages;
        for(const SubtitleLanguage& language : m_subtitleLanguages)
        {
            subtitleLanguages.push_back(language.toJson());
        }
        json["SubtitleLanguages"] = subtitleLanguages;
        json["SplitChapters"] = m_splitChapters;
        json["ExportDescription"] = m_exportDescription;
        if(m_postProcessorArgument)
        {
            json["PostProcessorArgument"] = m_postProcessorArgument->toJson();
        }
        if(m_timeFrame)
        {
            json["TimeFrame"] = m_timeFrame->toJson();
        }
        json["PlaylistPosition"] = m_playlistPosition;
        return json;
    }

    void DownloadOptions::validateFileNamesAndPaths()
    {
        if(m_saveFolder.empty() || m_saveFilename.empty())
        {
            return;
        }
        //Check filename extension
        std::filesystem::path filenamePath{ m_saveFilename };
        if(filenamePath.extension().string() == m_fileType.getDotExtension())
        {
            m_saveFilename = filenamePath.stem().string();
        }
        //Find max extension length
        size_t maxExtensionLength{ 11 }; //.part.aria2
        for(const Format& format : m_availableFormats)
        {
            size_t formatSize{ std::string(".f" + format.getId() + "." + format.getExtension() + ".part").size() };
            if(formatSize > maxExtensionLength)
            {
                maxExtensionLength = formatSize;
            }
        }
        //Check filename length
#ifdef _WIN32
        static size_t maxFileNameLength{ MAX_PATH - 1 };
#else
        static size_t maxFileNameLength{ NAME_MAX };
#endif
        if(m_saveFilename.size() + maxExtensionLength > maxFileNameLength)
        {
            m_saveFilename = m_saveFilename.substr(0, maxFileNameLength - maxExtensionLength);
        }
        //Check path length
#ifdef _WIN32
        static size_t maxPathLength{ MAX_PATH };
#else
        static size_t maxPathLength{ PATH_MAX - 1 };
#endif
        if((m_saveFolder / m_saveFilename).string().size() + maxExtensionLength > maxPathLength)
        {
            int newFileNameLength{ static_cast<int>(maxPathLength) - static_cast<int>(m_saveFolder.string().size()) - static_cast<int>(maxExtensionLength) };
            if(newFileNameLength > 0)
            {
                m_saveFilename = m_saveFilename.substr(0, static_cast<size_t>(newFileNameLength));
            }
            else
            {
                m_saveFolder = m_saveFolder.string().substr(0, maxPathLength - m_saveFilename.size() - maxExtensionLength);
            }
        }
    }

    bool DownloadOptions::shouldDownloadResume() const
    {
        //Check for part files
        if(std::filesystem::exists(m_saveFolder / (m_saveFilename + ".part")) || std::filesystem::exists(m_saveFolder / (m_saveFilename + ".part.aria2")))
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
        //Check for already downloaded subtitles
        for(const SubtitleLanguage& language : m_subtitleLanguages)
        {
            if(std::filesystem::exists(m_saveFolder / (m_saveFilename + "." + language.getLanguage() + ".vtt")))
            {
                return true;
            }
            else if(std::filesystem::exists(m_saveFolder / (m_saveFilename + "." + language.getLanguage() + ".srt")))
            {
                return true;
            }
            else if(std::filesystem::exists(m_saveFolder / (m_saveFilename + "." + language.getLanguage() + ".ass")))
            {
                return true;
            }
            else if(std::filesystem::exists(m_saveFolder / (m_saveFilename + "." + language.getLanguage() + ".lrc")))
            {
                return true;
            }
        }
        return false;
    }
}
