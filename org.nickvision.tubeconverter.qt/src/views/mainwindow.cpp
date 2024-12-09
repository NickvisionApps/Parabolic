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
#include "views/keyringpage.h"
#include "views/settingspage.h"

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
    enum Page
    {
        Home = 0,
        Keyring,
        History,
        Downloading,
        Settings
    };

    MainWindow::MainWindow(const std::shared_ptr<MainWindowController>& controller, QWidget* parent) 
        : QMainWindow{ parent },
        m_ui{ new Ui::MainWindow() },
        m_navigationBar{ new NavigationBar(this) },
        m_controller{ controller }
    {
        m_ui->setupUi(this);
        m_ui->mainLayout->insertLayout(0, m_navigationBar);
        setWindowTitle(m_controller->getAppInfo().getVersion().getVersionType() == VersionType::Stable ? _("Parabolic") : _("Parabolic (Preview)"));
        setAcceptDrops(true);
        //Navigation Bar
        QMenu* helpMenu{ new QMenu(this) };
        helpMenu->addAction(_("Check for Updates"), this, &MainWindow::checkForUpdates);
        helpMenu->addSeparator();
        helpMenu->addAction(_("Documentation"), this, &MainWindow::documentation);
        helpMenu->addAction(_("GitHub Repo"), this, &MainWindow::gitHubRepo);
        helpMenu->addAction(_("Report a Bug"), this, &MainWindow::reportABug);
        helpMenu->addAction(_("Discussions"), this, &MainWindow::discussions);
        helpMenu->addSeparator();
        helpMenu->addAction(_("About"), this, &MainWindow::about);
        m_navigationBar->addTopItem("home", _("Home"), QIcon::fromTheme(QIcon::ThemeIcon::GoHome));
        m_navigationBar->addTopItem("keyring", _("Keyring"), QIcon::fromTheme(QIcon::ThemeIcon::DialogPassword));
        m_navigationBar->addTopItem("history", _("History"), QIcon::fromTheme(QIcon::ThemeIcon::EditFind));
        m_navigationBar->addTopItem("downloading", _("Downloading"), QIcon::fromTheme("emblem-downloads"));
        m_navigationBar->addTopItem("queued", _("Queued"), QIcon::fromTheme(QIcon::ThemeIcon::ListAdd));
        m_navigationBar->addTopItem("completed", _("Completed"), QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave));
        m_navigationBar->addBottomItem("help", _("Help"), QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout), helpMenu);
#ifdef _WIN32
        m_navigationBar->addBottomItem("settings", _("Settings"), QIcon::fromTheme("document-properties"));
#else
        m_navigationBar->addBottomItem("settings", _("Settings"), QIcon::fromTheme(QIcon::ThemeIcon::DocumentProperties));
#endif
        //Localize Home Page
        m_ui->lblHomeGreeting->setText(_("Download Media"));
        m_ui->lblHomeDescription->setText(_("Add a video, audio, or playlist URL to start downloading"));
        m_ui->btnHomeAddDownload->setText(_("Add Download"));
        //Localize History Page
        m_ui->lblNoHistory->setText(_("No history available"));
        m_ui->btnClearHistory->setText(_("Clear"));
        //Localize Downloads Page
        m_ui->lblLog->setText(_("Log"));
        //Signals
        connect(m_navigationBar, &NavigationBar::itemSelected, this, &MainWindow::onNavigationItemSelected);
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
        m_navigationBar->selectItem("home");
        bool canDownload{ m_controller->canDownload() };
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

    void MainWindow::onNavigationItemSelected(const QString& id)
    {
        //Ensure new KeyringPage
        if(m_ui->viewStack->widget(Page::Keyring))
        {
            KeyringPage* oldKeyring{ qobject_cast<KeyringPage*>(m_ui->viewStack->widget(Page::Keyring)) };
            if(oldKeyring)
            {
                m_ui->viewStack->removeWidget(oldKeyring);
                delete oldKeyring;
            }
        }
        m_ui->viewStack->insertWidget(Page::Keyring, new KeyringPage(m_controller->createKeyringDialogController(), this));
        //Save and ensure new SettingsPage
        if(m_ui->viewStack->widget(Page::Settings))
        {
            SettingsPage* oldSettings{ qobject_cast<SettingsPage*>(m_ui->viewStack->widget(Page::Settings)) };
            oldSettings->close();
            m_ui->viewStack->removeWidget(oldSettings);
            delete oldSettings;
        }
        m_ui->viewStack->insertWidget(Page::Settings, new SettingsPage(m_controller->createPreferencesViewController(), this));
        //Navigate to new page
        if(id == "home")
        {
            m_ui->viewStack->setCurrentIndex(Page::Home);
        }
        else if(id == "keyring")
        {
            m_ui->viewStack->setCurrentIndex(Page::Keyring);
        }
        else if(id == "history")
        {
            m_ui->viewStack->setCurrentIndex(Page::History);
        }
        else if(id == "downloading")
        {
            m_ui->viewStack->setCurrentIndex(Page::Downloading);
        }
        else if(id == "queued")
        {
            m_ui->viewStack->setCurrentIndex(Page::Downloading);
        }
        else if(id == "completed")
        {
            m_ui->viewStack->setCurrentIndex(Page::Downloading);
        }
        else if(id == "settings")
        {
            m_ui->viewStack->setCurrentIndex(Page::Settings);
        }
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
            m_navigationBar->selectItem("home");
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
            m_navigationBar->selectItem("home");
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
#else
        ShellNotification::send(args);
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
        m_navigationBar->selectItem("downloading");
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