#pragma once

#include <string>
#include <adwaita.h>
#include "../widget.h"
#include "../../models/configuration.h"

namespace NickvisionTubeConverter::UI::Views
{
    class MainWindow : public NickvisionTubeConverter::UI::Widget
    {
    public:
        MainWindow(NickvisionTubeConverter::Models::Configuration& configuration);
        ~MainWindow();
        void showMaximized();

    private:
        NickvisionTubeConverter::Models::Configuration& m_configuration;
        bool m_opened;
        //==App Actions==//
        GSimpleAction* m_gio_actOpenFolder;
        GSimpleAction* m_gio_actCloseFolder;
        //==Help Actions==//
        GSimpleAction* m_gio_actPreferences;
        GSimpleAction* m_gio_actKeyboardShortcuts;
        GSimpleAction* m_gio_actChangelog;
        GSimpleAction* m_gio_actAbout;
        //==Signals==//
        void onStartup();
        void openFolder();
        void closeFolder();
        void preferences();
        void keyboardShortcuts();
        void changelog();
        void about();
        //==Other Functions==//
        void sendToast(const std::string& message);
    };
}
