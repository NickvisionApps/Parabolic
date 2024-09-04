#ifndef KEYRINGDIALOGCONTROLLER_H
#define KEYRINGDIALOGCONTROLLER_H

#include <optional>
#include <string>
#include <vector>
#include <libnick/keyring/credential.h>
#include <libnick/keyring/keyring.h>
#include "models/credentialcheckstatus.h"

namespace Nickvision::TubeConverter::Shared::Controllers
{
    /**
     * @brief A controller for the KeyringDialog.
     */
    class KeyringDialogController
    {
    public:
        /**
         * @brief Constructs a KeyringDialogController.
         * @param keyring The Keyring for the controller to manage
         */
        KeyringDialogController(Keyring::Keyring& keyring);
        /**
         * @brief Gets whether or not the Keyring is saving to disk.
         * @return True if the Keyring is saving to disk, false otherwise
         */
        bool isSavingToDisk() const;
        /**
         * @brief Gets all the credentials from the Keyring.
         * @return The list of credentials
         */
        const std::vector<Keyring::Credential>& getCredentials() const;
        /**
         * @brief Gets a credential by name.
         * @param name The name of the credential
         * @return The credential if found, else std::nullopt
         */
        std::optional<Keyring::Credential> getCredential(const std::string& name) const;
        /**
         * @brief Adds a credential to the Keyring.
         * @param name The name of the credential
         * @param url The url of the credential
         * @param username The username of the credential
         * @param password The password of the credential
         * @return CredentialCheckStatus
         */
        Models::CredentialCheckStatus addCredential(const std::string& name, const std::string& url, const std::string& username, const std::string& password);
        /**
         * @brief Updates a credential in the Keyring.
         * @param name The name of the credential to update
         * @param url The new url of the credential
         * @param username The new username of the credential
         * @param password The new password of the credential
         * @return CredentialCheckStatus
         */
        Models::CredentialCheckStatus updateCredential(const std::string& name, const std::string& url, const std::string& username, const std::string& password);
        /**
         * @brief Deletes a credential from the Keyring.
         * @param name The name of the credential to delete
         * @return CredentialCheckStatus
         */
        Models::CredentialCheckStatus deleteCredential(const std::string& name);

    private:
        Keyring::Keyring& m_keyring;
    };
}

#endif //KEYRINGDIALOGCONTROLLER_H