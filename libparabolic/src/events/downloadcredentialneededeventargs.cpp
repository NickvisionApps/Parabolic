#include "events/downloadcredentialneededeventargs.h"

using namespace Nickvision::Keyring;

namespace Nickvision::TubeConverter::Shared::Events
{
    DownloadCredentialNeededEventArgs::DownloadCredentialNeededEventArgs(int id, const std::string& url, const std::shared_ptr<Credential>& credential)
        : m_id{ id },
        m_url{ url },
        m_credential{ credential }
    {

    }

    int DownloadCredentialNeededEventArgs::getId() const
    {
        return m_id;
    }

    const std::string& DownloadCredentialNeededEventArgs::getUrl() const
    {
        return m_url;
    }

    std::shared_ptr<Credential>& DownloadCredentialNeededEventArgs::getCredential()
    {
        return m_credential;
    }
}