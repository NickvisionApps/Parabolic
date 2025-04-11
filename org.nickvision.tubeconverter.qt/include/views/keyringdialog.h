#ifndef KEYRINGDIALOG_H
#define KEYRINGDIALOG_H

#include <memory>
#include <QDialog>
#include "controllers/keyringdialogcontroller.h"

namespace Ui { class KeyringDialog; }

namespace Nickvision::TubeConverter::Qt::Views
{
    /**
     * @brief A dialog for managing credentials in the app's keyring.
     */
    class KeyringDialog : public QDialog
    {
    Q_OBJECT

    public:
        /**
         * @brief Constructs a KeyringDialog.
         * @param controller The KeyringDialogController
         * @param parent The parent widget
         */
        KeyringDialog(const std::shared_ptr<Shared::Controllers::KeyringDialogController>& controller, QWidget* parent = nullptr);
        /**
         * @brief Destructs a KeyringPage.
         */
        ~KeyringDialog();

    private Q_SLOTS:
        /**
         * @brief Prompts the user to add a new credential.
         */
        void addCredential();

    private:
        Ui::KeyringDialog* m_ui;
        std::shared_ptr<Shared::Controllers::KeyringDialogController> m_controller;
    };
}

#endif //KEYRINGDIALOG_H
