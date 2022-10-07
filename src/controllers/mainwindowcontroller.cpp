#include "mainwindowcontroller.hpp"
#include <filesystem>

using namespace NickvisionTubeConverter::Controllers;
using namespace NickvisionTubeConverter::Models;

MainWindowController::MainWindowController(AppInfo& appInfo, Configuration& configuration) : m_appInfo{ appInfo }, m_configuration{ configuration }, m_isOpened{ false }
{

}

const AppInfo& MainWindowController::getAppInfo() const
{
    return m_appInfo;
}

AddDownloadDialogController MainWindowController::createAddDownloadDialogController() const
{
    return { };
}

PreferencesDialogController MainWindowController::createPreferencesDialogController() const
{
    return { m_configuration };
}

void MainWindowController::registerSendToastCallback(const std::function<void(const std::string& message)>& callback)
{
    m_sendToastCallback = callback;
}

void MainWindowController::startup()
{
    if(!m_isOpened)
    {
        m_isOpened = true;
    }
}

void MainWindowController::onConfigurationChanged()
{

}
