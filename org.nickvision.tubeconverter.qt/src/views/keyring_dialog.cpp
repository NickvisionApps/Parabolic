#include "views/keyring_dialog.h"
#include <QApplication>
#include <QDialog>
#include <QKeySequence>
#include <QMessageBox>
#include <QObject>
#include <QShortcut>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QToolButton>
#include <QWidget>
#include <cstddef>
#include <oclero/qlementine/icons/Icons16.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <string>
#include <vector>
#include "app.h"
#include "application.h"
#include "controls/keyring_credential_dialog.h"
#include "helpers/translation_notifier.h"
#include "keyring.h"
#include "translation.h"
#include "ui_keyring_dialog.h"

namespace parabolic::qt::views
{
    KeyringDialog::KeyringDialog(QWidget* parent)
        : QDialog{ parent },
          m_ui{ new Ui::KeyringDialog() }
    {
        m_ui->setupUi(this);
        m_ui->btn_add->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Action_Plus)));
        m_ui->btn_edit->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Misc_Pen)));
        m_ui->btn_remove->setIcon(oclero::qlementine::appStyle()->makeThemedIcon(
                oclero::qlementine::icons::iconPath(oclero::qlementine::icons::Icons16::Action_Minus)));
        translate();
        reload();
        connect(qobject_cast<App*>(QApplication::instance())->translation_notifier(),
                &helpers::TranslationNotifier::translation_changed,
                this,
                &KeyringDialog::translate);
        connect(new QShortcut(QKeySequence::Close, this), &QShortcut::activated, this, &QDialog::reject);
        connect(m_ui->btn_add, &QToolButton::clicked, this, &KeyringDialog::add_credential);
        connect(m_ui->btn_edit, &QToolButton::clicked, this, &KeyringDialog::edit_credential);
        connect(m_ui->btn_remove, &QToolButton::clicked, this, &KeyringDialog::remove_credential);
        connect(m_ui->tbl_credentials, &QTableWidget::itemDoubleClicked, this, &KeyringDialog::edit_credential);
    }

    KeyringDialog::~KeyringDialog()
    {
        delete m_ui;
    }

    void KeyringDialog::add_credential()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        std::vector<std::string> names;
        names.reserve(m_credentials.size());
        for (const keyring::Credential& credential : m_credentials)
        {
            names.push_back(credential.name());
        }
        controls::KeyringCredentialDialog dialog{ {}, names, this };
        if (dialog.exec() != QDialog::Accepted)
        {
            return;
        }
        if (!application::State::instance().keyring().add(dialog.credential()))
        {
            QMessageBox::critical(this, translator._("Error"), translator._("Unable to add the credential."));
            return;
        }
        reload();
    }

    void KeyringDialog::edit_credential()
    {
        int row{ m_ui->tbl_credentials->currentRow() };
        if (row < 0)
        {
            return;
        }
        const translation::Translator& translator{ application::State::instance().translator() };
        controls::KeyringCredentialDialog dialog{ m_credentials[static_cast<std::size_t>(row)], {}, this };
        if (dialog.exec() != QDialog::Accepted)
        {
            return;
        }
        if (!application::State::instance().keyring().update(dialog.credential()))
        {
            QMessageBox::critical(this, translator._("Error"), translator._("Unable to update the credential."));
            return;
        }
        reload();
    }

    void KeyringDialog::reload()
    {
        m_credentials = application::State::instance().keyring().get_all();
        m_ui->tbl_credentials->setRowCount(static_cast<int>(m_credentials.size()));
        for (std::size_t i{ 0 }; i < m_credentials.size(); i++)
        {
            m_ui->tbl_credentials->setItem(static_cast<int>(i),
                                           0,
                                           new QTableWidgetItem(QString::fromStdString(m_credentials[i].name())));
            m_ui->tbl_credentials->setItem(static_cast<int>(i),
                                           1,
                                           new QTableWidgetItem(QString::fromStdString(m_credentials[i].url())));
            m_ui->tbl_credentials->setItem(static_cast<int>(i),
                                           2,
                                           new QTableWidgetItem(QString::fromStdString(m_credentials[i].username())));
        }
    }

    void KeyringDialog::remove_credential()
    {
        int row{ m_ui->tbl_credentials->currentRow() };
        if (row < 0)
        {
            return;
        }
        const translation::Translator& translator{ application::State::instance().translator() };
        const keyring::Credential& credential{ m_credentials[static_cast<std::size_t>(row)] };
        if (QMessageBox::question(this,
                                  translator._("Remove Credential"),
                                  QString::fromStdString(translator._("Are you sure you want to remove \"{}\"?",
                                                                      credential.name()))) != QMessageBox::Yes)
        {
            return;
        }
        if (!application::State::instance().keyring().remove(credential.name()))
        {
            QMessageBox::critical(this, translator._("Error"), translator._("Unable to remove the credential."));
            return;
        }
        reload();
    }

    void KeyringDialog::translate()
    {
        const translation::Translator& translator{ application::State::instance().translator() };
        setWindowTitle(translator._("Keyring"));
        m_ui->lbl_description->setText(
                translator._("Add, delete, and modify credentials for validating websites. These will show in the add download "
                             "dialog for selection."));
        m_ui->btn_add->setToolTip(translator._("Add"));
        m_ui->btn_edit->setToolTip(translator._("Edit"));
        m_ui->btn_remove->setToolTip(translator._("Remove"));
        m_ui->tbl_credentials->setHorizontalHeaderLabels(
                { translator._("Name"), translator._("URL"), translator._("Username") });
    }
}
