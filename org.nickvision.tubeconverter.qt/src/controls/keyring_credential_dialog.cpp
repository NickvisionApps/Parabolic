#include "controls/keyring_credential_dialog.h"
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QKeySequence>
#include <QLineEdit>
#include <QObject>
#include <QPushButton>
#include <QShortcut>
#include <QString>
#include <QWidget>
#include <algorithm>
#include <string>
#include <vector>
#include "app.h"
#include "application.h"
#include "helpers/translation_notifier.h"
#include "keyring.h"
#include "translation.h"
#include "ui_keyring_credential_dialog.h"

namespace parabolic::qt::controls
{
    KeyringCredentialDialog::KeyringCredentialDialog(const keyring::Credential& credential,
                                                     const std::vector<std::string>& taken_names,
                                                     QWidget* parent)
        : QDialog{ parent },
          m_taken_names{ taken_names },
          m_ui{ new Ui::KeyringCredentialDialog() }
    {
        m_ui->setupUi(this);
        translate();
        m_ui->txt_name->setText(QString::fromStdString(credential.name()));
        m_ui->txt_name->setReadOnly(!credential.name().empty());
        m_ui->txt_url->setText(QString::fromStdString(credential.url()));
        m_ui->txt_username->setText(QString::fromStdString(credential.username()));
        m_ui->txt_password->setText(QString::fromStdString(credential.password()));
        m_ui->buttons->button(QDialogButtonBox::Ok)
                ->setEnabled(!credential.name().empty() && !std::ranges::contains(m_taken_names, credential.name()) &&
                             !m_ui->txt_password->text().isEmpty());
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &KeyringCredentialDialog::translate);
        connect(new QShortcut(QKeySequence::Close, this), &QShortcut::activated, this, &QDialog::reject);
        connect(m_ui->txt_name, &QLineEdit::textChanged, this, &KeyringCredentialDialog::validate);
        connect(m_ui->txt_password, &QLineEdit::textChanged, this, &KeyringCredentialDialog::validate);
    }

    KeyringCredentialDialog::~KeyringCredentialDialog()
    {
        delete m_ui;
    }

    keyring::Credential KeyringCredentialDialog::credential() const
    {
        return { m_ui->txt_name->text().trimmed().toStdString(),
                 m_ui->txt_url->text().trimmed().toStdString(),
                 m_ui->txt_username->text().trimmed().toStdString(),
                 m_ui->txt_password->text().toStdString() };
    }

    void KeyringCredentialDialog::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        setWindowTitle(translator._("Credential"));
        m_ui->lbl_name->setText(QString::fromStdString(translator._("{}:", translator._("Name"))));
        m_ui->lbl_url->setText(QString::fromStdString(translator._("{}:", translator._("URL"))));
        m_ui->txt_url->setPlaceholderText(translator._("Optional"));
        m_ui->lbl_username->setText(QString::fromStdString(translator._("{}:", translator._("Username"))));
        m_ui->txt_username->setPlaceholderText(translator._("Optional"));
        m_ui->lbl_password->setText(QString::fromStdString(translator._("{}:", translator._("Password"))));
    }

    void KeyringCredentialDialog::validate()
    {
        std::string name{ m_ui->txt_name->text().trimmed().toStdString() };
        m_ui->buttons->button(QDialogButtonBox::Ok)
                ->setEnabled(!name.empty() && !std::ranges::contains(m_taken_names, name) &&
                             !m_ui->txt_password->text().isEmpty());
    }
}
