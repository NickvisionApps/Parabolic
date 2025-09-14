#include "controllers/keyringdialogcontroller.h"
#include <libnick/helpers/stringhelpers.h>

using namespace Nickvision::Helpers;
using namespace Nickvision::Keyring;
using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::Shared::Controllers
{
    KeyringDialogController::KeyringDialogController(Keyring::Keyring& keyring)
        : m_keyring{ keyring }
    {

    }

    bool KeyringDialogController::isSavingToDisk() const
    {
        return m_keyring.isSavingToDisk();
    }

    const std::vector<Credential>& KeyringDialogController::getCredentials() const
    {
        return m_keyring.getAll();
    }

    std::optional<Credential> KeyringDialogController::getCredential(const std::string& name) const
    {
        return m_keyring.get(name);
    }

    CredentialCheckStatus KeyringDialogController::addCredential(const std::string& name, const std::string& url, const std::string& username, const std::string& password)
    {
        if(name.empty())
        {
            return CredentialCheckStatus::EmptyName;
        }
        else if(username.empty() && password.empty())
        {
            return CredentialCheckStatus::EmptyUsernamePassword;
        }
        else if(!StringHelpers::isValidUrl(StringHelpers::trim(url)))
        {
            return CredentialCheckStatus::InvalidUri;
        }
        else if(m_keyring.get(name).has_value())
        {
            return CredentialCheckStatus::ExistingName;
        }
        else
        {
            return m_keyring.add({ name, StringHelpers::trim(url), username, password }) ? CredentialCheckStatus::Valid : CredentialCheckStatus::DatabaseError;
        }
    }

    CredentialCheckStatus KeyringDialogController::updateCredential(const std::string& name, const std::string& url, const std::string& username, const std::string& password)
    {
        if(name.empty())
        {
            return CredentialCheckStatus::EmptyName;
        }
        else if(username.empty() && password.empty())
        {
            return CredentialCheckStatus::EmptyUsernamePassword;
        }
        else if(!StringHelpers::isValidUrl(StringHelpers::trim(url)))
        {
            return CredentialCheckStatus::InvalidUri;
        }
        else
        {
            return m_keyring.update({ name, StringHelpers::trim(url), username, password }) ? CredentialCheckStatus::Valid : CredentialCheckStatus::DatabaseError;
        }
    }

    CredentialCheckStatus KeyringDialogController::deleteCredential(const std::string& name)
    {
        if(name.empty())
        {
            return CredentialCheckStatus::EmptyName;
        }
        else
        {
            return m_keyring.remove(name) ? CredentialCheckStatus::Valid : CredentialCheckStatus::DatabaseError;
        }
    }
}