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
}