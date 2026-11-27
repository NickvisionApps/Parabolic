#ifndef KEYRINGDIALOG_H
#define KEYRINGDIALOG_H

#include <QDialog>
#include <QWidget>

namespace Ui
{
    class KeyringDialog;
}

namespace parabolic::qt::views
{
    class KeyringDialog : public QDialog
    {
        Q_OBJECT

    public:
        KeyringDialog(QWidget* parent = nullptr);
        ~KeyringDialog() override;
        KeyringDialog(const KeyringDialog&) = delete;
        KeyringDialog(KeyringDialog&&) = delete;
        KeyringDialog& operator=(const KeyringDialog&) = delete;
        KeyringDialog& operator=(KeyringDialog&&) = delete;

    private:
        Q_SLOT void translate();
        Ui::KeyringDialog* m_ui{ nullptr };
    };
}

#endif
