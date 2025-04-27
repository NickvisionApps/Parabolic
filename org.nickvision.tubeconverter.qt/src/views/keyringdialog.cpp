#include "views/keyringdialog.h"
#include <QFont>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <libnick/localization/gettext.h>
#include "controls/statuspage.h"
#include "helpers/qthelpers.h"

using namespace Nickvision::Keyring;
using namespace Nickvision::TubeConverter::Shared::Controllers;
using namespace Nickvision::TubeConverter::Shared::Models;
using namespace Nickvision::TubeConverter::Qt::Controls;
using namespace Nickvision::TubeConverter::Qt::Helpers;

enum KeyringDialogPage
{
    NoCredentials = 0,
    Credentials
};

namespace Ui
{
    class KeyringDialog
    {
    public:
        void setupUi(Nickvision::TubeConverter::Qt::Views::KeyringDialog* parent)
        {
            viewStack = new QStackedWidget(parent);
            QFont boldFont;
            boldFont.setBold(true);
            QLabel* lblCredentials{ new QLabel(parent) };
            lblCredentials->setText(_("Credentials"));
            lblCredentials->setFont(boldFont);
            QLabel* lblDescription{ new QLabel(parent) };
            lblDescription->setText(_("Add, delete, and modify credentials for validating websites"));
            btnAddCredential = new QPushButton(parent);
            btnAddCredential->setAutoDefault(true);
            btnAddCredential->setDefault(true);
            btnAddCredential->setIcon(QLEMENTINE_ICON(Action_Plus));
            btnAddCredential->setText(_("Add"));
            btnAddCredential->setToolTip(_("Add Credential"));
            QVBoxLayout* layoutLabels{ new QVBoxLayout() };
            layoutLabels->addWidget(lblCredentials);
            layoutLabels->addWidget(lblDescription);
            QHBoxLayout* layoutHeader{ new QHBoxLayout() };
            layoutHeader->addLayout(layoutLabels);
            layoutHeader->addStretch();
            layoutHeader->addWidget(btnAddCredential);
            statNoCredentials = new Nickvision::TubeConverter::Qt::Controls::StatusPage(parent);
            statNoCredentials->setIcon(QLEMENTINE_ICON(Misc_EmptySlot));
            statNoCredentials->setTitle(_("No Credentials"));
            statNoCredentials->setDescription(_("Add a credential to get started"));
            viewStack->addWidget(statNoCredentials);
            listCredentials = new QListWidget(parent);
            listCredentials->setContextMenuPolicy(::Qt::ContextMenuPolicy::CustomContextMenu);
            viewStack->addWidget(listCredentials);
            QVBoxLayout* layoutMain{ new QVBoxLayout() };
            layoutMain->addLayout(layoutHeader);
            layoutMain->addWidget(QtHelpers::createHLine(parent));
            layoutMain->addWidget(viewStack);
            parent->setLayout(layoutMain);
        }

        QPushButton* btnAddCredential;
        QStackedWidget* viewStack;
        Nickvision::TubeConverter::Qt::Controls::StatusPage* statNoCredentials;
        QListWidget* listCredentials;
    };
}

namespace Nickvision::TubeConverter::Qt::Views
{
    KeyringDialog::KeyringDialog(const std::shared_ptr<KeyringDialogController>& controller, QWidget* parent)
        : QDialog{ parent },
        m_ui{ new Ui::KeyringDialog() },
        m_controller{ controller }
    {
        //Dialog Settigns
        setWindowTitle(_("Keyring"));
        setMinimumSize(500, 400);
        setModal(true);
        //Load Ui
        m_ui->setupUi(this);
		reloadCredentials();
        if(!m_controller->isSavingToDisk())
        {
            QMessageBox::warning(this, _("Warning"), _("The keyring is not saving changes to disk. Any changes made will be lost when the application closes."), QMessageBox::Ok);
        }
        //Signals
        connect(m_ui->btnAddCredential, &QPushButton::clicked, this, &KeyringDialog::addCredential);
        connect(m_ui->listCredentials, &QListWidget::customContextMenuRequested, this, &KeyringDialog::onListCredentialsContextMenu);
        connect(m_ui->listCredentials, &QListWidget::itemDoubleClicked, this, &KeyringDialog::onCredentialDoubleClicked);
    }

