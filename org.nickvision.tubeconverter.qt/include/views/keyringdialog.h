#ifndef KEYRINGDIALOG_H
#define KEYRINGDIALOG_H

#include <memory>
#include <QDialog>
#include <QShowEvent>
#include "controllers/keyringdialogcontroller.h"

namespace Ui { class KeyringDialog; }

namespace Nickvision::TubeConverter::QT::Views
{
    class KeyringDialog : public QDialog
    {
    Q_OBJECT

    public:
        /**
         * @brief Constructs a KeyringDialog.
         * @param parent The parent widget
         */
        KeyringDialog(const std::shared_ptr<Shared::Controllers::KeyringDialogController>& controller, QWidget* parent = nullptr);
        /**
         * @brief Destructs a KeyringDialog.
         */
        ~KeyringDialog();

    protected:
        /**
         * @brief The event for when the KeyringDialog is shown.
         * @param event QShowEvent
         */
        void showEvent(QShowEvent* event) override;

    private Q_SLOTS:
        /**
         * @brief Goes back to the manage page.
         */
        void backToManage();
        /**
         * @brief Prompts the user to add a new credential.
         */
        void addNewCredential();
        /**
         * @brief Prompts the user to edit a credential.
         * @param name The name of the credential to edit
         */
        void editCredential(const QString& name);
        /**
         * @brief Delete a credential.
         * @param name The name of the credential to delete
         */
        void deleteCredential(const QString& name);
        /**
         * @brief Confirms a edit to a credential.
         */
        void editConfirm();
    
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
         * @brief Reloads the credentials to show in the dialog.
         */
        void reloadCredentials();
        Ui::KeyringDialog* m_ui;
        std::shared_ptr<Shared::Controllers::KeyringDialogController> m_controller;
        EditMode m_editMode;
    };
}

#endif //KEYRINGDIALOG_H