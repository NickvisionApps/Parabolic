#include "views/settingsdialog.h"
#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QStyleHints>
#include <libnick/localization/gettext.h>
#include <oclero/qlementine/widgets/Switch.hpp>
#include "helpers/qthelpers.h"

using namespace Nickvision::TubeConverter::Shared::Controllers;
using namespace Nickvision::TubeConverter::Shared::Models;
using namespace oclero::qlementine;

namespace Ui
{
    class SettingsDialog
    {
    public:
        void setupUi(Nickvision::TubeConverter::Qt::Views::SettingsDialog* parent)
        {
            viewStack = new QStackedWidget(parent);
            //User Interface Page
            QLabel* lblTheme{ new QLabel(parent) };
            lblTheme->setText(_("Theme"));
            cmbTheme = new QComboBox(parent);
            cmbTheme->addItem(_("Light"));
            cmbTheme->addItem(_("Dark"));
            cmbTheme->addItem(_("System"));
            QLabel* lblUpdates{ new QLabel(parent) };
            lblUpdates->setText(_("Automatically Check for Updates"));
            chkUpdates = new Switch(parent);
            QLabel* lblCompletedNotificationTrigger{ new QLabel(parent) };
            lblCompletedNotificationTrigger->setText(_("Completed Notification Trigger"));
            cmbCompletedNotificationTrigger = new QComboBox(parent);
            cmbCompletedNotificationTrigger->addItem(_("For each download"));
            cmbCompletedNotificationTrigger->addItem(_("When all downloads finish"));
            cmbCompletedNotificationTrigger->addItem(_("Never"));
            QLabel* lblPreventSuspend{ new QLabel(parent) };
            lblPreventSuspend->setText(_("Prevent Suspend"));
            chkPreventSuspend = new Switch(parent);
            QLabel* lblRecoverCrashedDownloads{ new QLabel(parent) };
            lblRecoverCrashedDownloads->setText(_("Recover Crashed Downloads"));
            chkRecoverCrashedDownloads = new Switch(parent);
            QLabel* lblDownloadImmediately{ new QLabel(parent) };
            lblDownloadImmediately->setText(_("Download Immediately After Validation"));
            chkDownloadImmediately = new Switch(parent);
            QLabel* lblHistoryLength{ new QLabel(parent) };
            lblHistoryLength->setText(_("Download History Length"));
            cmbHistoryLength = new QComboBox(parent);
            cmbHistoryLength->addItem(_("Never"));
            cmbHistoryLength->addItem(_("One Day"));
            cmbHistoryLength->addItem(_("One Week"));
            cmbHistoryLength->addItem(_("One Month"));
            cmbHistoryLength->addItem(_("Three Months"));
            cmbHistoryLength->addItem(_("Forever"));
            QFormLayout* layoutUserInterface{ new QFormLayout() };
            layoutUserInterface->addRow(lblTheme, cmbTheme);
            layoutUserInterface->addRow(lblUpdates, chkUpdates);
            layoutUserInterface->addRow(lblCompletedNotificationTrigger, cmbCompletedNotificationTrigger);
            layoutUserInterface->addRow(lblPreventSuspend, chkPreventSuspend);
            layoutUserInterface->addRow(lblRecoverCrashedDownloads, chkRecoverCrashedDownloads);
            layoutUserInterface->addRow(lblDownloadImmediately, chkDownloadImmediately);
            layoutUserInterface->addRow(lblHistoryLength, cmbHistoryLength);
            QWidget* userInterfacePage{ new QWidget(parent) };
            userInterfacePage->setLayout(layoutUserInterface);
            viewStack->addWidget(userInterfacePage);
            //Navigation List
            listNavigation = new QListWidget(parent);
            listNavigation->setMaximumWidth(160);
            listNavigation->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
            listNavigation->setDropIndicatorShown(false);
            listNavigation->addItem(new QListWidgetItem(QLEMENTINE_ICON(Navigation_UiPanelLeft), _("User Interface"), listNavigation));
            listNavigation->addItem(new QListWidgetItem(QLEMENTINE_ICON(Misc_ItemsList), _("Downloads"), listNavigation));
            listNavigation->addItem(new QListWidgetItem(QLEMENTINE_ICON(Action_Download), _("Downloader"), listNavigation));
            listNavigation->addItem(new QListWidgetItem(QLEMENTINE_ICON(Misc_Tool), _("Converter"), listNavigation));
            listNavigation->addItem(new QListWidgetItem(QLEMENTINE_ICON(Software_CommandLine), _("aria2"), listNavigation));
            QObject::connect(listNavigation, &QListWidget::currentRowChanged, [this]()
            {
                viewStack->setCurrentIndex(listNavigation->currentRow());
            });
            //Main Layout
            QHBoxLayout* layout{ new QHBoxLayout() };
            layout->addWidget(listNavigation);
            layout->addWidget(viewStack);
            parent->setLayout(layout);
        }

