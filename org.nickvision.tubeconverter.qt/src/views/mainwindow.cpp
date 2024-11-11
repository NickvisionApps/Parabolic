#include "views/mainwindow.h"
#include "ui_mainwindow.h"
#include <QCheckBox>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QProgressBar>
#include <QPushButton>
#include <libnick/helpers/codehelpers.h>
#include <libnick/localization/gettext.h>
#include <libnick/notifications/shellnotification.h>
#include "controls/aboutdialog.h"
#include "controls/historyrow.h"
#include "helpers/qthelpers.h"
#include "views/adddownloaddialog.h"
#include "views/credentialdialog.h"
#include "views/keyringdialog.h"
#include "views/settingsdialog.h"

using namespace Nickvision::App;
using namespace Nickvision::Events;
using namespace Nickvision::Helpers;
using namespace Nickvision::Keyring;
using namespace Nickvision::Notifications;
using namespace Nickvision::TubeConverter::QT::Controls;
using namespace Nickvision::TubeConverter::QT::Helpers;
using namespace Nickvision::TubeConverter::Shared::Controllers;
using namespace Nickvision::TubeConverter::Shared::Events;
using namespace Nickvision::TubeConverter::Shared::Models;
using namespace Nickvision::Update;

namespace Nickvision::TubeConverter::QT::Views
{
    MainWindow::MainWindow(const std::shared_ptr<MainWindowController>& controller, QWidget* parent) 
        : QMainWindow{ parent },
        m_ui{ new Ui::MainWindow() },
        m_controller{ controller }
    {
        m_ui->setupUi(this);
        setWindowTitle(m_controller->getAppInfo().getVersion().getVersionType() == VersionType::Stable ? _("Parabolic") : _("Parabolic (Preview)"));
        setAcceptDrops(true);
        //Localize Menu Strings
        m_ui->menuFile->setTitle(_("File"));
        m_ui->actionAddDownload->setText(_("Add Download"));
        m_ui->actionExit->setText(_("Exit"));
        m_ui->menuEdit->setTitle(_("Edit"));
        m_ui->actionKeyring->setText(_("Keyring"));
        m_ui->actionSettings->setText(_("Settings"));
        m_ui->menuView->setTitle(_("View"));
        m_ui->actionHistory->setText(_("History"));
        m_ui->menuDownloader->setTitle(_("Downloader"));
        m_ui->actionStopAllDownloads->setText(_("Stop All Downloads"));
        m_ui->actionRetryFailedDownloads->setText(_("Retry Failed Downloads"));
        m_ui->actionClearQueuedDownloads->setText(_("Clear Queued Downloads"));
        m_ui->actionClearCompletedDownloads->setText(_("Clear Completed Downloads"));
        m_ui->menuHelp->setTitle(_("Help"));
        m_ui->actionCheckForUpdates->setText(_("Check for Updates"));
        m_ui->actionDocumentation->setText(_("Documentation"));
        m_ui->actionGitHubRepo->setText(_("GitHub Repo"));
        m_ui->actionReportABug->setText(_("Report a Bug"));
        m_ui->actionDiscussions->setText(_("Discussions"));
        m_ui->actionAbout->setText(_("About Parabolic"));
        //Localize Home Page
        m_ui->lblHomeGreeting->setText(_("Download Media"));
        m_ui->lblHomeDescription->setText(_("Add a video, audio, or playlist URL to start downloading"));
        m_ui->btnHomeAddDownload->setText(_("Add Download"));
        //Localize History Dock
        m_ui->dockHistory->setWindowTitle(_("History"));
        m_ui->lblNoHistory->setText(_("No history available"));
        m_ui->btnClearHistory->setText(_("Clear"));
        //Localize Downloads Page
        m_ui->lblLog->setText(_("Log"));
        //Signals
        connect(m_ui->actionAddDownload, &QAction::triggered, [this]() { addDownload(); });
        connect(m_ui->actionExit, &QAction::triggered, this, &MainWindow::exit);
        connect(m_ui->actionKeyring, &QAction::triggered, this, &MainWindow::keyring);
        connect(m_ui->actionSettings, &QAction::triggered, this, &MainWindow::settings);
        connect(m_ui->actionHistory, &QAction::triggered, this, &MainWindow::history);
        connect(m_ui->actionStopAllDownloads, &QAction::triggered, this, &MainWindow::stopAllDownloads);
        connect(m_ui->actionRetryFailedDownloads, &QAction::triggered, this, &MainWindow::retryFailedDownloads);
        connect(m_ui->actionClearQueuedDownloads, &QAction::triggered, this, &MainWindow::clearQueuedDownloads);
        connect(m_ui->actionClearCompletedDownloads, &QAction::triggered, this, &MainWindow::clearCompletedDownloads);
        connect(m_ui->actionCheckForUpdates, &QAction::triggered, this, &MainWindow::checkForUpdates);
        connect(m_ui->actionDocumentation, &QAction::triggered, this, &MainWindow::documentation);
        connect(m_ui->actionGitHubRepo, &QAction::triggered, this, &MainWindow::gitHubRepo);
        connect(m_ui->actionReportABug, &QAction::triggered, this, &MainWindow::reportABug);
        connect(m_ui->actionDiscussions, &QAction::triggered, this, &MainWindow::discussions);
        connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
        connect(m_ui->btnHomeAddDownload, &QPushButton::clicked, [this]() { addDownload(); });
        connect(m_ui->btnClearHistory, &QPushButton::clicked, this, &MainWindow::clearHistory);
        connect(m_ui->listDownloads, &QListWidget::itemSelectionChanged, this, &MainWindow::onListDownloadsSelectionChanged);
        //Events
        m_controller->notificationSent() += [&](const NotificationSentEventArgs& args) { QTHelpers::dispatchToMainThread([this, args]() { onNotificationSent(args); }); };
        m_controller->shellNotificationSent() += [&](const ShellNotificationSentEventArgs& args) { onShellNotificationSent(args); };
        m_controller->disclaimerTriggered() += [&](const ParamEventArgs<std::string>& args) { onDisclaimerTriggered(args); };
        m_controller->getDownloadManager().historyChanged() += [&](const ParamEventArgs<std::vector<HistoricDownload>>& args) { QTHelpers::dispatchToMainThread([this, args]() { onHistoryChanged(args); }); };
        m_controller->getDownloadManager().downloadCredentialNeeded() += [&](const DownloadCredentialNeededEventArgs& args) { onDownloadCredentialNeeded(args); };
        m_controller->getDownloadManager().downloadAdded() += [&](const DownloadAddedEventArgs& args) { QTHelpers::dispatchToMainThread([this, args]() { onDownloadAdded(args); }); };
        m_controller->getDownloadManager().downloadCompleted() += [&](const DownloadCompletedEventArgs& args) { QTHelpers::dispatchToMainThread([this, args]() { onDownloadCompleted(args); }); };
        m_controller->getDownloadManager().downloadProgressChanged() += [&](const DownloadProgressChangedEventArgs& args) { QTHelpers::dispatchToMainThread([this, args]() { onDownloadProgressChanged(args); }); };
        m_controller->getDownloadManager().downloadStopped() += [&](const ParamEventArgs<int>& args) { QTHelpers::dispatchToMainThread([this, args]() { onDownloadStopped(args); }); };
        m_controller->getDownloadManager().downloadRetried() += [&](const ParamEventArgs<int>& args) { QTHelpers::dispatchToMainThread([this, args]() { onDownloadRetried(args); }); };
        m_controller->getDownloadManager().downloadStartedFromQueue() += [&](const ParamEventArgs<int>& args) { QTHelpers::dispatchToMainThread([this, args]() { onDownloadStartedFromQueue(args); }); };
    }

