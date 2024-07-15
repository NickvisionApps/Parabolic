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
        m_keyring{ keyring }
    {
        
    }

    AddDownloadDialogController::~AddDownloadDialogController()
    {
        m_previousOptions.save();   
    }

    Event<ParamEventArgs<std::vector<Media>>>& AddDownloadDialogController::urlValidated()
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
            names.push_back(_("Use manual credential"));
            for(const Credential& credential : m_keyring->getAllCredentials())
            {
                names.push_back(credential.getName());
            }
        }   
        return names;
    }

    void AddDownloadDialogController::validateUrl(const std::string& url, const std::optional<Credential>& credential)
    {
        std::thread worker{ [&]()
        {
            std::vector<Media> media;
            std::optional<UrlInfo> urlInfo{ UrlInfo::fetch(url, m_downloaderOptions, credential) };
            if(urlInfo)
            {
                for(size_t i = 0; i < urlInfo->count(); i++)
                {
                    media.push_back(urlInfo->get(i));
                }
            }
            m_urlValidated.invoke({ media });
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