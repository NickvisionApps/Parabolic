#include "views/mainwindow.h"
#include "ui_mainwindow.h"
#include <QCheckBox>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <libnick/helpers/codehelpers.h>
#include <libnick/localization/gettext.h>
#include <libnick/notifications/shellnotification.h>
#include "controls/aboutdialog.h"
#include "views/adddownloaddialog.h"
#include "views/settingsdialog.h"

using namespace Nickvision::App;
using namespace Nickvision::Events;
using namespace Nickvision::Helpers;
using namespace Nickvision::Notifications;
using namespace Nickvision::TubeConverter::Shared::Controllers;
using namespace Nickvision::TubeConverter::Shared::Models;
using namespace Nickvision::TubeConverter::QT::Controls;
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
        m_ui->actionGitHubRepo->setText(_("GitHub Repo"));
        m_ui->actionReportABug->setText(_("Report a Bug"));
        m_ui->actionDiscussions->setText(_("Discussions"));
        m_ui->actionAbout->setText(_("About Parabolic"));
        //Localize Home Page
        m_ui->lblHomeGreeting->setText(_("Download Media"));
        m_ui->lblHomeDescription->setText(_("Add a video, audio, or playlist URL to start downloading"));
        m_ui->btnHomeAddDownload->setText(_("Add Download"));
        //Localize History Dock
        m_ui->lblHistory->setText(_("History"));
        m_ui->lblNoHistory->setText(_("No history available"));
        m_ui->btnClearHistory->setText(_("Clear"));
        m_ui->tblHistory->setHorizontalHeaderLabels({ "", _("Title"), "", "" });
        //Signals
        connect(m_ui->actionAddDownload, &QAction::triggered, this, &MainWindow::addDownload);
        connect(m_ui->actionExit, &QAction::triggered, this, &MainWindow::exit);
        connect(m_ui->actionSettings, &QAction::triggered, this, &MainWindow::settings);
        connect(m_ui->actionHistory, &QAction::triggered, this, &MainWindow::history);
        connect(m_ui->actionCheckForUpdates, &QAction::triggered, this, &MainWindow::checkForUpdates);
        connect(m_ui->actionGitHubRepo, &QAction::triggered, this, &MainWindow::gitHubRepo);
        connect(m_ui->actionReportABug, &QAction::triggered, this, &MainWindow::reportABug);
        connect(m_ui->actionDiscussions, &QAction::triggered, this, &MainWindow::discussions);
        connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
        connect(m_ui->btnHomeAddDownload, &QPushButton::clicked, this, &MainWindow::addDownload);
        connect(m_ui->btnClearHistory, &QPushButton::clicked, this, &MainWindow::clearHistory);
        m_controller->notificationSent() += [&](const NotificationSentEventArgs& args) { QMetaObject::invokeMethod(this, [this, args]() { onNotificationSent(args); }, Qt::QueuedConnection); };
        m_controller->shellNotificationSent() += [&](const ShellNotificationSentEventArgs& args) { onShellNotificationSent(args); };
        m_controller->disclaimerTriggered() += [&](const ParamEventArgs<std::string>& args) { QMetaObject::invokeMethod(this, [this, args]() { onDisclaimerTriggered(args); }, Qt::QueuedConnection); };
        m_controller->downloadAbilityChanged() += [&](const ParamEventArgs<bool>& args) { onDownloadAbilityChanged(args); };
        m_controller->historyChanged() += [&](const ParamEventArgs<std::vector<HistoricDownload>>& args) { onHistoryChanged(args); };
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
    }

    void MainWindow::closeEvent(QCloseEvent* event)
    {
        if(!m_controller->canShutdown())
        {
            return event->ignore();
        }
        m_controller->shutdown({ geometry().width(), geometry().height(), isMaximized() });
        event->accept();
    }

    void MainWindow::addDownload()
    {
        AddDownloadDialog dialog{ m_controller->createAddDownloadDialogController(), this };
        dialog.exec();
    }

    void MainWindow::exit()
    {
        close();
    }

    void MainWindow::history()
    {
        m_ui->dockHistory->show();
    }

    void MainWindow::settings()
    {
        SettingsDialog dialog{ m_controller->createPreferencesViewController(), this };
        dialog.exec();
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
        AboutDialog dialog{ m_controller->getAppInfo(), m_controller->getDebugInformation(), this };
        dialog.exec();
    }

    void MainWindow::clearHistory()
    {
        m_ui->dockHistory->hide();
        m_controller->clearHistory();
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
        if(args.getAction() == "update")
        {
            QPushButton* updateButton{ msgBox.addButton(_("Update"), QMessageBox::ButtonRole::ActionRole) };
            connect(updateButton, &QPushButton::clicked, this, &MainWindow::checkForUpdates);
        }
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

    void MainWindow::onDownloadAbilityChanged(const ParamEventArgs<bool>& args)
    {
        m_ui->actionAddDownload->setEnabled(args.getParam());
        m_ui->btnHomeAddDownload->setEnabled(args.getParam());
    }

    void MainWindow::onHistoryChanged(const ParamEventArgs<std::vector<HistoricDownload>>& args)
    {
        m_ui->viewStackHistory->setCurrentIndex(args.getParam().empty() ? 0 : 1);
        m_ui->tblHistory->setRowCount(0);
        int i{ 0 };
        for(const HistoricDownload& download : args.getParam())
        {
            m_ui->tblHistory->insertRow(i);
            m_ui->tblHistory->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(download.getTitle())));
            //Delete Button
            QPushButton* btnDelete{ new QPushButton(QIcon::fromTheme(QIcon::ThemeIcon::EditDelete), {}, m_ui->tblHistory) };
            btnDelete->setToolTip(_("Delete"));
            connect(btnDelete, &QPushButton::clicked, [this, download]() { m_controller->removeHistoricDownload(download); });
            m_ui->tblHistory->setCellWidget(i, 0, btnDelete);
            //Download Button
            QPushButton* btnDownload{ new QPushButton(QIcon::fromTheme(QIcon::ThemeIcon::GoDown), {}, m_ui->tblHistory) };
            btnDownload->setToolTip(_("Download"));
            m_ui->tblHistory->setCellWidget(i, 2, btnDownload);
            //TODO: Download
            //Play Button
            if(std::filesystem::exists(download.getPath()))
            {
                QPushButton* btnPlay{ new QPushButton(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStart), {}, m_ui->tblHistory) };
                btnPlay->setToolTip(_("Play"));
                connect(btnPlay, &QPushButton::clicked, [this, download]() { QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(download.getPath().string()))); });
                m_ui->tblHistory->setCellWidget(i, 3, btnPlay);
            }
            i++;
        }
        m_ui->tblHistory->resizeColumnToContents(0);
        m_ui->tblHistory->resizeColumnToContents(2);
        m_ui->tblHistory->resizeColumnToContents(3);
    }
}