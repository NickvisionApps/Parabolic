#ifndef KEYRINGDIALOGCONTROLLER_H
#define KEYRINGDIALOGCONTROLLER_H

#include <libnick/keyring/keyring.h>

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

    private:
        Keyring::Keyring& m_keyring;
    };
}

#endif //KEYRINGDIALOGCONTROLLER_H