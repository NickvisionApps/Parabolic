#include "views/credentialdialog.h"
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <libnick/localization/gettext.h>
#include <oclero/qlementine/widgets/LineEdit.hpp>
#include "helpers/qthelpers.h"

using namespace Nickvision::TubeConverter::Shared::Controllers;
using namespace Nickvision::TubeConverter::Qt::Helpers;
using namespace oclero::qlementine;

namespace Ui
{
    class CredentialDialog
    {
    public:
        void setupUi(Nickvision::TubeConverter::Qt::Views::CredentialDialog* parent, const std::string& url)
        {
            QLabel* lblMessage{ new QLabel(parent) };
            lblMessage->setText(QString::fromStdString(_f("{} needs a credential to download. Please select or enter one to use.", url)));
            QLabel* lblCredential{ new QLabel(parent) };
            lblCredential->setText(_("Credential"));
            cmbCredential = new QComboBox(parent);
            lblUsername = new QLabel(parent);
            lblUsername->setText(_("Username"));
            txtUsername = new LineEdit(parent);
            txtUsername->setPlaceholderText(_("Enter username here"));
            lblPassword = new QLabel(parent);
            lblPassword->setText(_("Password"));
            txtPassword = new LineEdit(parent);
            txtPassword->setPlaceholderText(_("Enter password here"));
            btnUse = new QPushButton(parent);
            btnUse->setIcon(QLEMENTINE_ICON(Shape_CheckTick));
            btnUse->setText(_("Use"));
            QFormLayout* form{ new QFormLayout() };
            form->addRow(lblCredential, cmbCredential);
            form->addRow(lblUsername, txtUsername);
            form->addRow(lblPassword, txtPassword);
            QVBoxLayout* layout{ new QVBoxLayout() };
            layout->addWidget(lblMessage);
            layout->addLayout(form);
            layout->addStretch();
            layout->addWidget(btnUse);
            parent->setLayout(layout);
        }

        QComboBox* cmbCredential;
        QLabel* lblUsername;
        LineEdit* txtUsername;
        QLabel* lblPassword;
        LineEdit* txtPassword;
        QPushButton* btnUse;
    };
}

namespace Nickvision::TubeConverter::Qt::Views
{
    CredentialDialog::CredentialDialog(const std::shared_ptr<CredentialDialogController>& controller, QWidget* parent)
        : m_controller{ controller },
        m_ui{ new Ui::CredentialDialog() }
    {
        setWindowTitle(_("Credential Needed"));
        m_ui->setupUi(this, m_controller->getUrl());
        //Signals
        connect(m_ui->cmbCredential, &QComboBox::currentIndexChanged, this, &CredentialDialog::onCmbCredentialChanged);
        connect(m_ui->btnUse, &QPushButton::clicked, this, &CredentialDialog::use);
        //Load
        std::vector<std::string> credentialNames{ m_controller->getKeyringCredentialNames() };
        credentialNames.insert(credentialNames.begin(), _("Use manual credential"));
        QtHelpers::setComboBoxItems(m_ui->cmbCredential, credentialNames);
    }

    CredentialDialog::~CredentialDialog()
    {
        delete m_ui;
    }

    void CredentialDialog::onCmbCredentialChanged(int index)
    {
        bool show{ index == 0 };
        m_ui->lblUsername->setVisible(show);
        m_ui->txtUsername->setVisible(show);
        m_ui->txtUsername->clear();
        m_ui->lblPassword->setVisible(show);
        m_ui->txtPassword->setVisible(show);
        m_ui->txtPassword->clear();
    }

    void CredentialDialog::use()
    {
        if(m_ui->cmbCredential->currentIndex() == 0)
        {
            if(m_ui->txtUsername->text().isEmpty() && m_ui->txtPassword->text().isEmpty())
            {
                QMessageBox::critical(this, _("Error"), _("Both the username and password cannot be empty."), QMessageBox::Ok);
                return;
            }
            m_controller->use(m_ui->txtUsername->text().toStdString(), m_ui->txtPassword->text().toStdString());
        }
        else
        {
            m_controller->use(m_ui->cmbCredential->currentIndex() - 1);
        }
        close();
    }
}
