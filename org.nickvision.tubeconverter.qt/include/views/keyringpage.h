#ifndef KEYRINGPAGE_H
#define KEYRINGPAGE_H

#include <memory>
#include <QShowEvent>
#include <QWidget>
#include "controllers/keyringdialogcontroller.h"

namespace Ui { class KeyringPage; }

namespace Nickvision::TubeConverter::QT::Views
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
        Ui::KeyringPage* m_ui;
        std::shared_ptr<Shared::Controllers::KeyringDialogController> m_controller;
        EditMode m_editMode;
    };
}

#endif //KEYRINGPAGE_H