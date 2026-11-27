#include "controls/keyring_credential_dialog.h"
#include <QAction>
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QKeySequence>
#include <QLineEdit>
#include <QObject>
#include <QPalette>
#include <QPushButton>
#include <QShortcut>
#include <QString>
#include <QWidget>
#include <algorithm>
#include <oclero/qlementine/icons/Icons16.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <string>
#include <vector>
#include "app.h"
#include "application.h"
#include "helpers/translation_notifier.h"
#include "keyring.h"
#include "translation.h"
#include "ui_keyring_credential_dialog.h"
#include "url.h"

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
        QPalette error_palette{ m_ui->lbl_error->palette() };
        error_palette.setColor(QPalette::WindowText, oclero::qlementine::appStyle()->theme().statusColorError);
        m_ui->lbl_error->setPalette(error_palette);
        m_ui->txt_password->addAction(m_ui->act_toggle_password, QLineEdit::TrailingPosition);
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
        connect(m_ui->txt_url, &QLineEdit::textChanged, this, &KeyringCredentialDialog::validate);
        connect(m_ui->txt_password, &QLineEdit::textChanged, this, &KeyringCredentialDialog::validate);
        connect(m_ui->act_toggle_password, &QAction::toggled, this, &KeyringCredentialDialog::toggle_password);
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

    void KeyringCredentialDialog::toggle_password(bool checked)
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        m_ui->txt_password->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
        m_ui->act_toggle_password->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(checked ? oclero::qlementine::icons::Icons16::Action_EyeCrossed
                                                            : oclero::qlementine::icons::Icons16::Action_Eye)));
        m_ui->act_toggle_password->setToolTip(checked ? translator._("Hide Password") : translator._("Show Password"));
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
        toggle_password(m_ui->act_toggle_password->isChecked());
        validate();
    }

    void KeyringCredentialDialog::validate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        std::string name{ m_ui->txt_name->text().trimmed().toStdString() };
        std::string url{ m_ui->txt_url->text().trimmed().toStdString() };
        bool is_taken{ std::ranges::contains(m_taken_names, name) };
        bool is_url_valid{ url.empty() || url::is_valid(url) };
        QString error;
        if (name.empty() && m_ui->txt_name->isModified())
        {
            error = translator._("A name is required.");
        }
        else if (is_taken)
        {
            error = translator._("A credential with this name already exists.");
        }
        else if (!is_url_valid)
        {
            error = translator._("The URL is not valid.");
        }
        else if (m_ui->txt_password->text().isEmpty() && m_ui->txt_password->isModified())
        {
            error = translator._("A password is required.");
        }
        bool was_visible{ m_ui->lbl_error->isVisibleTo(this) };
        m_ui->lbl_error->setText(error);
        m_ui->lbl_error->setVisible(!error.isEmpty());
        if (was_visible != !error.isEmpty())
        {
            adjustSize();
        }
        m_ui->buttons->button(QDialogButtonBox::Ok)
                ->setEnabled(!name.empty() && !is_taken && is_url_valid && !m_ui->txt_password->text().isEmpty());
    }
}
