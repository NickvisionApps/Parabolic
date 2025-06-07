#include "events/downloadcredentialneededeventargs.h"

using namespace Nickvision::Keyring;

namespace Nickvision::TubeConverter::Shared::Events
{
    DownloadCredentialNeededEventArgs::DownloadCredentialNeededEventArgs(const std::string& url, const std::shared_ptr<Credential>& credential)
        : m_url{ url },
        m_credential{ credential }
    {

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