    KeyringDialog::~KeyringDialog()
    {
        delete m_ui;
    }

    void KeyringDialog::addCredential()
    {
        //Add Credential Dialog
        QDialog* dialog{ new QDialog(this) };
        dialog->setMinimumSize(300, 360);
        dialog->setWindowTitle(_("New Credential"));
        QLabel* lblName{ new QLabel(dialog) };
        lblName->setText(_("Name"));
        QLineEdit* txtName{ new QLineEdit(dialog) };
        txtName->setPlaceholderText("Enter name here");
        QLabel* lblUrl{ new QLabel(dialog) };
        lblUrl->setText(_("URL"));
        QLineEdit* txtUrl{ new QLineEdit(dialog) };
        txtUrl->setPlaceholderText("Enter url here");
        QLabel* lblUsername{ new QLabel(dialog) };
        lblUsername->setText(_("Username"));
        QLineEdit* txtUsername{ new QLineEdit(dialog) };
        txtUsername->setPlaceholderText("Enter username here");
        QLabel* lblPassword{ new QLabel(dialog) };
        lblPassword->setText(_("Password"));
        QLineEdit* txtPassword{ new QLineEdit(dialog) };
        txtPassword->setPlaceholderText("Enter password here");
        txtPassword->setEchoMode(QLineEdit::Password);
        QPushButton* btnAdd{ new QPushButton(dialog) };
        btnAdd->setAutoDefault(true);
        btnAdd->setDefault(true);
        btnAdd->setIcon(QLEMENTINE_ICON(Action_Plus));
        btnAdd->setText(_("Add"));
        QFormLayout* layoutForm{ new QFormLayout() };
        layoutForm->addRow(lblName, txtName);
        layoutForm->addRow(lblUrl, txtUrl);
        layoutForm->addRow(lblUsername, txtUsername);
        layoutForm->addRow(lblPassword, txtPassword);
        QVBoxLayout* layout{ new QVBoxLayout() };
        layout->addLayout(layoutForm);
        layout->addWidget(btnAdd);
        dialog->setLayout(layout);
        connect(btnAdd, &QPushButton::clicked, [&]()
        {
            CredentialCheckStatus status{ m_controller->addCredential(txtName->text().toStdString(), txtUrl->text().toStdString(), txtUsername->text().toStdString(), txtPassword->text().toStdString()) };
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
                dialog->close();
            }
        });
        dialog->exec();
        reloadCredentials();
    }
	
    void KeyringDialog::editCredential(const QString& name)
    {
        std::optional<Credential> credential{ m_controller->getCredential(name.toStdString()) };
        if(!credential)
        {
            return;
        }
        //Edit Credential Dialog
        QDialog* dialog{ new QDialog(this) };
        dialog->setMinimumSize(300, 360);
        dialog->setWindowTitle(_("Edit Credential"));
        QLabel* lblName{ new QLabel(dialog) };
        lblName->setText(_("Name"));
        QLineEdit* txtName{ new QLineEdit(dialog) };
        txtName->setPlaceholderText("Enter name here");
        txtName->setText(QString::fromStdString(credential->getName()));
        txtName->setEnabled(false);
        QLabel* lblUrl{ new QLabel(dialog) };
        lblUrl->setText(_("URL"));
        QLineEdit* txtUrl{ new QLineEdit(dialog) };
        txtUrl->setPlaceholderText("Enter url here");
        txtUrl->setText(QString::fromStdString(credential->getUri()));
        QLabel* lblUsername{ new QLabel(dialog) };
        lblUsername->setText(_("Username"));
        QLineEdit* txtUsername{ new QLineEdit(dialog) };
        txtUsername->setPlaceholderText("Enter username here");
        txtUsername->setText(QString::fromStdString(credential->getUsername()));
        QLabel* lblPassword{ new QLabel(dialog) };
        lblPassword->setText(_("Password"));
        QLineEdit* txtPassword{ new QLineEdit(dialog) };
        txtPassword->setPlaceholderText("Enter password here");
        txtPassword->setEchoMode(QLineEdit::Password);
        txtPassword->setText(QString::fromStdString(credential->getPassword()));
        QPushButton* btnSave{ new QPushButton(dialog) };
        btnSave->setAutoDefault(true);
        btnSave->setDefault(true);
        btnSave->setIcon(QLEMENTINE_ICON(Action_Save));
        btnSave->setText(_("Save"));
        QPushButton* btnDelete{ new QPushButton(dialog) };
        btnDelete->setAutoDefault(false);
        btnDelete->setDefault(false);
        btnDelete->setIcon(QLEMENTINE_ICON(Action_Trash));
        btnDelete->setText(_("Delete"));
        QFormLayout* layoutForm{ new QFormLayout() };
        layoutForm->addRow(lblName, txtName);
        layoutForm->addRow(lblUrl, txtUrl);
        layoutForm->addRow(lblUsername, txtUsername);
        layoutForm->addRow(lblPassword, txtPassword);
        QHBoxLayout* layoutButtons{ new QHBoxLayout() };
        layoutButtons->addWidget(btnDelete);
        layoutButtons->addWidget(btnSave);
        QVBoxLayout* layout{ new QVBoxLayout() };
        layout->addLayout(layoutForm);
        layout->addLayout(layoutButtons);
        dialog->setLayout(layout);
        connect(btnSave, &QPushButton::clicked, [&]()
        {
            CredentialCheckStatus status{ m_controller->updateCredential(txtName->text().toStdString(), txtUrl->text().toStdString(), txtUsername->text().toStdString(), txtPassword->text().toStdString()) };
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
                dialog->close();
            }
        });
        connect(btnDelete, &QPushButton::clicked, [&]()
        {
            QMessageBox msgBox{ QMessageBox::Icon::Warning, _("Delete Credential?"), _("Are you sure you want to delete this credential?"), QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, this };
            if(msgBox.exec() == QMessageBox::StandardButton::Yes)
            {
                m_controller->deleteCredential(txtName->text().toStdString());
                dialog->close();
            }
        });
        dialog->exec();
        reloadCredentials();
    }

    void KeyringDialog::onListCredentialsContextMenu(const QPoint& pos)
    {
        QListWidgetItem* selected;
        if((selected = m_ui->listCredentials->itemAt(pos)))
        {

            QMenu menu{ this };
            menu.addAction(QLEMENTINE_ICON(Misc_Pen), _("Edit Credential"), [this, selected]()
            {
                editCredential(selected->text());
            });
            menu.addAction(QLEMENTINE_ICON(Action_Trash), _("Delete Credential"), [this, selected]()
            {
                QMessageBox msgBox{ QMessageBox::Icon::Warning, _("Delete Credential?"), _("Are you sure you want to delete this credential?"), QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, this };
                if(msgBox.exec() == QMessageBox::StandardButton::Yes)
                {
                    m_controller->deleteCredential(selected->text().toStdString());
                    reloadCredentials();
                }
            });
            menu.exec(mapToGlobal(pos));
        }
    }

    void KeyringDialog::onCredentialDoubleClicked(QListWidgetItem* item)
    {
        editCredential(item->text());
    }

    void KeyringDialog::reloadCredentials()
    {
        m_ui->listCredentials->clear();
        std::vector<Credential> credentials{ m_controller->getCredentials() };
        for(const Credential& credential : credentials)
        {
            QListWidgetItem* item{ new QListWidgetItem(QString::fromStdString(credential.getName()), m_ui->listCredentials) };
            m_ui->listCredentials->addItem(item);
        }
        m_ui->viewStack->setCurrentIndex(credentials.size() > 0 ? KeyringDialogPage::Credentials : KeyringDialogPage::NoCredentials);
    }
}
