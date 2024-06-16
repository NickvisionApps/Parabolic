#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
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
         * @brief Handles when a file is dropped on the window.
         * @param value The GValue dropped on the window
         * @return True if drop was accepted, else false
         */
        bool onDrop(const GValue* value);
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
         * @brief Handles when the open folder is changed (including closed).
         * @param args Nickvision::Events::EventArgs 
         */
        void onFolderChanged(const Nickvision::Events::EventArgs& args);
        /**
         * @brief Quits the application. 
         */
        void quit();
        /**
         * @brief Opens a folder. 
         */
        void openFolder();
        /**
         * @brief Closes the folder if one is open. 
         */
        void closeFolder();
        /**
         * @brief Opens the application's preferences dialog. 
         */
        void preferences();
        /**
         * @brief Opens the application's keyboard shortcut dialog.
         */
        void keyboardShortcuts();
        /**
         * @brief Opens the application's about dialog. 
         */
        void about();
        std::shared_ptr<Shared::Controllers::MainWindowController> m_controller;
        GtkApplication* m_app;
        GtkBuilder* m_builder;
        AdwApplicationWindow* m_window;
    };
}

#endif //MAINWINDOW_H