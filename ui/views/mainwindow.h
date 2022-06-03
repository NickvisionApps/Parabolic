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
        GSimpleAction* m_gio_actSelectSaveFolder;
        GSimpleAction* m_gio_actAddToQueue;
        GSimpleAction* m_gio_actRemoveFromQueue;
        GSimpleAction* m_gio_actClearQueue;
        GSimpleAction* m_gio_actDownloadVideos;
        //==Help Actions==//
        GSimpleAction* m_gio_actPreferences;
        GSimpleAction* m_gio_actKeyboardShortcuts;
        GSimpleAction* m_gio_actChangelog;
        GSimpleAction* m_gio_actAbout;
        //==Signals==//
        void onStartup();
        void selectSaveFolder();
        void addToQueue();
        void removeFromQueue();
        void clearQueue();
        void downloadVideos();
        void preferences();
        void keyboardShortcuts();
        void changelog();
        void about();
        //==Other Functions==//
        void sendToast(const std::string& message);
    };
}
