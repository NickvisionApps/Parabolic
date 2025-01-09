#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <QApplication>
#include "controllers/mainwindowcontroller.h"
#include "views/mainwindow.h"

namespace Nickvision::TubeConverter::Qt
{
    /**
     * @brief The main Qt application point.
     */
    class Application : public QApplication
    {
    Q_OBJECT

    public:
        /**
         * @brief Constructs an Application.
         * @param argc The number of arguments passed to the application
         * @param argv The array of argument strings passed to the application
         */
        Application(int argc, char* argv[]);
        /**
         * @brief Runs the application.
         * @brief This runs the qt application loop.
         * @return The return code from the qt application 
         */
        int exec();

    private:
        std::shared_ptr<Shared::Controllers::MainWindowController> m_controller;
        std::shared_ptr<Views::MainWindow> m_mainWindow;
    };
}


#endif //APPLICATION_H
