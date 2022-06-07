#pragma once

#include <string>
#include <memory>
#include <adwaita.h>
#include "../models/configuration.h"
#include "views/mainwindow.h"

namespace NickvisionTubeConverter::UI
{
    class Application
    {
    public:
        Application(const std::string& id, GApplicationFlags flags = G_APPLICATION_FLAGS_NONE);
        int run(int argc, char* argv[]);

    private:
        //==UI==//
        AdwApplication* m_adwApp;
        NickvisionTubeConverter::Models::Configuration m_configuration;
        std::shared_ptr<NickvisionTubeConverter::UI::Views::MainWindow> m_mainWindow;
        //==Signals==//
        void onActivate(GtkApplication* app);
    };
}
