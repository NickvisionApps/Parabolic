#include "views/mainwindow.h"
#include <QAction>
#include <QDesktopServices>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <libnick/helpers/codehelpers.h>
#include <libnick/localization/gettext.h>
#include <oclero/qlementine/widgets/ActionButton.hpp>
#include "controls/aboutdialog.h"
#include "controls/historypane.h"
#include "controls/infobar.h"
#include "controls/statuspage.h"
#include "helpers/qthelpers.h"
#include "views/adddownloaddialog.h"
#include "views/keyringdialog.h"
#include "views/settingsdialog.h"

using namespace Nickvision::App;
using namespace Nickvision::TubeConverter::Qt::Controls;
using namespace Nickvision::TubeConverter::Qt::Helpers;
using namespace Nickvision::TubeConverter::Shared::Controllers;
using namespace Nickvision::TubeConverter::Shared::Models;
using namespace Nickvision::Events;
using namespace Nickvision::Helpers;
using namespace Nickvision::Notifications;
using namespace Nickvision::Update;
using namespace oclero::qlementine;

enum DownloadPage
{
    None = 0,
    Has
};

namespace Ui
{
    class MainWindow
    {
    public:
        void setupUi(Nickvision::TubeConverter::Qt::Views::MainWindow* parent)
        {
            //HistoryPane
            historyPane = new Nickvision::TubeConverter::Qt::Controls::HistoryPane(parent);
            parent->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, historyPane);
            historyPane->hide();
            //Actions
            actionAddDownload = new QAction(parent);
            actionAddDownload->setText(_("Add Download"));
            actionAddDownload->setIcon(QLEMENTINE_ICON(Action_Plus));
            actionAddDownload->setShortcut(Qt::CTRL | Qt::Key_N);
            actionExit = new QAction(parent);
            actionExit->setText(_("Exit"));
            actionExit->setIcon(QLEMENTINE_ICON(Action_Close));
            actionExit->setShortcut(Qt::CTRL | Qt::Key_Q);
            actionKeyring = new QAction(parent);
            actionKeyring->setText(_("Keyring"));
            actionKeyring->setIcon(QLEMENTINE_ICON(Misc_Password));
            actionKeyring->setShortcut(Qt::CTRL | Qt::Key_K);
            actionSettings = new QAction(parent);
            actionSettings->setText(_("Settings"));
            actionSettings->setIcon(QLEMENTINE_ICON(Navigation_Settings));
            actionSettings->setShortcut(Qt::CTRL | Qt::Key_Comma);
            actionHistory = historyPane->toggleViewAction();
            actionHistory->setText(_("History"));
            actionHistory->setIcon(QLEMENTINE_ICON(Misc_Clock));
            actionHistory->setShortcut(Qt::CTRL | Qt::Key_H);
            actionStopAllDownloads = new QAction(parent);
            actionStopAllDownloads->setText(_("Stop All Downloads"));
            actionStopAllDownloads->setIcon(QLEMENTINE_ICON(Media_Stop));
            actionRetryFailedDownloads = new QAction(parent);
            actionRetryFailedDownloads->setText(_("Retry Failed Downloads"));
            actionRetryFailedDownloads->setIcon(QLEMENTINE_ICON(Action_Refresh));
            actionClearQueuedDownloads = new QAction(parent);
            actionClearQueuedDownloads->setText(_("Clear Queued Downloads"));
            actionClearQueuedDownloads->setIcon(QLEMENTINE_ICON(Action_Trash));
            actionClearCompletedDownloads = new QAction(parent);
            actionClearCompletedDownloads->setText(_("Clear Completed Downloads"));
            actionClearCompletedDownloads->setIcon(QLEMENTINE_ICON(Action_Trash));
            actionCheckForUpdates = new QAction(parent);
            actionCheckForUpdates->setText(_("Check for Updates"));
            actionCheckForUpdates->setIcon(QLEMENTINE_ICON(Action_Update));
            actionGitHubRepo = new QAction(parent);
            actionGitHubRepo->setText(_("GitHub Repo"));
            actionGitHubRepo->setIcon(QLEMENTINE_ICON(Software_VersionControl));
            actionReportABug = new QAction(parent);
            actionReportABug->setText(_("Report a Bug"));
            actionReportABug->setIcon(QLEMENTINE_ICON(Misc_Debug));
            actionDiscussions = new QAction(parent);
            actionDiscussions->setText(_("Discussions"));
            actionDiscussions->setIcon(QLEMENTINE_ICON(Misc_Users));
            actionAbout = new QAction(parent);
            actionAbout->setText(_("About Parabolic"));
            actionAbout->setIcon(QLEMENTINE_ICON(Misc_Info));
            actionAbout->setShortcut(Qt::Key_F1);
            //InfoBar
            infoBar = new Nickvision::TubeConverter::Qt::Controls::InfoBar(parent);
            parent->addDockWidget(::Qt::BottomDockWidgetArea, infoBar);
            //MenuBar
            QMenu* menuFile{ new QMenu(parent) };
            menuFile->setTitle(_("File"));
            menuFile->addAction(actionAddDownload);
            menuFile->addSeparator();
            menuFile->addAction(actionExit);
            QMenu* menuEdit{ new QMenu(parent) };
            menuEdit->setTitle(_("Edit"));
            menuEdit->addAction(actionKeyring);
            menuEdit->addSeparator();
            menuEdit->addAction(actionSettings);
            QMenu* menuView{ new QMenu(parent) };
            menuView->setTitle(_("View"));
            menuView->addAction(actionHistory);
            QMenu* menuDownloader{ new QMenu(parent) };
            menuDownloader->setTitle(_("Downloader"));
            menuDownloader->addAction(actionStopAllDownloads);
            menuDownloader->addAction(actionRetryFailedDownloads);
            menuDownloader->addSeparator();
            menuDownloader->addAction(actionClearQueuedDownloads);
            menuDownloader->addAction(actionClearCompletedDownloads);
            QMenu* menuHelp{ new QMenu(parent) };
            menuHelp->setTitle(_("Help"));
            menuHelp->addAction(actionCheckForUpdates);
            menuHelp->addSeparator();
            menuHelp->addAction(actionGitHubRepo);
            menuHelp->addAction(actionReportABug);
            menuHelp->addAction(actionDiscussions);
            menuHelp->addSeparator();
            menuHelp->addAction(actionAbout);
            parent->menuBar()->addMenu(menuFile);
            parent->menuBar()->addMenu(menuEdit);
            parent->menuBar()->addMenu(menuView);
            parent->menuBar()->addMenu(menuDownloader);
            parent->menuBar()->addMenu(menuHelp);
            //ToolBar
            QToolBar* toolBar{ new QToolBar(parent) };
            toolBar->setAllowedAreas(::Qt::ToolBarArea::TopToolBarArea);
            toolBar->setMovable(false);
            toolBar->setFloatable(false);
            toolBar->addAction(actionAddDownload);
            toolBar->addAction(actionKeyring);
            toolBar->addSeparator();
            toolBar->addAction(actionStopAllDownloads);
            toolBar->addAction(actionRetryFailedDownloads);
            toolBar->addSeparator();
            toolBar->addAction(actionSettings);
            parent->addToolBar(toolBar);
            //Home Page
            ActionButton* btnHomeAddDownload{ new ActionButton(parent) };
            btnHomeAddDownload->setAutoDefault(true);
            btnHomeAddDownload->setDefault(true);
            btnHomeAddDownload->setAction(actionAddDownload);
            Nickvision::TubeConverter::Qt::Controls::StatusPage* homePage{ new Nickvision::TubeConverter::Qt::Controls::StatusPage(parent) };
            homePage->setIcon(QIcon(":/icon-symbolic.svg"));
            homePage->setTitle(_("Download Media"));
            homePage->setDescription(_("Add a video, audio, or playlist URL to start downloading"));
            homePage->addWidget(btnHomeAddDownload);
            //Downloading Page
            ActionButton* btnDownloadingAddDownload{ new ActionButton(parent) };
            btnDownloadingAddDownload->setAutoDefault(true);
            btnDownloadingAddDownload->setDefault(true);
            btnDownloadingAddDownload->setAction(actionAddDownload);
            Nickvision::TubeConverter::Qt::Controls::StatusPage* noDownloadingPage{ new Nickvision::TubeConverter::Qt::Controls::StatusPage(parent) };
            noDownloadingPage->setIcon(QLEMENTINE_ICON(Misc_EmptySlot));
            noDownloadingPage->setTitle(_("No Downloads Running"));
            noDownloadingPage->setDescription(_("Add a video, audio, or playlist URL to start downloading"));
            noDownloadingPage->addWidget(btnDownloadingAddDownload);
            downloadingViewStack = new QStackedWidget(parent);
            downloadingViewStack->addWidget(noDownloadingPage);
            downloadingViewStack->setCurrentIndex(DownloadPage::None);
            //Queued Page
            ActionButton* btnQueuedAddDownload{ new ActionButton(parent) };
            btnQueuedAddDownload->setAutoDefault(true);
            btnQueuedAddDownload->setDefault(true);
            btnQueuedAddDownload->setAction(actionAddDownload);
            Nickvision::TubeConverter::Qt::Controls::StatusPage* noQueuedPage{ new Nickvision::TubeConverter::Qt::Controls::StatusPage(parent) };
            noQueuedPage->setIcon(QLEMENTINE_ICON(Misc_EmptySlot));
            noQueuedPage->setTitle(_("No Downloads Queued"));
            noQueuedPage->setDescription(_("Add a video, audio, or playlist URL to start downloading"));
            noQueuedPage->addWidget(btnQueuedAddDownload);
            queuedViewStack = new QStackedWidget(parent);
            queuedViewStack->addWidget(noQueuedPage);
            queuedViewStack->setCurrentIndex(DownloadPage::None);
            //Completed Page
            ActionButton* btnCompletedAddDownload{ new ActionButton(parent) };
            btnCompletedAddDownload->setAutoDefault(true);
            btnCompletedAddDownload->setDefault(true);
            btnCompletedAddDownload->setAction(actionAddDownload);
            Nickvision::TubeConverter::Qt::Controls::StatusPage* noCompletedPage{ new Nickvision::TubeConverter::Qt::Controls::StatusPage(parent) };
            noCompletedPage->setIcon(QLEMENTINE_ICON(Misc_EmptySlot));
            noCompletedPage->setTitle(_("No Downloads Completed"));
            noCompletedPage->setDescription(_("Add a video, audio, or playlist URL to start downloading"));
            noCompletedPage->addWidget(btnCompletedAddDownload);
            completedViewStack = new QStackedWidget(parent);
            completedViewStack->addWidget(noCompletedPage);
            completedViewStack->setCurrentIndex(DownloadPage::None);
            //Tabs
            QTabWidget* tabs{ new QTabWidget(parent) };
            tabs->addTab(homePage, QLEMENTINE_ICON(Navigation_Home), _("Home"));
            tabs->addTab(downloadingViewStack, QLEMENTINE_ICON(Action_Save), _("Downloading"));
            tabs->addTab(queuedViewStack, QLEMENTINE_ICON(Misc_Hourglass), _("Queued"));
            tabs->addTab(completedViewStack, QLEMENTINE_ICON(Shape_CheckTick), _("Completed"));
            tabs->setCurrentIndex(0);
            parent->setCentralWidget(tabs);
        }

