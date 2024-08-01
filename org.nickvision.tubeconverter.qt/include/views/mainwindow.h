#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMainWindow>
#include "controllers/mainwindowcontroller.h"

namespace Ui { class MainWindow; }

namespace Nickvision::TubeConverter::QT::Views
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
         * @param parent The parent widget
         */
        MainWindow(const std::shared_ptr<Shared::Controllers::MainWindowController>& controller, QWidget* parent = nullptr);
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

    private Q_SLOTS:
        /**
         * @brief Prompts the user to add a download.
         */
        void addDownload();
        /**
         * @brief Exits the application.
         */
        void exit();
        /**
         * @brief Displays the settings dialog.
         */
        void settings();
        /**
         * @brief Displays the history pane.
         */
        void history();
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
        /**
         * @brief Clears the download history.
         */
        void clearHistory();

    private:
        void onNotificationSent(const Notifications::NotificationSentEventArgs& args);
        void onShellNotificationSent(const Notifications::ShellNotificationSentEventArgs& args);
        void onDisclaimerTriggered(const Events::ParamEventArgs<std::string>& args);
        void onDownloadAbilityChanged(const Events::ParamEventArgs<bool>& args);
        void onHistoryChanged(const Events::ParamEventArgs<std::vector<Shared::Models::HistoricDownload>>& args);
        Ui::MainWindow* m_ui;
        std::shared_ptr<Shared::Controllers::MainWindowController> m_controller;
    };
}

#endif //MAINWINDOW_H