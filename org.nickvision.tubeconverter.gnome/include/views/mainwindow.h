#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <vector>
#include <adwaita.h>
#include "controls/downloadrow.h"
#include "controllers/mainwindowcontroller.h"
#include "helpers/builder.h"
#include "helpers/controlptr.h"

namespace Nickvision::TubeConverter::GNOME::Views
{
    /**
     * @brief The main window for the application. 
     */
    class MainWindow
    {
    public:
        /**
         * @brief Constructs a MainWindow.
         * @param controller The MainWindowController
         * @param app The GtkApplication object of the running app 
         */
        MainWindow(const std::shared_ptr<Shared::Controllers::MainWindowController>& controller, GtkApplication* app);
        /**
         * @brief Destructs the MainWindow. 
         */
        ~MainWindow();
        /**
         * @brief Shows the main window. 
         */
        void show();
        /**
         * @brief Opens the application's add download dialog.
         * @param url An optional url to start download validation with
         */
        void addDownload(const std::string& url = "");

    private:
        /**
         * @brief Handles when the window requests to close.
         * @return True to prevent closing, else false 
         */
        bool onCloseRequested();
        /**
         * @brief Handles when the window's visibility is changed.
         */
        void onVisibilityChanged();
        /**
         * @brief Handles when a notification is sent to the window.
         * @param args NotificationSentEventArgs 
         */
        void onNotificationSent(const Notifications::NotificationSentEventArgs& args);
        /**
         * @brief Handles when a navigation item is selected.
         * @param box The listNavItems box
         * @param row The selected row
         */
        void onNavItemSelected(GtkListBox* box, GtkListBoxRow* row);
        /**
         * @brief Handles when the history is changed.
         * @param args ParamEventArgs<std::vector<Models::HistoricDownload>>
         */
        void onHistoryChanged(const Events::ParamEventArgs<std::vector<Shared::Models::HistoricDownload>>& args);
        /**
         * @brief Handles when a recovered download needs a credential. 
         * @param args DownloadCredentialNeededEventArgs
         */
        void onDownloadCredentialNeeded(const Shared::Events::DownloadCredentialNeededEventArgs& args);
        /**
         * @brief Handles when a download is added.
         * @param args DownloadAddedEventArgs
         */
        void onDownloadAdded(const Shared::Events::DownloadAddedEventArgs& args);
        /**
         * @brief Handles when a download is completed.
         * @param args DownloadCompletedEventArgs
         */
        void onDownloadCompleted(const Shared::Events::DownloadCompletedEventArgs& args);
        /**
         * @brief Handles when a download's progress is changed.
         * @param args DownloadProgressChangedEventArgs
         */
        void onDownloadProgressChanged(const Shared::Events::DownloadProgressChangedEventArgs& args);
        /**
         * @brief Handles when a download is stopped.
         * @param args ParamEventArgs<int>
         */
        void onDownloadStopped(const Events::ParamEventArgs<int>& args);
        /**
         * @brief Handles when a download is retried.
         * @param args ParamEventArgs<int>
         */
        void onDownloadRetried(const Events::ParamEventArgs<int>& args);
        /**
         * @brief Handles when a download is started from the queue.
         * @param args ParamEventArgs<int>
         */
        void onDownloadStartedFromQueue(const Events::ParamEventArgs<int>& args);
        /**
         * @brief Quits the application. 
         */
        void quit();
        /**
         * @brief Opens the application's preferences dialog. 
         */
        void preferences();
        /**
         * @brief Opens the application's keyboard shortcut dialog.
         */
        void keyboardShortcuts();
        /**
         * @brief Opens the application's help documentation. 
         */
        void help();
        /**
         * @brief Opens the application's about dialog. 
         */
        void about();
        /**
         * @brief Clears the download history.
         */
        void clearHistory();
        /**
         * @brief Stops all downloads.
         */
        void stopAllDownloads();
        /**
         * @brief Clears all queued downloads.
         */
        void clearQueuedDownloads();
        /**
         * @brief Clears all completed downloads.
         */
        void clearCompletedDownloads();
        /**
         * @brief Retries all failed downloads.
         */
        void retryFailedDownloads();
        std::shared_ptr<Shared::Controllers::MainWindowController> m_controller;
        GtkApplication* m_app;
        Helpers::Builder m_builder;
        AdwApplicationWindow* m_window;
        GSimpleAction* m_actAddDownload;
        std::vector<AdwActionRow*> m_historyRows;
        std::unordered_map<int, Helpers::ControlPtr<Controls::DownloadRow>> m_downloadRows;
    };
}

#endif //MAINWINDOW_H
