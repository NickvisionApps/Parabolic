#include "views/settingsdialog.h"
#include "ui_settingsdialog.h"
#include <libnick/localization/gettext.h>

using namespace Nickvision::TubeConverter::Shared::Controllers;
using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::QT::Views
{
    SettingsDialog::SettingsDialog(const std::shared_ptr<PreferencesViewController>& controller, QWidget* parent)
        : QDialog{ parent },
        m_ui{ new Ui::SettingsDialog() },
        m_controller{ controller }
    {
        m_ui->setupUi(this);
        setWindowTitle(_("Settings"));
        //Localize Strings
        m_ui->listPages->addItem(_("User Interface"));
        m_ui->chkUpdates->setText(_("Automatically Check for Updates"));
        //Load Settings
        m_ui->listPages->setCurrentRow(0);
        m_ui->chkUpdates->setChecked(m_controller->getAutomaticallyCheckForUpdates());
        //Signals
        connect(m_ui->listPages, &QListWidget::currentRowChanged, this, &SettingsDialog::onPageChanged);
    }
    
    SettingsDialog::~SettingsDialog()
    {
        delete m_ui;
    }

    void SettingsDialog::closeEvent(QCloseEvent* event)
    {
        m_controller->setAutomaticallyCheckForUpdates(m_ui->chkUpdates->isChecked());
        event->accept();
    }

    void SettingsDialog::onPageChanged(int index)
    {
        m_ui->viewStack->setCurrentIndex(index);
    }
}