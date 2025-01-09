#ifndef KEYRINGPAGE_H
#define KEYRINGPAGE_H

#include <memory>
#include <QShowEvent>
#include <QWidget>
#include "controllers/keyringdialogcontroller.h"

namespace Ui { class KeyringPage; }

namespace Nickvision::TubeConverter::Qt::Views
{
    class KeyringPage : public QWidget
    {
    Q_OBJECT

    public:
        /**
         * @brief Constructs a KeyringPage.
         * @param parent The parent widget
         */
        KeyringPage(const std::shared_ptr<Shared::Controllers::KeyringDialogController>& controller, QWidget* parent = nullptr);
        /**
         * @brief Destructs a KeyringPage.
         */
        ~KeyringPage();

    protected:
        /**
         * @brief The event for when the KeyringPage is shown.
         * @param event QShowEvent
         */
        void showEvent(QShowEvent* event) override;
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
         * @brief Discards the changes being made to a credential.
         */
        void discard();
        /**
         * @brief Saves a credential being edited.
         */
        void save();

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
        Ui::KeyringPage* m_ui;
        std::shared_ptr<Shared::Controllers::KeyringDialogController> m_controller;
        EditMode m_editMode;
    };
}

#endif //KEYRINGPAGE_H