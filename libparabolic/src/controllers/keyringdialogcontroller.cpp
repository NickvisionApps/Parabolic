#include "controllers/keyringdialogcontroller.h"

using namespace Nickvision::Keyring;

namespace Nickvision::TubeConverter::Shared::Controllers
{
    KeyringDialogController::KeyringDialogController(Keyring::Keyring& keyring)
        : m_keyring{ keyring }
    {

    }
}