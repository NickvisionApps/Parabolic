#include "application.h"

using namespace Nickvision::TubeConverter::Shared::Controllers;

namespace Nickvision::TubeConverter::QT
{
    Application::Application(int argc, char* argv[])
        : QApplication{ argc, argv },
        m_controller{ std::make_shared<MainWindowController>(std::vector<std::string>(argv, argv + argc)) },
        m_mainWindow{ nullptr }
    {
        
    }

    int Application::exec()
    {
        m_controller->log(Logging::LogLevel::Info, "Started QT application.");
        m_mainWindow = std::make_shared<Views::MainWindow>(m_controller);
        m_mainWindow->show();
        return QApplication::exec();
    }
}