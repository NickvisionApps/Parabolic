#include "controllers/adddownloaddialogcontroller.h"
#include <format>
#include <thread>
#include <libnick/localization/gettext.h>
#include "models/urlinfo.h"

using namespace Nickvision::Events;
using namespace Nickvision::Keyring;
using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::Shared::Controllers
{
    AddDownloadDialogController::AddDownloadDialogController(DownloadManager& downloadManager, PreviousDownloadOptions& previousOptions, std::optional<Keyring::Keyring>& keyring)
        : m_downloadManager{ downloadManager },
        m_previousOptions{ previousOptions },
        m_keyring{ keyring },
        m_urlInfo{ std::nullopt },
        m_credential{ std::nullopt }
    {
        
    }

    AddDownloadDialogController::~AddDownloadDialogController()
    {
        m_previousOptions.save();   
    }

    Event<ParamEventArgs<bool>>& AddDownloadDialogController::urlValidated()
    {
        return m_urlValidated;
    }

    const PreviousDownloadOptions& AddDownloadDialogController::getPreviousDownloadOptions() const
    {
        return m_previousOptions;
    }

    std::vector<std::string> AddDownloadDialogController::getKeyringCredentialNames() const
    {
        std::vector<std::string> names;
        if (m_keyring)
        {
            for(const Credential& credential : m_keyring->getAllCredentials())
            {
                names.push_back(credential.getName());
            }
        }   
        return names;
    }

    bool AddDownloadDialogController::isUrlValid() const
    {
        return m_urlInfo.has_value() && m_urlInfo->count() > 0;
    }

    bool AddDownloadDialogController::isUrlPlaylist() const
    {
        return m_urlInfo.has_value() && m_urlInfo->isPlaylist();
    }

    size_t AddDownloadDialogController::getMediaCount() const
    {
        if(m_urlInfo)
        {
            return m_urlInfo->count();
        }
        return 0;
    }

    std::vector<std::string> AddDownloadDialogController::getFileTypeStrings() const
    {
        std::vector<std::string> fileTypes;
        if(!m_urlInfo)
        {
            return fileTypes;
        }
        if(!m_urlInfo->isPlaylist())
        {
            const Media& media{ m_urlInfo->get(0) };
            if(media.getType() == MediaType::Video)
            {
                fileTypes.push_back(_("MP4 (Video)"));
                fileTypes.push_back(_("WEBM (Video)"));
                fileTypes.push_back(_("MKV (Video)"));
                fileTypes.push_back(_("MOV (Video)"));
                fileTypes.push_back(_("AVI (Video)"));
            }
            fileTypes.push_back(_("MP3 (Audio)"));
            fileTypes.push_back(_("M4A (Audio)"));
            fileTypes.push_back(_("OPUS (Audio)"));
            fileTypes.push_back(_("FLAC (Audio)"));
            fileTypes.push_back(_("WAV (Audio)"));
        }
        else
        {
            fileTypes.push_back(_("MP4 (Video)"));
            fileTypes.push_back(_("WEBM (Video)"));
            fileTypes.push_back(_("MKV (Video)"));
            fileTypes.push_back(_("MOV (Video)"));
            fileTypes.push_back(_("AVI (Video)"));
            fileTypes.push_back(_("MP3 (Audio)"));
            fileTypes.push_back(_("M4A (Audio)"));
            fileTypes.push_back(_("OPUS (Audio)"));
            fileTypes.push_back(_("FLAC (Audio)"));
            fileTypes.push_back(_("WAV (Audio)"));
        }
        return fileTypes;
    }

    std::vector<std::string> AddDownloadDialogController::getQualityStrings(size_t index) const
    {
        std::vector<std::string> qualities;
        if(!m_urlInfo)
        {
            return qualities;
        }
        MediaFileType type{ static_cast<MediaFileType::MediaFileTypeValue>(index) };
        if(!m_urlInfo->isPlaylist())
        {
            const Media& media{ m_urlInfo->get(0) };
            if(media.getType() == MediaType::Audio)
            {
                index += 5;
                type = static_cast<MediaFileType::MediaFileTypeValue>(index);
            }
            if(type.isAudio())
            {
                qualities.push_back(_("Best"));
                qualities.push_back(_("Good"));
                qualities.push_back(_("Worst"));
            }
            else
            {
                for(const VideoResolution& resolution : media.getVideoResolutions())
                {
                    qualities.push_back(resolution.str());
                }
            }
        }
        else
        {
            if(type.isAudio())
            {
                qualities.push_back(_("Best"));
                qualities.push_back(_("Good"));
                qualities.push_back(_("Worst"));
            }
            else
            {
                qualities.push_back(_("Best"));
            }
        }
        return qualities;
    }

    std::vector<std::string> AddDownloadDialogController::getAudioLanguageStrings() const
    {
        std::vector<std::string> languages;
        if(!m_urlInfo)
        {
            return languages;
        }
        if(!m_urlInfo->isPlaylist())
        {
            const Media& media{ m_urlInfo->get(0) };
            if(media.getAudioLanguages().empty())
            {
                languages.push_back(_("Default"));
            }
            else
            {
                for(const std::string& language : media.getAudioLanguages())
                {
                    languages.push_back(language);
                }
            }
        }
        else
        {
            languages.push_back(_("Default"));
        }
        return languages;
    }

    const std::string& AddDownloadDialogController::getMediaUrl(size_t index) const
    {
        static std::string empty;
        if(m_urlInfo && index < m_urlInfo->count())
        {
            return m_urlInfo->get(index).getUrl();
        }
        return empty;
    }

    std::string AddDownloadDialogController::getMediaTitle(size_t index, bool numbered) const
    {
        static std::string empty;
        if(m_urlInfo && index < m_urlInfo->count())
        {
            std::string title{ m_urlInfo->get(index).getTitle() };
            m_previousOptions.setNumberTitles(numbered);
            return numbered ? std::format("{} - {}", index + 1, title) : title;
        }
        return empty;
    }

    const TimeFrame& AddDownloadDialogController::getMediaTimeFrame(size_t index) const
    {
        static TimeFrame empty{ std::chrono::seconds(0), std::chrono::seconds(0) };
        if(m_urlInfo && index < m_urlInfo->count())
        {
            return m_urlInfo->get(index).getTimeFrame();
        }
        return empty;
    }

    void AddDownloadDialogController::validateUrl(const std::string& url, const std::optional<Credential>& credential)
    {
        std::thread worker{ [this, url, credential]()
        {
            m_credential = credential;
            m_urlInfo = UrlInfo::fetch(url, m_downloadManager.getDownloaderOptions(), m_credential);
            m_urlValidated.invoke({ isUrlValid() });
        } };
        worker.detach();
    }

    void AddDownloadDialogController::validateUrl(const std::string& url, size_t credentialNameIndex)
    {
        if (m_keyring)
        {
            std::vector<Credential> credentials{ m_keyring->getAllCredentials() };
            if(credentialNameIndex < credentials.size())
            {
                validateUrl(url, credentials[credentialNameIndex]);
            }
        }
        validateUrl(url, std::nullopt);
    }

    void AddDownloadDialogController::addSingleDownload(const std::filesystem::path& saveFolder, const std::string& filename, size_t fileTypeIndex, size_t qualityIndex, size_t audioLanguageIndex, bool downloadSubtitles, bool preferAV1, bool splitChapters, bool limitSpeed, const std::string& startTime, const std::string& endTime)
    {
        const Media& media{ m_urlInfo->get(0) };
        //Create Download Options
        DownloadOptions options{ media.getUrl() };
        options.setCredential(m_credential);
        options.setSaveFolder(std::filesystem::exists(saveFolder) ? saveFolder : m_previousOptions.getSaveFolder());
        options.setSaveFilename(!filename.empty() ? filename : media.getTitle());
        if(media.getType() == MediaType::Audio)
        {
            fileTypeIndex += 5; 
        }
        options.setFileType(static_cast<MediaFileType::MediaFileTypeValue>(fileTypeIndex));
        if(options.getFileType().isAudio())
        {
            options.setQuality(static_cast<Quality>(qualityIndex));
        }
        else
        {
            options.setQuality(media.getVideoResolutions()[qualityIndex]);
        }
        if(media.getAudioLanguages().size() > 1)
        {
            options.setAudioLanguage(media.getAudioLanguages()[audioLanguageIndex]);
        }
        options.setDownloadSubtitles(downloadSubtitles);
        options.setPreferAV1(preferAV1);
        options.setSplitChapters(splitChapters);
        options.setLimitSpeed(limitSpeed);
        options.setTimeFrame(TimeFrame::parse(startTime, endTime, media.getTimeFrame().getDuration()));
        //Save Previous Options
        m_previousOptions.setSaveFolder(options.getSaveFolder());
        m_previousOptions.setFileType(options.getFileType());
        m_previousOptions.setDownloadSubtitles(options.getDownloadSubtitles());
        m_previousOptions.setPreferAV1(options.getPreferAV1());
        m_previousOptions.setSplitChapters(options.getSplitChapters());
        m_previousOptions.setLimitSpeed(options.getLimitSpeed());
        //Add Download
        m_downloadManager.addDownload(options);
    }

    void AddDownloadDialogController::addPlaylistDownload(const std::filesystem::path& saveFolder, const std::unordered_map<size_t, std::string>& filenames, size_t fileTypeIndex, bool downloadSubtitles, bool preferAV1, bool splitChapters, bool limitSpeed)
    {
        //Save Previous Options
        m_previousOptions.setSaveFolder(saveFolder);
        m_previousOptions.setFileType(static_cast<MediaFileType::MediaFileTypeValue>(fileTypeIndex));
        m_previousOptions.setDownloadSubtitles(downloadSubtitles);
        m_previousOptions.setPreferAV1(preferAV1);
        m_previousOptions.setSplitChapters(splitChapters);
        m_previousOptions.setLimitSpeed(limitSpeed);
        for(const std::pair<const size_t, std::string>& pair : filenames)
        {
            const Media& media{ m_urlInfo->get(pair.first) };
            //Create Download Options
            DownloadOptions options{ media.getUrl() };
            options.setCredential(m_credential);
            options.setSaveFolder(std::filesystem::exists(saveFolder) ? saveFolder : m_previousOptions.getSaveFolder());
            options.setSaveFilename(!pair.second.empty() ? pair.second : media.getTitle());
            options.setFileType(static_cast<MediaFileType::MediaFileTypeValue>(fileTypeIndex));
            options.setDownloadSubtitles(downloadSubtitles);
            options.setPreferAV1(preferAV1);
            options.setSplitChapters(splitChapters);
            options.setLimitSpeed(limitSpeed);
            //Add Download
            m_downloadManager.addDownload(options);
        }
    }
}