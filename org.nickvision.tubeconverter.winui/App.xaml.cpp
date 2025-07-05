#include "App.xaml.h"
#include "Views/MainWindow.xaml.h"
#include <libnick/helpers/stringhelpers.h>

using namespace ::Nickvision::Helpers;
using namespace ::Nickvision::TubeConverter::Shared::Controllers;
using namespace ::Nickvision::TubeConverter::Shared::Models;
using namespace winrt::Microsoft::UI::Xaml;

namespace winrt::Nickvision::TubeConverter::WinUI::implementation
{
    App::App()
        : m_controller{ std::make_shared<MainWindowController>(StringHelpers::splitArgs(GetCommandLineA())) },
        m_window{ nullptr }
    {
        InitializeComponent();
#ifdef DEBUG
        UnhandledException([this](const IInspectable&, const UnhandledExceptionEventArgs& e)
        {
            if(IsDebuggerPresent())
            {
                auto errorMessage = e.Message();
                __debugbreak();
            }
            throw;
        });
#endif
        m_systemTheme = RequestedTheme() == ApplicationTheme::Light ? ElementTheme::Light : ElementTheme::Dark;
        switch (m_controller->getTheme())
        {
        case Theme::Light:
            RequestedTheme(ApplicationTheme::Light);
            break;
        case Theme::Dark:
            RequestedTheme(ApplicationTheme::Dark);
            break;
        default:
            break;
        }
    }

    void App::OnLaunched(const Microsoft::UI::Xaml::LaunchActivatedEventArgs&) noexcept
    {
        if(!m_window)
        {
            m_window = winrt::make<Views::implementation::MainWindow>();
            m_window.as<Views::implementation::MainWindow>()->Controller(m_controller);
            m_window.as<Views::implementation::MainWindow>()->SystemTheme(m_systemTheme);
        }
        m_window.Activate();
    }
}
