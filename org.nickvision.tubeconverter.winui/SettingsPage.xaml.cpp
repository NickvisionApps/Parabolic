#include "SettingsPage.xaml.h"
#if __has_include("SettingsPage.g.cpp")
#include "SettingsPage.g.cpp"
#endif
#include <libnick/localization/gettext.h>

using namespace ::Nickvision::TubeConverter::Shared::Controllers;
using namespace ::Nickvision::TubeConverter::Shared::Models;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;

namespace winrt::Nickvision::TubeConverter::WinUI::implementation 
{
    SettingsPage::SettingsPage()
        : m_constructing{ true }
    {
        InitializeComponent();
        //Localize Strings
        LblTitle().Text(winrt::to_hstring(_("Settings")));
        LblAppearanceBehavior().Text(winrt::to_hstring(_("Appearance & behavior")));
        RowTheme().Title(winrt::to_hstring(_("Theme")));
        CmbTheme().Items().Append(winrt::box_value(winrt::to_hstring(_p("Theme", "Light"))));
        CmbTheme().Items().Append(winrt::box_value(winrt::to_hstring(_p("Theme", "Dark"))));
        CmbTheme().Items().Append(winrt::box_value(winrt::to_hstring(_p("Theme", "System"))));
        RowAutomaticallyCheckForUpdates().Title(winrt::to_hstring(_("Automatically Check for Updates")));
        TglAutomaticallyCheckForUpdates().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglAutomaticallyCheckForUpdates().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
    }

    void SettingsPage::SetController(const std::shared_ptr<PreferencesViewController>& controller)
    {
        m_controller = controller;
        //Load
        m_constructing = true;
        CmbTheme().SelectedIndex(static_cast<int>(m_controller->getTheme()));
        TglAutomaticallyCheckForUpdates().IsOn(m_controller->getAutomaticallyCheckForUpdates());
        m_constructing = false;
    }

    void SettingsPage::OnCmbChanged(const IInspectable& sender, const SelectionChangedEventArgs& args)
    {
        ApplyChanges();
    }

    void SettingsPage::OnSwitchToggled(const IInspectable& sender, const RoutedEventArgs& args)
    {
        ApplyChanges();
    }

    void SettingsPage::ApplyChanges()
    {
        if(!m_constructing)
        {
            if(m_controller->getTheme() != static_cast<Theme>(CmbTheme().SelectedIndex()))
            {
                m_controller->setTheme(static_cast<Theme>(CmbTheme().SelectedIndex()));
            }
            m_controller->setAutomaticallyCheckForUpdates(TglAutomaticallyCheckForUpdates().IsOn());
            m_controller->saveConfiguration();
        }
    }
}
