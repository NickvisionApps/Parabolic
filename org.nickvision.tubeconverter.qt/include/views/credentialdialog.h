#ifndef CREDENTIALDIALOG_H
#define CREDENTIALDIALOG_H

#include <memory>
#include <QDialog>
#include "controllers/credentialdialogcontroller.h"

namespace Ui { class CredentialDialog; }

namespace Nickvision::TubeConverter::Qt::Views
{
    /**
     * @brief A dialog for entering credentials.
     */
    class CredentialDialog : public QDialog
    {
    Q_OBJECT

    public:
        /**
         * @brief Constructs a CredentialDialog.
         * @param controller The CredentialDialogController
         * @param parent The parent widget
         */
        CredentialDialog(const std::shared_ptr<Shared::Controllers::CredentialDialogController>& controller, QWidget* parent = nullptr);
        /**
         * @brief Destructs a CredentialDialog.
         */
        ~CredentialDialog();

    private Q_SLOTS:
        /**
         * @brief Handles when the cmbCredential's index has changed.
         */
        void onCmbCredentialChanged(int index);
        /**
         * @brief Uses the entered credential.
         */
        void use();

    private:
        Ui::CredentialDialog* m_ui;
        std::shared_ptr<Shared::Controllers::CredentialDialogController> m_controller;
    };
}

#endif //CREDENTIALDIALOG_H