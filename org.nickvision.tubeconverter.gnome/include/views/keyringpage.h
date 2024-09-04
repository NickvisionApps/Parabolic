#ifndef KEYRINGPAGE_H
#define KEYRINGPAGE_H

#include <memory>
#include <vector>
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
         * @param toastOverlay The AdwToastOverlay to send notifications to
         * @param parent The GtkWindow object of the parent window
         */
        KeyringPage(const std::shared_ptr<Shared::Controllers::KeyringDialogController>& controller, AdwToastOverlay* toastOverlay, GtkWindow* parent);

    private:
        /**
         * @brief Modes for editing a credential.
         */
        enum class EditMode
        {
            None,
            Add,
            Modify
        };
        /**
         * @brief Reloads the credentials to show on the page.
         */
        void reloadCredentials();
        /**
         * @brief Prompts the user to add a new credential.
         */
        void addNewCredential();
        /**
         * @brief Prompts the user to edit a credential.
         * @param name The name of the credential to edit
         */
        void editCredential(const std::string& name);
        /**
         * @brief Deletes a credential.
         * @param name The name of the credential to delete
         */
        void deleteCredential(const std::string& name);
        /**
         * @brief Confirms an edit to a credential.
         */
        void editConfirm();
        std::shared_ptr<Shared::Controllers::KeyringDialogController> m_controller;
        EditMode m_editMode;
        std::vector<AdwActionRow*> m_credentialRows;
    };
}

#endif //KEYRINGPAGE_H