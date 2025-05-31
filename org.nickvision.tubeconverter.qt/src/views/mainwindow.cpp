#include "views/mainwindow.h"
#include <QAction>
#include <QCheckBox>
#include <QDesktopServices>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScrollArea>
#include <QStackedWidget>
#include <QTabWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <libnick/localization/gettext.h>
#include <oclero/qlementine/widgets/ActionButton.hpp>
#include "controls/aboutdialog.h"
#include "controls/historypane.h"
#include "controls/infobar.h"
#include "controls/logpane.h"
#include "controls/statuspage.h"
#include "helpers/qthelpers.h"
#include "views/adddownloaddialog.h"
#include "views/credentialdialog.h"
#include "views/keyringdialog.h"
#include "views/settingsdialog.h"

using namespace Nickvision::App;
using namespace Nickvision::TubeConverter::Qt::Controls;
using namespace Nickvision::TubeConverter::Qt::Helpers;
using namespace Nickvision::TubeConverter::Shared::Controllers;
using namespace Nickvision::TubeConverter::Shared::Events;
using namespace Nickvision::TubeConverter::Shared::Models;
using namespace Nickvision::Events;
using namespace Nickvision::Notifications;
using namespace Nickvision::Update;
using namespace oclero::qlementine;

enum MainWindowPage
{
    Home = 0,
    Downloading,
    Queued,
    Completed
};

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
            historyPane->hide();
            parent->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, historyPane);
            //LogPane
            logPane = new Nickvision::TubeConverter::Qt::Controls::LogPane(parent);
            logPane->hide();
            parent->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, logPane);
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
            actionLog = logPane->toggleViewAction();
            actionLog->setText(_("Log"));
            actionLog->setIcon(QLEMENTINE_ICON(File_FileScript));
            actionLog->setShortcut(QKeyCombination(Qt::ControlModifier | Qt::ShiftModifier, Qt::Key_L));
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
            actionClearHistory = new QAction(parent);
            actionClearHistory->setText(_("Clear History"));
            actionClearHistory->setIcon(QLEMENTINE_ICON(Action_Trash));
            actionCheckForUpdates = new QAction(parent);
            actionCheckForUpdates->setText(_("Check for Updates"));
            actionCheckForUpdates->setIcon(QLEMENTINE_ICON(Action_Update));
            actionDocumentation = new QAction(parent);
            actionDocumentation->setText(_("Documentation"));
            actionDocumentation->setIcon(QLEMENTINE_ICON(Misc_Library));
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
            menuView->addSeparator();
            menuView->addAction(actionLog);
            QMenu* menuDownloader{ new QMenu(parent) };
            menuDownloader->setTitle(_("Downloader"));
            menuDownloader->addAction(actionStopAllDownloads);
            menuDownloader->addAction(actionRetryFailedDownloads);
            menuDownloader->addSeparator();
            menuDownloader->addAction(actionClearQueuedDownloads);
            menuDownloader->addAction(actionClearCompletedDownloads);
            menuDownloader->addSeparator();
            menuDownloader->addAction(actionClearHistory);
            QMenu* menuHelp{ new QMenu(parent) };
            menuHelp->setTitle(_("Help"));
            menuHelp->addAction(actionCheckForUpdates);
            menuHelp->addSeparator();
            menuHelp->addAction(actionDocumentation);
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
            listDownloading = new QVBoxLayout();
            listDownloading->setContentsMargins(0, 0, 0, 0);
            listDownloading->addStretch();
            QWidget* widgetDownloading{ new QWidget(parent) };
            widgetDownloading->setLayout(listDownloading);
            QScrollArea* scrollDownloading{ new QScrollArea(parent) };
            scrollDownloading->setWidgetResizable(true);
            scrollDownloading->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
            scrollDownloading->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
            scrollDownloading->setWidget(widgetDownloading);
            downloadingViewStack = new QStackedWidget(parent);
            downloadingViewStack->addWidget(noDownloadingPage);
            downloadingViewStack->addWidget(scrollDownloading);
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
            listQueued = new QVBoxLayout();
            listQueued->setContentsMargins(0, 0, 0, 0);
            listQueued->addStretch();
            QWidget* widgetQueued{ new QWidget(parent) };
            widgetQueued->setLayout(listQueued);
            QScrollArea* scrollQueued{ new QScrollArea(parent) };
            scrollQueued->setWidgetResizable(true);
            scrollQueued->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
            scrollQueued->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
            scrollQueued->setWidget(widgetQueued);
            queuedViewStack = new QStackedWidget(parent);
            queuedViewStack->addWidget(noQueuedPage);
            queuedViewStack->addWidget(scrollQueued);
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
            listCompleted = new QVBoxLayout();
            listCompleted->setContentsMargins(0, 0, 0, 0);
            listCompleted->addStretch();
            QWidget* widgetCompleted{ new QWidget(parent) };
            widgetCompleted->setLayout(listCompleted);
            QScrollArea* scrollCompleted{ new QScrollArea(parent) };
            scrollCompleted->setWidgetResizable(true);
            scrollCompleted->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
            scrollCompleted->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
            scrollCompleted->setWidget(widgetCompleted);
            completedViewStack = new QStackedWidget(parent);
            completedViewStack->addWidget(noCompletedPage);
            completedViewStack->addWidget(scrollCompleted);
            completedViewStack->setCurrentIndex(DownloadPage::None);
            //Tabs
            tabs = new QTabWidget(parent);
            tabs->addTab(homePage, QLEMENTINE_ICON(Navigation_Home), _("Home"));
            tabs->addTab(downloadingViewStack, QLEMENTINE_ICON(Action_Save), _("Downloading (0)"));
            tabs->addTab(queuedViewStack, QLEMENTINE_ICON(Misc_Hourglass), _("Queued (0)"));
            tabs->addTab(completedViewStack, QLEMENTINE_ICON(Shape_CheckTick), _("Completed (0)"));
            tabs->setCurrentIndex(MainWindowPage::Home);
            parent->setCentralWidget(tabs);
        }

        QAction* actionAddDownload;
        QAction* actionExit;
        QAction* actionKeyring;
        QAction* actionSettings;
        QAction* actionHistory;
        QAction* actionLog;
        QAction* actionStopAllDownloads;
        QAction* actionRetryFailedDownloads;
        QAction* actionClearQueuedDownloads;
        QAction* actionClearCompletedDownloads;
        QAction* actionClearHistory;
        QAction* actionCheckForUpdates;
        QAction* actionDocumentation;
        QAction* actionGitHubRepo;
        QAction* actionReportABug;
        QAction* actionDiscussions;
        QAction* actionAbout;
        Nickvision::TubeConverter::Qt::Controls::InfoBar* infoBar;
        Nickvision::TubeConverter::Qt::Controls::HistoryPane* historyPane;
        Nickvision::TubeConverter::Qt::Controls::LogPane* logPane;
        QStackedWidget* downloadingViewStack;
        QVBoxLayout* listDownloading;
        QStackedWidget* queuedViewStack;
        QVBoxLayout* listQueued;
        QStackedWidget* completedViewStack;
        QVBoxLayout* listCompleted;
        QTabWidget* tabs;
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
        connect(m_ui->actionStopAllDownloads, &QAction::triggered, this, &MainWindow::stopAllDownloads);
        connect(m_ui->actionRetryFailedDownloads, &QAction::triggered, this, &MainWindow::retryFailedDownloads);
        connect(m_ui->actionClearQueuedDownloads, &QAction::triggered, this, &MainWindow::clearQueuedDownloads);
        connect(m_ui->actionClearCompletedDownloads, &QAction::triggered, this, &MainWindow::clearCompletedDownloads);
        connect(m_ui->actionClearHistory, &QAction::triggered, this, &MainWindow::clearHistory);
        connect(m_ui->actionCheckForUpdates, &QAction::triggered, this, &MainWindow::checkForUpdates);
        connect(m_ui->actionDocumentation, &QAction::triggered, this, &MainWindow::documentation);
        connect(m_ui->actionGitHubRepo, &QAction::triggered, this, &MainWindow::gitHubRepo);
        connect(m_ui->actionReportABug, &QAction::triggered, this, &MainWindow::reportABug);
        connect(m_ui->actionDiscussions, &QAction::triggered, this, &MainWindow::discussions);
        connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
        connect(m_ui->historyPane, &HistoryPane::downloadAgain, [this](const std::string& url){ addDownload(url); });
        connect(m_ui->historyPane, &HistoryPane::deleteItem, [this](const HistoricDownload& download){ m_controller->getDownloadManager().removeHistoricDownload(download); });
        m_controller->notificationSent() += [this](const NotificationSentEventArgs& args) { QtHelpers::dispatchToMainThread([this, args]() { onNotificationSent(args); }); };
        m_controller->getDownloadManager().historyChanged() += [&](const ParamEventArgs<std::vector<HistoricDownload>>& args) { QtHelpers::dispatchToMainThread([this, args]() { onHistoryChanged(args); }); };
        m_controller->getDownloadManager().downloadCredentialNeeded() += [&](const DownloadCredentialNeededEventArgs& args) { onDownloadCredentialNeeded(args); };
        m_controller->getDownloadManager().downloadAdded() += [&](const DownloadAddedEventArgs& args) { QtHelpers::dispatchToMainThread([this, args]() { onDownloadAdded(args); }); };
        m_controller->getDownloadManager().downloadCompleted() += [&](const DownloadCompletedEventArgs& args) { QtHelpers::dispatchToMainThread([this, args]() { onDownloadCompleted(args); }); };
        m_controller->getDownloadManager().downloadProgressChanged() += [&](const DownloadProgressChangedEventArgs& args) { QtHelpers::dispatchToMainThread([this, args]() { onDownloadProgressChanged(args); }); };
        m_controller->getDownloadManager().downloadStopped() += [&](const ParamEventArgs<int>& args) { QtHelpers::dispatchToMainThread([this, args]() { onDownloadStopped(args); }); };
        m_controller->getDownloadManager().downloadPaused() += [&](const ParamEventArgs<int>& args) { QtHelpers::dispatchToMainThread([this, args]() { onDownloadPaused(args); }); };
        m_controller->getDownloadManager().downloadResumed() += [&](const ParamEventArgs<int>& args) { QtHelpers::dispatchToMainThread([this, args]() { onDownloadResumed(args); }); };
        m_controller->getDownloadManager().downloadRetried() += [&](const ParamEventArgs<int>& args) { QtHelpers::dispatchToMainThread([this, args]() { onDownloadRetried(args); }); };
        m_controller->getDownloadManager().downloadStartedFromQueue() += [&](const ParamEventArgs<int>& args) { QtHelpers::dispatchToMainThread([this, args]() { onDownloadStartedFromQueue(args); }); };
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
        if(info.getWindowGeometry().isMaximized())
        {
            showMaximized();
        }
        else
        {
#ifdef _WIN32
            info.getWindowGeometry().apply(reinterpret_cast<HWND>(winId()));
#else
            setGeometry(QWidget::geometry().x(), QWidget::geometry().y(), info.getWindowGeometry().getWidth(), info.getWindowGeometry().getHeight());
#endif
        }
        if(info.showDisclaimer())
        {
            QMessageBox msg{ QMessageBox::Icon::Warning, _("Disclaimer"), _("The authors of Nickvision Parabolic are not responsible/liable for any misuse of this program that may violate local copyright/DMCA laws. Users use this application at their own risk."), QMessageBox::StandardButton::Ok, this };
            QCheckBox* chk{ new QCheckBox(_("Don't show this message again"), this) };
            msg.setCheckBox(chk);
            msg.exec();
            m_controller->setShowDisclaimerOnStartup(!chk->isChecked());
        }
        if(!info.getUrlToValidate().empty())
        {
            addDownload(info.getUrlToValidate());
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
#ifdef _WIN32
        m_controller->shutdown({ reinterpret_cast<HWND>(winId()) });
#else
        m_controller->shutdown({ geometry().width(), geometry().height(), isMaximized() });
#endif
        event->accept();
    }

    void MainWindow::changeEvent(QEvent* event)
    {
        if(event->type() == QEvent::WindowStateChange || event->type() == QEvent::ActivationChange || event->type() == QEvent::WindowActivate)
        {
            m_controller->setIsWindowActive(isActiveWindow());
        }
        QMainWindow::changeEvent(event);
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
            DownloadRow* row{ m_downloadRows[id] };
            QFrame* line{ m_downloadLines[id] };
            m_ui->listQueued->removeWidget(row);
            m_ui->listQueued->removeWidget(line);
            m_downloadRows.erase(id);
            m_downloadLines.erase(id);
            delete row;
            delete line;
        }
        m_ui->queuedViewStack->setCurrentIndex(DownloadPage::None);
        m_ui->tabs->setTabText(MainWindowPage::Queued, _("Queued (0)"));
    }

    void MainWindow::clearCompletedDownloads()
    {
        for(int id : m_controller->getDownloadManager().clearCompletedDownloads())
        {
            DownloadRow* row{ m_downloadRows[id] };
            QFrame* line{ m_downloadLines[id] };
            m_ui->listCompleted->removeWidget(row);
            m_ui->listCompleted->removeWidget(line);
            m_downloadRows.erase(id);
            m_downloadLines.erase(id);
            delete row;
            delete line;
        }
        m_ui->completedViewStack->setCurrentIndex(DownloadPage::None);
        m_ui->tabs->setTabText(MainWindowPage::Completed, _("Completed (0)"));
    }

    void MainWindow::clearHistory()
    {
        m_controller->getDownloadManager().clearHistory();
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

    void MainWindow::onNotificationSent(const NotificationSentEventArgs& args)
    {
        QString actionText;
        std::function<void()> actionCallback;
        if(args.getAction() == "error")
        {
            QMessageBox::critical(this, _("Error"), QString::fromStdString(args.getMessage()));
            return;
        }
#ifdef _WIN32
        else if(args.getAction() == "update")
        {
            actionText = _("Update");
            actionCallback = [this]() { windowsUpdate(); };
        }
#endif
        m_ui->infoBar->show(args, actionText, actionCallback);
    }

    void MainWindow::onHistoryChanged(const ParamEventArgs<std::vector<HistoricDownload>>& args)
    {
        m_ui->historyPane->update(*args);
    }

    void MainWindow::onDownloadCredentialNeeded(const DownloadCredentialNeededEventArgs& args)
    {
        CredentialDialog dialog{ m_controller->createCredentialDialogController(args), this };
        dialog.exec();
    }

    void MainWindow::onDownloadAdded(const DownloadAddedEventArgs& args)
    {
        DownloadRow* row{ new DownloadRow(args, this) };
        QFrame* line{ QtHelpers::createHLine(this) };
        connect(row, &DownloadRow::showLog, [this](int id)
        {
            m_ui->logPane->setId(id);
            m_ui->logPane->update(QString::fromStdString(m_controller->getDownloadManager().getDownloadLog(id)));
            m_ui->logPane->show();
        });
        connect(row, &DownloadRow::stop, [this](int id) { m_controller->getDownloadManager().stopDownload(id); });
        connect(row, &DownloadRow::pause, [this](int id) { m_controller->getDownloadManager().pauseDownload(id); });
        connect(row, &DownloadRow::resume, [this](int id) { m_controller->getDownloadManager().resumeDownload(id); });
        connect(row, &DownloadRow::retry, [this](int id) { m_controller->getDownloadManager().retryDownload(id); });
        m_downloadRows[args.getId()] = row;
        m_downloadLines[args.getId()] = line;
        if(args.getStatus() == DownloadStatus::Queued)
        {
            m_ui->listQueued->insertWidget(0, row);
            m_ui->listQueued->insertWidget(1, line);
            m_ui->queuedViewStack->setCurrentIndex(m_controller->getDownloadManager().getQueuedCount() > 0 ? DownloadPage::Has : DownloadPage::None);
            if(m_ui->tabs->currentIndex() == MainWindowPage::Home)
            {
                m_ui->tabs->setCurrentIndex(MainWindowPage::Queued);
            }
            m_ui->tabs->setTabText(MainWindowPage::Queued, QString::fromStdString(_f("Queued ({})", m_controller->getDownloadManager().getQueuedCount())));
        }
        else
        {
            m_ui->listDownloading->insertWidget(0, row);
            m_ui->listDownloading->insertWidget(1, line);
            m_ui->downloadingViewStack->setCurrentIndex(m_controller->getDownloadManager().getDownloadingCount() > 0 ? DownloadPage::Has : DownloadPage::None);
            if(m_ui->tabs->currentIndex() == MainWindowPage::Home)
            {
                m_ui->tabs->setCurrentIndex(MainWindowPage::Downloading);
            }
            m_ui->tabs->setTabText(MainWindowPage::Downloading, QString::fromStdString(_f("Downloading ({})", m_controller->getDownloadManager().getDownloadingCount())));
        }
    }

    void MainWindow::onDownloadCompleted(const DownloadCompletedEventArgs& args)
    {
        DownloadRow* row{ m_downloadRows[args.getId()] };
        QFrame* line{ m_downloadLines[args.getId()] };
        row->setCompleteState(args);
        m_ui->listDownloading->removeWidget(row);
        m_ui->listDownloading->removeWidget(line);
        m_ui->listCompleted->insertWidget(0, row);
        m_ui->listCompleted->insertWidget(1, line);
        m_ui->downloadingViewStack->setCurrentIndex(m_controller->getDownloadManager().getDownloadingCount() > 0 ? DownloadPage::Has : DownloadPage::None);
        m_ui->completedViewStack->setCurrentIndex(m_controller->getDownloadManager().getCompletedCount() > 0 ? DownloadPage::Has : DownloadPage::None);
        m_ui->tabs->setTabText(MainWindowPage::Downloading, QString::fromStdString(_f("Downloading ({})", m_controller->getDownloadManager().getDownloadingCount())));
        m_ui->tabs->setTabText(MainWindowPage::Completed, QString::fromStdString(_f("Completed ({})", m_controller->getDownloadManager().getCompletedCount())));
    }

    void MainWindow::onDownloadProgressChanged(const DownloadProgressChangedEventArgs& args)
    {
        m_downloadRows[args.getId()]->setProgressState(args);
        if(m_ui->logPane->id() == args.getId())
        {
            m_ui->logPane->update(QString::fromStdString(args.getLog()));
        }
    }

    void MainWindow::onDownloadStopped(const ParamEventArgs<int>& args)
    {
        DownloadRow* row{ m_downloadRows[*args] };
        QFrame* line{ m_downloadLines[*args] };
        row->setStopState();
        m_ui->listDownloading->removeWidget(row);
        m_ui->listDownloading->removeWidget(line);
        m_ui->listQueued->removeWidget(row);
        m_ui->listQueued->removeWidget(line);
        m_ui->listCompleted->insertWidget(0, row);
        m_ui->listCompleted->insertWidget(1, line);
        m_ui->downloadingViewStack->setCurrentIndex(m_controller->getDownloadManager().getDownloadingCount() > 0 ? DownloadPage::Has : DownloadPage::None);
        m_ui->queuedViewStack->setCurrentIndex(m_controller->getDownloadManager().getQueuedCount() > 0 ? DownloadPage::Has : DownloadPage::None);
        m_ui->completedViewStack->setCurrentIndex(m_controller->getDownloadManager().getCompletedCount() > 0 ? DownloadPage::Has : DownloadPage::None);
        m_ui->tabs->setTabText(MainWindowPage::Downloading, QString::fromStdString(_f("Downloading ({})", m_controller->getDownloadManager().getDownloadingCount())));
        m_ui->tabs->setTabText(MainWindowPage::Queued, QString::fromStdString(_f("Queued ({})", m_controller->getDownloadManager().getQueuedCount())));
        m_ui->tabs->setTabText(MainWindowPage::Completed, QString::fromStdString(_f("Completed ({})", m_controller->getDownloadManager().getCompletedCount())));
    }

    void MainWindow::onDownloadPaused(const ParamEventArgs<int>& args)
    {
        m_downloadRows[*args]->setPauseState();
    }

    void MainWindow::onDownloadResumed(const ParamEventArgs<int>& args)
    {
        m_downloadRows[*args]->setResumeState();
    }

    void MainWindow::onDownloadRetried(const ParamEventArgs<int>& args)
    {
        DownloadRow* row{ m_downloadRows[*args] };
        QFrame* line{ m_downloadLines[*args] };
        m_ui->listCompleted->removeWidget(row);
        m_ui->listCompleted->removeWidget(line);
        m_ui->downloadingViewStack->setCurrentIndex(m_controller->getDownloadManager().getDownloadingCount() > 0 ? DownloadPage::Has : DownloadPage::None);
        m_ui->tabs->setTabText(MainWindowPage::Downloading, QString::fromStdString(_f("Downloading ({})", m_controller->getDownloadManager().getDownloadingCount())));
        m_downloadRows.erase(*args);
        m_downloadLines.erase(*args);
        delete row;
        delete line;
    }

    void MainWindow::onDownloadStartedFromQueue(const ParamEventArgs<int>& args)
    {
        DownloadRow* row{ m_downloadRows[*args] };
        QFrame* line{ m_downloadLines[*args] };
        row->setStartFromQueueState();
        m_ui->listQueued->removeWidget(row);
        m_ui->listQueued->removeWidget(line);
        m_ui->listDownloading->insertWidget(0, row);
        m_ui->listDownloading->insertWidget(1, line);
        m_ui->queuedViewStack->setCurrentIndex(m_controller->getDownloadManager().getQueuedCount() > 0 ? DownloadPage::Has : DownloadPage::None);
        m_ui->downloadingViewStack->setCurrentIndex(m_controller->getDownloadManager().getDownloadingCount() > 0 ? DownloadPage::Has : DownloadPage::None);
        m_ui->tabs->setTabText(MainWindowPage::Queued, QString::fromStdString(_f("Queued ({})", m_controller->getDownloadManager().getQueuedCount())));
        m_ui->tabs->setTabText(MainWindowPage::Completed, QString::fromStdString(_f("Completed ({})", m_controller->getDownloadManager().getCompletedCount())));
    }

    void MainWindow::addDownload(const std::string& url)
    {
        AddDownloadDialog dialog{ m_controller->createAddDownloadDialogController(), url, this };
        dialog.exec();
    }
}
