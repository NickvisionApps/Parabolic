#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMainWindow>
#include <oclero/qlementine/style/ThemeManager.hpp>
#include "controllers/mainwindowcontroller.h"

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
         * @brief Prompts the user to add a download.
         * @param url A url to fill in the dialog with
         */
        void addDownload(const std::string& url = "");
        Ui::MainWindow* m_ui;
        std::shared_ptr<Shared::Controllers::MainWindowController> m_controller;
        oclero::qlementine::ThemeManager* m_themeManager;
    };
}

#endif //MAINWINDOW_H
