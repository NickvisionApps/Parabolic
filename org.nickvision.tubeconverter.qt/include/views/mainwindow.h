#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QCloseEvent>
#include <QEvent>
#include <QMainWindow>
#include <oclero/qlementine/style/ThemeManager.hpp>
#include "controllers/mainwindowcontroller.h"
#include "controls/downloadrow.h"

namespace Ui { class MainWindow; }

namespace Nickvision::TubeConverter::Qt::Views
{
    /**
     * @brief The main window for the application.
     */
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        /**
         * @brief Constructs a MainWindow.
         * @param controller The MainWindowController
         * @param themeManager The ThemeManager
         * @param parent The parent widget
         */
        MainWindow(const std::shared_ptr<Shared::Controllers::MainWindowController>& controller, oclero::qlementine::ThemeManager* themeManager, QWidget* parent = nullptr);
        /**
         * @brief Destructs a MainWindow.
         */
        ~MainWindow();
        /**
         * @brief Shows the MainWindow.
         */
        void show();

    protected:
        /**
         * @brief Handles when the window is closed.
         * @param event QCloseEvent
         */
        void closeEvent(QCloseEvent* event) override;
        /**
         * @brief Handles when the widget changes.
         * @param event QEvent
         */
        void changeEvent(QEvent* event) override;

    private Q_SLOTS:
        /**
         * @brief Opens the application's keyring dialog.
         */
        void keyring();
        /**
         * @brief Opens the application's settings dialog.
         */
        void settings();
        /**
         * @brief Stops all downloads that are queued or in progress.
         */
        void stopAllDownloads();
        /**
         * @brief Retries all downloads that have failed.
         */
        void retryFailedDownloads();
        /**
         * @brief Clears all downloads that are queued.
         */
        void clearQueuedDownloads();
        /**
         * @brief Clears all downloads that have failed.
         */
        void clearCompletedDownloads();
        /**
         * @brief Checks for application updates.
         */
        void checkForUpdates();
#ifdef _WIN32
        /**
         * @brief Downloads and installs the latest application update in the background.
         */
        void windowsUpdate();
#endif
        /**
         * @brief Opens the application's GitHub repo in the browser.
         */
        void gitHubRepo();
        /**
         * @brief Opens the application's issue tracker in the browser.
         */
        void reportABug();
        /**
         * @brief Opens the application's discussions page in the browser.
         */
        void discussions();
        /**
         * @brief Displays the about dialog.
         */
        void about();

    private:
        /**
         * @brief Handles when a notification is sent.
         * @param args The NotificationSentEventArgs
         */
        void onNotificationSent(const Notifications::NotificationSentEventArgs& args);
        /**
         * @brief Handles when a download is added.
         * @param args The DownloadAddedEventArgs
         */
        void onDownloadAdded(const Shared::Events::DownloadAddedEventArgs& args);
        /**
         * @brief Handles when a download is completed.
         * @param args The DownloadCompletedEventArgs
         */
        void onDownloadCompleted(const Shared::Events::DownloadCompletedEventArgs& args);
        /**
         * @brief Handles when a download's progress is changed.
         * @param args The DownloadProgressChangedEventArgs
         */
        void onDownloadProgressChanged(const Shared::Events::DownloadProgressChangedEventArgs& args);
        /**
         * @brief Handles when a download is stopped.
         * @param args The ParamEventArgs<int>
         */
        void onDownloadStopped(const Events::ParamEventArgs<int>& args);
        /**
         * @brief Handles when a download is retried.
         * @param args The ParamEventArgs<int>
         */
        void onDownloadRetried(const Events::ParamEventArgs<int>& args);
        /**
         * @brief Handles when a download is started from the queue.
         * @param args The ParamEventArgs<int>
         */
        void onDownloadStartedFromQueue(const Events::ParamEventArgs<int>& args);
        /**
         * @brief Prompts the user to add a download.
         * @param url A url to fill in the dialog with
         */
        void addDownload(const std::string& url = "");
        Ui::MainWindow* m_ui;
        std::shared_ptr<Shared::Controllers::MainWindowController> m_controller;
        oclero::qlementine::ThemeManager* m_themeManager;
        std::unordered_map<int, Controls::DownloadRow*> m_downloadRows;
    };
}

#endif //MAINWINDOW_H
