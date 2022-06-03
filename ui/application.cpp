#include "application.h"

using namespace NickvisionTubeConverter::Models;
using namespace NickvisionTubeConverter::UI;
using namespace NickvisionTubeConverter::UI::Views;

Application::Application(const std::string& id, GApplicationFlags flags) : m_adwApp{adw_application_new(id.c_str(), flags)}
{
    //==Resources==//
    g_resources_register(g_resource_load("/usr/share/org.nickvision.tubeconverter/resources.gresource", nullptr));
    //==Signals==//
    g_signal_connect(m_adwApp, "activate", G_CALLBACK((void (*)(GtkApplication*, gpointer*))[](GtkApplication* app, gpointer* data) { reinterpret_cast<Application*>(data)->onActivate(app); }), this);
}

int Application::run(int argc, char* argv[])
{
    return g_application_run(G_APPLICATION(m_adwApp), argc, argv);
}

void Application::onActivate(GtkApplication* app)
{
    //==Configuration==//
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
    //==Main Window==//
    m_mainWindow = std::make_shared<MainWindow>(m_configuration);
    gtk_application_add_window(app, GTK_WINDOW(m_mainWindow->gobj()));
    m_mainWindow->showMaximized();
}
