#ifndef ADDDOWNLOADDIALOGCONTROLLER_H
#define ADDDOWNLOADDIALOGCONTROLLER_H

#include <optional>
#include <string>
#include <vector>
#include <libnick/events/event.h>
#include <libnick/events/parameventargs.h>
#include <libnick/keyring/keyring.h>
#include "models/downloaderoptions.h"
#include "models/media.h"
#include "models/previousdownloadoptions.h"

namespace Nickvision::TubeConverter::Shared::Controllers
{
    /**
     * @brief A controller for the AddDownloadDialog.
     */
    class AddDownloadDialogController
    {
    public:
        /**
         * @brief Constructs an AddDownloadDialogController.
         * @param downloaderOptions The DownloaderOptions from the configuration
         * @param keyring The Keyring to use
         * @param configuration The Configuration to use
         */
        AddDownloadDialogController(const Models::DownloaderOptions& downloaderOptions, Models::PreviousDownloadOptions& previousOptions, std::optional<Keyring::Keyring>& keyring);
        /**
         * @brief Destructs the AddDownloadDialogController.
         */
        ~AddDownloadDialogController();
        /**
         * @brief Gets the event for when a url is validated.
         * @return The url validated event
         */
        Events::Event<Events::ParamEventArgs<std::vector<Models::Media>>>& urlValidated();
        /**
         * @brief Gets the PreviousDownloadOptions.
         * @return The PreviousDownloadOptions
         */
        const Models::PreviousDownloadOptions& getPreviousDownloadOptions() const;
        /**
         * @briefs Gets the list of names of credentials in the keyring.
         * @return The list of credential names in the keyring
         */
        std::vector<std::string> getKeyringCredentialNames() const;
        /**
         * @brief Validates a url.
         * @brief This method will invoke the urlValidated event with the list of media found at the url.
         * @param url The url to validate
         * @param credential An optional credential to use when accessing the url
         */
        void validateUrl(const std::string& url, const std::optional<Keyring::Credential>& credential);
        /**
         * @brief Validates a url.
         * @brief This method will invoke the urlValidated event with the list of media found at the url.
         * @param url The url to validate
         * @param credentialNameIndex The index of the name of the credential to use when accessing the url
         */
        void validateUrl(const std::string& url, size_t credentialNameIndex);

    private:
        Models::DownloaderOptions m_downloaderOptions;
        Models::PreviousDownloadOptions& m_previousOptions;
        std::optional<Keyring::Keyring>& m_keyring;
        Events::Event<Events::ParamEventArgs<std::vector<Models::Media>>> m_urlValidated;
    };
}

#endif //ADDDOWNLOADDIALOGCONTROLLER_H