    MainWindow::~MainWindow()
    {
        delete m_ui;
    }

    void MainWindow::show()
    {
        QMainWindow::show();
        m_ui->dockHistory->hide();
        m_ui->viewStack->setCurrentIndex(0);
        m_ui->toolBar->hide();
#ifdef _WIN32
        WindowGeometry geometry{ m_controller->startup(reinterpret_cast<HWND>(winId())) };
#elif defined(__linux__)
        WindowGeometry geometry{ m_controller->startup(m_controller->getAppInfo().getId() + ".desktop") };
#else
        WindowGeometry geometry{ m_controller->startup() };
#endif
        if(geometry.isMaximized())
        {
            showMaximized();
        }
        else
        {
            setGeometry(QWidget::geometry().x(), QWidget::geometry().y(), geometry.getWidth(), geometry.getHeight());
        }
        bool canDownload{ m_controller->canDownload() };
        m_ui->actionAddDownload->setEnabled(canDownload);
        m_ui->btnHomeAddDownload->setEnabled(canDownload);
    }

    void MainWindow::closeEvent(QCloseEvent* event)
    {
        if(!m_controller->canShutdown())
        {
            event->ignore();
            QMessageBox msgBox{ QMessageBox::Icon::Warning, QString::fromStdString(m_controller->getAppInfo().getShortName()), _("There are downloads in progress. Are you sure you want to exit?"), QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, this };
            if(msgBox.exec() == QMessageBox::StandardButton::Yes)
            {
                m_controller->getDownloadManager().stopAllDownloads();
                close();
            }
            return;
        }
        m_controller->shutdown({ geometry().width(), geometry().height(), isMaximized() });
        event->accept();
    }

