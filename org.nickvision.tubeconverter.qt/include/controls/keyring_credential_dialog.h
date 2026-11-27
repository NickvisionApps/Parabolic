#ifndef KEYRINGCREDENTIALDIALOG_H
#define KEYRINGCREDENTIALDIALOG_H

#include <QDialog>
#include <QWidget>
#include <string>
#include <vector>
#include "keyring.h"

namespace Ui
{
    class KeyringCredentialDialog;
}

namespace parabolic::qt::controls
{
    class KeyringCredentialDialog : public QDialog
    {
        Q_OBJECT

    public:
        KeyringCredentialDialog(const keyring::Credential& credential,
                                const std::vector<std::string>& taken_names,
                                QWidget* parent = nullptr);
        ~KeyringCredentialDialog() override;
        KeyringCredentialDialog(const KeyringCredentialDialog&) = delete;
        KeyringCredentialDialog(KeyringCredentialDialog&&) = delete;
        KeyringCredentialDialog& operator=(const KeyringCredentialDialog&) = delete;
        KeyringCredentialDialog& operator=(KeyringCredentialDialog&&) = delete;
        [[nodiscard]] keyring::Credential credential() const;

    private:
        Q_SLOT void toggle_password(bool checked);
        Q_SLOT void translate();
        Q_SLOT void validate();
        std::vector<std::string> m_taken_names;
        Ui::KeyringCredentialDialog* m_ui{ nullptr };
    };
}

#endif
