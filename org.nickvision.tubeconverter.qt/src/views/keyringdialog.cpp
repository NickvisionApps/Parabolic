#include "views/keyringdialog.h"
#include "ui_keyringdialog.h"
#include <QMessageBox>
#include <libnick/localization/gettext.h>
#include "controls/keyringrow.h"

using namespace Nickvision::Keyring;
using namespace Nickvision::TubeConverter::QT::Controls;
using namespace Nickvision::TubeConverter::Shared::Controllers;
using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::QT::Views
{
    KeyringDialog::KeyringDialog(const std::shared_ptr<KeyringDialogController>& controller, QWidget* parent)
        : QDialog{ parent },
        m_ui{ new Ui::KeyringDialog() },
        m_controller{ controller },
        m_editMode{ EditMode::None }
    {
        m_ui->setupUi(this);
        setWindowTitle(_("Keyring"));
        //Localize the Manage page
        m_ui->lblDescription->setText(_("Create, save, and manage your credentials for media websites."));
        m_ui->btnAddNew->setText(_("Add"));
        m_ui->lblNoCredentials->setText(_("No Credentials Found"));
        //Localize the Edit page
        m_ui->btnBack->setText(_("Back"));
        m_ui->lblName->setText(_("Name"));
        m_ui->txtName->setPlaceholderText(_("Enter name here"));
        m_ui->lblUrl->setText(_("URL"));
        m_ui->txtUrl->setPlaceholderText(_("Enter url here"));
        m_ui->lblUsername->setText(_("Username"));
        m_ui->txtUsername->setPlaceholderText(_("Enter username here"));
        m_ui->lblPassword->setText(_("Password"));
        m_ui->txtPassword->setPlaceholderText(_("Enter password here"));
        //Signals
        connect(m_ui->btnBack, &QPushButton::clicked, this, &KeyringDialog::backToManage);
        connect(m_ui->btnAddNew, &QPushButton::clicked, this, &KeyringDialog::addNewCredential);
        connect(m_ui->btnEditConfirm, &QPushButton::clicked, this, &KeyringDialog::editConfirm);
        //Load
        m_ui->viewStack->setCurrentIndex(0);
        reloadCredentials();
    }

    KeyringDialog::~KeyringDialog()
    {
        delete m_ui;
    }

    void KeyringDialog::showEvent(QShowEvent* event)
    {
        QDialog::showEvent(event);
        if(!m_controller->isSavingToDisk())
        {
            QMessageBox::warning(this, _("Warning"), _("The keyring is not saving changes to disk. Any changes made will be lost when the application closes."), QMessageBox::Ok);
        }
    }

    void KeyringDialog::backToManage()
    {
        m_editMode = EditMode::None;
        m_ui->viewStack->setCurrentIndex(0);
    }

    void KeyringDialog::addNewCredential()
    {
        m_editMode = EditMode::Add;
        m_ui->viewStack->setCurrentIndex(1);
        m_ui->txtName->setText("");
        m_ui->txtUrl->setText("");
        m_ui->txtUsername->setText("");
        m_ui->txtPassword->setText("");
        m_ui->btnEditConfirm->setText(_("Add"));
    }

    void KeyringDialog::editCredential(const QString& name)
    {
        std::optional<Credential> credential{ m_controller->getCredential(name.toStdString()) };
        if(!credential)
        {
            return;
        }
        m_editMode = EditMode::Modify;
        m_ui->viewStack->setCurrentIndex(1);
        m_ui->txtName->setText(QString::fromStdString(credential->getName()));
        m_ui->txtUrl->setText(QString::fromStdString(credential->getUri()));
        m_ui->txtUsername->setText(QString::fromStdString(credential->getUsername()));
        m_ui->txtPassword->setText(QString::fromStdString(credential->getPassword()));
        m_ui->btnEditConfirm->setText(_("Modify"));
    }

    void KeyringDialog::deleteCredential(const QString& name)
    {
        QMessageBox msgBox{ QMessageBox::Icon::Warning, _("Delete Credential?"), _("Are you sure you want to delete this credential?"), QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, this };
        if(msgBox.exec() == QMessageBox::StandardButton::Yes)
        {
            m_controller->deleteCredential(name.toStdString());
            reloadCredentials();
        }
    }

    void KeyringDialog::editConfirm()
    {
        CredentialCheckStatus status;
        switch(m_editMode)
        {
        case EditMode::Add:
            status = m_controller->addCredential(m_ui->txtName->text().toStdString(), m_ui->txtUrl->text().toStdString(), m_ui->txtUsername->text().toStdString(), m_ui->txtPassword->text().toStdString());
            break;
        case EditMode::Modify:
            status = m_controller->updateCredential(m_ui->txtName->text().toStdString(), m_ui->txtUrl->text().toStdString(), m_ui->txtUsername->text().toStdString(), m_ui->txtPassword->text().toStdString());
            break;
        default:
            return;
        }
        switch(status)
        {
        case CredentialCheckStatus::EmptyName:
            QMessageBox::critical(this, _("Error"), _("The credential name cannot be empty."), QMessageBox::Ok);
            break;
        case CredentialCheckStatus::EmptyUsernamePassword:
            QMessageBox::critical(this, _("Error"), _("Both the username and password cannot be empty."), QMessageBox::Ok);
            break;
        case CredentialCheckStatus::InvalidUri:
            QMessageBox::critical(this, _("Error"), _("The provided url is invalid."), QMessageBox::Ok);
            break;
        case CredentialCheckStatus::ExistingName:
            QMessageBox::critical(this, _("Error"), _("A credential with this name already exists."), QMessageBox::Ok);
            break;
        case CredentialCheckStatus::DatabaseError:
            QMessageBox::critical(this, _("Error"), _("There was an unknown error adding the credential to the keyring."), QMessageBox::Ok);
            break;
        default:
            reloadCredentials();
            backToManage();
        }
    }

    void KeyringDialog::reloadCredentials()
    {
        m_ui->listCredentials->clear();
        for(const Credential& credential : m_controller->getCredentials())
        {
            KeyringRow* row{ new KeyringRow(credential) };
            connect(row, &KeyringRow::editCredential, this, &KeyringDialog::editCredential);
            connect(row, &KeyringRow::deleteCredential, this, &KeyringDialog::deleteCredential);
            QListWidgetItem* item{ new QListWidgetItem() };
            item->setSizeHint(row->sizeHint());
            m_ui->listCredentials->insertItem(0, item);
            m_ui->listCredentials->setItemWidget(item, row);
        }
        m_ui->viewStackCredentials->setCurrentIndex(m_controller->getCredentials().empty() ? 0 : 1);
    }
}