#include "views/keyringdialog.h"
#include "ui_keyringdialog.h"
#include <QMessageBox>
#include <libnick/localization/gettext.h>

using namespace Nickvision::TubeConverter::Shared::Controllers;

namespace Nickvision::TubeConverter::QT::Views
{
    KeyringDialog::KeyringDialog(const std::shared_ptr<KeyringDialogController>& controller, QWidget* parent)
        : QDialog{ parent },
        m_ui{ new Ui::KeyringDialog() },
        m_controller{ controller }
    {
        setWindowTitle(_("Keyring"));
    }

    KeyringDialog::~KeyringDialog()
    {
        delete m_ui;
    }
}