    void MainWindow::exit()
    {
        close();
    }

    void MainWindow::keyring()
    {
        KeyringDialog dialog{ m_controller->createKeyringDialogController(), this };
        dialog.exec();
    }

    void MainWindow::settings()
    {
        SettingsDialog dialog{ m_controller->createPreferencesViewController(), this };
        dialog.exec();
    }

    void MainWindow::history()
    {
        m_ui->dockHistory->show();
    }

    void MainWindow::checkForUpdates()
    {
        m_controller->checkForUpdates();
    }

#ifdef _WIN32
    void MainWindow::windowsUpdate()
    {
        m_controller->windowsUpdate();
    }
#endif

    void MainWindow::documentation()
    {
        QDesktopServices::openUrl(QString::fromStdString(m_controller->getHelpUrl()));
    }

    void MainWindow::gitHubRepo()
    {
        QDesktopServices::openUrl(QString::fromStdString(m_controller->getAppInfo().getSourceRepo()));
    }

    void MainWindow::reportABug()
    {
        QDesktopServices::openUrl(QString::fromStdString(m_controller->getAppInfo().getIssueTracker()));
    }

    void MainWindow::discussions()
    {
        QDesktopServices::openUrl(QString::fromStdString(m_controller->getAppInfo().getSupportUrl()));
    }

    void MainWindow::about()
    {
        std::string extraDebug;
        extraDebug += "Qt " + std::string(qVersion()) + "\n";
        AboutDialog dialog{ m_controller->getAppInfo(), m_controller->getDebugInformation(extraDebug), this };
        dialog.exec();
    }

    void MainWindow::clearHistory()
    {
        m_ui->dockHistory->hide();
        m_controller->getDownloadManager().clearHistory();
    }

    void MainWindow::stopAllDownloads()
    {
        m_controller->getDownloadManager().stopAllDownloads();
    }

    void MainWindow::retryFailedDownloads()
    {
        m_controller->getDownloadManager().retryFailedDownloads();
    }

    void MainWindow::clearQueuedDownloads()
    {
        for(int id : m_controller->getDownloadManager().clearQueuedDownloads())
        {
            for(int i = 0; i < m_ui->listDownloads->count(); i++)
            {
                if(m_ui->listDownloads->itemWidget(m_ui->listDownloads->item(i)) == m_downloadRows[id])
                {
                    delete m_ui->listDownloads->takeItem(i);
                    m_downloadRows.erase(id);
                    break;
                }
            }
        }
        if(m_downloadRows.empty())
        {
            m_ui->viewStack->setCurrentIndex(0);
            m_ui->toolBar->hide();
        }
    }

    void MainWindow::clearCompletedDownloads()
    {
        for(int id : m_controller->getDownloadManager().clearCompletedDownloads())
        {
            for(int i = 0; i < m_ui->listDownloads->count(); i++)
            {
                if(m_ui->listDownloads->itemWidget(m_ui->listDownloads->item(i)) == m_downloadRows[id])
                {
                    delete m_ui->listDownloads->takeItem(i);
                    m_downloadRows.erase(id);
                    break;
                }
            }
        }
        if(m_downloadRows.empty())
        {
            m_ui->viewStack->setCurrentIndex(0);
            m_ui->toolBar->hide();
        }
    }

    void MainWindow::onListDownloadsSelectionChanged()
    {
        m_ui->lblDownloadLog->setText("");
        QListWidgetItem* item{ m_ui->listDownloads->currentItem() };
        if(!item)
        {
            return;
        }
        DownloadRow* row{ static_cast<DownloadRow*>(m_ui->listDownloads->itemWidget(item)) };
        m_ui->lblDownloadLog->setText(QString::fromStdString(m_controller->getDownloadManager().getDownloadCommand(row->getId())) + "\n" + row->getLog());
    }

    void MainWindow::addDownload(const std::string& url)
    {
        AddDownloadDialog dialog{ m_controller->createAddDownloadDialogController(), url, this };
        dialog.exec();
    }

    void MainWindow::onNotificationSent(const NotificationSentEventArgs& args)
    {
        QMessageBox::Icon icon{ QMessageBox::Icon::NoIcon };
        switch(args.getSeverity())
        {
        case NotificationSeverity::Informational:
        case NotificationSeverity::Success:
            icon = QMessageBox::Icon::Information;
            break;
        case NotificationSeverity::Warning:
            icon = QMessageBox::Icon::Warning;
            break;
        case NotificationSeverity::Error:
            icon = QMessageBox::Icon::Critical;
            break;
        }
        QMessageBox msgBox{ icon, QString::fromStdString(m_controller->getAppInfo().getShortName()), QString::fromStdString(args.getMessage()), QMessageBox::StandardButton::Ok, this };
#ifdef _WIN32
        if(args.getAction() == "update")
        {
            QPushButton* updateButton{ msgBox.addButton(_("Update"), QMessageBox::ButtonRole::ActionRole) };
            connect(updateButton, &QPushButton::clicked, this, &MainWindow::windowsUpdate);
        }
#endif
        msgBox.exec();
    }

