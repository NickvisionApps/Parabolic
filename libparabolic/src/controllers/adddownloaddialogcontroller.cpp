#include "controllers/adddownloaddialogcontroller.h"
#include <thread>
#include <libnick/localization/gettext.h>
#include "models/urlinfo.h"

using namespace Nickvision::Events;
using namespace Nickvision::Keyring;
using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::Shared::Controllers
{
    AddDownloadDialogController::AddDownloadDialogController(const DownloaderOptions& downloaderOptions, PreviousDownloadOptions& previousOptions, std::optional<Keyring::Keyring>& keyring)
        : m_downloaderOptions{ downloaderOptions },
        m_previousOptions{ previousOptions },
        m_keyring{ keyring },
        m_urlInfo{ std::nullopt }
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
                index += 2;
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
                qualities.push_back(_("Best"));
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
                qualities.push_back(_("Default"));
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

    const std::string& AddDownloadDialogController::getMediaTitle(size_t index) const
    {
        static std::string empty;
        if(m_urlInfo && index < m_urlInfo->count())
        {
            return m_urlInfo->get(index).getTitle();
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
            m_urlInfo = UrlInfo::fetch(url, m_downloaderOptions, credential);
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
}