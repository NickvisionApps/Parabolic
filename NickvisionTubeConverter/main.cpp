#include <QApplication>
#include "Models/AppInfo.h"
#include "UI/Views/MainWindow.h"

using namespace NickvisionTubeConverter::Models;
using namespace NickvisionTubeConverter::UI::Views;

int main(int argc, char *argv[])
{
    //==App Info==//
    AppInfo& appInfo = AppInfo::getInstance();
    appInfo.setName("Nickvision Tube Converter");
    appInfo.setDescription("An easy-to-use YouTube video downloader.");
    appInfo.setVersion("2022.8.0");
    appInfo.setChangelog("- Application rewrite with C++ and Qt\n- New workflow design");
    appInfo.setGitHubRepo("https://github.com/nlogozzo/NickvisionTubeConverter");
    appInfo.setIssueTracker("https://github.com/nlogozzo/NickvisionTubeConverter/issues/new");
    //==App Settings==//
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QApplication::setStyle("fusion");
    //==Run App==//
    QApplication application{ argc, argv };
    MainWindow mainWindow;
    mainWindow.showMaximized();
    return application.exec();
}
