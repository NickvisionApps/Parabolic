#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <vector>
#include <adwaita.h>
#include "controllers/mainwindowcontroller.h"

#define SET_ACCEL_FOR_ACTION(App, Action, Accel) { \
const char* accels[2] { Accel, nullptr }; \
gtk_application_set_accels_for_action(App, Action, accels); \
}

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
         * @brief Gets the GObject object for the main window.
         * @return The GObject for the main window 
         */
        GObject* gobj() const;
        /**
         * @brief Shows the main window. 
         */
        void show();

    private:
        /**
         * @brief Handles when the window requests to close.
         * @return True to prevent closing, else false 
         */
        bool onCloseRequested();
        /**
         * @brief Handles when a notification is sent to the window.
         * @param args Nickvision::Notifications::NotificationSentEventArgs 
         */
        void onNotificationSent(const Nickvision::Notifications::NotificationSentEventArgs& args);
        /**
         * @brief Handles when a shell notification is sent to the window.
         * @param args Nickvision::Notifications::ShellNotificationSentEventArgs
         */
        void onShellNotificationSent(const Nickvision::Notifications::ShellNotificationSentEventArgs& args);
        /**
         * @brief Handles when a navigation item is selected.
         * @param box The listNavItems box
         * @param row The selected row
         */
        void onNavItemSelected(GtkListBox* box, GtkListBoxRow* row);
        /**
         * @brief Handles when the disclaimer is triggered.
         * @param args Nickvision::Events::ParamEventArgs<std::string>
         */
        void onDisclaimerTriggered(const Nickvision::Events::ParamEventArgs<std::string>& args);
        /**
         * @brief Handles when the ability to download is changed.
         * @param args Nickvision::Events::ParamEventArgs<bool>
         */
        void onDownloadAbilityChanged(const Nickvision::Events::ParamEventArgs<bool>& args);
        /**
         * @brief Handles when the history is changed.
         * @param args Nickvision::Events::ParamEventArgs<std::vector<Models::HistoricDownload>>
         */
        void onHistoryChanged(const ::Nickvision::Events::ParamEventArgs<std::vector<::Nickvision::TubeConverter::Shared::Models::HistoricDownload>>& args);
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
        std::shared_ptr<Shared::Controllers::MainWindowController> m_controller;
        GtkApplication* m_app;
        GtkBuilder* m_builder;
        AdwApplicationWindow* m_window;
        std::vector<AdwActionRow*> m_historyRows;
    };
}

#endif //MAINWINDOW_H