        QListWidget* listNavigation;
        QStackedWidget* viewStack;
        QComboBox* cmbTheme;
        Switch* chkUpdates;
        QComboBox* cmbCompletedNotificationTrigger;
        Switch* chkPreventSuspend;
        Switch* chkRecoverCrashedDownloads;
        Switch* chkDownloadImmediately;
        QComboBox* cmbHistoryLength;
    };
}

namespace Nickvision::TubeConverter::Qt::Views
{
    SettingsDialog::SettingsDialog(const std::shared_ptr<PreferencesViewController>& controller, oclero::qlementine::ThemeManager* themeManager, QWidget* parent)
        : QDialog{ parent },
        m_ui{ new Ui::SettingsDialog() },
        m_controller{ controller },
        m_themeManager{ themeManager }
    {
        //Dialog Settings
        setWindowTitle(_("Settings"));
        setMinimumSize(600, 400);
        setModal(true);
        //Load Ui
        m_ui->setupUi(this);
        DownloaderOptions options{ m_controller->getDownloaderOptions() };
        m_ui->cmbTheme->setCurrentIndex(static_cast<int>(m_controller->getTheme()));
        m_ui->chkUpdates->setChecked(m_controller->getAutomaticallyCheckForUpdates());
        m_ui->cmbCompletedNotificationTrigger->setCurrentIndex(static_cast<int>(m_controller->getCompletedNotificationPreference()));
        m_ui->chkPreventSuspend->setChecked(m_controller->getPreventSuspend());
        m_ui->chkRecoverCrashedDownloads->setChecked(m_controller->getRecoverCrashedDownloads());
        m_ui->chkDownloadImmediately->setChecked(m_controller->getDownloadImmediatelyAfterValidation());
        m_ui->cmbHistoryLength->setCurrentIndex(static_cast<int>(m_controller->getHistoryLengthIndex()));
        m_ui->listNavigation->setCurrentRow(0);
        //Signals
        connect(m_ui->cmbTheme, &QComboBox::currentIndexChanged, this, &SettingsDialog::onThemeChanged);
    }

    SettingsDialog::~SettingsDialog()
    {
        delete m_ui;
    }

    void SettingsDialog::closeEvent(QCloseEvent* event)
    {
        DownloaderOptions options{ m_controller->getDownloaderOptions() };
        m_controller->setTheme(static_cast<Shared::Models::Theme>(m_ui->cmbTheme->currentIndex()));
        m_controller->setAutomaticallyCheckForUpdates(m_ui->chkUpdates->isChecked());
        m_controller->setCompletedNotificationPreference(static_cast<CompletedNotificationPreference>(m_ui->cmbCompletedNotificationTrigger->currentIndex()));
        m_controller->setPreventSuspend(m_ui->chkPreventSuspend->isChecked());
        m_controller->setRecoverCrashedDownloads(m_ui->chkRecoverCrashedDownloads->isChecked());
        m_controller->setDownloadImmediatelyAfterValidation(m_ui->chkDownloadImmediately->isChecked());
        m_controller->setHistoryLengthIndex(m_ui->cmbHistoryLength->currentIndex());
        m_controller->setDownloaderOptions(options);
        m_controller->saveConfiguration();
        event->accept();
    }

    void SettingsDialog::onThemeChanged()
    {
        switch (static_cast<Shared::Models::Theme>(m_ui->cmbTheme->currentIndex()))
        {
        case Shared::Models::Theme::Light:
            QApplication::styleHints()->setColorScheme(::Qt::ColorScheme::Light);
            m_themeManager->setCurrentTheme("Light");
            break;
        case Shared::Models::Theme::Dark:
            QApplication::styleHints()->setColorScheme(::Qt::ColorScheme::Dark);
            m_themeManager->setCurrentTheme("Dark");
            break;
        default:
            QApplication::styleHints()->unsetColorScheme();
            m_themeManager->setCurrentTheme(QApplication::styleHints()->colorScheme() == ::Qt::ColorScheme::Light ? "Light" : "Dark");
            break;
        }
    }
}
