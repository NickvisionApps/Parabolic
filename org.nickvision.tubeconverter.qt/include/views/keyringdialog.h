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
    
    private:
        Ui::KeyringDialog* m_ui;
        std::shared_ptr<Shared::Controllers::KeyringDialogController> m_controller;
    };
}

#endif //KEYRINGDIALOG_H