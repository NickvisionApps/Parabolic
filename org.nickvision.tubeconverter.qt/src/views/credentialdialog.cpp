#include "views/credentialdialog.h"
#include "ui_credentialdialog.h"
#include <format>
#include <QMessageBox>
#include <libnick/localization/gettext.h>
#include "helpers/qthelpers.h"

using namespace Nickvision::TubeConverter::Shared::Controllers;
using namespace Nickvision::TubeConverter::Qt::Helpers;

namespace Nickvision::TubeConverter::Qt::Views
{
    CredentialDialog::CredentialDialog(const std::shared_ptr<CredentialDialogController>& controller, QWidget* parent)
        : m_controller{ controller },
        m_ui{ new Ui::CredentialDialog() }
    {
        m_ui->setupUi(this);
        setWindowTitle(_("Credential Needed"));
        //Localize Strings
        m_ui->lblMessage->setText(QString::fromStdString(std::vformat(_("{} needs a credential to download. Please select or enter one to use."), std::make_format_args(m_controller->getUrl()))));
        m_ui->lblCredential->setText(_("Credential"));
        m_ui->lblUsername->setText(_("Username"));
        m_ui->txtUsername->setPlaceholderText(_("Enter username here"));
        m_ui->lblPassword->setText(_("Password"));
        m_ui->txtPassword->setPlaceholderText(_("Enter password here"));
        m_ui->btnUse->setText(_("Use"));
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