#ifndef APP_H
#define APP_H

#include "pch.h"
#include "App.xaml.g.h"
#include <memory>
#include "controllers/mainwindowcontroller.h"

namespace winrt::Nickvision::TubeConverter::WinUI::implementation
{
    /**
     * @brief The main WinUI application point.
     */
    struct App : AppT<App>
    {
    public:
        /**
         * @brief Constructs an App.
         */
        App();
        /**
         * @brief Handles when the application is launched.
         * @param args Microsoft::UI::Xaml::LaunchActivatedEventArgs
         */
        void OnLaunched(const Microsoft::UI::Xaml::LaunchActivatedEventArgs& args) noexcept;

    private:
        std::shared_ptr<::Nickvision::TubeConverter::Shared::Controllers::MainWindowController> m_controller;
        winrt::Microsoft::UI::Xaml::Window m_window;
        Microsoft::UI::Xaml::ElementTheme m_systemTheme;
    };
}

#endif //APP_H