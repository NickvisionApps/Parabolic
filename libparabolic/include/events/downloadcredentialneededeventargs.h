#ifndef DOWNLOADCREDENTIALNEEDEDEVENTARGS_H
#define DOWNLOADCREDENTIALNEEDEDEVENTARGS_H

#include <memory>
#include <string>
#include <libnick/events/eventargs.h>
#include <libnick/keyring/credential.h>

namespace Nickvision::TubeConverter::Shared::Events
{
    /**
     * @brief Event arguments for when a download credential is needed.
     */
    class DownloadCredentialNeededEventArgs : public Nickvision::Events::EventArgs
    {
    public:
        /**
         * @brief Constructs a DownloadCredentialNeededEventArgs.
         * @param url The URL of the download
         * @param credential The credential to fill in
         */
        DownloadCredentialNeededEventArgs(const std::string& url, const std::shared_ptr<Keyring::Credential>& credential);
        /**
         * @brief Gets the URL of the download.
         * @return The URL of the download
         */
        const std::string& getUrl() const;
        /**
         * @brief Gets the credential to fill in.
         * @return The credential to fill in
         */
        std::shared_ptr<Keyring::Credential>& getCredential();

    private:
        std::string m_url;
        std::shared_ptr<Keyring::Credential> m_credential;
    };
}

#endif //DOWNLOADCREDENTIALNEEDEDEVENTARGS_H
