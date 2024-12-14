#include "views/keyringpage.h"
#include "ui_keyringpage.h"
#include <QMessageBox>
#include <libnick/localization/gettext.h>
#include "controls/keyringrow.h"

using namespace Nickvision::Keyring;
using namespace Nickvision::TubeConverter::QT::Controls;
using namespace Nickvision::TubeConverter::Shared::Controllers;
using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::QT::Views
{
    enum Pages
    {
        NoCredentials = 0,
        Credentials,
        Edit
    };

    KeyringPage::KeyringPage(const std::shared_ptr<KeyringDialogController>& controller, QWidget* parent)
        : QWidget{ parent },
        m_ui{ new Ui::KeyringPage() },
        m_controller{ controller },
        m_editMode{ EditMode::None }
    {
        m_ui->setupUi(this);
        //Localize Strings
        m_ui->lblIconNone->setPixmap(QIcon::fromTheme(QIcon::ThemeIcon::DialogPassword).pixmap(64, 64));
        m_ui->lblNoCredentials->setText(_("No Credentials Found"));
        m_ui->btnAddCredentialNone->setText(_("Add Credential"));
        m_ui->btnAddCredential->setText(_("Add Credential"));
        m_ui->btnEditBack->setText(_("Back"));
        m_ui->lblEditName->setText(_("Name"));
        m_ui->txtEditName->setPlaceholderText(_("Enter name here"));
        m_ui->lblEditUrl->setText(_("URL"));
        m_ui->txtEditUrl->setPlaceholderText(_("Enter url here"));
        m_ui->lblEditUsername->setText(_("Username"));
        m_ui->txtEditUsername->setPlaceholderText(_("Enter username here"));
        m_ui->lblEditPassword->setText(_("Password"));
        m_ui->txtEditPassword->setPlaceholderText(_("Enter password here"));
        m_ui->btnEditSave->setText(_("Save"));
        //Signals
        connect(m_ui->btnAddCredentialNone, &QPushButton::clicked, this, &KeyringPage::addNewCredential);
        connect(m_ui->btnAddCredential, &QPushButton::clicked, this, &KeyringPage::addNewCredential);
        connect(m_ui->btnEditBack, &QPushButton::clicked, this, &KeyringPage::discard);
        connect(m_ui->btnEditSave, &QPushButton::clicked, this, &KeyringPage::save);
        //Load
        reloadCredentials();
    }

    KeyringPage::~KeyringPage()
    {
        delete m_ui;
    }

    void KeyringPage::showEvent(QShowEvent* event)
    {
        QWidget::showEvent(event);
        if(!m_controller->isSavingToDisk())
        {
            QMessageBox::warning(this, _("Warning"), _("The keyring is not saving changes to disk. Any changes made will be lost when the application closes."), QMessageBox::Ok);
        }
    }

    void KeyringPage::addNewCredential()
    {
        m_editMode = EditMode::Add;
        m_ui->viewStack->setCurrentIndex(Pages::Edit);
        m_ui->lblEditDescription->setText(_("New Credential"));
        m_ui->txtEditName->setText("");
        m_ui->txtEditUrl->setText("");
        m_ui->txtEditUsername->setText("");
        m_ui->txtEditPassword->setText("");
        m_ui->btnEditSave->setText(_("Add"));
        m_ui->btnEditSave->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::ListAdd));
    }

    void KeyringPage::editCredential(const QString& name)
    {
        std::optional<Credential> credential{ m_controller->getCredential(name.toStdString()) };
        if(!credential)
        {
            return;
        }
        m_editMode = EditMode::Modify;
        m_ui->viewStack->setCurrentIndex(Pages::Edit);
        m_ui->lblEditDescription->setText(_("Edit Credential"));
        m_ui->txtEditName->setText(QString::fromStdString(credential->getName()));
        m_ui->txtEditUrl->setText(QString::fromStdString(credential->getUri()));
        m_ui->txtEditUsername->setText(QString::fromStdString(credential->getUsername()));
        m_ui->txtEditPassword->setText(QString::fromStdString(credential->getPassword()));
        m_ui->btnEditSave->setText(_("Save"));
        m_ui->btnEditSave->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave));
    }

    void KeyringPage::deleteCredential(const QString& name)
    {
        QMessageBox msgBox{ QMessageBox::Icon::Warning, _("Delete Credential?"), _("Are you sure you want to delete this credential?"), QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, this };
        if(msgBox.exec() == QMessageBox::StandardButton::Yes)
        {
            m_controller->deleteCredential(name.toStdString());
            reloadCredentials();
        }
    }

    void KeyringPage::discard()
    {
        m_editMode = EditMode::None;
        m_ui->viewStack->setCurrentIndex(m_controller->getCredentials().empty() ? Pages::NoCredentials : Pages::Credentials);
    }

    void KeyringPage::save()
    {
        CredentialCheckStatus status;
        switch(m_editMode)
        {
        case EditMode::Add:
            status = m_controller->addCredential(m_ui->txtEditName->text().toStdString(), m_ui->txtEditUrl->text().toStdString(), m_ui->txtEditUsername->text().toStdString(), m_ui->txtEditPassword->text().toStdString());
            break;
        case EditMode::Modify:
            status = m_controller->updateCredential(m_ui->txtEditName->text().toStdString(), m_ui->txtEditUrl->text().toStdString(), m_ui->txtEditUsername->text().toStdString(), m_ui->txtEditPassword->text().toStdString());
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
        }
    }

    void KeyringPage::reloadCredentials()
    {
        m_editMode = EditMode::None;
        m_ui->listCredentials->clear();
        for(const Credential& credential : m_controller->getCredentials())
        {
            KeyringRow* row{ new KeyringRow(credential) };
            connect(row, &KeyringRow::editCredential, this, &KeyringPage::editCredential);
            connect(row, &KeyringRow::deleteCredential, this, &KeyringPage::deleteCredential);
            QListWidgetItem* item{ new QListWidgetItem() };
            item->setSizeHint(row->sizeHint());
            m_ui->listCredentials->insertItem(0, item);
            m_ui->listCredentials->setItemWidget(item, row);
        }
        m_ui->viewStack->setCurrentIndex(m_controller->getCredentials().empty() ? Pages::NoCredentials : Pages::Credentials);
    }
}