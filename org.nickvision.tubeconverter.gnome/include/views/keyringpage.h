#ifndef KEYRINGPAGE_H
#define KEYRINGPAGE_H

#include <memory>
#include <adwaita.h>
#include "controllers/keyringdialogcontroller.h"
#include "helpers/controlbase.h"

namespace Nickvision::TubeConverter::GNOME::Views
{
    /**
     * @brief A page for managing keyring credentials.
     */
    class KeyringPage : public Helpers::ControlBase<AdwBin>
    {
    public:
        /**
         * @brief Constructs a KeyringPage.
         * @param controller The KeyringDialogController
         * @param parent The GtkWindow object of the parent window
         */
        KeyringPage(const std::shared_ptr<Shared::Controllers::KeyringDialogController>& controller, GtkWindow* parent);

    private:
        std::shared_ptr<Shared::Controllers::KeyringDialogController> m_controller;
    };
}

#endif //KEYRINGPAGE_H