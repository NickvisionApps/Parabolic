#ifndef CREDENTIALDIALOG_H
#define CREDENTIALDIALOG_H

#include <memory>
#include "controllers/credentialdialogcontroller.h"
#include "helpers/dialogbase.h"

namespace Nickvision::TubeConverter::GNOME::Views
{
    /**
     * @brief A dialog for entering credentials.
     */
    class CredentialDialog : public Helpers::DialogBase
    {
    public:
        /**
         * @brief Constructs a CredentialDialog.
         * @param controller The CredentialDialogController
         * @param parent The GtkWindow object of the parent window
         */
        CredentialDialog(const std::shared_ptr<Shared::Controllers::CredentialDialogController>& controller, GtkWindow* parent);

    private:
        /**
         * @brief Handles when the credential combobox is changed.
         */
        void onCmbCredentialChanged();
        /**
         * @brief Uses the entered credential.
         */
        void use();
        std::shared_ptr<Shared::Controllers::CredentialDialogController> m_controller;
    };
}

#endif //CREDENTIALDIALOG_H