#include "application.hpp"
#include "../controllers/mainwindowcontroller.hpp"
#include "../helpers/translation.hpp"

using namespace NickvisionTubeConverter::Controllers;
using namespace NickvisionTubeConverter::Models;
using namespace NickvisionTubeConverter::UI;
using namespace NickvisionTubeConverter::UI::Views;

Application::Application(const std::string& id, GApplicationFlags flags) : m_adwApp{ adw_application_new(id.c_str(), flags) }
{
    //AppInfo
    m_appInfo.setId(id);
    m_appInfo.setName("Nickvision Tube Converter");
    m_appInfo.setShortName("Tube Converter");
    m_appInfo.setDescription(_("An easy-to-use YouTube video downloader."));
    m_appInfo.setVersion("2022.11.0-beta2");
    m_appInfo.setChangelog("<ul><li>Fixed an issue where videos could not be downloaded on ARM64</li><li>Fixed an issue where 'Best' and 'Good' would download the same video quality</li><li>Improved design of the Logs dialog</li><li>Added translation support</li><li>Added Arabic translation (Thanks @fawaz006!)</li><li>Added Dutch translation (Thanks @Vistaus!)</li><li>Added French translation (Thanks @zothma!)</li><li>Added Russian translation (Thanks @fsobolev!)</li></ul>");
    m_appInfo.setGitHubRepo("https://github.com/nlogozzo/NickvisionTubeConverter");
    m_appInfo.setIssueTracker("https://github.com/nlogozzo/NickvisionTubeConverter/issues/new");
    m_appInfo.setSupportUrl("https://github.com/nlogozzo/NickvisionTubeConverter/discussions");
    //Signals
    g_signal_connect(m_adwApp, "activate", G_CALLBACK((void (*)(GtkApplication*, gpointer))[](GtkApplication* app, gpointer data) { reinterpret_cast<Application*>(data)->onActivate(app); }), this);
}

int Application::run(int argc, char* argv[])
{
    return g_application_run(G_APPLICATION(m_adwApp), argc, argv);
}

void Application::onActivate(GtkApplication* app)
{
    if(m_configuration.getTheme() == Theme::System)
    {
         adw_style_manager_set_color_scheme(adw_style_manager_get_default(), ADW_COLOR_SCHEME_PREFER_LIGHT);
    }
    else if(m_configuration.getTheme() == Theme::Light)
    {
         adw_style_manager_set_color_scheme(adw_style_manager_get_default(), ADW_COLOR_SCHEME_FORCE_LIGHT);
    }
    else if(m_configuration.getTheme() == Theme::Dark)
    {
         adw_style_manager_set_color_scheme(adw_style_manager_get_default(), ADW_COLOR_SCHEME_FORCE_DARK);
    }
    m_mainWindow = std::make_shared<MainWindow>(app, MainWindowController(m_appInfo, m_configuration));
    gtk_application_add_window(app, GTK_WINDOW(m_mainWindow->gobj()));
    m_mainWindow->start();
}