    void MainWindow::onShellNotificationSent(const ShellNotificationSentEventArgs& args)
    {
        m_controller->log(Logging::LogLevel::Info, "ShellNotification sent. (" + args.getMessage() + ")");
#ifdef _WIN32
        ShellNotification::send(args, reinterpret_cast<HWND>(winId()));
#elif defined(__linux__)
        ShellNotification::send(args, m_controller->getAppInfo().getId(), _("Open"));
#endif
    }

    void MainWindow::onDisclaimerTriggered(const ParamEventArgs<std::string>& args)
    {
        QMessageBox msgBox{ QMessageBox::Icon::Information, _("Disclaimer"), QString::fromStdString(args.getParam()), QMessageBox::StandardButton::Ok, this };
        QCheckBox* checkBox{ new QCheckBox(_("Don't show this message again"), &msgBox) };
        msgBox.setCheckBox(checkBox);
        msgBox.exec();
        m_controller->setShowDisclaimerOnStartup(!checkBox->isChecked());
    }

    void MainWindow::onHistoryChanged(const ParamEventArgs<std::vector<HistoricDownload>>& args)
    {
        m_ui->viewStackHistory->setCurrentIndex(args.getParam().empty() ? 0 : 1);
        m_ui->listHistory->clear();
        for(const HistoricDownload& download : args.getParam())
        {
            HistoryRow* row{ new HistoryRow(download) };
            connect(row, &HistoryRow::downloadAgain, [this](const std::string& url) { addDownload(url); });
            connect(row, &HistoryRow::deleteItem, [this](const HistoricDownload& download) { m_controller->getDownloadManager().removeHistoricDownload(download); });
            QListWidgetItem* item{ new QListWidgetItem() };
            item->setSizeHint(row->sizeHint() + QSize(0, 10));
            m_ui->listHistory->insertItem(0, item);
            m_ui->listHistory->setItemWidget(item, row);
        }
    }

    void MainWindow::onDownloadCredentialNeeded(const DownloadCredentialNeededEventArgs& args)
    {
        CredentialDialog dialog{ m_controller->createCredentialDialogController(args), this };
        dialog.exec();
    }

    void MainWindow::onDownloadAdded(const DownloadAddedEventArgs& args)
    {
        m_ui->viewStack->setCurrentIndex(1);
        m_ui->toolBar->show();
        DownloadRow* row{ new DownloadRow(args) };
        connect(row, &DownloadRow::stop, [this, id{ args.getId() }]() { m_controller->getDownloadManager().stopDownload(id); });
        connect(row, &DownloadRow::retry, [this, id{ args.getId() }]() { m_controller->getDownloadManager().retryDownload(id); });
        m_downloadRows[args.getId()] = row;
        QListWidgetItem* item{ new QListWidgetItem() };
        item->setSizeHint(row->sizeHint() + QSize(0, 10));
        m_ui->listDownloads->insertItem(0, item);
        m_ui->listDownloads->setItemWidget(item, row);
        m_ui->listDownloads->setCurrentRow(0);
    }

    void MainWindow::onDownloadCompleted(const DownloadCompletedEventArgs& args)
    {
        m_downloadRows[args.getId()]->setCompleteState(args);
        onListDownloadsSelectionChanged();
    }

    void MainWindow::onDownloadProgressChanged(const DownloadProgressChangedEventArgs& args)
    {
        m_downloadRows[args.getId()]->setProgressState(args);
        onListDownloadsSelectionChanged();
    }

    void MainWindow::onDownloadStopped(const ParamEventArgs<int>& args)
    {
        m_downloadRows[args.getParam()]->setStopState();
    }

    void MainWindow::onDownloadRetried(const ParamEventArgs<int>& args)
    {
        for(int i = 0; i < m_ui->listDownloads->count(); i++)
        {
            if(m_ui->listDownloads->itemWidget(m_ui->listDownloads->item(i)) == m_downloadRows[args.getParam()])
            {
                delete m_ui->listDownloads->takeItem(i);
                m_downloadRows.erase(args.getParam());
                break;
            }
        }
    }

    void MainWindow::onDownloadStartedFromQueue(const ParamEventArgs<int>& args)
    {
        m_downloadRows[args.getParam()]->setStartFromQueueState();
    }
}