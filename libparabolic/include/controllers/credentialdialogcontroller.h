#ifndef CREDENTIALDIALOGCONTROLLER_H
#define CREDENTIALDIALOGCONTROLLER_H

#include <string>
#include <vector>
#include <libnick/keyring/keyring.h>
#include "events/downloadcredentialneededeventargs.h"

namespace Nickvision::TubeConverter::Shared::Controllers
{
    /**
     * @brief A controller for the CredentialDialog.
     */
    class CredentialDialogController
    {
    public:
        /**
         * @brief Constructs a CredentialDialogController.
         * @param args The DownloadCredentialNeededEventArgs
         * @param keyring The application's keyring
         */
        CredentialDialogController(const Events::DownloadCredentialNeededEventArgs& args, Keyring::Keyring& keyring);
        /**
         * @brief Gets the URL of the download needing the credential.
         * @return The URL of the download
         */
        const std::string& getUrl() const;
        /**
         * @briefs Gets the list of names of credentials in the keyring.
         * @return The list of credential names in the keyring
         */
        std::vector<std::string> getKeyringCredentialNames() const;
        /**
         * @brief Uses the entered credential.
         * @param username The username
         * @param password The password
         */
        void use(const std::string& username, const std::string& password);
        /**
         * @brief Uses the credential from the keyring at the specified index.
         * @param index The index of the credential in the keyring
         */
        void use(int index);

    private:
        Events::DownloadCredentialNeededEventArgs m_args;
        Keyring::Keyring& m_keyring;
    };
}

#endif //CREDENTIALDIALOGCONTROLLER_H