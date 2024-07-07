#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <vector>
#include <adwaita.h>
#include "controllers/mainwindowcontroller.h"
#include "views/mainwindow.h"

namespace Nickvision::TubeConverter::GNOME
{
    /**
     * @brief The main GTK application point.
     */
    class Application
    {
    public:
        /**
         * @brief Constructs an Application.
         * @param argc The number of arguments passed to the application
         * @param argv The array of argument strings passed to the application
         */
        Application(int argc, char* argv[]);
        /**
         * @brief Runs the application.
         * @brief This runs the gtk application loop.
         * @return The return code from the gtk application 
         */
        int run();

    private:
        /**
         * @brief Handles starting the application.
         * @param app The GtkApplication for the running app 
         */
        void onActivate(GtkApplication* app);
        std::shared_ptr<Shared::Controllers::MainWindowController> m_controller;
        AdwApplication* m_adw;
        std::shared_ptr<Views::MainWindow> m_mainWindow;
    };
}

#endif //APPLCIATION_H