        QAction* actionAddDownload;
        QAction* actionExit;
        QAction* actionKeyring;
        QAction* actionSettings;
        QAction* actionHistory;
        QAction* actionStopAllDownloads;
        QAction* actionRetryFailedDownloads;
        QAction* actionClearQueuedDownloads;
        QAction* actionClearCompletedDownloads;
        QAction* actionCheckForUpdates;
        QAction* actionGitHubRepo;
        QAction* actionReportABug;
        QAction* actionDiscussions;
        QAction* actionAbout;
        Nickvision::TubeConverter::Qt::Controls::InfoBar* infoBar;
        Nickvision::TubeConverter::Qt::Controls::HistoryPane* historyPane;
        QStackedWidget* downloadingViewStack;
        QStackedWidget* queuedViewStack;
        QStackedWidget* completedViewStack;
    };
}

namespace Nickvision::TubeConverter::Qt::Views
{
    MainWindow::MainWindow(const std::shared_ptr<MainWindowController>& controller, oclero::qlementine::ThemeManager* themeManager, QWidget* parent)
        : QMainWindow{ parent },
        m_ui{ new Ui::MainWindow() },
        m_controller{ controller },
        m_themeManager{ themeManager }
    {
        //Window Settings
        bool stable{ m_controller->getAppInfo().getVersion().getVersionType() == VersionType::Stable };
        setWindowTitle(stable ? _("Parabolic") : _("Parabolic (Preview)"));
        setWindowIcon(QIcon(":/icon.ico"));
        //Load Ui
        m_ui->setupUi(this);
        //Signals
        connect(m_ui->actionAddDownload, &QAction::triggered, [this]() { addDownload(); });
        connect(m_ui->actionExit, &QAction::triggered, this, &MainWindow::close);
        connect(m_ui->actionKeyring, &QAction::triggered, this, &MainWindow::keyring);
        connect(m_ui->actionSettings, &QAction::triggered, this, &MainWindow::settings);
        connect(m_ui->actionCheckForUpdates, &QAction::triggered, this, &MainWindow::checkForUpdates);
        connect(m_ui->actionGitHubRepo, &QAction::triggered, this, &MainWindow::gitHubRepo);
        connect(m_ui->actionReportABug, &QAction::triggered, this, &MainWindow::reportABug);
        connect(m_ui->actionDiscussions, &QAction::triggered, this, &MainWindow::discussions);
        connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
        m_controller->notificationSent() += [this](const NotificationSentEventArgs& args) { QtHelpers::dispatchToMainThread([this, args]() { onNotificationSent(args); }); };
    }

