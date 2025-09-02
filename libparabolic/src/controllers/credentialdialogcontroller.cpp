#include "controllers/credentialdialogcontroller.h"

using namespace Nickvision::Keyring;
using namespace Nickvision::TubeConverter::Shared::Events;

namespace Nickvision::TubeConverter::Shared::Controllers
{
    CredentialDialogController::CredentialDialogController(const DownloadCredentialNeededEventArgs& args, Keyring::Keyring& keyring)
        : m_args{ args },
        m_keyring{ keyring }
    {

    }

    const std::string& CredentialDialogController::getUrl() const
    {
        return m_args.getUrl();
    }

    std::vector<std::string> CredentialDialogController::getKeyringCredentialNames() const
    {
        std::vector<std::string> names;
        for(const Credential& credential : m_keyring.getAll())
        {
            names.push_back(credential.getName());
        }
        return names;
    }

    void CredentialDialogController::use(const std::string& username, const std::string& password)
    {
        m_args.getCredential()->setUsername(username);
        m_args.getCredential()->setPassword(password);
    }

    void CredentialDialogController::use(int index)
    {
        if(index >= static_cast<int>(m_keyring.getAll().size()))
        {
            return;
        }
        const Credential& credential{ m_keyring.getAll()[index] };
        m_args.getCredential()->setUsername(credential.getUsername());
        m_args.getCredential()->setPassword(credential.getPassword());
    }
}