    MainWindow::~MainWindow()
    {
        delete m_ui;
    }

    void MainWindow::show()
    {
        QMainWindow::show();
#ifdef _WIN32
        const StartupInformation& info{ m_controller->startup(reinterpret_cast<HWND>(winId())) };
#elif defined(__linux__)
        const StartupInformation& info{ m_controller->startup(m_controller->getAppInfo().getId() + ".desktop") };
#else
        const StartupInformation& info{ m_controller->startup() };
#endif
        setGeometry(QWidget::geometry().x(), QWidget::geometry().y(), info.getWindowGeometry().getWidth(), info.getWindowGeometry().getHeight());
        if(info.getWindowGeometry().isMaximized())
        {
            showMaximized();
        }
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

    void MainWindow::keyring()
    {
        KeyringDialog dialog{ m_controller->createKeyringDialogController(), this };
        dialog.exec();
    }

    void MainWindow::settings()
    {
        SettingsDialog dialog{ m_controller->createPreferencesViewController(), m_themeManager, this };
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
        std::string extraDebug;
        extraDebug += "Qt " + std::string(qVersion()) + "\n";
        AboutDialog dialog{ m_controller->getAppInfo(), m_controller->getDebugInformation(extraDebug), this };
        dialog.exec();
    }

    void MainWindow::onNotificationSent(const NotificationSentEventArgs& args)
    {
        QString actionText;
        std::function<void()> actionCallback;
#ifdef _WIN32
        if(args.getAction() == "update")
        {
            actionText = _("Update");
            actionCallback = [this]() { windowsUpdate(); };
        }
#endif
        m_ui->infoBar->show(args, actionText, actionCallback);
    }

    void MainWindow::addDownload(const std::string& url)
    {
        AddDownloadDialog dialog{ m_controller->createAddDownloadDialogController(), url, this };
        dialog.exec();
    }
}
